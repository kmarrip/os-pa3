/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{

	STATWORD ps;
	unsigned top;
	if(size == 0){
		kprintf("size can't be zero\n");
		return SYSERR;
	}

	if((unsigned)block < 1024 * NBPG){
		krpintf("size can't be less then 2014 NBPG");
		return SYSERR;
	}

	// truncating the size to nearest mblock size
	size = (unsigned)roundmb(size);
	disable(ps);

	struct	mblock	*left, *right;
	left = proctab[currpid].vmemlist;
	right = proctab[currpid].vmemlist -> mnext;
	while(right != (struct mblock *)NULL && right < block){
		// move the right pointer to right and left pointer takes the pointer that was initially at right;
		left = right;
		right = right->mnext;
	}

	if(right != NULL){
		if(size + (unsigned)block  > (unsigned)right){
			restore(ps);
			return SYSERR;
		}
	}
	if(left != proctab[currpid].vmemlist){
		top = q->mlen + (unsigned)q;
		if(top > (unsigned)block && left != proctab[currpid].vmemlist){
			restore(ps);
			return SYSERR;
		}
		
	}


	// TODO Need to look at this

	// if ( left!= proctab[currpid].vmemlist && top == (unsigned)block )
	// 		left->mlen += size;
	// else {
	// 	block->mlen = size;
	// 	block->mnext = p;
	// 	left->mnext = block;
	// 	left = block;
	// }

	
	// if ( (unsigned)( left->mlen + (unsigned)left ) == (unsigned)right) {
	// 	left->mlen += right->mlen;
	// 	left->mnext = right->mnext;
	// }
	restore(ps);
	return(OK);
}
