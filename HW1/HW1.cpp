#include "pin.H"
#include <iostream>
#include <fstream>
using namespace std;

UINT64 fast_forward_count = 43000000000;	// Should be a command line input to your PIN tool
UINT64 insCount = 0;    // number of dynamically executed instructions
UINT64 bblCount = 0;    // number of dynamically executed basic blocks
UINT64 threadCount = 0; // total number of threads, including main thread
UINT64 num_ins = 0;    
UINT64 num_loads = 0;
UINT64 num_stores = 0;
UINT64 num_nops = 0;
UINT64 num_direct_calls = 0;
UINT64 num_indirect_calls = 0;
UINT64 num_returns = 0;
UINT64 num_uncond_branches = 0;
UINT64 num_cond_branches = 0;
UINT64 num_logical_ops = 0;
UINT64 num_rotate_shifts = 0;
UINT64 num_flag_ops = 0;
UINT64 num_vec_ins = 0;
UINT64 num_cond_moves = 0;
UINT64 num_mmx_sse_ins = 0;
UINT64 num_sys_calls = 0;
UINT64 num_flops = 0;
UINT64 num_others = 0;

std::ostream *out = &cerr;

KNOB<BOOL> KnobCount(KNOB_MODE_WRITEONCE, "pintool", "count", "1", "count instructions, basic blocks and threads in the application");

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "HW1.out", "specify output file name");

INT32 Usage()
{
    cerr << "This tool prints out the number of dynamically executed " << endl
         << "instructions, basic blocks and threads in the application." << endl
         << endl;

    cerr << KNOB_BASE::StringKnobSummary() << endl;

    return -1;
}

VOID count_ins() { insCount++; }

VOID count_loads(UINT32 x) {num_loads+=x; }

VOID count_stores(UINT32 x) {num_stores+=x;}

VOID count_nops() {num_nops++;}

VOID count_direct_calls() {num_direct_calls++;}

VOID count_indirect_calls() {num_indirect_calls++;}

VOID count_returns() { num_returns++; }

VOID count_uncond_branches() { num_uncond_branches++; }

VOID count_cond_branches() { num_cond_branches++; }

VOID count_logical_ops() { num_logical_ops++; }

VOID count_rotate_shifts() { num_rotate_shifts++; }

VOID count_flag_ops() { num_flag_ops++; }

VOID count_vec_ins() { num_vec_ins++; }

VOID count_cond_moves() { num_cond_moves++; }

VOID count_mmx_sse_ins() { num_mmx_sse_ins++; }

VOID count_sys_calls() { num_sys_calls++; }

VOID count_flops() { num_flops++; }

VOID count_others() { num_others++; }

ADDRINT Terminate(void) { return (insCount >= fast_forward_count + 1000000000); }

ADDRINT FastForward (void) { return ((insCount >= fast_forward_count) && (insCount < fast_forward_count + 1000000000)); }

void MyExitRoutine() {
	UINT64 total_ins = num_loads + num_stores + num_nops + num_direct_calls +
                       num_indirect_calls + num_returns + num_uncond_branches +
                       num_cond_branches + num_logical_ops + num_rotate_shifts +
                       num_flag_ops + num_vec_ins + num_cond_moves +
                       num_mmx_sse_ins + num_sys_calls + num_flops + num_others;

    *out << "===============================================" << endl;
    *out << "Part A Analysis Results: " << endl;
    // *out << "Total Micro Instructions: " << total_ins << endl;
    *out << "Total Instructions: " << insCount << endl;
    *out << "Total Instructions after fast forward: " << insCount - fast_forward_count << endl;
    *out << "===============================================" << endl;

    // Formatting the table header
    *out << left << setw(30) << "Instruction Type" << setw(15) << "Count" << setw(15) << "Percentage" << endl;
    *out << "------------------------------------------------------------" << endl;

    // Lambda function to print rows neatly
    auto print_row = [&](const string &name, UINT64 count) {
        double percentage = (total_ins == 0) ? 0.0 : (100.0 * count / total_ins);
        *out << left << setw(30) << name << setw(15) << count << setw(14) << fixed << setprecision(2) << percentage << "%" << endl;
    };

    // Printing each instruction type
    print_row("Loads", num_loads);
    print_row("Stores", num_stores);
    print_row("NOPs", num_nops);
    print_row("Direct Calls", num_direct_calls);
    print_row("Indirect Calls", num_indirect_calls);
    print_row("Returns", num_returns);
    print_row("Unconditional Branches", num_uncond_branches);
    print_row("Conditional Branches", num_cond_branches);
    print_row("Logical Operations", num_logical_ops);
    print_row("Rotate & Shift", num_rotate_shifts);
    print_row("Flag Operations", num_flag_ops);
    print_row("Vector Instructions", num_vec_ins);
    print_row("Conditional Moves", num_cond_moves);
    print_row("MMX & SSE Instructions", num_mmx_sse_ins);
    print_row("System Calls", num_sys_calls);
    print_row("Floating-Point", num_flops);
    print_row("Others", num_others);

    *out << "===============================================" << endl;
    *out << "Part B Analysis Results: " << endl;
    *out << "CPI = " << (69.0*(num_loads + num_stores) + total_ins)/(total_ins) << endl;
    *out << "===============================================" << endl;
	exit(0);
}

VOID Instrumentation(INS ins, VOID *v){
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, MyExitRoutine, IARG_END);
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_ins, IARG_END);
    UINT32 memOperands = INS_MemoryOperandCount(ins);
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        UINT32 memSize = INS_MemoryOperandSize(ins, memOp);
        UINT32 num = (memSize%4 == 0) ? memSize/4 : (memSize/4)+1;
        if (INS_MemoryOperandIsRead(ins, memOp)){
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_loads, IARG_UINT32, num, IARG_END);
        }
        if (INS_MemoryOperandIsWritten(ins, memOp)){  
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_stores, IARG_UINT32, num, IARG_END);
        }
    }

    if (INS_Category(ins) == XED_CATEGORY_NOP) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_nops, IARG_END);
    }
        
    if (INS_Category(ins) == XED_CATEGORY_CALL) {
        if (INS_IsDirectCall(ins)) {
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_direct_calls, IARG_END);
        }
        else {
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_indirect_calls, IARG_END);
        }
    }

    if (INS_Category(ins) == XED_CATEGORY_RET) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_returns, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_UNCOND_BR) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_uncond_branches, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_COND_BR) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_cond_branches, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_LOGICAL) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_logical_ops, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_ROTATE || INS_Category(ins) == XED_CATEGORY_SHIFT) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_rotate_shifts, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_FLAGOP) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_flag_ops, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_AVX || INS_Category(ins) == XED_CATEGORY_AVX2 || 
        INS_Category(ins) == XED_CATEGORY_AVX2GATHER || INS_Category(ins) == XED_CATEGORY_AVX512) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_vec_ins, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_CMOV) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_cond_moves, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_MMX || INS_Category(ins) == XED_CATEGORY_SSE) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_mmx_sse_ins, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_SYSCALL) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_sys_calls, IARG_END);
    }

    if (INS_Category(ins) == XED_CATEGORY_X87_ALU) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_flops, IARG_END);
    }

    else {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_others, IARG_END);
    }

    // INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_ins, IARG_UINT32, count, IARG_END);

}

VOID Fini(INT32 code, VOID *v)
{
    MyExitRoutine();
    cout << "This should not be printed :(" << endl;
    
}



int main(int argc, char *argv[])
{   
    // if(argc != 2){
    //     cerr << "2 arguments expected. Got " << argc << endl;
    //     return 0;
    // }
    for(int i=0; i<argc; i++){
        cout << argv[i] << endl;
    }
    // fast_forward_count = stoll(argv[1]);

    if (PIN_Init(argc, argv))
    {
        return Usage();
    }

    string fileName = KnobOutputFile.Value();

    if (!fileName.empty())
    {
        out = new std::ofstream(fileName.c_str());
    }

    if (KnobCount)
    {
        // Register Instruction to be called to instrument instructions
        INS_AddInstrumentFunction(Instrumentation, 0);

        // Register function to be called when the application exits
        PIN_AddFiniFunction(Fini, 0);
    }

    cerr << "===============================================" << endl;
    cerr << "Results of instrumenting the given binary:" << endl;
    if (!KnobOutputFile.Value().empty())
    {
        cerr << "See file " << KnobOutputFile.Value() << " for analysis results" << endl;
    }
    cerr << "===============================================" << endl;

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}