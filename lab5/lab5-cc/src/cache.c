/* Pranav Subramaniam, psubramaniam
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 *
 */
#include <string.h> 
#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int evict_SetBits = 0;      // This is an 8 bit quantity
int evict_tagBits = 0;      // This is a 19 bit quantity.
int evict_offset = 0;        // This is a 5 bit quantity.
uint64_t evict_addr;

void ccwriteMM(block *block, uint64_t address)
{
   
   uint32_t *writeData = (uint32_t*) block->data;
   int i;
   for (i = 0; i < 8; i++)
   {
      mem_write_32(address + 4 * i, writeData[i]);
   }
}

void update_others(cache_t *c, int crid, int setBits, int tbits, int rwflag, int cyc_cnt) {
    // Update other cores. Print transition messages. 
    int i;
    int cr;
    int found = 0;
    for (cr = 0; cr < NCOR; cr++) {
        if ((active_core[cr]) &&(cr != crid)) {
           for (i = 0; i < dcache[cr]->blockcnt; i = i + 1) {                        // Lookup
	       block *currBlock = (dcache[cr]->sets[setBits])->blocks[i];
	       if ((currBlock->tbits == tbits) && (currBlock->vb == 1)) {   // Found
                  found = 1;
                  if (rwflag == 0) {                                        // Update for read
                     if (currBlock->db == 2){
                        printf("CPU: %d Set: %d Transition from \"Modified\" to \"Shared\" at Cycle %d\n", cr, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Invalid\" to \"Shared\" at Cycle %d\n", crid, setBits, cyc_cnt);
                     }
                     if (currBlock->db == 1) {
                        printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Shared\" at Cycle %d\n", cr, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Invalid\" to \"Shared\" at Cycle %d\n", crid, setBits, cyc_cnt);
                     }
                     currBlock->db = 0;                                     // Set to Shared
                     break;
                  }
                  else {                                                    // Update for write
                     if (currBlock->db == 2){
                        printf("CPU: %d Set: %d Transition from \"Modified\" to \"Invalid\" at Cycle %d\n", cr, setBits, cyc_cnt);
                        //printf("CPU: %d Set: %d Transition from ""Invalid"" to ""Exclusive"" at Cycle %d\n", crid, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Invalid"" to \"Modified\" at Cycle %d\n", crid, setBits, cyc_cnt);
                     }
                     if (currBlock->db == 1) {
                        printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Invalid\" at Cycle %d\n", cr, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Invalid\" to \"Exclusive\" at Cycle %d\n", crid, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Modified\" at Cycle %d\n", crid, setBits, cyc_cnt);
                     }
                     if (currBlock->db == 0)  {
                        printf("CPU: %d Set: %d Transition from \"Shared\" to \"Invalid\" at Cycle %d\n", cr, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Invalid\" to \"Exclusive\" at Cycle %d\n", crid, setBits, cyc_cnt);
                        printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Modified\" at Cycle %d\n", crid, setBits, cyc_cnt);
                     }
                     currBlock->vb = 0;                                      // Set to Invalid
                     currBlock->db = 0;                                      // Set to Invalid
                    break;
                  }
               }
           }
           if (found)
              break;
        }
    }
    if (found == 0) { // Did not find anywhere else
     if (rwflag == 0)  {
        for (i = 0; i < c->blockcnt; i = i + 1) {                        // Lookup
	    block *currBlock = (c->sets[setBits])->blocks[i];
	    if ((currBlock->tbits == tbits) && (currBlock->vb == 1)){    // Found in the current core
               currBlock->db = 1;
               break;
            }
        }
        printf("CPU: %d Set: %d Transition from \"Invalid\" to \"Exclusive\" at Cycle %d\n", crid, setBits, cyc_cnt);
     }
     else{
      printf("CPU: %d Set: %d Transition from \"Invalid\" to \"Exclusive\" at Cycle %d\n", crid, setBits, cyc_cnt);
      printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Modified\" at Cycle %d\n", crid, setBits, cyc_cnt);
     }
      
    }
}

void update_mem (cache_t *c, int crid, int setBits, int tbits, uint64_t address) {
    // Check the other cores for a modified block. If present write it back to mem.
    int i;
    int cr;
    for (cr = 0; cr < NCOR; cr++) {
        if ((active_core[cr]) &&(cr != crid)) {
           for (i = 0; i < dcache[cr]->blockcnt; i = i + 1) {                        // Lookup
	       block *currBlock = (dcache[cr]->sets[setBits])->blocks[i];
	       if ((currBlock->tbits == tbits) && (currBlock->vb == 1)) {           // Found
                  if (currBlock->db == 2) {                                 // Modified
                     uint64_t addr = address & (~0x1F);
                     ccwriteMM(currBlock, addr);                   // Writeback modified to memory
                  }
               }
           }
        }
    }
}
int set_pending_delay(cache_t * c, int crid, int setBits, int tbits, int cflag, int rwflag, int cyc_cnt){
    int i;
    int cr;
    if (cflag == 1) {
       for (cr = 0; cr < NCOR; cr++) {
           if ((active_core[cr]) &&(cr != crid)) {
              for (i = 0; i < dcache[cr]->blockcnt; i = i + 1) {                        // Lookup in other caches
	          block *currBlock = (dcache[cr]->sets[setBits])->blocks[i];
	          if ((currBlock->tbits == tbits) && (currBlock->vb == 1)) {            // Found
                     if (currBlock->db == 2) {                                          // Modified
                        //printf("50 changed to 5\n");
                        return 50;             // Delay will be 50 cycles
                     }
                     return 50;                // In all other cases it will be a 5 cycle delay
                  }
              }
           }
       }
       return 50;                             // Did not find so delay should be 50
    }
}
void justinv_others(cache_t *c, int crid, int setBits, int tbits, int cflag, int cyc_cnt) {
     int i;
     int cr;
     if (cflag == 1) {
        for (cr = 0; cr < NCOR; cr++) {
            if ((active_core[cr]) &&(cr != crid)) {
               for (i = 0; i < dcache[cr]->blockcnt; i = i + 1) {                        // Lookup
	           block *currBlock = (dcache[cr]->sets[setBits])->blocks[i];
	           if ((currBlock->tbits == tbits) && (currBlock->vb == 1)) {   // Found
                     if (currBlock->db == 0) {
                        printf("CPU: %d Set: %d Transition from \"Shared\" to \"Invalid\" at Cycle %d\n", crid, setBits, cyc_cnt); 
                     }
                     if (currBlock->db == 1) {
                        printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Invalid\" at Cycle %d\n", crid, setBits, cyc_cnt); 
                     }
                     currBlock->vb = 0; // Invalidate 
                  }
               }
            }
        }
   }
}
void decode_tag_index_setbits(cache_t *c, uint64_t addr, int *indexBits, int *setBits, int *tbits)
{
        int nsets = c->setcnt;
        int tmp = 0;
        *indexBits = addr & 0x1F;
        *setBits = (addr >> 5) & (c->setcnt - 1);
        while (nsets >>= 1){
             tmp++;
        }
        *tbits = addr >> (5 + tmp);

}
void decFetch(cache_t *c)
{
   c->cst.pend_cycles = c->cst.pend_cycles - 1;
}

int is_Fetching(cache_t *c)
{
   return c->cst.pend_cycles;
}

void writeToBlock(cache_t* c, block* block, int offset, int setBits, uint32_t writeData, int writeSize)
{
   char *blockOffset = block->data + offset;
   uint32_t mergedData = writeData; //writeSize = 4 for STUR, 2 for STURH, and 1 for STURB
   uint32_t *retval = malloc(WDSIZE);

   if (writeSize == 2) //STURH
   {
      memcpy((void*)retval, (void*)(blockOffset), WDSIZE);
      mergedData = (*retval & ~0xffff) | (writeData & 0xffff);
   }

   if (writeSize == 1) //STURB
   {
      memcpy((void*)retval, (void*)(blockOffset), WDSIZE);
      mergedData = (*retval & ~0xff) | (writeData & 0xff);
   }
   memcpy((void*)(blockOffset), &mergedData, WDSIZE);
   free(retval);

   c->sets[setBits]->lrusetcnt = c->sets[setBits]->lrusetcnt + 1;
   block->lruind = c->sets[setBits]->lrusetcnt;

}

void writeMM(block* block, uint64_t addr)
{
   
   uint32_t address = evict_addr;

   uint32_t *writeData = (uint32_t*) block->data;
   int i;
   for (i = 0; i < 8; i++)
   {
      mem_write_32(address + 4 * i, writeData[i]);
   }
   // reset all eviction cases.
   evict_addr    = 0;
   evict_offset  = 0;
   evict_SetBits = 0;
   evict_tagBits = 0;
}


block* blockatAddr(cache_t *c, uint64_t addr)
{
   int indexBits = addr & 0x1F; 
   int setBits = (addr >> 5) & 0x3F; 
   int tbits = addr >> 11; 

   int i;
   for (i = 0; i < c->blockcnt; i = i + 1)
   {
      block *currBlock = (c->sets[setBits])->blocks[i];
      if (currBlock->tbits == tbits)
      {
	 return currBlock;
      }
   }
   return NULL;
}

block* block2Fetch(cache_t *c)
{
   return c->cst.sblock;
} 

void resetFetching(cache_t *c)
{
   c->cst.pend_cycles = 0;
   c->cst.sblock = NULL; 
   c->cst.sindbits = 0;
   c->cst.ssbits = 0;
   c->cst.stbits = 0;
   c->cst.saddr = 0;
}


uint32_t readBlock(block *currBlock, cache_t* c, int indexBits, int setBits)
{
   uint32_t *retval = malloc(4);
   memcpy((void*)retval, (void*)(currBlock->data + indexBits), 4);

   c->sets[setBits]->lrusetcnt = c->sets[setBits]->lrusetcnt + 1;
   currBlock->lruind = c->sets[setBits]->lrusetcnt;

   return *retval;
}


void insertBlock(block *currBlock, cache_t* c, int indexBits, int setBits, int tbits, uint64_t addr)
{
   uint32_t* loadedData = (uint32_t*) malloc(32);

   int j;
                 
   uint32_t address = addr & (~0x1F);
   for (j = 0; j < 8; j++)
   {
       loadedData[j] = mem_read_32(address);
       address = address + 4;
   }
   
   currBlock->data = (char*) loadedData;

   c->sets[setBits]->lrusetcnt = c->sets[setBits]->lrusetcnt + 1;
   currBlock->lruind = c->sets[setBits]->lrusetcnt;

   currBlock->vb = 1;
   currBlock->tbits = tbits;

}

cache_t *cache_new(int setcnt, int blockcnt)
{

        cache_t *newCache = malloc(sizeof(cache_t));
	newCache->cst.pend_cycles = 0;
        newCache->setcnt = setcnt;
	newCache->blockcnt = blockcnt;
	newCache->cst.sblock = NULL;
	newCache->cst.sindbits = 0;
	newCache->cst.ssbits = 0;
	newCache->cst.swrite = 0;
	newCache->cst.swriteData = 0;
	newCache->cst.swriteSize = 0;
	newCache->cst.stbits = 0;
	newCache->cst.saddr = 0;

        set **sets = (set **)malloc(setcnt * sizeof(set*));
 	int i;
	for (i = 0; i < setcnt; i = i + 1)
	{
           set *newSet = (set*) malloc(sizeof(set));
	   newSet->setind = i;
	   newSet->lrusetcnt = 0;
	   block **blocks = (block **)malloc(blockcnt * sizeof(block*));

	   int j;
	   for (j = 0; j < blockcnt; j++) 
	   {
		   block *newBlock = (block*) malloc(sizeof(block));
		   newBlock->lruind = 0;
	           newBlock->blockind = j;
		   newBlock->vb = 0;
		   newBlock->db = 0;
		   newBlock->tbits = 0;
		   newBlock->memSize = 32; 
		   newBlock->data = (void*) malloc(newBlock->memSize);
		   memset(newBlock->data, 0, newBlock->memSize);
		   blocks[j] = newBlock;
       }	
	   newSet->blocks = blocks;
	   sets[i] = newSet;
    }
	newCache->sets = sets;
        return newCache;
}

void cache_destroy(cache_t *c)
{

}


uint32_t cache_update(cache_t *c, uint64_t addr, int updateType, uint32_t writeData, int writeSize, int cflag, int cyc_cnt) {

/********************BACK FROM STALL *******************************************************************************************/
     if (c->cst.pend_cycles > 0) {
	if (c->cst.pend_cycles == 1)                           // Finishing Stall
	{
	   c->cst.pend_cycles = c->cst.pend_cycles - 1;
	   if (c->cst.sblock->db == 1)                        // If the saved block is dirty 
                                                              // This will happen when EVICT DIRTY
	   {
	      writeMM(c->cst.sblock, c->cst.saddr);           // Write it back to memory
	      c->cst.sblock->db = 0;                          // Clear the dirty bit to 0
           }
           // Now that eviction is done
           // Finish the pending read or write miss
           if (cflag == 1)     { // Done only for dcache
              update_mem (c, crid, c->cst.ssbits, c->cst.stbits, c->cst.saddr);
                                                    // Write back modified block if any to memory

           }
	   insertBlock(c->cst.sblock, c, c->cst.sindbits, c->cst.ssbits, c->cst.stbits, c->cst.saddr);
                                                              // Load 32 bytes from memory to the saved block.
	   if (c->cst.swrite == 0) {                          
              if (cflag == 1)     { // Dcache
                 update_others(c, crid, c->cst.ssbits, c->cst.stbits,c->cst.swrite, cyc_cnt);
                            // Update other cores. Print transition messages.
              }
	      return readBlock(c->cst.sblock, c, c->cst.sindbits, c->cst.ssbits);  // Read, data loaded by InsertBlock
										   // Update the LRU bits 
                                                                                   // Return the data to CPU.
	   }
           if (c->cst.swrite == 1) {
              if (cflag == 1)     { // Dcache
                 update_others(c, crid, c->cst.ssbits, c->cst.stbits,c->cst.swrite, cyc_cnt);
                            // Update other cores. Print transition messages.
              }
              writeToBlock(c, c->cst.sblock, c->cst.sindbits, c->cst.ssbits, c->cst.swriteData, c->cst.swriteSize);
              c->cst.sblock->db = 2;
                                                              // Merge the data to be stored by CPU, swriteData
					                      // that loaded in the insertBlock sblock->data
              return 0;		                              // The CPU is waiting for nothing.
          }

        }
	c->cst.pend_cycles = c->cst.pend_cycles - 1;
	return 0;
     }
/**************************************END OF BACK FROM STALL**********************************************/
// So we are NOT in STALL i.e, we are here first time for a icache or dcache

     int setBits, indexBits, tbits;
     int i;
     decode_tag_index_setbits(c, addr, &indexBits, &setBits, &tbits); // Get tag index and offset

/********************HIT HIT HIT**********************************************************************/
     for (i = 0; i < c->blockcnt; i = i + 1) {                        // Lookup
	 block *currBlock = (c->sets[setBits])->blocks[i];
	 if ((currBlock->tbits == tbits) && (currBlock->vb == 1)) {   // Found
	    if (updateType == 0) {                                   // If it is a READ hit
               //if (cflag == 0) 
                  //printf("icache hit at cycle  %ld\n", cyc_cnt);
               //else 
                  //printf("dcache hit at cycle  %ld\n", cyc_cnt);
	       return readBlock(currBlock, c, indexBits, setBits);   // Read the block
	    }
	    else {                                                   // It is a WRITE HIT
              // This can happen only for dcache since it is a write. So, I am not checking if it is a dcachce.
              if (currBlock -> db == 2) { // MODIFIED
                  writeToBlock(c, currBlock, indexBits, setBits, writeData, writeSize); // Merge CPU Write data
                  return 0;
              }
              if (currBlock -> db == 1) { // EXCLUSIVE
                  writeToBlock(c, currBlock, indexBits, setBits, writeData, writeSize);  // Merge CPU Write data
                  currBlock->db = 2;
                  printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Modified\" at Cycle %d\n", crid, setBits, cyc_cnt); 
                  return 0;
              }
              if (currBlock -> db == 0) { // Shared
                 justinv_others(c, crid, setBits, tbits, cflag, cyc_cnt);
                 writeToBlock(c, currBlock, indexBits, setBits, writeData, writeSize); // Merge CPU Data
                 currBlock->db = 2;
                 printf("CPU: %d Set: %d Transition from \"Shared\" to \"Exclusive\" at Cycle %d\n", crid, setBits, cyc_cnt); 
                 printf("CPU: %d Set: %d Transition from \"Exclusive\" to \"Modified\" at Cycle %d\n", crid, setBits, cyc_cnt); 
                 return 0;
              }
            }
         }
     }
/****************MISS MISS MISS **********************************************************************/
// So it is a miss

     int delay = 0;
     if (updateType == 1) {              // It is a WRITE MISS 
        c->cst.swrite = 1;               // store in saved block that the pending req is a write
        c->cst.swriteData = writeData;  // store in saved block the data to be written
        c->cst.swriteSize = writeSize;  // store in saved block the size of the write
        // It has to be dcache since it is a write
        // For pending cases try next cycle. So, set pending delay to 1 or 2.
        // Check if it is present in other cores and return an appropriate stall amount.

        delay = set_pending_delay(c, crid, setBits, tbits, cflag, updateType, cyc_cnt);
        //printf("Determined delay for write miss at core %d to be %d\n", crid, delay);
        dcache[crid]->cst.pend_cycles = delay;
     }
     else {                              // It is READ MISS
        if (cflag == 1) {               // Dcache
           delay = set_pending_delay(c, crid, setBits, tbits, cflag, updateType, cyc_cnt);
           //printf("Determined delay for read  miss at core %d to be %d\n", crid, delay);
           dcache[crid]->cst.pend_cycles = delay;
        }
        c->cst.swrite = 0;// store in saved block that read is pending
     }
     if (delay == 0) {                     // If the delay is not set This will happen only for icache.
        c->cst.pend_cycles = 50;           // Stall starts   should be 50
     }
                                        // Create a block without data and store it in the saved block
     for (i = 0; i < c->blockcnt; i = i + 1) {	   
       block *currBlock = (c->sets[setBits])->blocks[i];
       if (currBlock->vb == 0) {
          c->cst.sblock = currBlock;
          c->cst.sindbits = indexBits;
          c->cst.ssbits = setBits;
          c->cst.stbits = tbits;
 	  c->cst.saddr = addr;
          return 0;
       }
     }
     // read miss - either we still have space in the set (some block with 0 valid bit)
     // OR: all blocks are full, we need to evict the LRU block.
     // Locate the LRU block
        int counter = ((c->sets[setBits])->blocks[0])->lruind;
	block *tempBlock = (c->sets[setBits])->blocks[0];
        for (i = 0; i < c->blockcnt; i = i + 1) {
            block *currBlock = (c->sets[setBits])->blocks[i];
            if (currBlock->lruind < counter) {
	       counter = currBlock->lruind;
	       tempBlock = currBlock;
	    }
        }
        // STORE THIS BLOCK IN THE SAVED BLOCK
        //reconstruct the address of the evicted block
        //so that we don't store it in the wrong place
        evict_SetBits = 0; //clear evict values
        evict_tagBits = 0;
        evict_offset =  0;
        evict_SetBits = setBits;
        evict_tagBits = tempBlock->tbits;
        evict_addr = (evict_tagBits << 13)|(evict_SetBits << 5);
        c->cst.sblock = tempBlock;
        c->cst.sindbits = indexBits;
        c->cst.ssbits = setBits;
        c->cst.stbits = tbits;
        c->cst.saddr = addr;
        return 0;
}

/**********************************************************************************************************************/
// LAB 5 code
/**********************************************************************************************************************/
// Decode Dcache for cache coherent version
// Parse the address into offset, index and tag and return these in the parameters
void decode_ccdcache(cache_t *c, uint64_t addr, int *offset, int *index, int *tag) { 

     int nsets = 256;
     int lgnsets = 8;
     *offset = addr & 0x1F;   // Get the 5 lsbs
     *index = (addr >> 5) & 255;
     *tag = addr >> (5 + lgnsets);

}
/*---------------------------------------------------------------------------------------------------------------------*/
// Check if any read or write is pending at a core other than crid
int cc_rdwrpend() {
    int cr;
    int rdpend;
    int wrpend;
    for (cr = 0; cr < NCOR; cr++) {
        if (active_core[cr] || !(cr == crid)) {
           rdpend = ccrd_modelay[cr] || ccrd_excldelay[cr] || ccrd_shrdelay[cr] || ccrd_invdelay[cr];
           wrpend = ccwr_modelay[cr] || ccwr_excldelay[cr] || ccwr_shrdelay[cr] || ccwr_invdelay[cr];
        }
        if (rdpend || wrpend)
           return 1;
    }
    return 0;
}
