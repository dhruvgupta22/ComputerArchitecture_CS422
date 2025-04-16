#include "decode.h"

Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}

void
Decode::MainLoop (void)
{
    unsigned int ins;
    while (1) {
        AWAIT_P_PHI0;	// @posedge
        /* Do Nothing in first half*/
        _mc->_if_id_r = _mc->if_id_w;
        ins = _mc->_if_id_r._ins;
       
        AWAIT_P_PHI1;	// @negedge
        /* Work in second half */
        _mc->Dec(ins);
#ifdef MIPC_DEBUG
        fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
#endif
		  _id_ex_w = _if_id_r;
    }
}
