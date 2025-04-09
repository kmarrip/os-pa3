/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  STATWORD    ps;
  disable(ps);


  if (virtpage < 4096 || source < 0 || source >= MAX_BS || npages <=0 || npages > 128) {
    restore(ps);
    return SYSERR;
  }

  if (bsm_tab[source].bs_status == BSM_UNMAPPED && bsm_tab[source].bs_npages == 0) {
    restore(ps);
    return SYSERR;
  }

  if(npages > bsm_tab[source].bs_npages) {
    restore(ps);
    return SYSERR;
  }

  int result = bsm_map(currpid, virtpage, source, npages);

  restore(ps);
  return result;
  
  // kprintf("xmmap - to be implemented!\n");
  // return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
  STATWORD    ps;
  disable(ps);

  if (virtpage < 0) {
    restore(ps);
    return SYSERR;
  }

  int result = bsm_unmap(currpid, virtpage, 1);

  restore(ps);
  return result;

  // kprintf("To be implemented!");
  // return SYSERR;
}
