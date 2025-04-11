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
 	STATWORD 	ps;
	disable(ps);
	
	int i = 0;
  // here we initialize the frames,
  // all the frames in the physical memory are initially mapped to unmapped
  // all the frame's pid is set to -1 since they haven't been owned
  // the virtual page number that corresponds to this frame is made zero
  // reference count of the frame is zero
  // fr_type is FR_PAGE unlike the directory page
  // fr_dirty lets know if frame has been modfied and needs to flushed
	for (i = 0; i < NFRAMES; i++)
	{
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = 0;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_PAGE;
		frm_tab[i].fr_dirty = 0;				
	}
	
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* freeFrame)
{

  STATWORD 	ps;
  // disable the current process
	disable(ps);
	
	int i = 0;
	int firstFreeFrame;
  // loop over and find the first free frame that is availble for now
	for (i = 0; i < NFRAMES; i++){
		if (frm_tab[i].fr_status != FRM_MAPPED){
			*freeFrame = i;	
			restore(ps);
			return OK;
		}				
	}
		
	// what if there are no free frames available, this is where the page replacement policy needs to be done
  return SYSERR; // TODO returning error for now
	 
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

// This is needed in initialize.c
// when the frames are first initialized, we need to 
void init_pageReplacement_queue()
{
	int i = 0;
	for (i = 0; i < NFRAMES; i++)
	{
		pr_qtab[i].frId = i;
		pr_qtab[i].frAge = 0;
		pr_qtab[i].next = -1;
	}
}