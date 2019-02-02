/* 
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 */

#ifndef _PIPE_H_
#define _PIPE_H_
#define NCOR 4

#include "shell.h"
#include "stdbool.h"
#include <limits.h>


typedef struct CPU_State {
	/* register file state */
	int64_t REGS[ARM_REGS];
	int FLAG_N;        /* flag N */
	int FLAG_Z;        /* flag Z */

	/* program counter in fetch stage */
	uint64_t PC;
	
} CPU_State;

// LAB5
int RUN_BIT[NCOR];
// CPU 0 is active and none others initially




/**************Pipeline Registers (as recommended) ****************/
typedef struct Pipe_Reg_IFtoDE{
  uint64_t PC_4; //holds the value of the next PC
  uint32_t instruction; //holds the current instruction
  int ptaken; //does the branch predictor predict that the branch is taken?
} Pipe_Reg_IFtoDE;

typedef struct Pipe_Reg_DEtoEX{
  uint64_t PC_4; //holds the value of the next PC
  
  int stall;  //do we stall for a cycle or not?
  uint32_t rm;
  int64_t rm_val;  //value of Rm register
  uint32_t rt;
  uint32_t rd;
  uint32_t rn;
  int64_t rn_val;  //value of Rn register
  uint32_t imm;
  int64_t wimm;
  int64_t address;
  int64_t shamt; 
  uint32_t hw; // just for movz 
  int ptaken; //does the branch predictor predict that the branch is taken?

  int instr_name; //looks like we'll need instruction name to know 
                       //when to forward
/*********Forward Checking **************/

  int forward; // if 0, use the register values. if 1, use the value in fValue
  uint32_t rf; // the number of the forwarded register
  int flag_forward; // if 0, use the current state values. if 1, use the forward values 
  int64_t fval; //contains the (possibly forwarded) value
  int fFLAG_N; //contains the (possibly forwarded) value of n-flag
  int fFLAG_Z; //contains the (possibly forwarded) value of z-flag

} Pipe_Reg_DEtoEX;

typedef struct Pipe_Reg_EXtoMEM{
  uint64_t PC_4; //contains the calculated next PC 
  uint64_t calc_PC; //contains the calculated next PC (needed for branch)
  int64_t ALU_val; //contains result calculated from ALU
  uint32_t rt;
  uint32_t rd;
  uint32_t address;
  uint32_t imm;
  int64_t wimm;
  int FLAG_Z;
  int FLAG_N;
  int instr_name; //looks like we'll need instruction name to know 
                       //when to forward
  int taken; //is the branch actually taken?
   
  /*************Forward Handling *****************/
  int fff; //means "flag for forwarding flags"-indicates if flags should be
           //forwarded-if EX stage changed them, then yes, otherwise no
} Pipe_Reg_EXtoMEM;

typedef struct Pipe_Reg_MEMtoWB{

  uint64_t PC_4; //contains the calculated next PC 
  uint64_t mem_val; //the value obtained from memory
  int64_t ALU_val; //value from ALU
  uint32_t rt;
  uint32_t rd;
  int FLAG_Z;
  int FLAG_N;
  int instr_name; 
   
  /*************Forward Handling *****************/
  int fff; 

} Pipe_Reg_MEMtoWB;

/* global variable -- pipeline state */
extern CPU_State CURRENT_STATE[];
extern Pipe_Reg_IFtoDE IF_DE_REG[];
extern Pipe_Reg_DEtoEX DE_EX_REG[];
extern Pipe_Reg_EXtoMEM EX_MEM_REG[];
extern Pipe_Reg_MEMtoWB MEM_WB_REG[];
extern int format[]; //instruction format
extern int crid;

/* called during simulator startup */
void pipe_init();

/* this function calls the others */
void pipe_cycle();

/* each of these functions implements one stage of the pipeline */
void pipe_stage_fetch();
void pipe_stage_decode();
void pipe_stage_execute();
void pipe_stage_mem();
void pipe_stage_wb();

#endif
