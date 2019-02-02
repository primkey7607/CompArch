/* Pranav Subramaniam, psubramaniam
 * ARM pipeline timing simulator
 *
 * CMSC 22200
 */
 
#include "bp.h"
#include "pipe.h"
#include <stdlib.h>
#include <stdio.h>
//#include "shell.h" not sure we need this
#include <string.h>
#include <assert.h>

#define FB            2 //B format
#define FCB           3 //CB format
#define BR              14
#define NCOR          4
    
bp_t bp_State[NCOR];
//memset(&bp_State, 0, sizeof(bp_t)); 

int gshare_predict(uint64_t PCcur)
{
   uint64_t braddr = (PCcur & 0x00000000000003fc) >> 2;
   uint8_t index = bp_State[crid].pred.ghr ^ braddr;
   int pred_st = (bp_State[crid].pred.pht)[index];
   if (pred_st == 0 || pred_st == 1){ // Predict not taken
      return 0;
   }else if (pred_st == 2 || pred_st == 3){  // Predict taken
      return 1;
   }else{
      printf("Error: unknown pred_st \n");
      return -1;
   } 
}

void bp_predict(uint64_t PCcur, int *taken)
{
   static int first_time = 1;
   int i;
   // Set the counters to 0 if first_time
   if (first_time) {
      first_time = 0;
      for (i = 0; i < 256; i ++) {
          bp_State[crid].pred.pht[i] = 0;
      }
   }
   /* Predict next PC */
   uint64_t index = (PCcur & 0x0000000000001ffc) >> 2;
   BTB_entry entry = (bp_State[crid].BTB)[index];
   if (entry.address == PCcur && entry.vb != 0){ 
      // A hit in the BTB
      if (entry.cond == 0 || gshare_predict(PCcur) == 1){
          //*PCres = PCcur + entry.target;
          *taken = 1;
          // NEED TO SET PC*************** DO WE NEED TO SET IF_ID_***PC4 also?
          CURRENT_STATE[crid].PC = entry.target;
      }else{
          *taken = 0;
      }
      return;
   }
   if (entry.address != PCcur || entry.vb == 0){ 
      // Did not find in BTB
      *taken = 0;
      return;
   } 
}

void bp_update(uint64_t PCcur)
{
    /* Update BTB */
   uint64_t index = (PCcur & 0x0000000000001ffc) >> 2;
   if (bp_State[crid].BTB[index].address != PCcur || bp_State[crid].BTB[index].vb == 0){ 
      // Not found in the BTB
      // Set the BTB entry
      if ((format[crid] == FB) || (format[crid] == BR)){
         bp_State[crid].BTB[index].vb = 1;   // Set the valid bit
         bp_State[crid].BTB[index].address = PCcur;   // Set the indexing address
         bp_State[crid].BTB[index].target = PCcur + DE_EX_REG[crid].address;  // Set the target address
         bp_State[crid].BTB[index].cond = 0;
      }else if (format[crid] == FCB){
         bp_State[crid].BTB[index].cond = 1;
         bp_State[crid].BTB[index].vb = 1;   // Set the valid bit
         bp_State[crid].BTB[index].address = PCcur;   // Set the indexing address
         bp_State[crid].BTB[index].target = PCcur + DE_EX_REG[crid].address;  // Set the target address
      }
    }else{ 
         //Found in BTB, assign target to handle indirect branches 
         bp_State[crid].BTB[index].target = PCcur + DE_EX_REG[crid].address; 
    }
   if (format[crid] == FCB){
    /* Update the gshare */
         uint64_t braddr = (PCcur & 0x00000000000003fc) >> 2;
         uint8_t index = bp_State[crid].pred.ghr ^ braddr;
         if (EX_MEM_REG[crid].taken == 0){
            if ((bp_State[crid].pred.pht)[index] > 0){
               (bp_State[crid].pred.pht)[index] = (bp_State[crid].pred.pht)[index] - 1;
            }
         }
         if (EX_MEM_REG[crid].taken == 1){
            if ((bp_State[crid].pred.pht)[index] < 3){
               (bp_State[crid].pred.pht)[index] = (bp_State[crid].pred.pht)[index] + 1;
            }
         }
    /* Update global history register */
         bp_State[crid].pred.ghr = (bp_State[crid].pred.ghr << 1) + EX_MEM_REG[crid].taken; 
   } 
}
/*must be determined before bp_update */
int isSquash(uint64_t PCcur){
   static int fnum = 1;
   uint64_t index = (PCcur & 0x0000000000001ffc) >> 2;
   BTB_entry entry = (bp_State[crid].BTB)[index];

   /* Instruction is branch, but predicted != actual direction */
   if ((format[crid] == FCB || format[crid] == FB || DE_EX_REG[crid].instr_name == BR) && 
      DE_EX_REG[crid].ptaken != EX_MEM_REG[crid].taken){
      //printf("Flush -- %d\n", fnum++);
      if (EX_MEM_REG[crid].taken == 0 && DE_EX_REG[crid].ptaken == 1){ 
         // Incorrect taken prediction; 
         // The pipe will be flushed.
          //printf("HERE\n");                                                   
        // Set the PC to fetch sequentially after branch.
         CURRENT_STATE[crid].PC = PCcur + 4;
      }
      // What if incorrect not taken prediction. That is already handled outside.
      // The bottom seems unreachable; and should  be removed.
      else if (EX_MEM_REG[crid].taken == 0)
         CURRENT_STATE[crid].PC = CURRENT_STATE[crid].PC - 4;
      return 1;
   /* Instruction is a branch, but isn't recognized as such */
   }
   else{ // Instruction is an indirect branch
      if (format[crid] == FCB || format[crid] == FB || DE_EX_REG[crid].instr_name == BR){
         if (EX_MEM_REG[crid].taken == 1){
            if (entry.vb == 1 && entry.address == PCcur){ //A hit in BTB
               if (entry.target != (PCcur + DE_EX_REG[crid].address)){
                  //printf("Flush3\n");
                  return 1;
               }
            }
         }
      } 
   }
   return 0;
}
