#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
  
  /* release the backing store with ID bs_id */
  STATWORD    ps;
  disable(ps);

  if (bs_id < 0 || bs_id >= MAX_BS) {
    restore(ps);
    return SYSERR;
  }

  if (bsm_tab[bs_id].bs_status == BSM_MAPPED) {
    restore(ps);
    return SYSERR;
  }

  bsm_tab[bs_id].bs_npages = 0;
 
  restore(ps);
  return OK;

  // kprintf("To be implemented!\n");
  //  return OK;

}

