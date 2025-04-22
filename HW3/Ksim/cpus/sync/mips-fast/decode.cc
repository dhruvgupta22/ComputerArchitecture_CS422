#include "decode.h"

Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}

bool Decode::checkDataHazard()
{
   unsigned int src1 = _mc->_if_id_r._regSRC1;
   unsigned int src2 = _mc->_if_id_r._regSRC2;
   Bool hiRPort = _mc->_if_id_r._hiRPort;
   Bool loRPort = _mc->_if_id_r._loRPort;
   Bool hasFPSRC = _mc->_if_id_r._hasFPSRC;

   unsigned int dst1 = _mc->_id_ex_w._decodedDST;
   Bool writeReg1 = _mc->_id_ex_w._writeREG;
   Bool writeFReg1 = _mc->_id_ex_w._writeFREG;
   Bool loWPort1 = _mc->_id_ex_w._loWPort;
   Bool hiWPort1 = _mc->_id_ex_w._hiWPort;
   Bool memControl1 = _mc->_id_ex_w._memControl;
   
   unsigned int dst2 = _mc->_ex_mem_w._decodedDST;
   Bool writeReg2 = _mc->_ex_mem_w._writeREG;
   Bool writeFReg2 = _mc->_ex_mem_w._writeFREG;
   Bool loWPort2 = _mc->_ex_mem_w._loWPort;
   Bool hiWPort2 = _mc->_ex_mem_w._hiWPort;
   Bool memControl2 = _mc->_ex_mem_w._memControl;


   // #if MIPC_DEBUG
   //      fprintf(_mc->_debugLog, "<%llu> Checking data hazard src1 = %u, src2 = %u, dst1 = %u, dst2 = %u\n", SIM_TIME, src1, src2, dst1, dst2);
   //      fprintf(_mc->_debugLog, "<%llu> Checking data hazard writeReg1 = %u, writeReg2 = %u, writeFReg1 = %u, writeFReg2 = %u\n", SIM_TIME, writeReg1, writeReg2, writeFReg1, writeFReg2);
   //      fprintf(_mc->_debugLog, "<%llu> Checking data hazard loRPort = %u, hiRPort = %u, hasFPSRC = %u\n", SIM_TIME, loRPort, hiRPort, hasFPSRC);
   //      fprintf(_mc->_debugLog, "<%llu> Checking data hazard loWPort1 = %u, hiWPort1 = %u, loWPort2 = %u, hiWPort2 = %u\n", SIM_TIME, loWPort1, hiWPort1, loWPort2, hiWPort2);
   // #endif

   #if !BYPASS_ENABLED
   if(hasFPSRC){
      if((src1 != 0) && ((src1 == dst1 && writeFReg1) || (src1 == dst2 && writeFReg2))){
         return true;
      }
   }
   if(loRPort){
      if(loWPort1 || loWPort2){
         return true;
      }
   }
   if(hiRPort){
      if(hiWPort1 || hiWPort2){
         return true;
      }
   }
   if(src1 != DEFAULT_REG && src1 != 0){
      if((src1 == dst1 && writeReg1) || (src1 == dst2 && writeReg2)){
         return true;
      }
   }

   if(src2 != DEFAULT_REG && src2 != 0){
      if((src2 == dst1 && writeReg1) || (src2 == dst2 && writeReg2)){
         return true;
      }
   }
   #elif BYPASS_EX_EX_ENABLED
   if(hasFPSRC && (src1 != 0) && (src1 == dst2 && writeFReg2)) return true;
   if(loRPort && loWPort2) return true;
   if(hiRPort && hiWPort2) return true;
   if((dst2 != DEFAULT_REG) && writeReg2 && ((src1 == dst2 && src1 != 0) || (src2 == dst2 && src2 != 0))) return true;

   if(hasFPSRC && (src1 != 0) && (src1 == dst1 && writeFReg1) && memControl1) return true;
   if((dst1 != DEFAULT_REG) && writeReg1 && ((src1 == dst1 && src1 != 0) || (src2 == dst1 && src2 != 0)) && memControl1) return true;

   #endif

   return false;
}

void Decode::ex_ex_bypass(){
   unsigned int src1 = _mc->_if_id_r._regSRC1;
   unsigned int src2 = _mc->_if_id_r._regSRC2;
   Bool hiRPort = _mc->_if_id_r._hiRPort;
   Bool loRPort = _mc->_if_id_r._loRPort;
   Bool hasFPSRC = _mc->_if_id_r._hasFPSRC;

   unsigned int dst1 = _mc->_id_ex_w._decodedDST;
   Bool writeReg1 = _mc->_id_ex_w._writeREG;
   Bool writeFReg1 = _mc->_id_ex_w._writeFREG;
   Bool loWPort1 = _mc->_id_ex_w._loWPort;
   Bool hiWPort1 = _mc->_id_ex_w._hiWPort;

   if(hasFPSRC && src1 != 0 && src1 == dst1 && writeFReg1 && src1 != DEFAULT_REG){
      _mc->_if_id_r._decodedSRC1 = _mc->_id_ex_r._opResultLo;
   }
   if(loRPort && loWPort1){
      _mc->_if_id_r._decodedSRC1 = _mc->_id_ex_r._opResultLo;
   }
   if(hiRPort && hiWPort1){
      _mc->_if_id_r._decodedSRC1 = _mc->_id_ex_r._opResultLo;
   }
   if(src1 == dst1 && src1 != DEFAULT_REG && src1 != 0 && writeReg1){
      _mc->_if_id_r._decodedSRC1 = _mc->_id_ex_r._opResultLo;
   }
   if(src2 == dst1 && src2 != DEFAULT_REG && src2 != 0 && writeReg1){
      _mc->_if_id_r._decodedSRC2 = _mc->_id_ex_r._opResultLo;
   }
}

void
Decode::MainLoop (void)
{
    unsigned int ins;
    while (1) {
        AWAIT_P_PHI0;	// @posedge
        _mc->_if_id_r = _mc->_if_id_w;
        ins = _mc->_if_id_r._ins;
        _mc->Dec(ins);
        bool stall = checkDataHazard();
        _mc->_data_stall = stall;

       if(!_mc->_data_stall && _mc->_if_id_r._isSyscall) {
        #if MIPC_DEBUG
        fprintf(_mc->_debugLog, "<%llu> Decoded syscall ins %#x\n", SIM_TIME, ins);
        #endif
          _mc->_syscall_present = TRUE;
       }

        AWAIT_P_PHI1;	// @negedge    
        if(stall){
           _mc->_id_ex_w.flush_regs();
           #if MIPC_DEBUG
                   fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x, Flushed \n", SIM_TIME, ins);
           #endif
        }
        else{
           _mc->Dec(ins);
           #if BYPASS_EX_EX_ENABLED
           ex_ex_bypass();
           #endif
           #if MIPC_DEBUG
                   fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
           #endif
           _mc->_id_ex_w = _mc->_if_id_r;
        }

                  
    }
}
