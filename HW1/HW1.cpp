#include "pin.H"
#include <iostream>
#include <fstream>
#include <set>
using namespace std;

UINT32 fast_forward_count = 0;	// Should be a command line input to your PIN tool
UINT32 insCount = 0;    // number of dynamically executed instructions
UINT32 num_ins = 0;    
UINT32 num_loads = 0;
UINT32 num_stores = 0;
UINT32 num_nops = 0;
UINT32 num_direct_calls = 0;
UINT32 num_indirect_calls = 0;
UINT32 num_returns = 0;
UINT32 num_uncond_branches = 0;
UINT32 num_cond_branches = 0;
UINT32 num_logical_ops = 0;
UINT32 num_rotate_shifts = 0;
UINT32 num_flag_ops = 0;
UINT32 num_vec_ins = 0;
UINT32 num_cond_moves = 0;
UINT32 num_mmx_sse_ins = 0;
UINT32 num_sys_calls = 0;
UINT32 num_flops = 0;
UINT32 num_others = 0;

std::ostream *out = &cerr;
UINT32 total_ins = 0;

set<UINT32> instrAddr;
set<UINT32> dataAddr;
map<UINT32, UINT32> insLength; 
map<UINT32, UINT32> insOp; 
map<UINT32, UINT32> readReg; 
map<UINT32, UINT32> writeReg;
map<UINT32, UINT32> memOp;
map<UINT32, UINT32> memReadOp;
map<UINT32, UINT32> memWriteOp;
UINT32 maxMemBytes = 0;
UINT32 totalMemBytes = 0;
INT32 maxImm = INT32_MIN;
INT32 minImm = INT32_MAX;
INT32 maxDisp = INT32_MIN;
INT32 minDisp = INT32_MAX;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "HW1.out", "specify output file name");
KNOB<UINT32> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "Fast-forward amount");
KNOB<BOOL>   KnobCount(KNOB_MODE_WRITEONCE,  "pintool", "count", "1", "count instructions, basic blocks and threads in the application");

INT32 Usage()
{
    cerr << "This tool prints out the number of dynamically executed " << endl
         << "instructions, basic blocks and threads in the application." << endl
         << endl;

    cerr << KNOB_BASE::StringKnobSummary() << endl;

    return -1;
}

ADDRINT Terminate(void) { return (insCount >= fast_forward_count + 1000000000); }

ADDRINT FastForward (void) { return ((insCount >= fast_forward_count) && (insCount < fast_forward_count + 1000000000)); }

VOID count_ins() { insCount++; }
VOID count_loads(UINT32 x) { num_loads+=x; }
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

VOID ins_footprint(UINT32 startAddr, UINT32 endAddr){
    while(startAddr <= endAddr){
        instrAddr.insert(startAddr);
        startAddr += 32;
    }
}

VOID data_footprint(ADDRINT memAddr, UINT32 memSize){
    UINT32 mAddr = (UINT32) memAddr;
    UINT32 startAddr = (mAddr/32)*32;
    UINT32 endAddr = ((mAddr + memSize)/32)*32;
    while(startAddr <= endAddr){
        dataAddr.insert(startAddr);
        startAddr += 32;
    }
}

VOID ins_distribution(
    UINT32 insSize, 
    UINT32 opCount, 
    UINT32 insReadReg, 
    UINT32 insWriteReg
){
    insLength[insSize]++;
    insOp[opCount]++;
    readReg[insReadReg]++; 
    writeReg[insWriteReg]++;
    
}

VOID mem_distribution(
    UINT32 memOperands, 
    UINT32 numRead, 
    UINT32 numWrite,
    UINT32 totalMemSize
){
    memOp[memOperands]++; 
    memReadOp[numRead]++; 
    memWriteOp[numWrite]++;
    maxMemBytes = max(maxMemBytes, totalMemSize);
    totalMemBytes += totalMemSize;
}

VOID imm_distribution(ADDRINT immValue){
    INT32 imm = (INT32) immValue;
    maxImm = max(maxImm, imm);
    minImm = min(minImm, imm);
}

VOID disp_distribution(ADDRINT dispValue){
    INT32 disp = (INT32) dispValue;
    maxDisp = max(maxDisp, disp);
    minDisp = min(minDisp, disp);
}

void PrintRow(const string &name, UINT32 count){
    double percentage = (total_ins == 0) ? 0.0 : (100.0 * count / total_ins);
    *out << left << setw(30) << name
         << setw(15) << count
         << setw(14) << fixed << setprecision(6) << percentage << "%" << endl;
}

void PrintResults(const std::string &title, const std::map<UINT32, UINT32> &data) {
    *out << title << " Results: " << std::endl;
    for (const auto &entry : data) {
        *out << entry.first << " : " << entry.second << std::endl;
    }
    *out << std::endl;
}

void MyExitRoutine() {
    total_ins = num_loads + num_stores + num_nops + num_direct_calls +
                num_indirect_calls + num_returns + num_uncond_branches +
                num_cond_branches + num_logical_ops + num_rotate_shifts +
                num_flag_ops + num_vec_ins + num_cond_moves +
                num_mmx_sse_ins + num_sys_calls + num_flops + num_others;
    
    UINT32 memInsCount = 0;
    for(const auto &entry : memOp){
        if(entry.first > 0) memInsCount += entry.second;
    }
    double avgMemBytes = (memInsCount == 0) ? 0.0 : ((1.0*totalMemBytes) / memInsCount);
    
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
    // auto PrintRow = [&](const string &name, UINT32 count) {
    //     double percentage = (total_ins == 0) ? 0.0 : (100.0 * count / total_ins);
    //     *out << left << setw(30) << name << setw(15) << count << setw(14) << fixed << setprecision(6) << percentage << "%" << endl;
    // };

    // Printing each instruction type
    PrintRow("Loads", num_loads);
    PrintRow("Stores", num_stores);
    PrintRow("NOPs", num_nops);
    PrintRow("Direct Calls", num_direct_calls);
    PrintRow("Indirect Calls", num_indirect_calls);
    PrintRow("Returns", num_returns);
    PrintRow("Unconditional Branches", num_uncond_branches);
    PrintRow("Conditional Branches", num_cond_branches);
    PrintRow("Logical Operations", num_logical_ops);
    PrintRow("Rotate & Shift", num_rotate_shifts);
    PrintRow("Flag Operations", num_flag_ops);
    PrintRow("Vector Instructions", num_vec_ins);
    PrintRow("Conditional Moves", num_cond_moves);
    PrintRow("MMX & SSE Instructions", num_mmx_sse_ins);
    PrintRow("System Calls", num_sys_calls);
    PrintRow("Floating-Point", num_flops);
    PrintRow("Others", num_others);

    *out << "===============================================" << endl;
    *out << "Part B Analysis Results: " << endl;
    *out << "CPI = " << (69.0*(num_loads + num_stores) + total_ins)/(total_ins) << endl;

    *out << "===============================================" << endl;
    *out << "Part C Analysis Results: " << endl;
    *out << "Instruction Footprint Blocks     = " << instrAddr.size() << endl;
    *out << "Instruction Footprint (in bytes) = " << 32*instrAddr.size() << endl;
    *out << "Data Footprint Blocks            = " << dataAddr.size() << endl;
    *out << "Data Footprint (in bytes)        = " << 32*dataAddr.size() << endl;

    *out << "===============================================" << endl;
    *out << "Part D Analysis Results: " << endl;
    PrintResults("Instruction Size", insLength);
    PrintResults("Memory Instruction Operand", memOp);
    PrintResults("Memory Instruction Read Operand", memReadOp);
    PrintResults("Memory Instruction Write Operand", memWriteOp);
    PrintResults("Instruction Operand", insOp);
    PrintResults("Instruction Register Read Operand", readReg);
    PrintResults("Instruction Register Write Operand", writeReg);
    

    *out << "Maximum number of bytes touched by an instruction : " << maxMemBytes << endl;
    *out << "Average number of bytes touched by an instruction : " << fixed << setprecision(6) << avgMemBytes << endl;
    *out << "Maximum value of immediate : " << maxImm << endl;
    *out << "Minimum value of immediate : " << minImm << endl;
    *out << "Maximum value of displacement used in memory addressing : " << maxDisp << endl;
    *out << "Minimum value of displacement used in memory addressing : " << minDisp << endl;
	exit(0);
}

VOID Instrumentation(INS ins, VOID *v){
    
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, MyExitRoutine, IARG_END);

    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_ins, IARG_END);

    UINT32 startAddr, endAddr;
    UINT32 insAddr = INS_Address(ins);
    UINT32 insSize = INS_Size(ins);
    startAddr = (insAddr/32)*32;
    endAddr = ((insAddr + insSize)/32)*32;
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)ins_footprint, IARG_UINT32, startAddr, IARG_UINT32, endAddr, IARG_END);
    
    UINT32 opCount = INS_OperandCount(ins);
    UINT32 insReadReg = INS_MaxNumRRegs(ins);
    UINT32 insWriteReg = INS_MaxNumWRegs(ins);

    UINT32 memOperands = INS_MemoryOperandCount(ins);
    UINT32 numRead = 0, numWrite = 0;
    UINT32 totalMemSize = 0;
    for (UINT32 memOper = 0; memOper < memOperands; memOper++)
    {
        UINT32 memSize = INS_MemoryOperandSize(ins, memOper);
        UINT32 num = (memSize%4 == 0) ? memSize/4 : (memSize/4)+1;
        if (INS_MemoryOperandIsRead(ins, memOper)) {
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_loads, IARG_UINT32, num, IARG_END);
            numRead++;
        }
        if (INS_MemoryOperandIsWritten(ins, memOper)) {  
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_stores, IARG_UINT32, num, IARG_END);
            numWrite++;
        }
        if (INS_OperandIsMemory(ins, memOper)) {
            ADDRDELTA displacement = INS_OperandMemoryDisplacement(ins, memOper);
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)disp_distribution, IARG_ADDRINT, displacement, IARG_END);
        }
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)data_footprint, IARG_END);
        totalMemSize += memSize;
    }

    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)ins_distribution, 
                       IARG_UINT32, insSize, 
                       IARG_UINT32, opCount, 
                       IARG_UINT32, insReadReg, 
                       IARG_UINT32, insWriteReg, 
                       IARG_END);

    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
    INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)mem_distribution,  
                        IARG_UINT32, memOperands, 
                        IARG_UINT32, numRead, 
                        IARG_UINT32, numWrite,
                        IARG_UINT32, totalMemSize,
                        IARG_END);

    if (INS_Category(ins) == XED_CATEGORY_NOP) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_nops, IARG_END);
    }
        
    else if (INS_Category(ins) == XED_CATEGORY_CALL) {
        if (INS_IsDirectCall(ins)) {
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_direct_calls, IARG_END);
        }
        else {
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_indirect_calls, IARG_END);
        }
    }

    else if (INS_Category(ins) == XED_CATEGORY_RET) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_returns, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_UNCOND_BR) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_uncond_branches, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_COND_BR) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_cond_branches, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_LOGICAL) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_logical_ops, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_ROTATE || INS_Category(ins) == XED_CATEGORY_SHIFT) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_rotate_shifts, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_FLAGOP) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_flag_ops, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_AVX || INS_Category(ins) == XED_CATEGORY_AVX2 || 
        INS_Category(ins) == XED_CATEGORY_AVX2GATHER || INS_Category(ins) == XED_CATEGORY_AVX512) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_vec_ins, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_CMOV) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_cond_moves, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_MMX || INS_Category(ins) == XED_CATEGORY_SSE) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_mmx_sse_ins, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_SYSCALL) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_sys_calls, IARG_END);
    }

    else if (INS_Category(ins) == XED_CATEGORY_X87_ALU) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_flops, IARG_END);
    }

    else {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count_others, IARG_END);
    }

    UINT32 operandCount = INS_OperandCount(ins);
    for (UINT32 opIdx = 0; opIdx < operandCount; opIdx++) {
        if (INS_OperandIsImmediate(ins, opIdx)) {
            INT32 immValue = INS_OperandImmediate(ins, opIdx);
            INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
            INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)imm_distribution, IARG_ADDRINT, immValue,IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    MyExitRoutine();
    cout << "This should not be printed :(" << endl;
    
}



int main(int argc, char *argv[])
{   
    if (PIN_Init(argc, argv))
    {
        return Usage();
    }
    
    string fileName = KnobOutputFile.Value();
    
    if (!fileName.empty())
    {
        out = new std::ofstream(fileName.c_str());
    }
    
    fast_forward_count = KnobFastForward.Value();
    // printf("ff = %ld\n", fast_forward_count);
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