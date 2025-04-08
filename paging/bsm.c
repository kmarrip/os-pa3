/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

bs_map_t bsm_tab[MAX_BS];

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{

    STATWORD    ps;
    disable(ps);

    int i;

    for (i = 0; i < MAX_BS; i++) {
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        bsm_tab[i].bs_pid = -1;
        bsm_tab[i].bs_vpno = -1;
        bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].bs_id = i;
    }

    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
    STATWORD    ps;
    disable(ps);
    
    int i;

    for ( i = 0; i < MAX_BS; i++) {
        if(bsm_tab[i].bs_status == BSM_UNMAPPED) {
            *avail = i;

            restore(ps);
            return OK;
        }
    }

    restore(ps);
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
    STATWORD    ps;
    disable(ps);

    if ( i < 0 || i >= MAX_BS) {
        restore(ps);
        return SYSERR;
    }

    bsm_tab[i].bs_status = BSM_UNMAPPED;
    bsm_tab[i].bs_pid = -1;
    bsm_tab[i].bs_vpno = -1;
    bsm_tab[i].bs_npages = 0;
    bsm_tab[i].bs_sem = 0;

    restore(ps);
    return OK;

}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{

    STATWORD    ps;
    disable(ps);

    int i;
    int vpno = vaddr / NBPG;    // Converting virtual address to virtual page number

    for ( i = 0; i < MAX_BS; i++) {
        if (bsm_tab[i].bs_status == BSM_MAPPED &&
            bsm_tab[i].bs_pid == pid &&
            vpno >= bsm_tab[i].bs_vpno &&
            vpno < bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages) {

                *store = i;
                *pageth = vpno - bsm_tab[i].bs_vpno;

                restore(ps);
                return OK;
        }
    }

    restore(ps);
    return SYSERR;


}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    STATWORD    ps;
    disable(ps);

    if (source < 0 || source >= MAX_BS || npages <= 0 || npages > 128) {
        restore(ps);
        return SYSERR;
    }

    if (bsm_tab[source].bs_npages > 0 && npages > bsm_tab[source].bs_npages) {
        restore(ps);
        return SYSERR;
    }

    // If it is already mapped, then verify  if it isn't mapped to a different proc
    if (bsm_tab[source].bs_status == BSM_MAPPED &&
        bsm_tab[source].bs_pid != pid) {
        restore(ps);
        return SYSERR;
    }

    // Setting up the mapping
    bsm_tab[source].bs_status = BSM_MAPPED;
    bsm_tab[source].bs_pid = pid;
    bsm_tab[source].bs_vpno = vpno;
    bsm_tab[source].bs_npages = npages;

    restore(ps);
    return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    STATWORD    ps;
    disable(ps);
    
    int i;
    int result;

    for (i = 0; i < MAX_BS; i++) {
        if (bsm_tab[i].bs_status == BSM_MAPPED &&
            bsm_tab[i].bs_pid == pid &&
            vpno >= bsm_tab[i].bs_vpno &&
            vpno < bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages) {

            if (vpno == bsm_tab[i].bs_vpno || flag == 1) {
                    result = free_bsm(i);
                    restore(ps);
                    return result;
            } else {
                restore(ps);
                return SYSERR;
            }
        
        }
        
    }


    restore(ps);
    return SYSERR;
}


