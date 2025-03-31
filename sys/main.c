/* main.c - Test memory layout configuration */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <i386.h>
#include <paging.h>

extern char *maxaddr;
extern struct mblock memlist;
extern int page_replace_policy;
extern int initsp;

#define HOLESIZE	(600)	
#define	HOLESTART	(640 * 1024)
#define	HOLEEND		((1024 + HOLESIZE) * 1024)  

/*------------------------------------------------------------------------
 * main - test memory layout configuration
 *------------------------------------------------------------------------
 */
int main()
{
    kprintf("\n\n======== Memory Layout Test ========\n\n");
    
    /* Print memory boundaries */
    kprintf("Memory Layout:\n");
    kprintf("  maxaddr: 0x%08X\n", (unsigned int)maxaddr);
    kprintf("  NBPG (page size): %d bytes\n", NBPG);
    
    /* Calculate and print memory regions */
    unsigned int total_mem_pages = (unsigned int)(maxaddr + 1) / NBPG;
    unsigned int total_mem_bytes = total_mem_pages * NBPG;
    
    kprintf("\nMemory Regions:\n");
    kprintf("  Total memory: %d pages (%d bytes)\n", 
            total_mem_pages, total_mem_bytes);
    
    /* Print backing store information */
    kprintf("\nBacking Store Configuration:\n");
    kprintf("  Backing store start: 0x%08X (page %d)\n", 
            2048 * NBPG, 2048);
    kprintf("  Backing store end: 0x%08X (page %d)\n", 
            4096 * NBPG - 1, 4095);
    kprintf("  Backing store size: %d pages (%d bytes)\n", 
            2048, 2048 * NBPG);
    
    /* Print frame information */
    kprintf("\nFrame Information:\n");
    kprintf("  Frame pool start: 0x%08X (page %d)\n", 
            1024 * NBPG, 1024);
    kprintf("  Frame pool end: 0x%08X (page %d)\n", 
            2048 * NBPG - 1, 2047);
    kprintf("  Available frames: %d frames (%d bytes)\n", 
            1024, 1024 * NBPG);
    
    /* Print free memory list information */
    kprintf("\nFree Memory List:\n");
    kprintf("  First free block address: 0x%08X\n", 
            (unsigned int)memlist.mnext);
    kprintf("  First free block size: %d bytes\n", 
            memlist.mnext ? memlist.mnext->mlen : 0);
    
    /* Print page replacement policy */
    kprintf("\nPage Replacement Policy: %s\n", 
            (page_replace_policy == SC) ? "Second-Chance (SC)" : "FIFO");
    
    /* Verify memory hole configuration */
    kprintf("\nMemory Hole Configuration:\n");
    kprintf("  HOLESTART: 0x%08X\n", HOLESTART);
    kprintf("  HOLEEND: 0x%08X\n", HOLEEND);
    kprintf("  HOLESIZE: %d KB\n", HOLESIZE);
    
    /* Verify stack initialization */
    kprintf("\nStack Initialization:\n");
    kprintf("  initsp: 0x%08X\n", initsp);
    
    kprintf("\n======== Memory Layout Test Complete ========\n\n");
    
    return 0;
}
