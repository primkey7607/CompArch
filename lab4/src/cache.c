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


uint32_t cache_update(cache_t *c, uint64_t addr, int updateType, uint32_t writeData, int writeSize)
{
	if (c->cst.pend_cycles > 0)
	{
	   if (c->cst.pend_cycles == 1)
	   {
	      c->cst.pend_cycles = c->cst.pend_cycles - 1;

	      if (c->cst.sblock->db == 1)
	      {
	         writeMM(c->cst.sblock, c->cst.saddr);
	 	 c->cst.sblock->db = 0;
              }

	      insertBlock(c->cst.sblock, c, c->cst.sindbits, c->cst.ssbits, c->cst.stbits, c->cst.saddr);

	      if (c->cst.swrite == 0)
	      {
	         return readBlock(c->cst.sblock, c, c->cst.sindbits, c->cst.ssbits);
	      }

              if (c->cst.swrite == 1)
              {
                 writeToBlock(c, c->cst.sblock, c->cst.sindbits, c->cst.ssbits, c->cst.swriteData, c->cst.swriteSize);
                 c->cst.sblock->db = 1;
                 return 0;		
              }

           }
	      c->cst.pend_cycles = c->cst.pend_cycles - 1;
	      return 0;
	}


        int setBits, indexBits, tbits;
        
        decode_tag_index_setbits(c, addr, &indexBits, &setBits, &tbits);
	int i;
	for (i = 0; i < c->blockcnt; i = i + 1)
	{
	   block *currBlock = (c->sets[setBits])->blocks[i];
	   if ((currBlock->tbits == tbits) && (currBlock->vb == 1))
	   {
	      if (updateType == 0) 
	      {
		 return readBlock(currBlock, c, indexBits, setBits);
	 	 
	      }

	       else {
                  writeToBlock(c, currBlock, indexBits, setBits, writeData, writeSize);
		  currBlock->db = 1;
		  return 0;
              }
           }
	}
	
	if (updateType == 1)
        {
           c->cst.swrite = 1;
           c->cst.swriteData = writeData;
           c->cst.swriteSize = writeSize;
        }

        else
        {
           c->cst.swrite = 0;
        }

	   c->cst.pend_cycles = 50;
	   for (i = 0; i < c->blockcnt; i = i + 1)
           {	   
	      block *currBlock = (c->sets[setBits])->blocks[i];
	      if (currBlock->vb == 0)
	      {
		 c->cst.sblock = currBlock;
		 c->cst.sindbits = indexBits;
		 c->cst.ssbits = setBits;
		 c->cst.stbits = tbits;
 	  	 c->cst.saddr = addr;
          	 return 0;
              }
	   }

           int counter = ((c->sets[setBits])->blocks[0])->lruind;
	   block *tempBlock = (c->sets[setBits])->blocks[0];
           for (i = 0; i < c->blockcnt; i = i + 1)
	   {
		block *currBlock = (c->sets[setBits])->blocks[i];
		if (currBlock->lruind < counter)
		{
		   counter = currBlock->lruind;
		   tempBlock = currBlock;
		}
           }

           //reconstruct the address of the evicted block
           //so that we don't store it in the wrong place

           evict_SetBits = 0; //clear evict values
           evict_tagBits = 0;
           evict_offset = 0;
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

