#include "decode.h"

Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}


Bool Decode::detectStall(){
   Bool stall=0;
   unsigned int src1 = _mc->_if_id_r._regSRC1;
   unsigned int src2 = _mc->_if_id_r._regSRC2;
   unsigned int dst = _mc->_if_id_r._decodedDST;
   Bool hiRPort = _mc->_if_id_r._hiRPort;
   Bool loRPort = _mc->_if_id_r._loRPort;
   Bool hasFPSRC = _mc->_if_id_r._hasFPSRC;
   Bool memControl = _mc->_if_id_r._memControl;

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

   _mc->_if_id_r._bypass1 = BYPASS_NONE;
   _mc->_if_id_r._bypass2 = BYPASS_NONE;
   _mc->_if_id_r._bypass3 = BYPASS_NONE;
   _mc->_if_id_r._bypass_val = 0;

   #if BYPASS_MEM_EX_ENABLED
      if(writeReg2 && src1 == dst2 && src1 != 0 && src1 != DEFAULT_REG){
         _mc->_if_id_r._bypass1 = BYPASS_MEM_EX;
      }
      if(writeReg2 && src2 == dst2 && src2 != 0 && src2 != DEFAULT_REG){
         _mc->_if_id_r._bypass2 = BYPASS_MEM_EX;
      }
      if(writeReg2 && dst == dst2 && dst != 0 && dst != DEFAULT_REG && memControl){
         _mc->_if_id_r._bypass3 = BYPASS_MEM_EX;
      }
      if(hasFPSRC && writeFReg2 && src1 == dst2 && src1 != 0 && src1 != DEFAULT_REG){
         _mc->_if_id_r._bypass1 = BYPASS_MEM_EX;
      }
      if(loRPort && loWPort2){
         _mc->_if_id_r._bypass1 = BYPASS_MEM_EX;
      }
      if(hiRPort && hiWPort2){
         _mc->_if_id_r._bypass1 = (BYPASS_MEM_EX | TAKE_FROM_HI);
      }
   #else
      if(writeReg2 && src1 == dst2 && src1 != 0 && src1 != DEFAULT_REG){
         stall = 1;
      }
      if(writeReg2 && src2 == dst2 && src2 != 0 && src2 != DEFAULT_REG){
         stall = 1;
      }
      if(writeReg2 && dst == dst2 && dst != 0 && dst != DEFAULT_REG && memControl){
         stall = 1;
      }
      if(hasFPSRC && writeFReg2 && src1 == dst2 && src1 != 0 && src1 != DEFAULT_REG){
         stall = 1;
      }
      if(loRPort && loWPort2){
         stall = 1;
      }
      if(hiRPort && hiWPort2){
         stall = 1;
      }
   #endif

   #if BYPASS_EX_EX_ENABLED
      if(writeReg1 && src1 == dst1 && src1 != 0 && src1 != DEFAULT_REG && !memControl1){
         #if MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Decoded ex-ex bypass for src1 of ins %#x\n", SIM_TIME, _mc->_if_id_r._ins);
         #endif
         _mc->_if_id_r._bypass1 = BYPASS_EX_EX;
      }
      if(writeReg1 && src2 == dst1 && src2 != 0 && src2 != DEFAULT_REG && !memControl1){
         _mc->_if_id_r._bypass2 = BYPASS_EX_EX;
      }
      if(writeReg1 && dst == dst1 && dst != 0 && dst != DEFAULT_REG && !memControl1 && memControl){
         _mc->_if_id_r._bypass3 = BYPASS_EX_EX;
      }
      if(hasFPSRC && writeFReg1 && src1 == dst1 && src1 != 0 && src1 != DEFAULT_REG){
         _mc->_if_id_r._bypass1 = BYPASS_EX_EX;
      }
      if(loRPort && loWPort1){
         _mc->_if_id_r._bypass1 = BYPASS_EX_EX;
      }
      if(hiRPort && hiWPort1){
         _mc->_if_id_r._bypass1 = (BYPASS_EX_EX | TAKE_FROM_HI);
      }
   #else
      if(writeReg1 && src1 == dst1 && src1 != 0 && src1 != DEFAULT_REG && !memControl1){
         stall = 1;
      }
      if(writeReg1 && src2 == dst1 && src2 != 0 && src2 != DEFAULT_REG && !memControl1){
         stall = 1;
      }
      if(writeReg1 && !memControl1 && memControl && dst==dst1 && dst!=0 && dst!=DEFAULT_REG){
         stall = 1;
      }
      if(hasFPSRC && writeFReg1 && src1 == dst1 && src1 != 0 && src1 != DEFAULT_REG){
         stall = 1;
      }
      if(loRPort && loWPort1){
         stall = 1;
      }
      if(hiRPort && hiWPort1){
         stall = 1;
      }
   #endif

   #if BYPASS_MEM_MEM_ENABLED
      if(writeReg1 && memControl1 && memControl && dst == dst1 && dst != 0 && dst != DEFAULT_REG){
         _mc->_if_id_r._bypass3 = BYPASS_MEM_MEM;
      }
      if(writeFReg1 && memControl1 && memControl && dst == dst1 && dst != 0 && dst != DEFAULT_REG){
         _mc->_if_id_r._bypass3 = BYPASS_MEM_MEM;
      }
   #else
      if(writeReg1 && memControl1 && memControl && dst == dst1 && dst != 0 && dst != DEFAULT_REG){
         stall = 1;
      }
      if(writeFReg1 && memControl1 && memControl && dst == dst1 && dst != 0 && dst != DEFAULT_REG){
         stall = 1;
      }
   #endif

   if(writeReg1 && src1 == dst1 && src1 != 0 && src1 != DEFAULT_REG && memControl1){
      stall = 1;
   }
   if(writeReg1 && src2 == dst1 && src2 != 0 && src2 != DEFAULT_REG && memControl1){
      stall = 1;
   }
   if(writeFReg1 && src1 == dst1 && src1 != 0 && src1 != DEFAULT_REG && memControl1){
      stall = 1;
   }
   if(writeFReg1 && src2 == dst1 && src2 != 0 && src2 != DEFAULT_REG && memControl1){
      stall = 1;
   }


   return stall;
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
        bool stall = detectStall();
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
           #if MIPC_DEBUG
                   fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
           #endif
           _mc->_id_ex_w = _mc->_if_id_r;
        }

                  
    }
}


/*
if(hasFPSRC){

   }

   if(loRPort){

   }

   if(hiRPort){

   }

   if(src1 != 0 && src1 != DEFAULT_REG){
      if(src1 == dst1 && writeReg1){
         if(!memControl1){
            #if BYPASS_EX_EX_ENABLED
               _mc->_if_id_r._bypass1 = BYPASS_EX_EX;
            #else
               stall = 1;
            #endif
         }

         else {
            if(memControl) {
               #if BYPASS_MEM_MEM_ENABLED
                  _mc->_if_id_r._bypass1 = BYPASS_MEM_MEM;
               #else
                  stall = 1;
               #endif
            }

            // load interlock
            else stall = 1;
         }
      }

      else if(src1 == dst2 && writeReg2){
        #if BYPASS_MEM_EX_ENABLED
         _mc->_if_id_r._bypass1 = BYPASS_MEM_EX;
        #else
         stall = 1;
        #endif
      }
   }

   if(src2 != 0 && src2 != DEFAULT_REG){
      if(src2 == dst1 && writeReg1){
         if(!memControl1){
            #if BYPASS_EX_EX_ENABLED
               _mc->_if_id_r._bypass2 = BYPASS_EX_EX;
            #else
               stall = 1;
            #endif
         }

         else {
            if(memControl) {
               #if BYPASS_MEM_MEM_ENABLED
                  _mc->_if_id_r._bypass2 = BYPASS_MEM_MEM;
               #else
                  stall = 1;
               #endif
            }

            // load interlock
            else stall = 1;
         }
      }

      else if(src2 == dst2 && writeReg2){
        #if BYPASS_MEM_EX_ENABLED
         _mc->_if_id_r._bypass2 = BYPASS_MEM_EX;
        #else
         stall = 1;
        #endif
      }
   }

*/
