#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
  
  /* release the backing store with ID bs_id */
  STATWORD    ps;
  disable(ps);

  int result;

  if (bs_id < 0 || bs_id >= MAX_BS) {
    restore(ps);
    return SYSERR;
  }

  if (bsm_tab[bs_id].bs_status == BSM_MAPPED) {
    restore(ps);
    return SYSERR;
  }

  result = free_bsm(bs_id);
  restore(ps);
  return result;

  // kprintf("To be implemented!\n");
  //  return OK;

}

