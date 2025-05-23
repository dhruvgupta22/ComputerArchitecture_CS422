#ifndef __MIPS_H__
#define __MIPS_H__

#include "sim.h"

class Mipc;
class MipcSysCall;
class SysCall;
class PipeReg;

typedef unsigned Bool;
#define TRUE 1
#define FALSE 0

#if BYTE_ORDER == LITTLE_ENDIAN

#define FP_TWIDDLE 0

#else

#define FP_TWIDDLE 1

#endif

#include "mem.h"
#include "../../common/syscall.h"
#include "queue.h"

#define MIPC_DEBUG 0
#define BRANCH_INTERLOCK_ENABLED 0
#define BYPASS_EX_EX_ENABLED 1
#define BYPASS_MEM_EX_ENABLED 1
#define BYPASS_MEM_MEM_ENABLED 1
// #define BYPASS_ENABLED (BYPASS_EX_EX_ENABLED | BYPASS_MEM_EX_ENABLED | BYPASS_MEM_MEM_ENABLED)
#define BYPASS_NONE 0x0
#define BYPASS_EX_EX 0x1    // ex_mem_w
#define BYPASS_MEM_EX 0x2   // mem_wb_w
#define BYPASS_MEM_MEM 0x4  // mem_wb_w
#define TAKE_FROM_HI 0x8
#define DEFAULT_REG 0x20


class PipeReg{
   public:
    PipeReg();
    ~PipeReg();
    void flush_regs();
     unsigned int _pc;
     unsigned int _ins;
     signed int _decodedSRC1;
     signed int _decodedSRC2;
     unsigned _decodedDST;
     Bool _writeREG;
     Bool _writeFREG;
     Bool _hiWPort;
     Bool _loWPort;
     Bool _memControl;
     unsigned _decodedShiftAmt;
     unsigned int _btgt;
     Bool _isSyscall;
     Bool _isIllegalOp;
     signed int _branchOffset;
     unsigned int _hi, _lo;
     Bool _data_stall;
     unsigned int _bypass1; // src1
     unsigned int _bypass2; // src2
     unsigned int _bypass3; // dst
     unsigned int _bypass_val;
     
     #if BRANCH_INTERLOCK_ENABLED
     int _bdslot;
     Bool _lastbdslot;
     #endif

     Bool _btaken;
     unsigned int _opResultLo;
     unsigned int _opResultHi;
     unsigned int _mem_addr_reg;
     Bool _syscall_present;
     unsigned _regSRC1;
     unsigned _regSRC2;
     Bool _hiRPort;
     Bool _loRPort;
     Bool _hasFPSRC;
     unsigned int _subregOperand;
 
     void (*_opControl)(Mipc*, unsigned);
     void (*_memOp)(Mipc*);    
 };

class Mipc : public SimObject {
public:
   Mipc (Mem *m);
   ~Mipc ();
  
   FAKE_SIM_TEMPLATE;

   MipcSysCall *_sys;		// Emulated system call layer

   void dumpregs (void);	// Dumps current register state

   void Reboot (char *image = NULL);
				// Restart processor.
				// "image" = file name for new memory
				// image if any.

   void MipcDumpstats();			// Prints simulation statistics
   void Dec (unsigned int ins);			// Decoder function
   void fake_syscall (unsigned int ins);	// System call interface

   /* processor state */
   unsigned int _ins;   // instruction register

   signed int	_decodedSRC1, _decodedSRC2;	// Reg fetch output (source values)
   unsigned	_decodedDST;			// Decoder output (dest reg no)
   unsigned 	_subregOperand;			// Needed for lwl and lwr
   unsigned	_memory_addr_reg;				// Memory address register
   unsigned	_opResultHi, _opResultLo;	// Result of operation
   Bool 	_memControl;			// Memory instruction?
   Bool		_writeREG, _writeFREG;		// WB control
   signed int	_branchOffset;
   Bool 	_hiWPort, _loWPort;		// WB control
   unsigned	_decodedShiftAmt;		// Shift amount
   Bool _syscall_present;
   unsigned int 	_gpr[32];		// general-purpose integer registers
   unsigned _regSRC1;
   Bool _data_stall;
   unsigned _regSRC2;

   Bool _hiRPort;
   Bool _loRPort;
   Bool _hasFPSRC;

   union {
      unsigned int l[2];
      float f[2];
      double d;
   } _fpr[16];					// floating-point registers (paired)

   unsigned int _hi, _lo; 			// mult, div destination
   unsigned int	_pc;				// Program counter

   #if BRANCH_INTERLOCK_ENABLED
   unsigned int _lastbdslot;			// branch delay state
   int 		_bdslot;				// 1 if the next ins is delay slot
   #endif

   unsigned int _boot;				// boot code loaded?

   int 		_btaken; 			// taken branch (1 if taken, 0 if fall-through)
   unsigned int	_btgt;				// branch target
    int _stall;             // if 1, fetcher is stalled
    
   Bool		_isSyscall;			// 1 if system call
   Bool		_isIllegalOp;			// 1 if illegal opcode

   // Simulation statistics counters

   LL	_nfetched;
   LL	_num_cond_br;
   LL	_num_jal;
   LL	_num_jr;
   LL   _num_load;
   LL   _num_store;
   LL   _fpinst;
   LL  _num_load_interlock;

   Mem	*_mem;	// attached memory (not a cache)

   Log	_l;
   int  _sim_exit;		// 1 on normal termination

   void (*_opControl)(Mipc*, unsigned);
   void (*_memOp)(Mipc*);

   PipeReg _if_id_r;
   PipeReg _id_ex_r;
   PipeReg _ex_mem_r;
   PipeReg _mem_wb_r;
   PipeReg _if_id_w;
   PipeReg _id_ex_w;
   PipeReg _ex_mem_w;
   PipeReg _mem_wb_w;

   FILE *_debugLog;

   // EXE stage definitions

   static void func_add_addu (Mipc*, unsigned);
   static void func_and (Mipc*, unsigned);
   static void func_nor (Mipc*, unsigned);
   static void func_or (Mipc*, unsigned);
   static void func_sll (Mipc*, unsigned);
   static void func_sllv (Mipc*, unsigned);
   static void func_slt (Mipc*, unsigned);
   static void func_sltu (Mipc*, unsigned);
   static void func_sra (Mipc*, unsigned);
   static void func_srav (Mipc*, unsigned);
   static void func_srl (Mipc*, unsigned);
   static void func_srlv (Mipc*, unsigned);
   static void func_sub_subu (Mipc*, unsigned);
   static void func_xor (Mipc*, unsigned);
   static void func_div (Mipc*, unsigned);
   static void func_divu (Mipc*, unsigned);
   static void func_mfhi (Mipc*, unsigned);
   static void func_mflo (Mipc*, unsigned);
   static void func_mthi (Mipc*, unsigned);
   static void func_mtlo (Mipc*, unsigned);
   static void func_mult (Mipc*, unsigned);
   static void func_multu (Mipc*, unsigned);
   static void func_jalr (Mipc*, unsigned);
   static void func_jr (Mipc*, unsigned);
   static void func_await_break (Mipc*, unsigned);
   static void func_syscall (Mipc*, unsigned);
   static void func_addi_addiu (Mipc*, unsigned);
   static void func_andi (Mipc*, unsigned);
   static void func_lui (Mipc*, unsigned);
   static void func_ori (Mipc*, unsigned);
   static void func_slti (Mipc*, unsigned);
   static void func_sltiu (Mipc*, unsigned);
   static void func_xori (Mipc*, unsigned);
   static void func_beq (Mipc*, unsigned);
   static void func_bgez (Mipc*, unsigned);
   static void func_bgezal (Mipc*, unsigned);
   static void func_bltzal (Mipc*, unsigned);
   static void func_bltz (Mipc*, unsigned);
   static void func_bgtz (Mipc*, unsigned);
   static void func_blez (Mipc*, unsigned);
   static void func_bne (Mipc*, unsigned);
   static void func_j (Mipc*, unsigned);
   static void func_jal (Mipc*, unsigned);
   static void func_lb (Mipc*, unsigned);
   static void func_lbu (Mipc*, unsigned);
   static void func_lh (Mipc*, unsigned);
   static void func_lhu (Mipc*, unsigned);
   static void func_lwl (Mipc*, unsigned);
   static void func_lw (Mipc*, unsigned);
   static void func_lwr (Mipc*, unsigned);
   static void func_lwc1 (Mipc*, unsigned);
   static void func_swc1 (Mipc*, unsigned);
   static void func_sb (Mipc*, unsigned);
   static void func_sh (Mipc*, unsigned);
   static void func_swl (Mipc*, unsigned);
   static void func_sw (Mipc*, unsigned);
   static void func_swr (Mipc*, unsigned);
   static void func_mtc1 (Mipc*, unsigned);
   static void func_mfc1 (Mipc*, unsigned);

   // MEM stage definitions

   static void mem_lb (Mipc*);
   static void mem_lbu (Mipc*);
   static void mem_lh (Mipc*);
   static void mem_lhu (Mipc*);
   static void mem_lwl (Mipc*);
   static void mem_lw (Mipc*);
   static void mem_lwr (Mipc*);
   static void mem_lwc1 (Mipc*);
   static void mem_swc1 (Mipc*);
   static void mem_sb (Mipc*);
   static void mem_sh (Mipc*);
   static void mem_swl (Mipc*);
   static void mem_sw (Mipc*);
   static void mem_swr (Mipc*);
};


// Emulated system call interface

class MipcSysCall : public SysCall {
public:

   MipcSysCall (Mipc *ms) {

      char buf[1024];
      m = ms->_mem;
      _ms = ms;
      _num_load = 0;
      _num_store = 0;
   };

   ~MipcSysCall () { };

   LL GetDWord (LL addr);
   void SetDWord (LL addr, LL data);

   Word GetWord (LL addr);
   void SetWord (LL addr, Word data);
  
   void SetReg (int reg, LL val);
   LL GetReg (int reg);
   LL GetTime (void);

private:

   Mipc *_ms;
};


#endif /* __MIPS_H__ */
