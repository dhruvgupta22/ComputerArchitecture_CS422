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

#define BTB1_INDEX_BITS 7
#define BTB1_NUM_SETS (1 << (BTB1_INDEX_BITS))
#define BTB1_NUM_WAYS 4

#define MEMSET0(arr) do { \
	memset(arr, 0, sizeof(arr)); \
} while (0)

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
	UINT64 valid;
	UINT64 tag;
	UINT64 lru_state;
	UINT64 target;	
} BTB_Entry;

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

BTB_Entry btb1[BTB1_NUM_SETS][BTB1_NUM_WAYS];


ADDRINT fastForwardDone = 0;
UINT64 icount = 0; //number of dynamically executed instructions

UINT64 fastForwardIns; // number of instruction to fast forward
UINT64 maxIns; // maximum number of instructions to simulate

UINT64 dp_access[2];
UINT64 Mispred[DP_COUNT][2];
UINT64 btb1_access = 0;
UINT64 btb1_miss = 0;
UINT64 btb1_mispred = 0;

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
		*out << pred_names[i] << " : Accesses " << (dp_access[0] + dp_access[1]);
		*out << ", Mispredictions " << (Mispred[i][0] + Mispred[i][1]) ;
		*out << " (" << (1.0*Mispred[i][0] + Mispred[i][1])/(1.0*dp_access[0] + dp_access[1]) << ")";
		*out << ", Forward Branches " << dp_access[0];
		*out << ", Forward Mispredictions " << Mispred[i][0];
		*out << " (" << (1.0*Mispred[i][0])/(1.0*dp_access[0]) << ")";
		*out << ", Backward Branches " << dp_access[1];
		*out << ", Backward Mispredictions " << Mispred[i][1];
		*out << " (" << (1.0*Mispred[i][1])/(1.0*dp_access[1]) << ")";
		*out << endl;
	}
	// BTB1 : Accesses 28117852, Misses 3340 (0.000118786), Mispredictions 9806418 (0.348761)
	*out << endl;
	*out << "===============================================" << endl;
	*out << "Branch Target Predictors" << endl;
	*out << "BTB1 : Accesses "<< btb1_access << ", Misses "<< btb1_miss<< " (" << (double)(btb1_miss)/(btb1_access)<< "), Mispredictions "<< btb1_mispred <<" (" << (double)(btb1_mispred)/(btb1_access)<<")"<<endl;
	// *out << "BTB2 : Accesses "<< btb2_access << ", Misses "<< btb2_miss<< " (" << (double)(btb2_miss)/(btb2_access)<< "), Mispredictions "<< btb2_mispred <<" (" << (double)(btb2_mispred)/(btb2_access)<<")"<<endl;
	*out << endl;
	*out << "===============================================" << endl;
	endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = endTime - startTime;
    *out << "Time taken : " << elapsed.count() << " seconds." << endl;
	exit(0);
}

VOID DPAccess(ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	dp_access[dir]++;
}

VOID fnbt(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
    Mispred[FNBT][dir] += taken^dir;
}

VOID bimodal(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 pc = (UINT64)ins_addr;
	UINT64 hpc = pc%BIMODAL_ROW;
	UINT64 counter = bimodal_pht[hpc];
	BOOL prediction = (counter < BIMODAL_MID) ? 0 : 1;
	Mispred[BIMODAL][dir] += taken^prediction;
	bimodal_pht[hpc] = (taken) ? ((counter == BIMODAL_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
}

VOID sag(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 pc = (UINT64)ins_addr;
	UINT64 hpc = pc%SAg_BHT_ROW;
	UINT64 hist = SAg_bht[hpc];
	UINT64 counter = SAg_pht[hist];
	BOOL prediction = (counter < SAg_MID) ? 0 : 1;
	Mispred[SAg][dir] += taken^prediction;
	SAg_pht[hist] =  (taken) ? ((counter == SAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	SAg_bht[hpc] = ((hist << 1 | (taken)) & SAg_BHT_MASK);
}

VOID gag(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 hist = GAg_ghr & GAg_GHR_MASK;
	UINT64 counter = GAg_pht[hist];
	BOOL prediction = (counter < GAg_MID) ? 0 : 1;
	Mispred[GAg][dir] += taken^prediction;
	GAg_pht[hist] =  (taken) ? ((counter == GAg_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	GAg_ghr = ((hist << 1 | (taken)) & GAg_GHR_MASK);
}

VOID gshare(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 pc = (UINT64)ins_addr;
	UINT64 hist = gshare_ghr & GSHARE_GHR_MASK;
	UINT64 hash = (hist ^ pc) & GSHARE_GHR_MASK;
	UINT64 counter = gshare_pht[hash];
	BOOL prediction = (counter < GSHARE_MID) ? 0 : 1;
	Mispred[GSHARE][dir] += taken^prediction;
	gshare_pht[hash] =  (taken) ? ((counter == GSHARE_MAX) ? counter : counter+1) : ((counter == 0) ? 0 : counter-1);
	gshare_ghr = ((hist << 1 | (taken)) & GSHARE_GHR_MASK);
}

VOID hybrid_2(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 pc = (UINT64)ins_addr;
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
	
	if(choice == 0) Mispred[HYBRID_2][dir] += taken^prediction1;
	else Mispred[HYBRID_2][dir] += taken^prediction2;
	
	hybrid_2_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_2_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_2_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_2_SAg_BHT_MASK);
	hybrid_2_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_2_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_2_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_2_GAg_GHR_MASK);
	BOOL s = (taken == prediction1);
	BOOL g = (taken == prediction2);
	hybrid_2_meta_pred[hist] = (g & !s) ? ((counter == HYBRID_2_META_MAX) ? counter : counter+1) : ((s & !g) ? ((counter == 0) ? 0 : counter-1) : counter);
	hybrid_2_meta_ghr = ((hist << 1 | (taken)) & HYBRID_2_GHR_MASK);
}

VOID hybrid_3_maj(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 pc = (UINT64)ins_addr;
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

	Mispred[HYBRID_3_MAJ][dir] += taken^prediction;

	hybrid_3_maj_SAg_pht[hist1] =  (taken) ? ((counter1 == HYBRID_3_MAJ_SAg_MAX) ? counter1 : counter1+1) : ((counter1 == 0) ? 0 : counter1-1);
	hybrid_3_maj_SAg_bht[hpc1] = ((hist1 << 1 | (taken)) & HYBRID_3_MAJ_SAg_BHT_MASK);
	hybrid_3_maj_GAg_pht[hist2] =  (taken) ? ((counter2 == HYBRID_3_MAJ_GAg_MAX) ? counter2 : counter2+1) : ((counter2 == 0) ? 0 : counter2-1);
	hybrid_3_maj_GAg_ghr = ((hist2 << 1 | (taken)) & HYBRID_3_MAJ_GAg_GHR_MASK);
	hybrid_3_maj_gshare_pht[hash3] =  (taken) ? ((counter3 == HYBRID_3_MAJ_GSHARE_MAX) ? counter3 : counter3+1) : ((counter3 == 0) ? 0 : counter3-1);
	hybrid_3_maj_gshare_ghr = ((hist3 << 1 | (taken)) & HYBRID_3_MAJ_GSHARE_GHR_MASK);
}

VOID hybrid_3(BOOL taken, ADDRINT ins_addr, ADDRINT taken_addr){
	UINT64 dir = (taken_addr > ins_addr) ? 0 : 1; // 0 -> forward, 1 -> backward
	UINT64 pc = (UINT64)ins_addr;
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
	Mispred[HYBRID_3][dir] += taken^prediction;
	
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

VOID BTB1(ADDRINT taken_addr, UINT64 pc, UINT64 next_pc){
	btb1_access++;
	UINT64 set = pc&(BTB1_NUM_SETS-1);
	UINT64 tag = pc >> BTB1_INDEX_BITS;
	int hit = -1;
	for(int i = 0; i < BTB1_NUM_WAYS; i++){
		if(btb1[set][i].valid == 1 && btb1[set][i].tag == tag){ 
			hit = i; 
			break;
		}
	}
	UINT64 prediction = (hit != -1) ? btb1[set][hit].target : next_pc;

	if(hit == -1) btb1_miss++;

	if(prediction != (UINT64)taken_addr){
		btb1_mispred++;

		if(hit == -1){
			int idx = -1;
			for(int i = 0; i < BTB1_NUM_WAYS; i++){
				if(btb1[set][i].valid == 0){
					idx = i;
					break;
				}
				else if(btb1[set][i].lru_state == 1){
					idx = i;
				}
			}
			assert(idx != -1);
			btb1[set][idx].valid = 1;
			btb1[set][idx].tag = tag;
			btb1[set][idx].lru_state = BTB1_NUM_WAYS;
			btb1[set][idx].target = taken_addr;
	
			for(int j = 0; j < BTB1_NUM_WAYS; j++){
				if(j != idx && btb1[set][j].valid != 0){
					btb1[set][j].lru_state--;
				}
			}
		}
		else{
			if((UINT64)taken_addr == next_pc){
				UINT64 lru = btb1[set][hit].lru_state;
				for(int i=0; i<BTB1_NUM_WAYS; i++){
					if(btb1[set][i].lru_state < lru) btb1[set][i].lru_state++;
				}
				btb1[set][hit].valid = 0;
			}
			else{
				btb1[set][hit].target = taken_addr;
				UINT64 lru = btb1[set][hit].lru_state;
				for(int i=0; i<BTB1_NUM_WAYS; i++){
					if(btb1[set][i].lru_state > lru) btb1[set][i].lru_state--;
				}
				btb1[set][hit].lru_state = BTB1_NUM_WAYS;
			}
		}

	}
	else{
		if(hit != -1){
			UINT64 lru = btb1[set][hit].lru_state;
			for(int i=0; i<BTB1_NUM_WAYS; i++){
				if(btb1[set][i].lru_state > lru) btb1[set][i].lru_state--;
			}
			btb1[set][hit].lru_state = BTB1_NUM_WAYS;
		}
	}
}

VOID BTB2(ADDRINT taken_addr, UINT64 pc, UINT64 next_pc){

}

/* Instruction instrumentation routine */
VOID Trace(TRACE trace, VOID *v)
{
	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
		BBL_InsertIfCall(bbl, IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
        BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR) StatDump, IARG_END);

		BBL_InsertIfCall(bbl, IPOINT_BEFORE, (AFUNPTR) FastForward, IARG_END);
		BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR) FastForwardDone, IARG_END);

		for (INS ins = BBL_InsHead(bbl); ; ins = INS_Next(ins)) {

			/* Calculate instruction type */
			ADDRINT ins_addr = INS_Address(ins);
			UINT64 pc = (UINT64)ins_addr;
            if(INS_Category(ins) == XED_CATEGORY_COND_BR){
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) DPAccess, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END); 
				
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) fnbt, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);

				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) bimodal, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
				
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) sag, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);

				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) gag, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
				
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) gshare, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);

				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) hybrid_2, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
				
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) hybrid_3_maj, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
			
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) hybrid_3, IARG_BRANCH_TAKEN, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
            }
			else if(INS_IsIndirectControlFlow(ins)){
				UINT64 next_pc = (UINT64) INS_Address(INS_Next(ins));
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BTB1, IARG_BRANCH_TARGET_ADDR, IARG_UINT64, pc, IARG_UINT64, next_pc, IARG_END); 

				// INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				// INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) BTB2, IARG_END); 
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

	MEMSET0(dp_access);
	MEMSET0(Mispred);
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

	MEMSET0(btb1);
	// MEMSET0(btb2);

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