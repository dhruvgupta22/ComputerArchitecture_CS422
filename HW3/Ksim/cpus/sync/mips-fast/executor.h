#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "mips.h"

class Mipc;

class Exe : public SimObject {
public:
   Exe (Mipc*);
   ~Exe ();
   void pick_bypass_value();
  
   FAKE_SIM_TEMPLATE;

   Mipc *_mc;
};
#endif
