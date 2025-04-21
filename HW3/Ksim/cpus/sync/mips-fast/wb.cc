#include "wb.h"

Writeback::Writeback (Mipc *mc)
{
   _mc = mc;
}

Writeback::~Writeback (void) {}

void
Writeback::MainLoop (void)
{
   unsigned int ins;
   Bool writeReg;
   Bool writeFReg;
   Bool loWPort;
   Bool hiWPort;
   Bool isSyscall;
   Bool isIllegalOp;
   unsigned decodedDST;
   unsigned opResultLo, opResultHi;

   while (1) {
      AWAIT_P_PHI0;	// @posedge
      // Sample the important signals
      _mc->_mem_wb_r = _mc->_mem_wb_w;
      writeReg = _mc->_mem_wb_r._writeREG;
      writeFReg = _mc->_mem_wb_r._writeFREG;
      loWPort = _mc->_mem_wb_r._loWPort;
      hiWPort = _mc->_mem_wb_r._hiWPort;
      decodedDST = _mc->_mem_wb_r._decodedDST;
      opResultLo = _mc->_mem_wb_r._opResultLo;
      opResultHi = _mc->_mem_wb_r._opResultHi;
      isSyscall = _mc->_mem_wb_r._isSyscall;
      isIllegalOp = _mc->_mem_wb_r._isIllegalOp;
      ins = _mc->_mem_wb_r._ins;
         if (isSyscall) {
#if MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> SYSCALL! Trapping to emulation layer at PC %#x\n", SIM_TIME, _mc->_pc);
#endif      
            _mc->_mem_wb_r._opControl(_mc, ins);
            // _mc->_pc =  _mc->_mem_wb_r._pc + 4;
            _mc->_syscall_present = FALSE;
         }
         else if (isIllegalOp) {
            printf("Illegal ins %#x at PC %#x. Terminating simulation!\n", ins, _mc->_pc);
#if MIPC_DEBUG
            fclose(_mc->_debugLog);
#endif
            printf("Register state on termination:\n\n");
            _mc->dumpregs();
            exit(0);
         }
         else {
            if (writeReg) {
               _mc->_gpr[decodedDST] = opResultLo;
#if MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> Writing to reg %u, value: %#x\n", SIM_TIME, decodedDST, opResultLo);
#endif
            }
            else if (writeFReg) {
               _mc->_fpr[(decodedDST)>>1].l[FP_TWIDDLE^((decodedDST)&1)] = opResultLo;
#if MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> Writing to freg %u, value: %#x\n", SIM_TIME, decodedDST>>1, opResultLo);
#endif
            }
            else if (loWPort || hiWPort) {
               if (loWPort) {
                  _mc->_lo = opResultLo;
#if MIPC_DEBUG
                  fprintf(_mc->_debugLog, "<%llu> Writing to Lo, value: %#x\n", SIM_TIME, opResultLo);
#endif
               }
               if (hiWPort) {
                  _mc->_hi = opResultHi;
#if MIPC_DEBUG
                  fprintf(_mc->_debugLog, "<%llu> Writing to Hi, value: %#x\n", SIM_TIME, opResultHi);
#endif
               }
            }
            else{
#if MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> No writeback for ins %#x\n", SIM_TIME, ins);
#endif
            }
         }
         _mc->_gpr[0] = 0;
         AWAIT_P_PHI1;       // @negedge
   }
}
