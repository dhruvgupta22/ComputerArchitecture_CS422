#include <iostream>
#include <fstream>
#include <set>
#include "pin.H"
#include <cstdlib>
#include <algorithm>
#include <unordered_map>
#include <chrono>

using namespace std;
chrono::time_point<chrono::high_resolution_clock> startTime, endTime;

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

#define GSHARE_GHR_SIZE 9
#define GSHARE_GHR_MASK ((1 << GSHARE_GHR_SIZE)-1)
#define GSHARE_PHT_ROW 512
#define GSHARE_PHT_COL 3
#define GSHARE_MID (1 << (GSHARE_PHT_COL-1))
#define GSHARE_MAX ((1 << (GSHARE_PHT_COL))-1)

#define HYBRID_2_GHR_SIZE 9
#define HYBRID_2_GHR_MASK ((1 << HYBRID_2_GHR_SIZE)-1)
#define HYBRID_2_META_ROW 512
#define HYBRID_2_META_COL 2
#define HYBRID_2_META_MID (1 << (HYBRID_2_META_COL-1))
#define HYBRID_2_META_MAX ((1 << (HYBRID_2_META_COL))-1)
#define HYBRID_2_SAg_BHT_ROW 1024
#define HYBRID_2_SAg_BHT_COL 9
#define HYBRID_2_SAg_BHT_MASK ((1 << HYBRID_2_SAg_BHT_COL)-1)
#define HYBRID_2_SAg_PHT_ROW 512
#define HYBRID_2_SAg_PHT_COL 2
#define HYBRID_2_SAg_MID (1 << (HYBRID_2_SAg_PHT_COL-1))
#define HYBRID_2_SAg_MAX ((1 << (HYBRID_2_SAg_PHT_COL))-1)
#define HYBRID_2_GAg_GHR_SIZE 9
#define HYBRID_2_GAg_GHR_MASK ((1 << HYBRID_2_GAg_GHR_SIZE)-1)
#define HYBRID_2_GAg_PHT_ROW 512
#define HYBRID_2_GAg_PHT_COL 3
#define HYBRID_2_GAg_MID (1 << (HYBRID_2_GAg_PHT_COL-1))
#define HYBRID_2_GAg_MAX ((1 << (HYBRID_2_GAg_PHT_COL))-1)

#define HYBRID_3_MAJ_SAg_BHT_ROW 1024
#define HYBRID_3_MAJ_SAg_BHT_COL 9
#define HYBRID_3_MAJ_SAg_BHT_MASK ((1 << HYBRID_3_MAJ_SAg_BHT_COL)-1)
#define HYBRID_3_MAJ_SAg_PHT_ROW 512
#define HYBRID_3_MAJ_SAg_PHT_COL 2
#define HYBRID_3_MAJ_SAg_MID (1 << (HYBRID_3_MAJ_SAg_PHT_COL-1))
#define HYBRID_3_MAJ_SAg_MAX ((1 << (HYBRID_3_MAJ_SAg_PHT_COL))-1)
#define HYBRID_3_MAJ_GAg_GHR_SIZE 9
#define HYBRID_3_MAJ_GAg_GHR_MASK ((1 << HYBRID_3_MAJ_GAg_GHR_SIZE)-1)
#define HYBRID_3_MAJ_GAg_PHT_ROW 512
#define HYBRID_3_MAJ_GAg_PHT_COL 3
#define HYBRID_3_MAJ_GAg_MID (1 << (HYBRID_3_MAJ_GAg_PHT_COL-1))
#define HYBRID_3_MAJ_GAg_MAX ((1 << (HYBRID_3_MAJ_GAg_PHT_COL))-1)
#define HYBRID_3_MAJ_GSHARE_GHR_SIZE 9
#define HYBRID_3_MAJ_GSHARE_GHR_MASK ((1 << HYBRID_3_MAJ_GSHARE_GHR_SIZE)-1)
#define HYBRID_3_MAJ_GSHARE_PHT_ROW 512
#define HYBRID_3_MAJ_GSHARE_PHT_COL 3
#define HYBRID_3_MAJ_GSHARE_MID (1 << (HYBRID_3_MAJ_GSHARE_PHT_COL-1))
#define HYBRID_3_MAJ_GSHARE_MAX ((1 << (HYBRID_3_MAJ_GSHARE_PHT_COL))-1)

#define HYBRID_3_SAg_BHT_ROW 1024
#define HYBRID_3_SAg_BHT_COL 9
#define HYBRID_3_SAg_BHT_MASK ((1 << HYBRID_3_SAg_BHT_COL)-1)
#define HYBRID_3_SAg_PHT_ROW 512
#define HYBRID_3_SAg_PHT_COL 2
#define HYBRID_3_SAg_MID (1 << (HYBRID_3_SAg_PHT_COL-1))
#define HYBRID_3_SAg_MAX ((1 << (HYBRID_3_SAg_PHT_COL))-1)
#define HYBRID_3_GAg_GHR_SIZE 9
#define HYBRID_3_GAg_GHR_MASK ((1 << HYBRID_3_GAg_GHR_SIZE)-1)
#define HYBRID_3_GAg_PHT_ROW 512
#define HYBRID_3_GAg_PHT_COL 3
#define HYBRID_3_GAg_MID (1 << (HYBRID_3_GAg_PHT_COL-1))
#define HYBRID_3_GAg_MAX ((1 << (HYBRID_3_GAg_PHT_COL))-1)
#define HYBRID_3_GSHARE_GHR_SIZE 9
#define HYBRID_3_GSHARE_GHR_MASK ((1 << HYBRID_3_GSHARE_GHR_SIZE)-1)
#define HYBRID_3_GSHARE_PHT_ROW 512
#define HYBRID_3_GSHARE_PHT_COL 3
#define HYBRID_3_GSHARE_MID (1 << (HYBRID_3_GSHARE_PHT_COL-1))
#define HYBRID_3_GSHARE_MAX ((1 << (HYBRID_3_GSHARE_PHT_COL))-1)
#define HYBRID_3_GHR1_SIZE 9
#define HYBRID_3_GHR1_MASK ((1 << HYBRID_3_GHR1_SIZE)-1)
#define HYBRID_3_META1_ROW 512
#define HYBRID_3_META1_COL 2
#define HYBRID_3_META1_MID (1 << (HYBRID_3_META1_COL-1))
#define HYBRID_3_META1_MAX ((1 << (HYBRID_3_META1_COL))-1)
#define HYBRID_3_GHR2_SIZE 9
#define HYBRID_3_GHR2_MASK ((1 << HYBRID_3_GHR2_SIZE)-1)
#define HYBRID_3_META2_ROW 512
#define HYBRID_3_META2_COL 2
#define HYBRID_3_META2_MID (1 << (HYBRID_3_META2_COL-1))
#define HYBRID_3_META2_MAX ((1 << (HYBRID_3_META2_COL))-1)
#define HYBRID_3_GHR3_SIZE 9
#define HYBRID_3_GHR3_MASK ((1 << HYBRID_3_GHR3_SIZE)-1)
#define HYBRID_3_META3_ROW 512
#define HYBRID_3_META3_COL 2
#define HYBRID_3_META3_MID (1 << (HYBRID_3_META3_COL-1))
#define HYBRID_3_META3_MAX ((1 << (HYBRID_3_META3_COL))-1)

#define MEMSET0(arr) do { \
	memset(arr, 0, sizeof(arr)); \
} while (0)

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
    HYBRID_2,
	HYBRID_3_MAJ,
	HYBRID_3,
    DP_COUNT
} DIR_PRED;

typedef struct{
    UINT64 forw;
    UINT64 back;
} Mispred_Count;

/* Global variables */
std::ostream * out = &cerr;

UINT64 bimodal_pht[BIMODAL_ROW];

UINT64 SAg_bht[SAg_BHT_ROW];
UINT64 SAg_pht[SAg_PHT_ROW];

UINT64 GAg_ghr;
UINT64 GAg_pht[GAg_PHT_ROW];

UINT64 gshare_ghr;
UINT64 gshare_pht[GSHARE_PHT_ROW];

UINT64 hybrid_2_SAg_bht[HYBRID_2_SAg_BHT_ROW];
UINT64 hybrid_2_SAg_pht[HYBRID_2_SAg_PHT_ROW];
UINT64 hybrid_2_GAg_ghr;
UINT64 hybrid_2_GAg_pht[HYBRID_2_GAg_PHT_ROW];
UINT64 hybrid_2_meta_ghr;
UINT64 hybrid_2_meta_pred[HYBRID_2_META_ROW];

UINT64 hybrid_3_maj_SAg_bht[HYBRID_3_MAJ_SAg_BHT_ROW];
UINT64 hybrid_3_maj_SAg_pht[HYBRID_3_MAJ_SAg_PHT_ROW];
UINT64 hybrid_3_maj_GAg_ghr;
UINT64 hybrid_3_maj_GAg_pht[HYBRID_3_MAJ_GAg_PHT_ROW];
UINT64 hybrid_3_maj_gshare_ghr;
UINT64 hybrid_3_maj_gshare_pht[HYBRID_3_MAJ_GSHARE_PHT_ROW];

UINT64 hybrid_3_SAg_bht[HYBRID_3_SAg_BHT_ROW];
UINT64 hybrid_3_SAg_pht[HYBRID_3_SAg_PHT_ROW];
UINT64 hybrid_3_GAg_ghr;
UINT64 hybrid_3_GAg_pht[HYBRID_3_GAg_PHT_ROW];
UINT64 hybrid_3_gshare_ghr;
UINT64 hybrid_3_gshare_pht[HYBRID_3_GSHARE_PHT_ROW];
UINT64 hybrid_3_meta1_ghr;
UINT64 hybrid_3_meta1_pred[HYBRID_3_META1_ROW];
UINT64 hybrid_3_meta2_ghr;
UINT64 hybrid_3_meta2_pred[HYBRID_3_META2_ROW];
UINT64 hybrid_3_meta3_ghr;
UINT64 hybrid_3_meta3_pred[HYBRID_3_META3_ROW];

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
		*out << pred_names[i] << " : Accesses " << (dp_forwbr + dp_backbr);
		*out << ", Mispredictions " << (Mispred[i].forw + Mispred[i].back) ;
		*out << " (" << (1.0*Mispred[i].forw + Mispred[i].back)/(1.0*dp_forwbr + dp_backbr) << ")";
		*out << ", Forward Branches " << dp_forwbr;
		*out << ", Forward Mispredictions " << Mispred[i].forw;
		*out << " (" << (1.0*Mispred[i].forw)/(1.0*dp_forwbr) << ")";
		*out << ", Backward Branches " << dp_backbr;
		*out << ", Backward Mispredictions " << Mispred[i].back;
		*out << " (" << (1.0*Mispred[i].back)/(1.0*dp_backbr) << ")";
		*out << endl;
	}
	*out << endl;
	*out << "===============================================" << endl;
	*out << "Branch Target Predictors" << endl;
	*out << endl;
	*out << endl;
	*out << "===============================================" << endl;
	endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = endTime - startTime;
    *out << "Time taken : " << elapsed.count() << " seconds." << endl;
	exit(0);
}

VOID FwdAccess(){
    dp_forwbr++;
}

VOID BkdAccess(){
    dp_backbr++;
}

VOID FwdMispred_FNBT(BOOL taken){
    Mispred[FNBT].forw += taken^0;
}

VOID BkdMispred_FNBT(BOOL taken){
    Mispred[FNBT].back += taken^1;
}

VOID FwdMispred_bimodal(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%BIMODAL_ROW;
	UINT64 counter = bimodal_pht[hpc];
	BOOL prediction = (counter < BIMODAL_MID) ? 0 : 1;
	Mispred[BIMODAL].forw += taken^prediction;
	bimodal_pht[hpc] = (taken) ? ((counter == BIMODAL_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
}

VOID BkdMispred_bimodal(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%BIMODAL_ROW;
	UINT64 counter = bimodal_pht[hpc];
	BOOL prediction = (counter < BIMODAL_MID) ? 0 : 1;
	Mispred[BIMODAL].back += taken^prediction;
	bimodal_pht[hpc] = (taken) ? ((counter == BIMODAL_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
}

VOID FwdMispred_SAg(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%SAg_BHT_ROW;
	UINT64 hist = SAg_bht[hpc];
	UINT64 counter = SAg_pht[hist];
	BOOL prediction = (counter < SAg_MID) ? 0 : 1;
	Mispred[SAg].forw += taken^prediction;
	SAg_pht[hist] =  (taken) ? ((counter == SAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	SAg_bht[hpc] = ((hist << 1 | (taken)) & SAg_BHT_MASK);
}

VOID BkdMispred_SAg(BOOL taken, UINT64 pc){
	UINT64 hpc = pc%SAg_BHT_ROW;
	UINT64 hist = SAg_bht[hpc];
	UINT64 counter = SAg_pht[hist];
	BOOL prediction = (counter < SAg_MID) ? 0 : 1;
	Mispred[SAg].back += taken^prediction;
	SAg_pht[hist] =  (taken) ? ((counter == SAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	SAg_bht[hpc] = ((hist << 1 | (taken)) & SAg_BHT_MASK);
}

VOID FwdMispred_GAg(BOOL taken){
	UINT64 hist = GAg_ghr & GAg_GHR_MASK;
	UINT64 counter = GAg_pht[hist];
	BOOL prediction = (counter < GAg_MID) ? 0 : 1;
	Mispred[GAg].forw += taken^prediction;
	GAg_pht[hist] =  (taken) ? ((counter == GAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	GAg_ghr = ((hist << 1 | (taken)) & GAg_GHR_MASK);
}

VOID BkdMispred_GAg(BOOL taken){
	UINT64 hist = GAg_ghr & GAg_GHR_MASK;
	UINT64 counter = GAg_pht[hist];
	BOOL prediction = (counter < GAg_MID) ? 0 : 1;
	Mispred[GAg].back += taken^prediction;
	GAg_pht[hist] =  (taken) ? ((counter == GAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	GAg_ghr = ((hist << 1 | (taken)) & GAg_GHR_MASK);
}

VOID FwdMispred_gshare(BOOL taken, UINT64 pc){
	UINT64 hist = gshare_ghr & GSHARE_GHR_MASK;
	UINT64 hash = (hist ^ pc) & GSHARE_GHR_MASK;
	UINT64 counter = gshare_pht[hash];
	BOOL prediction = (counter < GSHARE_MID) ? 0 : 1;
	Mispred[GSHARE].forw += taken^prediction;
	gshare_pht[hash] =  (taken) ? ((counter == GSHARE_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	gshare_ghr = ((hist << 1 | (taken)) & GSHARE_GHR_MASK);
}

VOID BkdMispred_gshare(BOOL taken, UINT64 pc){
	UINT64 hist = gshare_ghr & GSHARE_GHR_MASK;
	UINT64 hash = (hist ^ pc) & GSHARE_GHR_MASK;
	UINT64 counter = gshare_pht[hash];
	BOOL prediction = (counter < GSHARE_MID) ? 0 : 1;
	Mispred[GSHARE].back += taken^prediction;
	gshare_pht[hash] =  (taken) ? ((counter == GSHARE_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	gshare_ghr = ((hist << 1 | (taken)) & GSHARE_GHR_MASK);
}

VOID FwdMispred_hybrid_2(BOOL taken, UINT64 pc){
	UINT64 hist = hybrid_2_meta_ghr & HYBRID_2_GHR_MASK;
	UINT64 counter = hybrid_2_meta_pred[hist];
	BOOL choice = (counter < HYBRID_2_META_MID) ? 0 : 1;

	UINT64 hpc1 = pc%HYBRID_2_SAg_BHT_ROW;
	UINT64 hist1 = hybrid_2_SAg_bht[hpc1];
	UINT64 counter1 = hybrid_2_SAg_pht[hist1];
	BOOL prediction1 = (counter1 < HYBRID_2_SAg_MID) ? 0 : 1;
	
	UINT64 hist2 = hybrid_2_GAg_ghr & HYBRID_2_GAg_GHR_MASK;
	UINT64 counter2 = hybrid_2_GAg_pht[hist2];
	BOOL prediction2 = (counter2 < HYBRID_2_GAg_MID) ? 0 : 1;
	
	if(choice == 0) Mispred[HYBRID_2].forw += taken^prediction1;
	else Mispred[HYBRID_2].forw += taken^prediction2;
	
	hybrid_2_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_2_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_2_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_2_SAg_BHT_MASK);
	hybrid_2_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_2_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_2_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_2_GAg_GHR_MASK);
	BOOL s = (taken == prediction1);
	BOOL g = (taken == prediction2);
	hybrid_2_meta_pred[hist] = (g & !s) ? ((counter == HYBRID_2_META_MAX) ? counter : counter+1) : ((s & !g) ? ((counter == 0) ? 0 : counter-1) : counter);
	hybrid_2_meta_ghr = ((hist << 1 | (taken)) & HYBRID_2_GHR_MASK);
}

VOID BkdMispred_hybrid_2(BOOL taken, UINT64 pc){
	UINT64 hist = hybrid_2_meta_ghr & HYBRID_2_GHR_MASK;
	UINT64 counter = hybrid_2_meta_pred[hist];
	BOOL choice = (counter < HYBRID_2_META_MID) ? 0 : 1;

	UINT64 hpc1 = pc%HYBRID_2_SAg_BHT_ROW;
	UINT64 hist1 = hybrid_2_SAg_bht[hpc1];
	UINT64 counter1 = hybrid_2_SAg_pht[hist1];
	BOOL prediction1 = (counter1 < HYBRID_2_SAg_MID) ? 0 : 1;
	
	UINT64 hist2 = hybrid_2_GAg_ghr & HYBRID_2_GAg_GHR_MASK;
	UINT64 counter2 = hybrid_2_GAg_pht[hist2];
	BOOL prediction2 = (counter2 < HYBRID_2_GAg_MID) ? 0 : 1;
	
	if(choice == 0) Mispred[HYBRID_2].back += taken^prediction1;
	else Mispred[HYBRID_2].back += taken^prediction2;
	
	hybrid_2_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_2_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_2_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_2_SAg_BHT_MASK);
	hybrid_2_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_2_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_2_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_2_GAg_GHR_MASK);
	BOOL s = (taken == prediction1);
	BOOL g = (taken == prediction2);
	hybrid_2_meta_pred[hist] = (g & !s) ? ((counter == HYBRID_2_META_MAX) ? counter : counter+1) : ((s & !g) ? ((counter == 0) ? 0 : counter-1) : counter);
	hybrid_2_meta_ghr = ((hist << 1 | (taken)) & HYBRID_2_GHR_MASK);
}

VOID FwdMispred_hybrid_3_maj(BOOL taken, UINT64 pc){
	UINT64 hpc1 = pc%HYBRID_3_MAJ_SAg_BHT_ROW;
	UINT64 hist1 = hybrid_3_maj_SAg_bht[hpc1];
	UINT64 counter1 = hybrid_3_maj_SAg_pht[hist1];
	BOOL prediction1 = (counter1 < HYBRID_3_MAJ_SAg_MID) ? 0 : 1;
	
	UINT64 hist2 = hybrid_3_maj_GAg_ghr & HYBRID_3_MAJ_GAg_GHR_MASK;
	UINT64 counter2 = hybrid_3_maj_GAg_pht[hist2];
	BOOL prediction2 = (counter2 < HYBRID_3_MAJ_GAg_MID) ? 0 : 1;

	UINT64 hist3 = hybrid_3_maj_gshare_ghr & HYBRID_3_MAJ_GSHARE_GHR_MASK;
	UINT64 hash3 = (hist3 ^ pc) & HYBRID_3_MAJ_GSHARE_GHR_MASK;
	UINT64 counter3 = hybrid_3_maj_gshare_pht[hash3];
	BOOL prediction3 = (counter3 < HYBRID_3_MAJ_GSHARE_MID) ? 0 : 1;

	UINT64 taken_votes = (UINT64) prediction1 + (UINT64) prediction2 + (UINT64) prediction3;
	BOOL prediction = (taken_votes < 2) ? 0 : 1;

	Mispred[HYBRID_3_MAJ].forw += taken^prediction;

	hybrid_3_maj_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_3_MAJ_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_3_maj_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_3_MAJ_SAg_BHT_MASK);
	hybrid_3_maj_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_3_MAJ_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_3_maj_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_3_MAJ_GAg_GHR_MASK);
	hybrid_3_maj_gshare_pht[hash3] =  (taken) ? ((counter3 == HYBRID_3_MAJ_GSHARE_MAX) ? counter3 : counter3+1) : ((counter3 == 0) ? 0 : counter3-1);
	hybrid_3_maj_gshare_ghr = ((hist3 << 1 | (taken)) & HYBRID_3_MAJ_GSHARE_GHR_MASK);
}
VOID BkdMispred_hybrid_3_maj(BOOL taken, UINT64 pc){
	UINT64 hpc1 = pc%HYBRID_3_MAJ_SAg_BHT_ROW;
	UINT64 hist1 = hybrid_3_maj_SAg_bht[hpc1];
	UINT64 counter1 = hybrid_3_maj_SAg_pht[hist1];
	BOOL prediction1 = (counter1 < HYBRID_3_MAJ_SAg_MID) ? 0 : 1;
	
	UINT64 hist2 = hybrid_3_maj_GAg_ghr & HYBRID_3_MAJ_GAg_GHR_MASK;
	UINT64 counter2 = hybrid_3_maj_GAg_pht[hist2];
	BOOL prediction2 = (counter2 < HYBRID_3_MAJ_GAg_MID) ? 0 : 1;

	UINT64 hist3 = hybrid_3_maj_gshare_ghr & HYBRID_3_MAJ_GSHARE_GHR_MASK;
	UINT64 hash3 = (hist3 ^ pc) & HYBRID_3_MAJ_GSHARE_GHR_MASK;
	UINT64 counter3 = hybrid_3_maj_gshare_pht[hash3];
	BOOL prediction3 = (counter3 < HYBRID_3_MAJ_GSHARE_MID) ? 0 : 1;

	UINT64 taken_votes = (UINT64) prediction1 + (UINT64) prediction2 + (UINT64) prediction3;
	BOOL prediction = (taken_votes < 2) ? 0 : 1;

	Mispred[HYBRID_3_MAJ].back += taken^prediction;

	hybrid_3_maj_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_3_MAJ_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_3_maj_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_3_MAJ_SAg_BHT_MASK);
	hybrid_3_maj_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_3_MAJ_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_3_maj_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_3_MAJ_GAg_GHR_MASK);
	hybrid_3_maj_gshare_pht[hash3] =  (taken) ? ((counter3 == HYBRID_3_MAJ_GSHARE_MAX) ? counter3 : counter3+1) : ((counter3 == 0) ? 0 : counter3-1);
	hybrid_3_maj_gshare_ghr = ((hist3 << 1 | (taken)) & HYBRID_3_MAJ_GSHARE_GHR_MASK);
}

VOID FwdMispred_hybrid_3(BOOL taken, UINT64 pc){
	UINT64 hpc1 = pc%HYBRID_3_SAg_BHT_ROW;
	UINT64 hist1 = hybrid_3_SAg_bht[hpc1];
	UINT64 counter1 = hybrid_3_SAg_pht[hist1];
	BOOL prediction1 = (counter1 < HYBRID_3_SAg_MID) ? 0 : 1;
	
	UINT64 hist2 = hybrid_3_GAg_ghr & HYBRID_3_GAg_GHR_MASK;
	UINT64 counter2 = hybrid_3_GAg_pht[hist2];
	BOOL prediction2 = (counter2 < HYBRID_3_GAg_MID) ? 0 : 1;

	UINT64 hist3 = hybrid_3_gshare_ghr & HYBRID_3_GSHARE_GHR_MASK;
	UINT64 hash3 = (hist3 ^ pc) & HYBRID_3_GSHARE_GHR_MASK;
	UINT64 counter3 = hybrid_3_gshare_pht[hash3];
	BOOL prediction3 = (counter3 < HYBRID_3_GSHARE_MID) ? 0 : 1;

	UINT64 hist4 = hybrid_3_meta1_ghr & HYBRID_3_GHR1_MASK;
	UINT64 counter4 = hybrid_3_meta1_pred[hist4];
	BOOL choice4 = (counter4 < HYBRID_3_META1_MID) ? 0 : 1; // 0 = SAg, 1 = GAg

	UINT64 hist5 = hybrid_3_meta2_ghr & HYBRID_3_GHR2_MASK;
	UINT64 counter5 = hybrid_3_meta2_pred[hist5];
	BOOL choice5 = (counter5 < HYBRID_3_META2_MID) ? 0 : 1; // 0 = GAg, 1 = gshare

	UINT64 hist6 = hybrid_3_meta3_ghr & HYBRID_3_GHR3_MASK;
	UINT64 counter6 = hybrid_3_meta3_pred[hist6];
	BOOL choice6 = (counter6 < HYBRID_3_META3_MID) ? 0 : 1; // 0 = SAg, 1 = gshare

	BOOL prediction = (choice4) ? ((choice5) ? (prediction3) : (prediction2)) : ((choice6) ? (prediction3) : (prediction1)); 
	Mispred[HYBRID_3].forw += taken^prediction;
	
	hybrid_3_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_3_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_3_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_3_SAg_BHT_MASK);
	hybrid_3_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_3_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_3_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_3_GAg_GHR_MASK);
	hybrid_3_gshare_pht[hash3] =  (taken) ? ((counter3 == HYBRID_3_GSHARE_MAX) ? counter3 : counter3+1) : ((counter3 == 0) ? 0 : counter3-1);
	hybrid_3_gshare_ghr = ((hist3 << 1 | (taken)) & HYBRID_3_GSHARE_GHR_MASK);
	BOOL s = (taken == prediction1);
	BOOL g = (taken == prediction2);
	BOOL r = (taken == prediction3); 
	hybrid_3_meta1_pred[hist4] = (g & !s) ? ((counter4 == HYBRID_3_META1_MAX) ? counter4 : counter4+1) : ((s & !g) ? ((counter4 == 0) ? 0 : counter4-1) : counter4);
	hybrid_3_meta1_ghr = ((hist4 << 1 | (taken)) & HYBRID_3_GHR1_MASK);
	hybrid_3_meta2_pred[hist5] = (r & !g) ? ((counter5 == HYBRID_3_META2_MAX) ? counter5 : counter5+1) : ((g & !r) ? ((counter5 == 0) ? 0 : counter5-1) : counter5);
	hybrid_3_meta2_ghr = ((hist5 << 1 | (taken)) & HYBRID_3_GHR2_MASK);
	hybrid_3_meta3_pred[hist6] = (r & !s) ? ((counter6 == HYBRID_3_META3_MAX) ? counter6 : counter6+1) : ((s & !r) ? ((counter6 == 0) ? 0 : counter6-1) : counter6);
	hybrid_3_meta3_ghr = ((hist6 << 1 | (taken)) & HYBRID_3_GHR3_MASK);
}

VOID BkdMispred_hybrid_3(BOOL taken, UINT64 pc){
	UINT64 hpc1 = pc%HYBRID_3_SAg_BHT_ROW;
	UINT64 hist1 = hybrid_3_SAg_bht[hpc1];
	UINT64 counter1 = hybrid_3_SAg_pht[hist1];
	BOOL prediction1 = (counter1 < HYBRID_3_SAg_MID) ? 0 : 1;
	
	UINT64 hist2 = hybrid_3_GAg_ghr & HYBRID_3_GAg_GHR_MASK;
	UINT64 counter2 = hybrid_3_GAg_pht[hist2];
	BOOL prediction2 = (counter2 < HYBRID_3_GAg_MID) ? 0 : 1;

	UINT64 hist3 = hybrid_3_gshare_ghr & HYBRID_3_GSHARE_GHR_MASK;
	UINT64 hash3 = (hist3 ^ pc) & HYBRID_3_GSHARE_GHR_MASK;
	UINT64 counter3 = hybrid_3_gshare_pht[hash3];
	BOOL prediction3 = (counter3 < HYBRID_3_GSHARE_MID) ? 0 : 1;

	UINT64 hist4 = hybrid_3_meta1_ghr & HYBRID_3_GHR1_MASK;
	UINT64 counter4 = hybrid_3_meta1_pred[hist4];
	BOOL choice4 = (counter4 < HYBRID_3_META1_MID) ? 0 : 1; // 0 = SAg, 1 = GAg

	UINT64 hist5 = hybrid_3_meta2_ghr & HYBRID_3_GHR2_MASK;
	UINT64 counter5 = hybrid_3_meta2_pred[hist5];
	BOOL choice5 = (counter5 < HYBRID_3_META2_MID) ? 0 : 1; // 0 = GAg, 1 = gshare

	UINT64 hist6 = hybrid_3_meta3_ghr & HYBRID_3_GHR3_MASK;
	UINT64 counter6 = hybrid_3_meta3_pred[hist6];
	BOOL choice6 = (counter6 < HYBRID_3_META3_MID) ? 0 : 1; // 0 = SAg, 1 = gshare

	BOOL prediction = (choice4) ? ((choice5) ? (prediction3) : (prediction2)) : ((choice6) ? (prediction3) : (prediction1)); 
	Mispred[HYBRID_3].back += taken^prediction;
	
	hybrid_3_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_3_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_3_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_3_SAg_BHT_MASK);
	hybrid_3_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_3_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_3_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_3_GAg_GHR_MASK);
	hybrid_3_gshare_pht[hash3] =  (taken) ? ((counter3 == HYBRID_3_GSHARE_MAX) ? counter3 : counter3+1) : ((counter3 == 0) ? 0 : counter3-1);
	hybrid_3_gshare_ghr = ((hist3 << 1 | (taken)) & HYBRID_3_GSHARE_GHR_MASK);
	BOOL s = (taken == prediction1);
	BOOL g = (taken == prediction2);
	BOOL r = (taken == prediction3); 
	hybrid_3_meta1_pred[hist4] = (g & !s) ? ((counter4 == HYBRID_3_META1_MAX) ? counter4 : counter4+1) : ((s & !g) ? ((counter4 == 0) ? 0 : counter4-1) : counter4);
	hybrid_3_meta1_ghr = ((hist4 << 1 | (taken)) & HYBRID_3_GHR1_MASK);
	hybrid_3_meta2_pred[hist5] = (r & !g) ? ((counter5 == HYBRID_3_META2_MAX) ? counter5 : counter5+1) : ((g & !r) ? ((counter5 == 0) ? 0 : counter5-1) : counter5);
	hybrid_3_meta2_ghr = ((hist5 << 1 | (taken)) & HYBRID_3_GHR2_MASK);
	hybrid_3_meta3_pred[hist6] = (r & !s) ? ((counter6 == HYBRID_3_META3_MAX) ? counter6 : counter6+1) : ((s & !r) ? ((counter6 == 0) ? 0 : counter6-1) : counter6);
	hybrid_3_meta3_ghr = ((hist6 << 1 | (taken)) & HYBRID_3_GHR3_MASK);
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
				UINT64 pc = (UINT64)ins_addr;
                if(taken_addr > ins_addr){
                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdAccess, IARG_END); 
                    
                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_FNBT, IARG_BRANCH_TAKEN, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_bimodal, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_SAg, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_GAg, IARG_BRANCH_TAKEN, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_gshare, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_hybrid_2, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_hybrid_3_maj, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
				
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) FwdMispred_hybrid_3, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
				
				}
                else{
                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdAccess, IARG_END); 

                    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
			        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_FNBT, IARG_BRANCH_TAKEN, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_bimodal, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_SAg, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
				
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_GAg, IARG_BRANCH_TAKEN, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_gshare, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);

					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_hybrid_2, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
				
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_hybrid_3_maj, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
					
					INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
					INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BkdMispred_hybrid_3, IARG_BRANCH_TAKEN, IARG_UINT64, pc, IARG_END);
				
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
    exit(0);
}

int main(int argc, char *argv[])
{
	// Initialize PIN library. Print help message if -h(elp) is specified
	// in the command line or the command line is invalid 

	startTime = chrono::high_resolution_clock::now();

	if (PIN_Init(argc, argv))
		return Usage();

	/* Set number of instructions to fast forward and simulate */
	fastForwardIns = KnobFastForward.Value() * BILLION;
	maxIns = fastForwardIns + BILLION;

	string fileName = KnobOutputFile.Value();

	if (!fileName.empty())
		out = new std::ofstream(fileName.c_str());

	MEMSET0(bimodal_pht);
	MEMSET0(SAg_bht);
	MEMSET0(SAg_pht);
	GAg_ghr = 0;
	MEMSET0(GAg_pht);
	gshare_ghr = 0;
	MEMSET0(gshare_pht);
	MEMSET0(hybrid_2_SAg_bht);
	MEMSET0(hybrid_2_SAg_pht);
	hybrid_2_GAg_ghr = 0;
	MEMSET0(hybrid_2_GAg_pht);
	hybrid_2_meta_ghr = 0;
	MEMSET0(hybrid_2_meta_pred);
	MEMSET0(hybrid_3_maj_SAg_bht);
	MEMSET0(hybrid_3_maj_SAg_pht);
	hybrid_3_maj_GAg_ghr = 0;
	MEMSET0(hybrid_3_maj_GAg_pht);
	hybrid_3_maj_gshare_ghr = 0;
	MEMSET0(hybrid_3_maj_gshare_pht);
	MEMSET0(hybrid_3_SAg_bht);
	MEMSET0(hybrid_3_SAg_pht);
	hybrid_3_GAg_ghr = 0;
	MEMSET0(hybrid_3_GAg_pht);
	hybrid_3_gshare_ghr = 0;
	MEMSET0(hybrid_3_gshare_pht);
	hybrid_3_meta1_ghr = 0;
	MEMSET0(hybrid_3_meta1_pred);
	hybrid_3_meta2_ghr = 0;
	MEMSET0(hybrid_3_meta2_pred);
	hybrid_3_meta3_ghr = 0;
	MEMSET0(hybrid_3_meta3_pred);

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