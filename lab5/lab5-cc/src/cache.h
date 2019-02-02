/*
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 *
 */
#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdint.h>
#include "shell.h"

#define WDSIZE 4
#define NCOR 4

typedef struct
{
   int blockind; //block index
   int vb; //valid bit
   int db; //dirty bit
   int lruind; //LRU index
   int tbits; //tag bits
   int memSize;
   char* data; //contains the actual 32 bytes of data in the block

} block;

typedef struct
{
   int lrusetcnt; //keeps track of the LRU set
   int setind;
   block** blocks;
} set;

typedef struct
{
   int pend_cycles; //if positive, means cache is still busy fetching from main memory (or that cache is still working)
                 //if 0, then cache is available   
   block *sblock; //block currently saved to the cache
   uint64_t saddr; //the address the cache is currently working on
   
   int swrite; // whether the miss was a write miss or not: if 1, the miss was a write, if 0, then read
   uint32_t swriteData; //the data to be written
   int swriteSize; //how large the data is
   
   int sindbits; //index bits currently saved to the cache
   int ssbits; //set bits currently saved to the cache
   int stbits; //tag bits currently saved to cache

} cache_state;

typedef struct
{
   cache_state cst;
   /*int pend_cycles; //if positive, means cache is still busy fetching from main memory (or that cache is still working)
                 //if 0, then cache is available   
   block *sblock; //block currently saved to the cache
   uint64_t saddr; //the address the cache is currently working on
   
   int swrite; // whether the miss was a write miss or not: if 1, the miss was a write, if 0, then read
   uint32_t swriteData; //the data to be written
   int swriteSize; //how large the data is
   
   int sindbits; //index bits currently saved to the cache
   int ssbits; //set bits currently saved to the cache
   int stbits; //tag bits currently saved to cache */
   
   int blockcnt;
   int setcnt;
   set** sets; 
} cache_t;


cache_t *cache_new(int setcnt, int blockcnt);
void cache_destroy(cache_t *c);
uint32_t cache_update(cache_t *c, uint64_t addr, int updateType, uint32_t writeData, int writeSize, int cflag, int cyccnt);
int is_Fetching(cache_t *c);
block* blockatAddr(cache_t *c, uint64_t addr);
block* block2Fetch(cache_t *c);
void resetFetching(cache_t *c);
void decFetch(cache_t *c);
extern int crid;
extern int active_core[NCOR];
extern int ccrd_modelay[NCOR];
extern int ccrd_shrdelay[NCOR];
extern int ccrd_excldelay[NCOR];
extern int ccrd_invdelay[NCOR];
extern int ccwr_modelay[NCOR];
extern int ccwr_shrdelay[NCOR];
extern int ccwr_excldelay[NCOR];
extern int ccwr_invdelay[NCOR];
extern int write_exclusion_stall[NCOR];
extern cache_t *dcache[NCOR];
extern void writeMM(block* block, uint64_t addr);

#endif
