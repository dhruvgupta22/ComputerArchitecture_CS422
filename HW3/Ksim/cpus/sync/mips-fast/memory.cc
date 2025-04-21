#include "memory.h"

Memory::Memory(Mipc *mc)
{
   _mc = mc;
}

Memory::~Memory(void) {}

void Memory::MainLoop(void)
{
   Bool memControl;

   while (1)
   {
      AWAIT_P_PHI0; // @posedge
      _mc->_ex_mem_r = _mc->_ex_mem_w;
      memControl = _mc->_ex_mem_r._memControl;
      AWAIT_P_PHI1; // @negedge
      if (memControl)
      {
         _mc->_ex_mem_r._memOp(_mc);
#if MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, _mc->_memory_addr_reg, _mc->_ex_mem_r._ins);
#endif
      }
      else
      {
#if MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, _mc->_ex_mem_r._ins);
#endif
      }
      _mc->_mem_wb_w = _mc->_ex_mem_r;
   }
}
