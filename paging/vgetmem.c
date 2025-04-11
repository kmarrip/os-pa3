/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */

/* modifying getmem file code to allocate virtual heap storage in current process's memlist  */

WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
	STATWORD ps;    
	struct	mblock	*left, *right, *left;

	disable(ps);

	if(nbytes == 0){
		restore(ps);
		return (WORD *) SYSERR;
	}
	
	if(proctab[currpid].vmemlist->mnext == (struct mblock *)NULL){
		restore(ps);
		return (WORD *)SYSERR;
	}

	//  q is left
	//  p is right

	nbytes = (unsigned int) roundmb(nbytes);
	left = proctab[currpid].vmemlist;
	right = left->mnext;
	while(right != (struct mblock *) NULL){
		if(right->mlen == nbytes){
			left->mnext = right->mnext;
			restore(ps);
			return (WORD *)right;
		}else if(right->mlen  > nbytes){
			left = (struct mblock *)((unsigned)p + nbytes);
			left->mnext = left;
			left->mnext = right->mnext;
			left->mlen = right->mlen - nbytes;
			restore(ps);
			return (WORD *)p;
		}
	}
	
	restore(ps);
	return( (WORD *)SYSERR );

}