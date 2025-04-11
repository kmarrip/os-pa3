/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD 	ps;
	disable(ps);

	// first create the process just like anyother process using the create function call
	int pid = create(procaddr,ssize,priority,name,nargs,args);
	// head of free memory list
	struct mblock *bsBaseAddress;

	// before allocating the memory on the backing store, check if any backing store is currently available
	int bs_id;
	if (get_bsm(&bs_id) == SYSERR){
		restore(ps);
		return SYSERR;
	}
	
	// adding this maping to bsm_tab table
	bsm_map(pid,4096,bs_id,hsize);

	// wil the backing store get blocked if the heap is allocated ??

	/* allocate heap size in corresponding backing store ID */
	// allocate this mapping on to the 
	bsBaseAddress =  (bs_id * BACKING_STORE_UNIT_SIZE) + BACKING_STORE_BASE;
	bsBaseAddress->mlen = hsize * NBPG;
	bsBaseAddress->mnext = NULL;

	// set the vpages size
	proctab[pid].vhpnpages = hsize;
	proctab[pid].vmemlist->mnext = 4096 * NBPG;
	
	restore(ps);	
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
