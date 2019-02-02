/* Pranav Subramaniam, psubramaniam
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "pipe.h"
#include "shell.h"
#include "bp.h"
#include "cache.h"

//Constants
#define ADD 1
#define B 2
#define BLE 3
#define LDURH 4
#define STURB 5
#define SUBS 6
#define BL 7
#define ADDI 8
#define BEQ 9
#define CBNZ 10
#define LSL 11
#define STURH 12
#define MUL 13
#define BR 14
#define ADDIS 15
#define BNE 16
#define CBZ 17
#define LSR 18
#define STURW 19
#define SDIV 20
#define ADDS 21
#define BGT 22
#define EOR 23
#define MOVZ 24
#define SUB 25
#define UDIV 26
#define AND 27
#define BLT 28
#define LDUR 29
#define ORR 30
#define SUBI 31
#define HLT 32
#define ANDS 33
#define BGE 34
#define LDURB 35
#define STUR 36
#define SUBIS 37
#define CMP 38
#define BCOND 39

//Format Types
#define FR 1
#define FB 2
#define FCB 3
#define FD 4
#define FI 5
#define FIW 6

//Global Variables

// Cache variables
int icache_start = 0;
int dcache_start = 0;
static int cyc_cnt = 0;
cache_t *icache = NULL;
cache_t *dcache = NULL;

// Instruction variables
uint32_t cur_instr;  //Current instruction
int format; //Current format
int instr_name; //store the name of the instruction here

//B format variables
int64_t baddr; //branch address

//CB format variables
int64_t cbaddr; //conditional branch address
//also uses rt

//R format variables
uint32_t rm; //second register source operand
uint32_t shamt; //optional shift amount
//also uses rn and rd

//D format variables
uint32_t rt; //can either be source or destination, depending on load or store
uint32_t address; //address to store to or load from
//also uses rn

//I format variables
uint32_t imm; //the immediate value
//also uses rn and rd

//IW format variables
int64_t wimm; //the immediate value
uint32_t hw; //the (optional) shift value
/*Explanation of how hw works
hw can be b00, b01, b10, or b11 and can shift wimm left by 0,16,32, or 48
so we shift wimm by hw*16
*/
//also uses rd

//variables common between formats
uint32_t rd; //register to be stored to
uint32_t rn; //first register source operand
//rt should be here too, but whatever...

/**************Stall or Forward Variables ******************/
//int stall_b = 0; //indicates whether to stall due to branch
int squash = 0; //indicates whether to squash instructions in the case of a branch
int stall = 0; //indicates whether to stall or not
int did_stall = 0; //indicates whether or not we have already stalled

/******************Halt Control***********************/
int did_halt = 0; //indicates we halted, so now we should complete all
                  //outstanding instructions and stop

/* global pipeline state */
CPU_State CURRENT_STATE;
Pipe_Reg_IFtoDE IF_DE_REG;
Pipe_Reg_DEtoEX DE_EX_REG;
Pipe_Reg_EXtoMEM EX_MEM_REG;
Pipe_Reg_MEMtoWB MEM_WB_REG;

void pipe_init()
{
    memset(&CURRENT_STATE, 0, sizeof(CPU_State));
    memset(&IF_DE_REG, 0, sizeof(IF_DE_REG));
    memset(&DE_EX_REG, 0, sizeof(DE_EX_REG));
    memset(&EX_MEM_REG, 0, sizeof(EX_MEM_REG));
    memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG));
    CURRENT_STATE.PC = 0x00400000;
    icache = cache_new(64, 4);
    dcache = cache_new(256, 8);
}

void pipe_cycle()
{
	pipe_stage_wb();
	pipe_stage_mem();
	pipe_stage_execute();
	pipe_stage_decode();
	pipe_stage_fetch();
        cyc_cnt++;
}

/************************************WRITEBACK FUNCTIONS****************************************/
void wb_ADD() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val; 
}

void wb_ADDS() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_ADDI() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_ADDIS() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_AND(){
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_ANDS() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_B() {

}

void wb_BL() {
   
}

void wb_BR() {
   
}

void wb_BCOND() {
   
}

void wb_CBZ() {
   
}

void wb_CBNZ() {
   
}

void wb_LDUR() {
   CURRENT_STATE.REGS[MEM_WB_REG.rt] = MEM_WB_REG.mem_val;
}

void wb_LDURB() {
   CURRENT_STATE.REGS[MEM_WB_REG.rt] = MEM_WB_REG.mem_val;
}

void wb_LDURH() {
   CURRENT_STATE.REGS[MEM_WB_REG.rt] = MEM_WB_REG.mem_val;
}

void wb_LSL() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_LSR() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_MOVZ() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_ORR() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_EOR() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_STUR() {
     
}

void wb_STURB() {
   
}

void wb_STURH() {
   
}

void wb_SUB() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_SUBS() {
   if (MEM_WB_REG.rd == 31){ //this is the zero register, so don't do anything
      return;
   }
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_SUBI() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_SUBIS() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_MUL() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_SDIV() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_UDIV() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_CMP() {
   CURRENT_STATE.REGS[MEM_WB_REG.rd] = MEM_WB_REG.ALU_val;
}

void wb_HLT(){

   RUN_BIT = 0;

}

void pipe_stage_wb()
{
	   // forward flags
    if (MEM_WB_REG.fff == 1)
    {
        DE_EX_REG.flag_forward = 1;
        DE_EX_REG.fFLAG_N = MEM_WB_REG.FLAG_N;
        DE_EX_REG.fFLAG_Z = MEM_WB_REG.FLAG_Z;
    }

    MEM_WB_REG.fff = 0; // clean up
    DE_EX_REG.forward = 0;

    if ((MEM_WB_REG.instr_name == LDUR || MEM_WB_REG.instr_name == LDURB || MEM_WB_REG.instr_name == LDURH)&& (DE_EX_REG.instr_name == STUR || DE_EX_REG.instr_name == STURB || DE_EX_REG.instr_name == STURH)){
       if (DE_EX_REG.rt == MEM_WB_REG.rt){
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rt;
           DE_EX_REG.fval = MEM_WB_REG.mem_val;
       }   


    }else if (MEM_WB_REG.instr_name == LDUR || MEM_WB_REG.instr_name == LDURB || MEM_WB_REG.instr_name == LDURH)
    {
        if (DE_EX_REG.rn == MEM_WB_REG.rt){
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rn;
           DE_EX_REG.fval = MEM_WB_REG.mem_val;
        }else if (DE_EX_REG.rm == MEM_WB_REG.rt){
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rm;
           DE_EX_REG.fval = MEM_WB_REG.mem_val;
        }
    }else { //not related to loading or storing

    if (DE_EX_REG.rn == MEM_WB_REG.rd){

        if (EX_MEM_REG.rd == DE_EX_REG.rn)
        {
           //since the instruction in mem stage is more recent, don't do anything
        }
        else
        {
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rn;
           DE_EX_REG.fval = MEM_WB_REG.ALU_val;
        }
    }

    if (DE_EX_REG.rm == MEM_WB_REG.rd){
        
        if (EX_MEM_REG.rd == DE_EX_REG.rm)
        {

        }    
        else 
        {
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rm;
           DE_EX_REG.fval = MEM_WB_REG.ALU_val;
        }
    }
    }

   if (RUN_BIT != 0 && MEM_WB_REG.instr_name != 0) {
      stat_inst_retire = stat_inst_retire + 1;	 
   }
   switch(MEM_WB_REG.instr_name) {   /* A large switch to execute instructions */
	case 0: {
	     break;
	}
        case ADD: {
             wb_ADD();
             break;
        }
        case ADDI: {
             wb_ADDI();
             break;
        }
        case ADDIS: {
             wb_ADDIS();
             CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
             CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;            
	     break;
        }
        case ADDS: {
             wb_ADDS();
	     CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
	     CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;
             break;
        }
        case AND: {
             wb_AND();
             break;
        }
        case ANDS: {
             wb_ANDS();
             CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
             CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;
             break;
        }
        case B: {
             wb_B();
             break;
        }
        case BL: {
             wb_BL();
             break;
        }
        case BR: {
             wb_BR();
             break;
        }
        case CBZ: {
             wb_CBZ();
             break;
        }
        case CBNZ: {
             wb_CBNZ();
             break;
        }
        case LDUR: {
             wb_LDUR();
             break;
        }
        case LDURB: {
             wb_LDURB();
             break;
        }
        case LDURH: {
             wb_LDURH();
             break;
        }
        case BCOND: {
             wb_BCOND();
             CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
             CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;
             break;
        }
        case CMP: {
             wb_CMP();
             CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
             CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;
             break;
        }
        case LSL: {
             wb_LSL();
             break;
        }
        case LSR: {
             wb_LSR();
             break;
        }
        case MOVZ: {
             wb_MOVZ();
             break;
        }
        case ORR: {
             wb_ORR();
             break;
        }
        case EOR: {
             wb_EOR();
             break;
        }
        case STUR: {
             wb_STUR();
             break;
        }
        case STURB: {
             wb_STURB();
             break;
        }
        case STURH: {
             wb_STURH();
             break;
        }
        case SUB: {
             wb_SUB();
             break;
        }
        case SUBS: {
             wb_SUBS();
             CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
             CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;
             break;
        }
        case SUBI: {
             wb_SUBI();
             break;
        }
        case SUBIS: {
             wb_SUBIS();
             CURRENT_STATE.FLAG_Z = MEM_WB_REG.FLAG_Z;
             CURRENT_STATE.FLAG_N = MEM_WB_REG.FLAG_N;
             break;
        }
        case MUL: {
             wb_MUL();
             break;
        }
        case SDIV: {
             wb_SDIV();
             break;
        }
        case UDIV: {
             wb_UDIV();
             break;
        }
        case HLT: {
             wb_HLT();
             break;
        }
        default: {
           printf(" Instruction is not recognized, %d\n", MEM_WB_REG.instr_name);
        }
    }
}

/************************************MEMORY FUNCTIONS****************************************/

void mem_ADD() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
   MEM_WB_REG.rd = EX_MEM_REG.rd;
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
}

void mem_ADDS() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_ADDI() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_ADDIS() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_AND(){
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_ANDS() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_B() {
 
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_BL() {
                                                          
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}


void mem_BR() {
                                                              
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_BCOND() {
                                                                 
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_CBZ() {
	
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_CBNZ() {                                       
                                                                 
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_LDUR() {
  int64_t tmp1;
  int64_t tmp2;
  int64_t value;
  uint64_t base_address = EX_MEM_REG.ALU_val + EX_MEM_REG.address;
  
  // CACHE CHANGE
  if (is_Fetching(dcache) > 0) {
      //printf("dcache stall (%d)\n", is_Fetching(dcache));
      //cache is stalling here
  }
  tmp1 = cache_update(dcache, base_address, 0, 0, 0); 
  if(is_Fetching(dcache) == 0) {
    tmp1 = cache_update(dcache, base_address, 0, 0, 0);  
    tmp2 = cache_update(dcache, base_address + 4, 0, 0, 0); 
  }
  // CACHE CHANGE
  value = (tmp2 << 32) | tmp1;
  
  MEM_WB_REG.mem_val = value;                                                                 
  MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
  MEM_WB_REG.rt = EX_MEM_REG.rt;
  MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;

  if (stall == 1) {
     EX_MEM_REG.instr_name = 0;
     EX_MEM_REG.rd = -1;
     EX_MEM_REG.rt = -1;
  }
  // CACHE CHANGE
  MEM_WB_REG.rd = EX_MEM_REG.rd;
  if (is_Fetching(dcache) > 0) {
     memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG)); 
  }
  // CACHE CHANGE
  
}

void mem_LDURB() {
  int64_t tmp1;
  int64_t msk = 0xff;
  uint64_t base = EX_MEM_REG.ALU_val + EX_MEM_REG.address;
  // CACHE CHANGE
  tmp1 = cache_update(dcache, base, 0, 0, 0); 
  // CACHE CHANGE
  uint64_t value = tmp1 & msk;
  MEM_WB_REG.mem_val = value;
                                                                
  MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
  MEM_WB_REG.rt = EX_MEM_REG.rt;
  MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;

  if (stall == 1) {
     EX_MEM_REG.instr_name = 0;
     EX_MEM_REG.rd = 0;
     EX_MEM_REG.rt = 0;
  }
  // CACHE CHANGE
  if (is_Fetching(dcache) > 0) {
     memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG)); 
  }
  // CACHE CHANGE
}

void mem_LDURH() {
  int64_t tmp1;
  int64_t msk = 0xffff;
  uint64_t base = EX_MEM_REG.ALU_val + EX_MEM_REG.address;
  // CACHE CHANGE
  tmp1 = cache_update(dcache, base, 0, 0, 0); 
  // CACHE CHANGE
  uint64_t value = tmp1 & msk;
  MEM_WB_REG.mem_val = value;
                                                                 
  MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
  MEM_WB_REG.rt = EX_MEM_REG.rt;
  MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;

  if (stall == 1) {
     EX_MEM_REG.instr_name = 0;
     EX_MEM_REG.rd = 0;
     EX_MEM_REG.rt = 0;
  }
  // CACHE CHANGE
  if (is_Fetching(dcache) > 0) {
     memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG)); 
  }
  // CACHE CHANGE
}

void mem_LSL() { 
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
}

void mem_LSR() { 
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
}

void mem_MOVZ() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
}

void mem_ORR() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
}

void mem_EOR() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
}

void mem_STUR() {
  int64_t tmp1;
  int64_t tmp2;
  int64_t value;
  uint64_t base_address = EX_MEM_REG.ALU_val + EX_MEM_REG.address;
  
  tmp1 = CURRENT_STATE.REGS[EX_MEM_REG.rt];
  tmp2 = CURRENT_STATE.REGS[EX_MEM_REG.rt] >> 32;

  cache_update(dcache, base_address, 1, tmp1, 4);
  if (is_Fetching(dcache) == 0) {
     cache_update(dcache, base_address + 4, 1, tmp2, 4);
  }else {
     //printf("dcache stall (%d)\n", is_Fetching(dcache));
  }

  MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
  MEM_WB_REG.rt = EX_MEM_REG.rt;
  MEM_WB_REG.rd = EX_MEM_REG.rd;
  MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
  // CACHE CHANGE
  if (is_Fetching(dcache) > 0) {
     memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG)); 
 }
  // CACHE CHANGE
}

void mem_STURB() {
  int32_t firstbyte = CURRENT_STATE.REGS[EX_MEM_REG.rt] & 0xff;
  uint64_t base_address = EX_MEM_REG.ALU_val + EX_MEM_REG.address;
  uint32_t tmp1 = ~0xff;
  uint32_t tmp2 = mem_read_32(base_address);
  uint32_t value = tmp1 & tmp2;
  uint32_t result = value | firstbyte;
  // CACHE CHANGE
  cache_update(dcache, base_address, 1, CURRENT_STATE.REGS[EX_MEM_REG.rt],1); 
  
  MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
  MEM_WB_REG.rt = EX_MEM_REG.rt;
  MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
  // CACHE CHANGE
  if (is_Fetching(dcache) > 0) {
     memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG)); 
 }
  // CACHE CHANGE
}

void mem_STURH() {
  int32_t t2bytes = CURRENT_STATE.REGS[EX_MEM_REG.rt] & 0xffff;
  uint64_t base_address = EX_MEM_REG.ALU_val + EX_MEM_REG.address;
  uint32_t tmp1 = ~0xffff;
  uint32_t tmp2 = mem_read_32(base_address);
  uint32_t value = tmp1 & tmp2;
  uint32_t result = value | t2bytes;
  // CACHE CHANGE
  cache_update(dcache, base_address, 1, CURRENT_STATE.REGS[EX_MEM_REG.rt], 2);
  // CACHE CHANGE
                                                                 
  MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;
  MEM_WB_REG.rt = EX_MEM_REG.rt;
  MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
  // CACHE CHANGE
  if (is_Fetching(dcache) > 0) {
     memset(&MEM_WB_REG, 0, sizeof(MEM_WB_REG)); 
  }
  // CACHE CHANGE
}

void mem_SUB() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_SUBS() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_SUBI() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_SUBIS() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_MUL() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_SDIV() { // signed divide
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_UDIV() { // unsigned divide
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_CMP() {
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name; 
}

void mem_HLT(){
   MEM_WB_REG.ALU_val = EX_MEM_REG.ALU_val;                                     
   MEM_WB_REG.rd = EX_MEM_REG.rd;                                       
   MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;  
}

void pipe_stage_mem()
{
    if (EX_MEM_REG.fff == 1)
    {
        DE_EX_REG.flag_forward = 1;
        DE_EX_REG.fFLAG_N = EX_MEM_REG.FLAG_N;
        DE_EX_REG.fFLAG_Z = EX_MEM_REG.FLAG_Z;
    }
	
    stall = 0;
    if ((EX_MEM_REG.instr_name == LDUR ||
         EX_MEM_REG.instr_name == LDURB ||
         EX_MEM_REG.instr_name == LDURH) &&
         (DE_EX_REG.instr_name == STUR ||
         DE_EX_REG.instr_name == STURB ||
         DE_EX_REG.instr_name == STURH)){
         if (EX_MEM_REG.rt == DE_EX_REG.rt){
             stall = 1;
         }
    }else if (EX_MEM_REG.instr_name == LDUR ||
        EX_MEM_REG.instr_name == LDURB ||
        EX_MEM_REG.instr_name == LDURH){
        if (EX_MEM_REG.rt == DE_EX_REG.rn){
            stall = 1;
        }
    }else if (EX_MEM_REG.rd == DE_EX_REG.rn){
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rn;
           DE_EX_REG.fval = EX_MEM_REG.ALU_val;
    }

    if (EX_MEM_REG.instr_name == LDUR ||
        EX_MEM_REG.instr_name == LDURB ||
        EX_MEM_REG.instr_name == LDURH && did_stall == 0){
        if (EX_MEM_REG.rt == DE_EX_REG.rm){
	   stall = 1;
        }
    }else if (EX_MEM_REG.rd == DE_EX_REG.rm){
           DE_EX_REG.forward = 1;
           DE_EX_REG.rf = DE_EX_REG.rm;
           DE_EX_REG.fval = EX_MEM_REG.ALU_val;
    }

    switch(EX_MEM_REG.instr_name) {
        case 0: {
             MEM_WB_REG.instr_name = EX_MEM_REG.instr_name;
             break;
        } 
        case ADD: {
             mem_ADD();
             break;
        }
        case ADDI: {
             mem_ADDI();
             break;
        }
        case ADDIS: {
             mem_ADDIS();
             MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
             MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
  	     MEM_WB_REG.fff = 1;
             EX_MEM_REG.fff = 0;
             break;
        }
        case ADDS: {
             mem_ADDS();
   	     MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
   	     MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
             MEM_WB_REG.fff = 1;
 	     EX_MEM_REG.fff = 0;
	     break;
        }
        case AND: {
             mem_AND();
             break;
        }
        case ANDS: {
             mem_ANDS();
             MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
             MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
             MEM_WB_REG.fff = 1;
	     EX_MEM_REG.fff = 0;
  	}     break;
        case B: {
             mem_B();
             break;
        }
        case BL: {
             mem_BL();
             break;
        }
        case BR: {
             mem_BR();
             break;
        }
        case CBZ: {
             mem_CBZ();
             break;
        }
        case CBNZ: {
             mem_CBNZ();
             break;
        }
        case LDUR: {
             mem_LDUR();
             break;
        }
        case LDURB: {
             mem_LDURB();
             break;
        }
        case LDURH: {
             mem_LDURH();
             break;
        }
        case BCOND: {
             mem_BCOND();
             MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
             MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
             MEM_WB_REG.fff = 1;
	     EX_MEM_REG.fff = 0;
	     break;
        }
        case CMP: {
             mem_CMP();
             MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
             MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
             MEM_WB_REG.fff = 1;
	     EX_MEM_REG.fff = 0;
	     break;
        }
        case LSL: {
             mem_LSL();
             break;
        }
        case LSR: {
             mem_LSR();
             break;
        }
        case MOVZ: {
             mem_MOVZ();
             break;
        }
        case ORR: {
             mem_ORR();
             break;
        }
        case EOR: {
             mem_EOR();
             break;
        }
        case STUR: {
             mem_STUR();
             break;
        }
        case STURB: {
             mem_STURB();
             break;
        }
        case STURH: {
             mem_STURH();
             break;
        }
        case SUB: {
             mem_SUB();
             break;
        }
        case SUBS: {
             mem_SUBS();
             MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
             MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
             MEM_WB_REG.fff = 1;
	     EX_MEM_REG.fff = 0;
	     break;
        }
        case SUBI: {
             mem_SUBI();
             break;
        }
        case SUBIS: {
             mem_SUBIS();
             MEM_WB_REG.FLAG_Z = EX_MEM_REG.FLAG_Z;
             MEM_WB_REG.FLAG_N = EX_MEM_REG.FLAG_N;
	     MEM_WB_REG.fff = 1;
             EX_MEM_REG.fff = 0;
	         break;
        }
        case MUL: {
             mem_MUL();
             break;
        }
        case SDIV: {
             mem_SDIV();
             break;
        }
        case UDIV: {
             mem_UDIV();
             break;
        }
        case HLT: {
             mem_HLT();
             break;
        }
        default: {
           printf(" Unknown Instruction in mem, %d\n", EX_MEM_REG.instr_name);
        }
    }
}

/************************************EXECUTE FUNCTIONS****************************************/

/* choose between value to forward and register value */
int64_t forVal(int reg) {
   if (DE_EX_REG.forward == 1 && DE_EX_REG.rf == reg){
      return DE_EX_REG.fval;
   }
   else {
      return CURRENT_STATE.REGS[reg];
   }
}
 
void exec_ALU(int64_t op1, int64_t op2, int64_t shamt, int64_t *result, int *Z_flag, int *N_flag, int name) {
     switch (name) {
          case ADD:
             op2 = op2 << shamt;
             *result = op1 + op2;
             break;
          case ADDI:
             *result = op1 + op2;
             break;
          case ADDIS:
             *result = op1 + op2;
             break;
          case ADDS:
             op2 = op2 << shamt;
             *result = op1 + op2;
             break;
          case SUB:
             op2 = op2 << shamt;
             *result = op1 - op2;
             break;
          case SUBI:
             *result = op1 - op2;
             break;
          case SUBIS:
             *result = op1 - op2;
             break;
          case SUBS:
             op2 = op2 << shamt;
             *result = op1 - op2;
             break;
          case AND: 
             op2 = op2 << shamt;
             *result = op1 & op2;
             break;
          case ANDS: 
             op2 = op2 << shamt;
             *result = op1 & op2;
             break;
          case EOR:
             op2 = op2 << shamt;
             *result = op1 ^ op2;
             break;
          case ORR:
             op2 = op2 << shamt;
             *result = op1 | op2;
             break;
          case MUL:
             op2 = op2 << shamt;
             *result = op1 * op2;
             break;
          default:
             fprintf(stderr, "Error: unknown ALU instruction\n");
             exit(1);
     }
     if (*result < 0){
        *N_flag = 1;
     }else{
        *N_flag = 0;
     }
     if (*result == 0){
        *Z_flag = 1;
     }else{
        *Z_flag = 0;
     }
}

void exec_ADD() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, ADD);
   EX_MEM_REG.ALU_val = result;

   //copy values to the next pipeline stage
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
   EX_MEM_REG.rd = DE_EX_REG.rd;
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address;
}

void exec_ADDS() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, ADDS);
   EX_MEM_REG.ALU_val = result; 

   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4; 
   EX_MEM_REG.rd = DE_EX_REG.rd;
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address;

/* copy the flags for the next stage */
   EX_MEM_REG.FLAG_Z = Z;
   EX_MEM_REG.FLAG_N = N;
   EX_MEM_REG.fff = 1;
}

void exec_ADDI() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = DE_EX_REG.imm;
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, ADDI);
   EX_MEM_REG.ALU_val = result;
                                          
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
   EX_MEM_REG.rd = DE_EX_REG.rd;         
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_ADDIS() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = DE_EX_REG.imm;
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, ADDIS);

   EX_MEM_REG.ALU_val = result;

   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
   EX_MEM_REG.rd = DE_EX_REG.rd;
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address;    

   EX_MEM_REG.FLAG_Z = Z;
   EX_MEM_REG.FLAG_N = N;
   EX_MEM_REG.fff = 1;
}

void exec_AND(){
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, AND);
   EX_MEM_REG.ALU_val = result;                                                 
                                                                                
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                          
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_ANDS() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int ALUop;
   int Z = 0;
   int N = 0;
    
   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);   
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, ANDS);
   EX_MEM_REG.ALU_val = result;
 
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  

   EX_MEM_REG.FLAG_Z = Z;
   EX_MEM_REG.FLAG_N = N;
   EX_MEM_REG.fff = 1;
}

void exec_B() {
   uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;
   if (DE_EX_REG.ptaken == 0){ // we predicted incorrectly
      CURRENT_STATE.PC = PC_calc; // set the PC to target address of the branch
   }
   EX_MEM_REG.taken = 1;
   EX_MEM_REG.ALU_val = 0;
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;
}

void exec_BR() {
   
   uint64_t target = forVal(DE_EX_REG.rn);
   DE_EX_REG.address = target;
   if (DE_EX_REG.ptaken == 0){   // if ptaken == 1 then the PC is in BTB.
      CURRENT_STATE.PC = target;
   }
   EX_MEM_REG.taken = 1; 
   EX_MEM_REG.ALU_val = 0; 
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;   
}	

void exec_BCOND() {

   int Z;
   int N;

   EX_MEM_REG.ALU_val = 0;
   EX_MEM_REG.rd = DE_EX_REG.rd;
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address;

   if (DE_EX_REG.flag_forward == 1) {
       Z = DE_EX_REG.fFLAG_Z;
       N = DE_EX_REG.fFLAG_N;
   }
   else {
       int Z = CURRENT_STATE.FLAG_Z;
       int N = CURRENT_STATE.FLAG_N;
   }

   if (DE_EX_REG.rn == 0){ 
      //BEQ
      if (Z == 1){
	 uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address; 
         EX_MEM_REG.taken = 1;
         if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else {
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
         //CURRENT_STATE.PC = CURRENT_STATE.PC - 4; // This is because of PC behavior of lab2
      }
      
   }else if (DE_EX_REG.rn == 1){ //BNE
      if (Z == 0){
         uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address; 
         EX_MEM_REG.taken = 1;
         if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else{     // Branch will not be taken. So let the stages proceed with no change.
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4; 
         //CURRENT_STATE.PC = CURRENT_STATE.PC - 4; // CHECK 

      }
   }else if (DE_EX_REG.rn == 12){ //BGT
      if (Z == 0 && N == 0) {
         uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;           
         EX_MEM_REG.taken = 1;
         if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else {
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
         //CURRENT_STATE.PC = CURRENT_STATE.PC - 4; // CHECK should it be commented out
      }
   }else if (DE_EX_REG.rn == 11){ //BLT
      if (N != 0) {
         uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;           
         EX_MEM_REG.taken = 1;
         if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else{
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;    
         //CURRENT_STATE.PC = CURRENT_STATE.PC - 4; // CHECK should it be commented out

      }
   }else if (DE_EX_REG.rn == 13){ //BLE
      if (!((Z == 0) && (N == 0))){
         uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;           
         EX_MEM_REG.taken = 1;
         if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
     }else {
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;        
         //CURRENT_STATE.PC = CURRENT_STATE.PC - 4;   // CHECK should it be commented out
     }
   }else if (DE_EX_REG.rn == 10){ //BGE
      if (N == 0){
         uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;           
         EX_MEM_REG.taken = 1;
         if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else {
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;          
         //CURRENT_STATE.PC = CURRENT_STATE.PC - 4;     // CHECK commented out?
      }
   }else {
      printf("ERROR: Not a BCOND instruction\n");      
   }
   DE_EX_REG.flag_forward = 0; // clean up
   DE_EX_REG.instr_name = 0;
}

void exec_CBZ() {

   EX_MEM_REG.ALU_val = 0;
   EX_MEM_REG.rd = DE_EX_REG.rd;
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address;

   if (forVal(DE_EX_REG.rn) == 0){
      uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;
	  EX_MEM_REG.taken = 1;
	  if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else {
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
      }
   DE_EX_REG.flag_forward == 0;
   DE_EX_REG.instr_name = 0;
}

void exec_CBNZ() {
   EX_MEM_REG.ALU_val = 0;
   EX_MEM_REG.rd = DE_EX_REG.rd;
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.imm = DE_EX_REG.imm;
   EX_MEM_REG.address = DE_EX_REG.address;

   if (forVal(DE_EX_REG.rn) != 0){
      uint64_t PC_calc = DE_EX_REG.PC_4 - 4 + DE_EX_REG.address;
      EX_MEM_REG.taken = 1;
	  if (DE_EX_REG.ptaken == 0){
            CURRENT_STATE.PC = PC_calc;
         }
      }else {
         EX_MEM_REG.taken = 0;
         EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
      }
   DE_EX_REG.flag_forward == 0;
   DE_EX_REG.instr_name = 0;
}

void exec_LDUR() {
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rn);                                                      
   EX_MEM_REG.rt = DE_EX_REG.rt;                                        
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_LDURB() {
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rn);                   
   EX_MEM_REG.rt = DE_EX_REG.rt;                                        
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_LDURH() {
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rn);                   
   EX_MEM_REG.rt = DE_EX_REG.rt;                                        
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;
}

void exec_LSL() {

   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rn) << DE_EX_REG.shamt;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;   
}

void exec_LSR() {

   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rn) >> DE_EX_REG.shamt;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  
}

void exec_MOVZ() {

   EX_MEM_REG.ALU_val = DE_EX_REG.wimm << (DE_EX_REG.hw * 16);
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
   EX_MEM_REG.wimm = DE_EX_REG.wimm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_ORR() {

   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rm) | (forVal(DE_EX_REG.rn) << DE_EX_REG.shamt);
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  
}

void exec_EOR() {

   EX_MEM_REG.ALU_val = forVal(DE_EX_REG.rm) ^ (forVal(DE_EX_REG.rn) << DE_EX_REG.shamt);
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_STUR() {
   uint64_t base_address = forVal(DE_EX_REG.rn);
   EX_MEM_REG.ALU_val = base_address;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rt = DE_EX_REG.rt;                                        
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  
}

void exec_STURB() {
   uint64_t base_address = forVal(DE_EX_REG.rn);                
   EX_MEM_REG.ALU_val = base_address;                                           
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rt = DE_EX_REG.rt;                                        
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_STURH() {
   uint64_t base_address = forVal(DE_EX_REG.rn);                
   EX_MEM_REG.ALU_val = base_address;                                           
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rt = DE_EX_REG.rt;                                        
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_SUB() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, SUB);
   EX_MEM_REG.ALU_val = result;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;
}

void exec_SUBS() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, SUBS);
   EX_MEM_REG.ALU_val = result;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;     

   EX_MEM_REG.FLAG_Z = Z;
   EX_MEM_REG.FLAG_N = N;
   EX_MEM_REG.fff = 1;
}

void exec_SUBI() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = DE_EX_REG.imm;
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, SUBI);
   EX_MEM_REG.ALU_val = result;                                                 
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  
}

void exec_SUBIS() {
   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = DE_EX_REG.imm;
   exec_ALU(op1, op2, DE_EX_REG.shamt, &result, &Z, &N, SUBIS);
   EX_MEM_REG.ALU_val = result;                                                 
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;   

   EX_MEM_REG.FLAG_Z = Z;
   EX_MEM_REG.FLAG_N = N;
   EX_MEM_REG.fff = 1;
}

void exec_MUL() {
   int64_t op1;
   int64_t op2;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   EX_MEM_REG.ALU_val = op1 * op2;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  
}

void exec_SDIV() { // signed divide
   int64_t op1;
   int64_t op2;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   EX_MEM_REG.ALU_val = op1 / op2;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void exec_UDIV() { // unsigned divide
   uint64_t op1;
   uint64_t op2;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   EX_MEM_REG.ALU_val = op1 / op2;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;    
}

void exec_CMP() {

   int64_t op1;
   int64_t op2;
   int64_t result;
   int Z = 0;
   int N = 0;

   op1 = forVal(DE_EX_REG.rn);
   op2 = forVal(DE_EX_REG.rm);
   exec_ALU(op1, op2, 0, &result, &Z, &N, SUBS);
   EX_MEM_REG.ALU_val = result;
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                    
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address;  
   EX_MEM_REG.FLAG_Z = Z;
   EX_MEM_REG.FLAG_N = N;
   EX_MEM_REG.fff = 1;
}

void exec_HLT(){
   EX_MEM_REG.ALU_val = 0;                                                 
   EX_MEM_REG.calc_PC = DE_EX_REG.PC_4;                                          
   EX_MEM_REG.rd = DE_EX_REG.rd;                                        
   EX_MEM_REG.instr_name = DE_EX_REG.instr_name;                                      
   EX_MEM_REG.imm = DE_EX_REG.imm;                                      
   EX_MEM_REG.address = DE_EX_REG.address; 
}

void pipe_stage_execute()
{
   // CACHE CHANGE
   if (is_Fetching(dcache) > 0) {
      return;
   }   
   // CACHE CHANGE
   squash = 0;
   /******************Control Dependency stalling***********************/
   if (stall == 1){
      return;
   }
   switch(DE_EX_REG.instr_name) {   /* A large switch to execute instructions */
       case 0: {
            EX_MEM_REG.instr_name = DE_EX_REG.instr_name;
            break;
       }
       case ADD: {
            exec_ADD();
            break;
       }
       case ADDI: {
            exec_ADDI();
            break;
       }
       case ADDIS: {
            exec_ADDIS();
            break;
       }
       case ADDS: {
            exec_ADDS();
            break;
       }
       case AND: {
            exec_AND();
            break;
       }
       case ANDS: {
            exec_ANDS();
            break;
       }
       case B: {
            exec_B();
            break;
       }
       case BR: {
            exec_BR();
            break;
       }
       case CBZ: {
            exec_CBZ();
            break;
       }
       case CBNZ: {
            exec_CBNZ();
            break;
       }
       case LDUR: {
            exec_LDUR();
            break;
       }
       case LDURB: {
            exec_LDURB();
            break;
       }
       case LDURH: {
            exec_LDURH();
            break;
       }
       case BCOND: {
            exec_BCOND();
            break;
       }
       case CMP: {
            exec_CMP();
            break;
       }
       case LSL: {
            exec_LSL();
            break;
       }
       case LSR: {
            exec_LSR();
            break;
       }
       case MOVZ: {
            exec_MOVZ();
            break;
       }
       case ORR: {
            exec_ORR();
            break;
       }
       case EOR: {
            exec_EOR();
            break;
       }
       case STUR: {
            exec_STUR();
            break;
       }
       case STURB: {
            exec_STURB();
            break;
       }
       case STURH: {
            exec_STURH();
            break;
       }
       case SUB: {
            exec_SUB();
            break;
       }
       case SUBS: {
            exec_SUBS();
            break;
       }
       case SUBI: {
            exec_SUBI();
            break;
       }
       case SUBIS: {
            exec_SUBIS();
            break;
       }
       case MUL: {
            exec_MUL();
            break;
       }
       case SDIV: {
            exec_SDIV();
            break;
       }
       case UDIV: {
            exec_UDIV();
            break;
       }
       case HLT: {
            exec_HLT();
            break;
       }
       default: {
          printf(" Error: unknown execute instruction, %d\n", DE_EX_REG.instr_name);
       }
    }
    if (DE_EX_REG.PC_4 != 0){ //make sure address is valid
       if (isSquash(DE_EX_REG.PC_4 - 4)){
          // CACHE CHANGE
          // I am fetching a group of 8 instructions now and
          // the instruction that I need to fetch belongs to
          // this range of 8 instructions; it need NOT BE EXACTLY EQUAL!!
           if (is_Fetching(icache) > 0) {
              uint64_t adrfet_beg = icache->cst.saddr;
              uint64_t adrfet_end = adrfet_beg + 32;
              if ((adrfet_beg <= CURRENT_STATE.PC) &&
                  (CURRENT_STATE.PC  <= adrfet_end )) {
                  printf("Overlap miss for (0x%lx) in (0x%lx)-(0x%lx) canceled\n", CURRENT_STATE.PC, adrfet_beg, adrfet_end);
              }
              else { resetFetching(icache); }
           }
          // CACHE CHANGE
          bp_update(DE_EX_REG.PC_4 - 4);
          memset(&DE_EX_REG, 0, sizeof(DE_EX_REG));
          memset(&IF_DE_REG, 0, sizeof(IF_DE_REG));
          squash = 1;
       }
       else {
         if(DE_EX_REG.PC_4 != 0){ //make sure address is valid
           bp_update(DE_EX_REG.PC_4 - 4);
         }
       }
    }
}
/************************************DECODE FUNCTIONS****************************************/

void getinfoFB(uint32_t instr)
{
   baddr = instr & 0x03ffffff;
   //next, we need to sign-extend this...
   baddr = baddr << 38;
   baddr = baddr >> 38;
   baddr = baddr << 2; //to ensure 32-bit alignment
/********Additional Pipeline Register management *******************/
   DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
   DE_EX_REG.rm = -1;
   DE_EX_REG.rd = -1;
   if (instr_name == BR){//in the case of BR, we actually need a register...
       rn = cur_instr & 0x000003e0;
       rn = rn >> 5;
       DE_EX_REG.rn = rn;
   }else{
       DE_EX_REG.rn = -1;
   }
   /**********these are the same no matter what**********************/
   DE_EX_REG.imm = 0;
   DE_EX_REG.address = baddr;
   DE_EX_REG.shamt = 0;
   DE_EX_REG.hw = 0;
   DE_EX_REG.wimm = 0;
    
}

void getinfoFCB(uint32_t instr)
{
  cbaddr = instr & 0x00ffffe0;
  cbaddr = cbaddr >> 5;
  cbaddr = cbaddr << 45;
  cbaddr = cbaddr >> 45;
  cbaddr = cbaddr << 2;
  rt = instr & 0x0000001f; 

  DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
  DE_EX_REG.rm = -1;
  DE_EX_REG.rn = rt;
  DE_EX_REG.rd = -1;
  DE_EX_REG.shamt = 0;
  DE_EX_REG.hw = 0;
  DE_EX_REG.rt = rt;
  DE_EX_REG.imm = rt;
  DE_EX_REG.address = cbaddr;
}

void getinfoFR(uint32_t instr)
{
  rd = instr & 0x0000001f; 
  rn = instr & 0x000003e0;
  rn = rn >> 5;
  shamt = instr & 0x0000fc00;
  shamt = shamt >> 10;
  rm = instr & 0x001f0000;
  rm = rm >> 16;
  if (DE_EX_REG.instr_name == SDIV && shamt != 2){
      DE_EX_REG.instr_name = UDIV;
  }
  if (DE_EX_REG.instr_name == LSR){
      shamt = instr & 0x003f0000;
      shamt = shamt >> 16;
  }
  if (DE_EX_REG.instr_name == LSL){
      shamt = instr & 0x0000fc00;
      shamt = shamt >> 10;
      shamt = 63 - shamt;
  }
  
  DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
  DE_EX_REG.rd = rd;
  DE_EX_REG.rn = rn;
  DE_EX_REG.rn_val = CURRENT_STATE.REGS[rn];
  DE_EX_REG.rm = rm;
  DE_EX_REG.rm_val = CURRENT_STATE.REGS[rm];
  
  DE_EX_REG.imm = 0;
  DE_EX_REG.address = 0;
  DE_EX_REG.shamt = shamt;
  DE_EX_REG.hw = 0; 
}

void getinfoFD(uint32_t instr)
{
  rt = instr & 0x0000001f;
  rn = instr & 0x000003e0;
  rn = rn >> 5;
  address = instr & 0x001ff000; 
  address = address >> 12;

  DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
  DE_EX_REG.rn = rn;
  DE_EX_REG.rn_val = CURRENT_STATE.REGS[rn];
  DE_EX_REG.rt = rt;
  DE_EX_REG.address = address;

  DE_EX_REG.rm = -1;
  DE_EX_REG.rd = -1;
  DE_EX_REG.imm = 0;
  DE_EX_REG.shamt = 0;
  DE_EX_REG.hw = 0;
}

void getinfoFI(uint32_t instr)
{
  rd = instr & 0x0000001f;
  rn = instr & 0x000003e0;
  rn = rn >> 5;
  imm = instr & 0x003ffc00;
  imm = imm >> 10;
  
  DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
  DE_EX_REG.rd = rd;
  DE_EX_REG.rn = rn;
  DE_EX_REG.rn_val = CURRENT_STATE.REGS[rn];
  DE_EX_REG.imm = imm;

  DE_EX_REG.rm = 0;
  DE_EX_REG.address = 0;
  DE_EX_REG.shamt = 0;
  DE_EX_REG.hw = 0;
}

void getinfoFIW(uint32_t instr)
{
  if (DE_EX_REG.instr_name == HLT){
     if (did_halt == 0){
        CURRENT_STATE.PC = CURRENT_STATE.PC + 4;
        did_halt = 1;
     }
     DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
     DE_EX_REG.rd = -1;
     DE_EX_REG.rn = -1;
     DE_EX_REG.rm = -1;
     DE_EX_REG.imm = 0;
     DE_EX_REG.address = -1;
     DE_EX_REG.shamt = -1;
     DE_EX_REG.hw = -1;
     return;
  }
     
  rd = instr & 0x0000001f;
  wimm = instr & 0x001fffe0;
  wimm = wimm >> 5;
  hw = instr & 0x00600000;
  hw = hw >> 21;

  DE_EX_REG.PC_4 = IF_DE_REG.PC_4;
  DE_EX_REG.rd = rd;
  DE_EX_REG.wimm = wimm;
  DE_EX_REG.hw = hw;

  DE_EX_REG.rm = 0;
  DE_EX_REG.address = 0;
  DE_EX_REG.shamt = 0;
  DE_EX_REG.rn = 0;
  DE_EX_REG.imm = 0;
}
 
void set_format_ops(uint32_t instr)
{
  uint32_t opc = instr >> 21; //get the top 11 bits of the instruction
  if (opc == 0){
     format = 0;
     instr_name = 0;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     return;
  }else if (opc >= 160 && opc <= 191){
     format = FB; 
     instr_name = B;
     DE_EX_REG.instr_name = instr_name;
     getinfoFB(instr);
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     memset(&IF_DE_REG, 0, sizeof(IF_DE_REG)); //need to clean this out, otherwise old branch instruction will still be in register
  }else if (opc == 448){
     format = FD;
     instr_name = STURB;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     DE_EX_REG.instr_name = instr_name;
     getinfoFD(instr);
  }else if (opc == 450){
     format = FD;
     instr_name = LDURB;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFD(instr);
  }else if (opc >= 672 && opc <= 679){
     format = FCB;
     instr_name = BCOND;
     DE_EX_REG.instr_name = instr_name;
     getinfoFCB(instr);
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     memset(&IF_DE_REG, 0, sizeof(IF_DE_REG)); //need to clean this out, otherwise old branch instruction will still be in register
  }else if (opc == 1112){
     format = FR;
     instr_name = ADD;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc == 1368){
     format = FR;
     instr_name = ADDS;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFR(instr);
  }else if (opc >= 1160 && opc <= 1161){
     format = FI;
     instr_name = ADDI;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFI(instr);
  }else if (opc >= 1416 && opc <= 1417){
     format = FI;
     instr_name = ADDIS;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFI(instr);
  }else if (opc == 1104){
     format = FR;
     instr_name = AND;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFR(instr);
  }else if (opc == 1872){
     format = FR;
     instr_name = ANDS;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFR(instr);
  }else if (opc >= 1440 && opc <= 1447){
     format = FCB;
     instr_name = CBZ;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFCB(instr);
     memset(&IF_DE_REG, 0, sizeof(IF_DE_REG)); //need to clean this out, otherwise old branch instruction will still be in register
  }else if (opc >= 1448 && opc <= 1455){
     format = FCB;
     instr_name = CBNZ;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFCB(instr);
     //stall_b = 1;
     memset(&IF_DE_REG, 0, sizeof(IF_DE_REG)); //need to clean this out, otherwise old branch instruction will still be in register
  }else if (opc == 1616){
     format = FR;
     instr_name = EOR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFR(instr);
  }else if (opc == 1986){
     format = FD;
     instr_name = LDUR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFD(instr);
  }else if (opc == 962){
     format = FD;
     instr_name = LDURH;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFD(instr);
  }else if (opc == 1691){
     format = FR;
     // Added if statement to fix a bug in lab2
     if (((instr & 0x0000fc00)>> 10) != 0x3f)
        instr_name = LSL;
     else
        instr_name = LSR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc == 1690){
     format = FR;
     // Added if statement to fix a bug in lab2
     if (((instr & 0x0000fc00)>> 10) != 0x3f)
        instr_name = LSL;
     else
        instr_name = LSR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken;
     getinfoFR(instr);
  }else if (opc >= 1684 && opc <= 1687){
     format = FIW;
     instr_name = MOVZ;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFIW(instr);
  }else if (opc == 1360){
     format = FR;
     instr_name = ORR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc == 1984){
     format = FD;
     instr_name = STUR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFD(instr);
  }else if (opc == 960){
     format = FD;
     instr_name = STURH;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFD(instr);
  }else if (opc == 1472){
     format = FD;
     instr_name = STURW;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFD(instr);
  }else if (opc == 1624){
     format = FR;
     instr_name = SUB;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc >= 1672 && opc <= 1673){
     format = FI;
     instr_name = SUBI;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFI(instr);
  }else if (opc == 1880){
     format = FR;
     instr_name = SUBS;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc >= 1928 && opc <= 1929){
     format = FI;
     instr_name = SUBIS;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFI(instr);
  }else if (opc == 1240){
     format = FR;
     instr_name = MUL;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc == 1238){
     format = FR;
     instr_name = SDIV;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
  }else if (opc >= 1184 && opc <= 1215){
     format = FB;
     instr_name = BL;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFB(instr);
  }else if (opc == 1712){
     format = FR;
     instr_name = BR;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFR(instr);
     memset(&IF_DE_REG, 0, sizeof(IF_DE_REG)); //need to clean this out, otherwise old branch instruction will still be in register
  }else if (opc == 1698){
     format = FIW;
     instr_name = HLT;
     DE_EX_REG.instr_name = instr_name;
     DE_EX_REG.ptaken = IF_DE_REG.ptaken; 
     getinfoFIW(instr);
  }else {
     // fprintf(stderr, "Error: Instruction not covered, %d\n", opc);
     exit(1);
  }
}

void pipe_stage_decode() {
   // CACHE CHANGE
      int IF_wait = dcache_start - icache_start; 
      int diff = 0; 
      if (IF_wait > 0 && IF_wait <= 2) {
        diff = IF_wait - 1; 
      }
      if (is_Fetching(dcache) > diff) {
        return;
      }
   // CACHE CHANGE
   if (stall == 1){
      return;
   }
   cur_instr = IF_DE_REG.instruction;
   set_format_ops(cur_instr);
}

void pipe_stage_fetch()
{
  // CACHE CHANGE
  if (is_Fetching(dcache) == 50)                   
    {                                                         
       dcache_start = cyc_cnt + 1;                                 
    }                                                       
    int output;                                               
    int IF_wait = dcache_start - icache_start;                          
    int diff = 0;                                        
    if (IF_wait > 0 && IF_wait <= 2)                      
    {                                                    
       diff = IF_wait;                                
    }                                                  
    if (is_Fetching(dcache) > diff)                
    {                                              
       if (is_Fetching(icache) > 1)                
       {                                         
          output = cache_update(icache, CURRENT_STATE.PC, 0,0,0);
       }                                                      
          return;                                            
    }                           
  // CACHE CHANGE
  if (stall == 1){
  // CACHE CHANGE
 if (is_Fetching(icache) > 1)                            
       {                                                      
          output = cache_update(icache, CURRENT_STATE.PC, 0,0,0);
       } 
  // CACHE CHANGE
     return;
  }
  // CACHE CHANGE
  if (squash == 1){
       if (is_Fetching(icache) > 1)
       {
          output = cache_update(icache, CURRENT_STATE.PC, 0, 0, 0);
       }
      squash = 0;
     return;
  }
  // CACHE CHANGE
  else {   
     if (did_halt == 0){
         if (squash == 1){
            // CACHE CHANGE
             if (is_Fetching(icache) > 1) {
               output = cache_update(icache, CURRENT_STATE.PC, 0, 0, 0);
             }
               output = cache_update(icache, CURRENT_STATE.PC, 0, 0, 0);
            // CACHE CHANGE
            // Will set squash to 0 in the execute stage
             memset(&IF_DE_REG, 0, sizeof(IF_DE_REG));
             return;
         }else {
           output = cache_update(icache, CURRENT_STATE.PC, 0, 0, 0);
           if (is_Fetching(icache) == 50) {
            icache_start = cyc_cnt + 1;
          }
          if (is_Fetching(icache) > 0) {
           // Stalling for cache fill
           memset(&IF_DE_REG, 0, sizeof(IF_DE_REG)); 
           //printf("icache bubble (%d)\n", icache->cst.pend_cycles - 1);
           return;
          }
          else {
           output = cache_update(icache, CURRENT_STATE.PC, 0, 0, 0);
           IF_DE_REG.instruction = output;
           //printf("icache hit (0x%lx) at cycle %d\n", CURRENT_STATE.PC, cyc_cnt + 1 );
           IF_DE_REG.PC_4 = CURRENT_STATE.PC + 4;
           IF_DE_REG.instruction = mem_read_32(CURRENT_STATE.PC);
           CURRENT_STATE.PC = IF_DE_REG.PC_4;
           bp_predict(CURRENT_STATE.PC - 4, &IF_DE_REG.ptaken); 
           // to predict, we need the current PC and 
           // a variable to store the result in
         }
       } 
     }
     else { // We have already halted, but we will check if it is a cache miss or not.
       
         output = cache_update(icache, CURRENT_STATE.PC -4, 0, 0, 0);
         if (is_Fetching(icache) == 50) {
          //printf("icache miss (0x%lx) at cycle %d\n", CURRENT_STATE.PC -4, cyc_cnt+1);
          CURRENT_STATE.PC = CURRENT_STATE.PC -4;
          icache_start = cyc_cnt + 1;
        }
     }
      
  }  
}
