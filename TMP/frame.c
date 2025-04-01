/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
    STATWORD  ps;
    disable(ps);
    
    //for (int i = 0; i < TOTAL_FRAMES; i++)
    //{
    //    frame_table[i].status = FRAME_UNMAPPED;
    //    frame_table[i].owner_pid = -1;
    //    frame_table[i].virtual_page = 0;
    //    frame_table[i].reference_count = 0;
    //    frame_table[i].type = FRAME_PAGE;
    //    frame_table[i].dirty = 0;               
    // }
    
    restore(ps);
    return OK;
}


/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  kprintf("To be implemented!\n");
  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  kprintf("To be implemented!\n");
  return OK;
}



