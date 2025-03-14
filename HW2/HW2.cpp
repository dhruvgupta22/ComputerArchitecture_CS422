#include <iostream>
#include <fstream>
#include <set>
#include "pin.H"
#include <cstdlib>
#include <algorithm>
#include <unordered_map>

using namespace std;

/* Macro and type definitions */
#define BILLION 1000000000

#define BIMODAL_ROW 512
#define BIMODAL_COL 2
#define BIMODAL_MID (1 << (BIMODAL_COL-1))
#define BIMODAL_MAX ((1 << BIMODAL_COL)-1)

#define SAg_BHT_ROW 1024
#define SAg_BHT_COL 9
#define SAg_BHT_MASK ((1 << SAg_BHT_COL)-1)
#define SAg_PHT_ROW 512
#define SAg_PHT_COL 2
#define SAg_MID (1 << (SAg_PHT_COL-1))
#define SAg_MAX ((1 << (SAg_PHT_COL))-1)

#define GAg_GHR_SIZE 9
#define GAg_GHR_MASK ((1 << GAg_GHR_SIZE)-1)
#define GAg_PHT_ROW 512
#define GAg_PHT_COL 3
#define GAg_MID (1 << (GAg_PHT_COL-1))
#define GAg_MAX ((1 << (GAg_PHT_COL))-1)

typedef enum{
    INS_DIRECT_CALL=0,
	INS_INDIRECT_CALL,
	INS_RETURN,
	INS_UNCOND_BRANCH,
	INS_COND_BRANCH,
    INS_OTHERS,
	INS_COUNT
} INS_CATEGORY;

typedef enum{
    FNBT=0,
    BIMODAL,
    SAg,
    GAg,
    GSHARE,
    COMB2,
    COMB3MAJORITY,
	COMB3,
    DP_COUNT
} DIR_PRED;

typedef struct{
    UINT64 forw;
    UINT64 back;
} Mispred_Count;

/* Global variables */
std::ostream * out = &cerr;
UINT64 bimodal_pht[BIMODAL_ROW];

UINT64 sag_bht[SAg_BHT_ROW];
UINT64 sag_pht[SAg_PHT_ROW];

UINT64 gag_ghr;
UINT64 gag_pht[GAg_PHT_ROW];

ADDRINT fastForwardDone = 0;
UINT64 icount = 0; //number of dynamically executed instructions

UINT64 fastForwardIns; // number of instruction to fast forward
UINT64 maxIns; // maximum number of instructions to simulate

UINT64 dp_forwbr = 0;
UINT64 dp_backbr = 0;
Mispred_Count Mispred[DP_COUNT];

/* Command line switches */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "HW2.out", "specify file name for HW2 output");
KNOB<UINT64> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "number of instructions to fast forward in billions");


/* Print out help message. */
INT32 Usage(){
	cerr << "CS422 Homework 2" << endl;
	cerr << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}

VOID InsCount(UINT32 c){
	icount += c;
}

ADDRINT FastForward(void){
	return (icount >= fastForwardIns && icount < maxIns);
}

VOID FastForwardDone(void){
	fastForwardDone = 1;
}

ADDRINT IsFastForwardDone(void){
	return fastForwardDone;
}

ADDRINT Terminate(void){
        return (icount >= maxIns);
}


string pred_names[DP_COUNT] = {
	"FNBT", "Bimodal", "SAg", "GAg", "gshare", "Combined2", "Combined3Majority", "Combined3"
};

VOID StatDump(void){
	*out << "===============================================" << endl;
	*out << "Direction Predictors" << endl;
	for(int i=0; i<DP_COUNT; i++){
		*out << pred_names[i] << ": Accesses " << (dp_forwbr + dp_backbr);
		*out << " Mispredictions " << (Mispred[i].forw + Mispred[i].back) ;
		*out << " (" << (1.0*Mispred[i].forw + Mispred[i].back)/(1.0*dp_forwbr + dp_backbr) << ")";
		*out << " Forward Branches " << dp_forwbr;
		*out << " Forward Mispredictions " << Mispred[i].forw;
		*out << " (" << (1.0*Mispred[i].forw)/(1.0*dp_forwbr) << ")";
		*out << " Backward Branches " << dp_backbr;
		*out << " Backward Mispredictions " << Mispred[i].back;
		*out << " (" << (1.0*Mispred[i].back)/(1.0*dp_backbr) << ")";
		*out << endl;
	}
	*out << endl;
	*out << "Branch Target Predictors" << endl;
	*out << endl;
	*out << endl;
	*out << "===============================================" << endl;
	exit(0);
}

VOID FwdAccess(){
    dp_forwbr++;
}

VOID BkdAccess(){
    dp_backbr++;
}

VOID FwdMispred_FNBT(BOOL taken){
    Mispred[0].forw += taken^0;
}

VOID BkdMispred_FNBT(BOOL taken){
    Mispred[0].back += taken^1;
}

VOID FwdMispred_bimodal(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%BIMODAL_ROW;
	UINT64 pred = bimodal_pht[hpc];
	BOOL prediction = (pred < BIMODAL_MID) ? 0 : 1;
	Mispred[1].forw += taken^prediction;
	bimodal_pht[hpc] = (taken) ? ((pred == BIMODAL_MAX) ? pred : pred+1) : ((pred == 0) ? 0 : pred-1);
}
VOID BkdMispred_bimodal(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%BIMODAL_ROW;
	UINT64 pred = bimodal_pht[hpc];
	BOOL prediction = (pred < BIMODAL_MID) ? 0 : 1;
	Mispred[1].back += taken^prediction;
	bimodal_pht[hpc] = (taken) ? ((pred == BIMODAL_MAX) ? pred : pred+1) : ((pred == 0) ? 0 : pred-1);
}

VOID FwdMispred_sag(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%SAg_BHT_ROW;
	UINT64 hist = sag_bht[hpc];
	UINT64 counter = sag_pht[hist];
	BOOL prediction = (counter < SAg_MID) ? 0 : 1;
	Mispred[2].forw += taken^prediction;
	sag_pht[hist] =  (taken) ? ((counter == SAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	sag_bht[hpc] = ((hist << 1 | (taken)) & SAg_BHT_MASK);
}
VOID BkdMispred_sag(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%SAg_BHT_ROW;
	UINT64 hist = sag_bht[hpc];
	UINT64 counter = sag_pht[hist];
	BOOL prediction = (counter < SAg_MID) ? 0 : 1;
	Mispred[2].back += taken^prediction;
	sag_pht[hist] =  (taken) ? ((counter == SAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	sag_bht[hpc] = ((hist << 1 | (taken)) & SAg_BHT_MASK);
}

VOID FwdMispred_gag(BOOL taken){
	UINT64 hist = gag_ghr & GAg_GHR_MASK;
	UINT64 counter = gag_pht[hist];
	BOOL prediction = (counter < GAg_MID) ? 0 : 1;
	Mispred[3].forw += taken^prediction;
	gag_pht[hist] =  (taken) ? ((counter == GAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	gag_ghr = ((hist << 1 | (taken)) & GAg_GHR_MASK);
}
VOID BkdMispred_gag(BOOL taken){
	UINT64 hist = gag_ghr & GAg_GHR_MASK;
	UINT64 counter = gag_pht[hist];
	BOOL prediction = (counter < GAg_MID) ? 0 : 1;
	Mispred[3].back += taken^prediction;
	gag_pht[hist] =  (taken) ? ((counter == GAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	gag_ghr = ((hist << 1 | (taken)) & GAg_GHR_MASK);
}

/* Instruction instrumentation routine */
VOID Trace(TRACE trace, VOID *v)
{
	INS_CATEGORY category;

	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
		BBL_InsertIfCall(bbl, IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
        	BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR) StatDump, IARG_END);

		BBL_InsertIfCall(bbl, IPOINT_BEFORE, (AFUNPTR) FastForward, IARG_END);
		BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR) FastForwardDone, IARG_END);

		for (INS ins = BBL_InsHead(bbl); ; ins = INS_Next(ins)) {

			/* Calculate instruction type */
			switch (INS_Category(ins))
			{

				case XED_CATEGORY_CALL:
					if (INS_IsDirectCall(ins)) category = INS_DIRECT_CALL;
					else category = INS_INDIRECT_CALL;
					break;

				case XED_CATEGORY_RET:
					category = INS_RETURN;
					break;

				case XED_CATEGORY_UNCOND_BR:
					category = INS_UNCOND_BRANCH;
					break;

				case XED_CATEGORY_COND_BR:
					category = INS_COND_BRANCH;
					break;

				default:
                    category = INS_OTHERS;
					break;
			}

			// /* Create a list for analysis arguments */
			// IARGLIST args = IARGLIST_Alloc();

			// /* Add category as the first argument to analysis routine */
			// IARGLIST_AddArguments(args, IARG_UINT32, category, IARG_END);

            if(category == INS_COND_BRANCH){
				ADDRINT ins_addr = INS_Address(ins);
                ADDRINT taken_addr = INS_DirectControlFlowTargetAddress(ins);
				// UINT32 pc = (UINT32)ins_addr;
                if(taken_addr > ins_addr){
                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdAccess, IARG_END); 
                    
                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_FNBT, IARG_BRANCH_TAKEN, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_bimodal, IARG_BRANCH_TAKEN, IARG_UINT64, (UINT64)ins_addr, IARG_END);
					// INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_bimodal, IARG_BRANCH_TAKEN, IARG_ADDRINT, ins_addr, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_sag, IARG_BRANCH_TAKEN, IARG_UINT64, (UINT64)ins_addr, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_gag, IARG_BRANCH_TAKEN, IARG_END);
				}
                else{
                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdAccess, IARG_END); 

                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_FNBT, IARG_BRANCH_TAKEN, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_bimodal, IARG_BRANCH_TAKEN, IARG_UINT64, (UINT64)ins_addr, IARG_END);
					// INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_bimodal, IARG_BRANCH_TAKEN, IARG_ADDRINT, ins_addr, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_sag, IARG_BRANCH_TAKEN, IARG_UINT64, (UINT64)ins_addr, IARG_END);
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_gag, IARG_BRANCH_TAKEN, IARG_END);
				}
            }
			// INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			// INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) <FuncName>, IARG_IARGLIST, args, IARG_END);

			// /* Free analysis argument list */
			// IARGLIST_Free(args);

			if (ins == BBL_InsTail(bbl)) break;
		}

		/* Called for each BBL */
        	BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR) InsCount, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
	}
}

/* Fini routine */
VOID Fini(INT32 code, VOID * v)
{
    *out << "FNBT Results : \n";
    *out << "Forw Access = " << dp_forwbr << endl;
    *out << "Back Access = " << dp_backbr << endl;
    *out << "Forw Mispred = " << Mispred[FNBT].forw << endl;
    *out << "Back Mispred = " << Mispred[FNBT].back << endl;
	*out << "Bimodal Results : \n";
    *out << "Forw Mispred = " << Mispred[BIMODAL].forw << endl;
    *out << "Back Mispred = " << Mispred[BIMODAL].back << endl;
}

int main(int argc, char *argv[])
{
	// Initialize PIN library. Print help message if -h(elp) is specified
	// in the command line or the command line is invalid 
	if (PIN_Init(argc, argv))
		return Usage();

	/* Set number of instructions to fast forward and simulate */
	fastForwardIns = KnobFastForward.Value() * BILLION;
	maxIns = fastForwardIns + BILLION;

	string fileName = KnobOutputFile.Value();

	if (!fileName.empty())
		out = new std::ofstream(fileName.c_str());

	for(int i=0; i<BIMODAL_ROW; i++) bimodal_pht[i] = 0;
	for(int i=0; i<SAg_BHT_ROW; i++) sag_bht[i] = 0;
	for(int i=0; i<SAg_PHT_ROW; i++) sag_pht[i] = 0;
	gag_ghr = 0;
	for(int i=0; i<GAg_PHT_ROW; i++) gag_pht[i] = 0;
    
	// Register function to be called to instrument instructions
	TRACE_AddInstrumentFunction(Trace, 0);

	// Register function to be called when the application exits
	PIN_AddFiniFunction(Fini, 0);

	cerr << "===============================================" << endl;
	cerr << "This application is instrumented by HW2" << endl;
	if (!KnobOutputFile.Value().empty())
		cerr << "See file " << KnobOutputFile.Value() << " for analysis results" << endl;
	cerr << "===============================================" << endl;

	// Start the program, never returns
	PIN_StartProgram();

	return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */