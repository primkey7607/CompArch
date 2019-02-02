/*
 * ARM pipeline timing simulator
 *
 * CMSC 22200
 */

#ifndef _BP_H_
#define _BP_H_

#include "shell.h"
#include "stdbool.h"
#include <limits.h>
#include <stdint.h>

//BTB entry struct
typedef struct
{
   int64_t address; //full 64 bits of the fetch stage PC
   int vb; //valid bit: 1 means entry valid, 0 means not
   int cond; //conditional flag: 1 means conditional, 0 means unconditional
   int64_t target; //target of the branch, and low two bits must always be 0
}BTB_entry;
   
typedef struct
{
   uint8_t ghr; //global branch history register (8 bits)
   int pht[256]; //pattern history table-should only have values 0 to 3 and not 1 to 4!

}gshare;

typedef struct
{
    /* gshare */
    gshare pred; 
    /* BTB */
    BTB_entry BTB[1024];
} bp_t;

/* global variable -- branch predictor state */
extern bp_t bp_State;
int isSquash(uint64_t PCcur); //need this for bp_update
void bp_predict(uint64_t PCcur, int *taken);
void bp_update(uint64_t PCcur);

#endif
