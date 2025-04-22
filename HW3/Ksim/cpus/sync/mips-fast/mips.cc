#include "mips.h"
#include <assert.h>
#include "mips-irix5.h"

Mipc::Mipc (Mem *m) : _l('M')
{
   _mem = m;
   _sys = new MipcSysCall (this);	// Allocate syscall layer
#if MIPC_DEBUG
   _debugLog = fopen("mipc.debug", "w");
   assert(_debugLog != NULL);
#endif
   
   Reboot (ParamGetString ("Mipc.BootROM"));
}

Mipc::~Mipc (void)
{

}

PipeReg::PipeReg(){
   PipeReg::flush_regs();
}
PipeReg::~PipeReg(){}

void PipeReg::flush_regs(){
   _pc = 0;
   _ins = 0;
   _decodedSRC1 = 0;
   _decodedSRC2 = 0;
   _decodedDST = 0;
   _writeREG = FALSE;
   _writeFREG = FALSE;
   _hiWPort = FALSE;
   _loWPort = FALSE;
   _memControl = FALSE;
   _decodedShiftAmt = 0;
   _btgt = 0;
   _isSyscall = FALSE;
   _isIllegalOp = FALSE;
   _branchOffset = 0;
   _hi = 0; 
   _lo = 0;
   _data_stall = FALSE;
   _opControl = NULL;
   _memOp = NULL;
   _opResultLo = 0;
   _opResultHi = 0;
   _syscall_present = FALSE;
   _mem_addr_reg = 0;
   
   #if BRANCH_INTERLOCK_ENABLED
   _bdslot = 0;
   _lastbdslot = FALSE;
   #endif

   _btaken = FALSE;
   _regSRC1 = 0;
   _regSRC2 = 0;
   _hiRPort = FALSE;
   _loRPort = FALSE;
   _hasFPSRC = FALSE;
   _subregOperand = 0;
   _data_stall = FALSE;
   _bypass1 = BYPASS_NONE;
   _bypass2 = BYPASS_NONE;
   _bypass3 = BYPASS_NONE;
   _bypass_val = 0;
}

void 
Mipc::MainLoop (void)
{
    LL addr;
    unsigned int ins;	// Local instruction register

    Assert (_boot, "Mipc::MainLoop() called without boot?");

    _nfetched = 0;

    while (!_sim_exit) {
        AWAIT_P_PHI0;	// @posedge
        /* Do nothing in positive half cycle */
     
        AWAIT_P_PHI1;	// @negedge
        /* Work in negative half cycle */
        if(_syscall_present){
         #if MIPC_DEBUG
         fprintf(_debugLog, "<%llu> Fetched PC syscall present %#x\n", SIM_TIME, _pc);
         #endif
            _if_id_w.flush_regs();

        }
        else if(_data_stall){
         #if MIPC_DEBUG
         fprintf(_debugLog, "<%llu> Fetched PC data stall %#x\n", SIM_TIME, _pc);
         #endif
         _num_load_interlock++;
         continue;
        }
        #if BRANCH_INTERLOCK_ENABLED
         else if(_lastbdslot == 1){
            #if MIPC_DEBUG
            fprintf(_debugLog, "<%llu> Fetched lbd %#x\n", SIM_TIME, _pc);
            #endif
            _if_id_w.flush_regs();
            _lastbdslot = 0;
         }
        #endif

        else{
            addr = _pc;
            ins = _mem->BEGetWord (addr, _mem->Read(addr & ~(LL)0x7));
            #if MIPC_DEBUG
                    fprintf(_debugLog, "<%llu> Fetched ins %#x from PC %#x addr = %#x\n", SIM_TIME, ins, _pc, addr);
            #endif
            _if_id_w._pc = _pc;
            _if_id_w._ins = ins;
            _nfetched++;
            _pc += 4;
            #if BRANCH_INTERLOCK_ENABLED
            if(_bdslot){
               _lastbdslot = 1;
               _bdslot = 0;
            }
            #endif
                
        }
      //   #if MIPC_DEBUG
      //    fprintf(_debugLog, "<%llu> Fetcher PC %#x\n", SIM_TIME, _pc);
      //    #endif
    }

    MipcDumpstats();
    Log::CloseLog();
   
#if MIPC_DEBUG
    assert(_debugLog != NULL);
    fclose(_debugLog);
#endif

    exit(0);
}

void
Mipc::MipcDumpstats()
{
  Log l('*');
  l.startLogging = 0;

  l.print ("");
  l.print ("************************************************************");
  l.print ("");
  l.print ("Number of instructions: %llu", _nfetched);
  l.print ("Number of simulated cycles: %llu", SIM_TIME);
  l.print ("CPI: %.2f", ((double)SIM_TIME)/_nfetched);
  l.print ("Int Conditional Branches: %llu", _num_cond_br);
  l.print ("Jump and Link: %llu", _num_jal);
  l.print ("Jump Register: %llu", _num_jr);
  l.print ("Number of fp instructions: %llu", _fpinst);
  l.print ("Number of loads: %llu", _num_load);
  l.print ("Number of syscall emulated loads: %llu", _sys->_num_load);
  l.print ("Number of stores: %llu", _num_store);
  l.print ("Number of syscall emulated stores: %llu", _sys->_num_store);
  l.print("Number of load interlocks: %llu", _num_load_interlock);
  l.print ("");

}

void 
Mipc::fake_syscall (unsigned int ins)
{
   _sys->pc = _pc;
   _sys->quit = 0;
   _sys->EmulateSysCall ();
   if (_sys->quit)
      _sim_exit = 1;
}

/*------------------------------------------------------------------------
 *
 *  Mipc::Reboot --
 *
 *   Reset processor state
 *
 *------------------------------------------------------------------------
 */
void 
Mipc::Reboot (char *image)
{
   FILE *fp;
   Log l('*');

   _boot = 0;

   if (image) {
      _boot = 1;
      printf ("Executing %s\n", image);
      fp = fopen (image, "r");
      if (!fp) {
	 fatal_error ("Could not open `%s' for booting host!", image);
      }
      _mem->ReadImage(fp);
      fclose (fp);

      // Reset state
      _ins = 0;

      _num_load = 0;
      _num_store = 0;
      _fpinst = 0;
      _num_cond_br = 0;
      _num_jal = 0;
      _num_jr = 0;
      _num_load_interlock = 0;

      #if BRANCH_INTERLOCK_ENABLED
      _lastbdslot = 0;
      _bdslot = 0;
      #endif

      _btaken = 0;
      _btgt = 0xdeadbeef;
      _sim_exit = 0;
      _pc = ParamGetInt ("Mipc.BootPC");	// Boom! GO
   }
}

LL
MipcSysCall::GetDWord(LL addr)
{
   _num_load++;      
   return m->Read (addr);
}

void
MipcSysCall::SetDWord(LL addr, LL data)
{
  
   m->Write (addr, data);
   _num_store++;
}

Word 
MipcSysCall::GetWord (LL addr) 
{ 
  
   _num_load++;   
   return m->BEGetWord (addr, m->Read (addr & ~(LL)0x7)); 
}

void 
MipcSysCall::SetWord (LL addr, Word data) 
{ 
  
   m->Write (addr & ~(LL)0x7, m->BESetWord (addr, m->Read(addr & ~(LL)0x7), data)); 
   _num_store++;
}
  
void 
MipcSysCall::SetReg (int reg, LL val) 
{ 
   _ms->_gpr[reg] = val; 
}

LL 
MipcSysCall::GetReg (int reg) 
{
   return _ms->_gpr[reg]; 
}

LL
MipcSysCall::GetTime (void)
{
  return SIM_TIME;
}
