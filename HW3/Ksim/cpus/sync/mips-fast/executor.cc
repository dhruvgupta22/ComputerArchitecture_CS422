#include "executor.h"

Exe::Exe (Mipc *mc)
{
   _mc = mc;
}

Exe::~Exe (void) {}

void Exe::pick_bypass_value(){
   #if BYPASS_MEM_EX_ENABLED
   if(_mc->_id_ex_r._bypass1 == BYPASS_MEM_EX){
      _mc->_id_ex_r._decodedSRC1 = _mc->_mem_wb_w._opResultLo;
   }
   if(_mc->_id_ex_r._bypass2 == BYPASS_MEM_EX){
      _mc->_id_ex_r._decodedSRC2 = _mc->_mem_wb_w._opResultLo;
   }
   if(_mc->_id_ex_r._bypass3 == BYPASS_MEM_EX){
      _mc->_id_ex_r._bypass_val = _mc->_mem_wb_w._opResultLo;
   }
   if(_mc->_id_ex_r._bypass1 == (BYPASS_MEM_EX | TAKE_FROM_HI)){
      _mc->_id_ex_r._decodedSRC1 = _mc->_mem_wb_w._opResultHi;
   }
   #endif

   #if BYPASS_EX_EX_ENABLED
   if(_mc->_id_ex_r._bypass1 == BYPASS_EX_EX){
      _mc->_id_ex_r._decodedSRC1 = _mc->_ex_mem_w._opResultLo;
      // #if MIPC_DEBUG
      //    fprintf(_mc->_debugLog, "<%llu> Executed ins %#x in ex-ex bypass. if-id-w %u id-ex-w %u ex-mem-w %u mem-wb-w %u if-id-r %u id-ex-r %u ex-mem-r %u mem-wb-r %u dSRC1 %u\n", SIM_TIME, _mc->_id_ex_r._ins, _mc->_if_id_w._opResultLo, _mc->_id_ex_w._opResultLo, _mc->_ex_mem_w._opResultLo, _mc->_mem_wb_w._opResultLo, _mc->_if_id_r._opResultLo, _mc->_id_ex_r._opResultLo, _mc->_ex_mem_r._opResultLo, _mc->_mem_wb_r._opResultLo, _mc->_id_ex_r._decodedSRC1);
      // #endif
   }
   if(_mc->_id_ex_r._bypass2 == BYPASS_EX_EX){
      _mc->_id_ex_r._decodedSRC2 = _mc->_ex_mem_w._opResultLo;
   }
   if(_mc->_id_ex_r._bypass3 == BYPASS_EX_EX){
      _mc->_id_ex_r._bypass_val = _mc->_ex_mem_w._opResultLo;
   }
   if(_mc->_id_ex_r._bypass1 == (BYPASS_EX_EX | TAKE_FROM_HI)){
      _mc->_id_ex_r._decodedSRC1 = _mc->_ex_mem_w._opResultHi;
   }
   #endif
}

void
Exe::MainLoop (void)
{
    unsigned int ins;
    Bool isSyscall, isIllegalOp;

    while (1) {
        AWAIT_P_PHI0;	// @posedge
        /* Work in positive half cycle */
        _mc->_id_ex_r = _mc->_id_ex_w;
        ins = _mc->_id_ex_r._ins;
        isSyscall = _mc->_id_ex_r._isSyscall;
        isIllegalOp = _mc->_id_ex_r._isIllegalOp;
        pick_bypass_value();
        #if MIPC_DEBUG
        fprintf(_mc->_debugLog, "<%llu> Executing 1.0 ins %#x, pc = %#x, dSRC1: %d, regSRC1: %d, dSRC2: %d, regSRC2: %d, dDST: %d, writeREG: %d, writeFREG: %d, hiWPort: %d, loWPort: %d, memControl: %d, decodedShiftAmt: %d, btgt: %u, btaken: %d, isSyscall: %d, isIllegalOp: %d, branchOffset: %d, opControl: %p, memOp: %p, opResultHi: %u, opResultLo: %u\n",
           SIM_TIME, _mc->_id_ex_r._ins, _mc->_id_ex_r._pc,
           _mc->_id_ex_r._decodedSRC1, _mc->_id_ex_r._regSRC1, _mc->_id_ex_r._decodedSRC2, _mc->_id_ex_r._regSRC2, 
           _mc->_id_ex_r._decodedDST, _mc->_id_ex_r._writeREG, _mc->_id_ex_r._writeFREG, 
           _mc->_id_ex_r._hiWPort, _mc->_id_ex_r._loWPort, _mc->_id_ex_r._memControl, 
           _mc->_id_ex_r._decodedShiftAmt, _mc->_id_ex_r._btgt, _mc->_id_ex_r._btaken, 
           _mc->_id_ex_r._isSyscall, _mc->_id_ex_r._isIllegalOp, 
           _mc->_id_ex_r._branchOffset, _mc->_id_ex_r._opControl, _mc->_id_ex_r._memOp, 
           _mc->_id_ex_r._opResultHi, _mc->_id_ex_r._opResultLo); 
       #endif
		  if (!isSyscall && !isIllegalOp) {
            if(_mc->_id_ex_r._opControl != NULL){
               _mc->_id_ex_r._opControl(_mc,ins);
            }
#if MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, ins);
#endif
         }
         else if (isSyscall) {
#if MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, ins);
#endif
         }
         else {
#if MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, ins, _mc->_pc);
#endif
         }

         #if MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Executing ins %#x, pc = %#x, dSRC1: %d, regSRC1: %d, dSRC2: %d, regSRC2: %d, dDST: %d, writeREG: %d, writeFREG: %d, hiWPort: %d, loWPort: %d, memControl: %d, decodedShiftAmt: %d, btgt: %u, btaken: %d, isSyscall: %d, isIllegalOp: %d, branchOffset: %d, opControl: %p, memOp: %p, opResultHi: %u, opResultLo: %u\n",
            SIM_TIME, _mc->_id_ex_r._ins, _mc->_id_ex_r._pc,
            _mc->_id_ex_r._decodedSRC1, _mc->_id_ex_r._regSRC1, _mc->_id_ex_r._decodedSRC2, _mc->_id_ex_r._regSRC2, 
            _mc->_id_ex_r._decodedDST, _mc->_id_ex_r._writeREG, _mc->_id_ex_r._writeFREG, 
            _mc->_id_ex_r._hiWPort, _mc->_id_ex_r._loWPort, _mc->_id_ex_r._memControl, 
            _mc->_id_ex_r._decodedShiftAmt, _mc->_id_ex_r._btgt, _mc->_id_ex_r._btaken, 
            _mc->_id_ex_r._isSyscall, _mc->_id_ex_r._isIllegalOp, 
            _mc->_id_ex_r._branchOffset, _mc->_id_ex_r._opControl, _mc->_id_ex_r._memOp, 
            _mc->_id_ex_r._opResultHi, _mc->_id_ex_r._opResultLo); 
        #endif

         if (!isIllegalOp && !isSyscall) {
            if (_mc->_id_ex_r._btaken)
            {
               fprintf(_mc->_debugLog, "<%llu> Branch ins %#x in execution stage with pc = %#x, Btgt %#x\n", SIM_TIME, ins, _mc->_pc, _mc->_id_ex_r._btgt);
               _mc->_pc = _mc->_id_ex_r._btgt;
            }

            #if BRANCH_INTERLOCK_ENABLED
            _mc->_lastbdslot = _mc->_id_ex_r._bdslot;
            #endif

         }
        AWAIT_P_PHI1;	// @negedge
         _mc->_ex_mem_w = _mc->_id_ex_r;

   }
}
