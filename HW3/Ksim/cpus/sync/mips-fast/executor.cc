#include "executor.h"

Exe::Exe (Mipc *mc)
{
   _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
    unsigned int ins;
    Bool isSyscall, isIllegalOp;

    while (1) {
        AWAIT_P_PHI0;	// @posedge
        /* Work in positive half cycle */
        _mc->_id_ex_r = _mc->id_ex_w;
        ins = _mc->_id_ex_r._ins;
        isSyscall = _mc->_id_ex_r._isSyscall;
        isIllegalOp = _mc->_id_ex_r._isIllegalOp;
        
		  if (!isSyscall && !isIllegalOp) {
            if(_mc->_id_ex_r._opControl != NULL)
               _mc->_id_ex_r._opControl(_mc,ins);
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, ins);
#endif
         }
         else if (isSyscall) {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, ins);
#endif
         }
         else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, ins, _mc->_pc);
#endif
         }

         if (!isIllegalOp && !isSyscall) {
            if (_mc->_id_ex_r._lastbdslot && _mc->_id_ex_r._btaken)
            {
               _mc->_pc = _mc->_id_ex_r._btgt;
            }
            _mc->_id_ex_r._lastbdslot = _mc->_id_ex_r._bdslot;
         }
        AWAIT_P_PHI1;	// @negedge
         _mc->_ex_mem_w = _mc->_id_ex_r;
   }
}
