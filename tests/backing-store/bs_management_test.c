#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

#define TESTPID 20
#define TESTPID2 21
#define TEST_BS 4
#define TEST_BS2 5
#define NPAGES 20

void test_bsm_init();
void test_get_bs();
void test_release_bs();
void test_bsm_lookup();
void test_bsm_map();

int main() {
    kprintf("\n\n====== BACKING STORE MANAGEMENT TEST ======\n\n");
    
    test_bsm_init();
    test_get_bs();
    test_bsm_map();
    test_bsm_lookup();
    test_release_bs();
    
    return 0;
}

void test_bsm_init() {
    kprintf("Testing init_bsm()...\n");
    
    int result = init_bsm();
    int i;
    if (result == OK) {
        kprintf("  init_bsm() successful\n");
    } else {
        kprintf("  ERROR: init_bsm() failed\n");
    }
    
    // Verify some entries are properly initialized
    kprintf("  Checking first few entries after initialization:\n");
    for (i = 0; i < 3; i++) {
        kprintf("  bs_tab[%d]: status=%d, pid=%d, vpno=%d, npages=%d\n", 
                i, bsm_tab[i].bs_status, bsm_tab[i].bs_pid, 
                bsm_tab[i].bs_vpno, bsm_tab[i].bs_npages);
    }
    kprintf("\n");
}

void test_get_bs() {
    kprintf("Testing get_bs()...\n");
    
    // Test valid allocation
    int pages = get_bs(TEST_BS, NPAGES);
    if (pages == NPAGES) {
        kprintf("  get_bs(%d, %d) successful, returned %d pages\n", TEST_BS, NPAGES, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", TEST_BS, NPAGES, pages);
    }
    
    // Test allocation of existing backing store
    pages = get_bs(TEST_BS, 30);
    if (pages == NPAGES) {
        kprintf("  get_bs(%d, 30) correctly returned existing size of %d pages\n", TEST_BS, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, 30) failed, returned %d instead of %d\n", TEST_BS, pages, NPAGES);
    }
    
    // Test invalid parameters
    pages = get_bs(MAX_BS, 10);
    if (pages == SYSERR) {
        kprintf("  get_bs(%d, 10) correctly failed for invalid BS ID\n", MAX_BS);
    } else {
        kprintf("  ERROR: get_bs(%d, 10) should have failed but returned %d\n", MAX_BS, pages);
    }
    
    pages = get_bs(TEST_BS2, 129);
    if (pages == SYSERR) {
        kprintf("  get_bs(%d, 129) correctly failed for too many pages\n", TEST_BS2);
    } else {
        kprintf("  ERROR: get_bs(%d, 129) should have failed but returned %d\n", TEST_BS2, pages);
    }
    
    kprintf("\n");
}

void test_bsm_map() {
    kprintf("Testing bsm_map()...\n");
    
    // Map a backing store to a process's virtual memory
    int vpno = 4096;  // Start of virtual memory
    int result = bsm_map(TESTPID, vpno, TEST_BS, NPAGES);
    if (result == OK) {
        kprintf("  bsm_map() successful: pid=%d, vpno=%d, store=%d, npages=%d\n", 
                TESTPID, vpno, TEST_BS, NPAGES);
    } else {
        kprintf("  ERROR: bsm_map() failed\n");
    }
    
    // Try to map the same backing store to another process (should fail)
    result = bsm_map(TESTPID2, vpno, TEST_BS, NPAGES);
    if (result == SYSERR) {
        kprintf("  bsm_map() correctly failed when mapping same BS to another process\n");
    } else {
        kprintf("  ERROR: bsm_map() should have failed but succeeded\n");
    }
    
    // Map another backing store to another process
    result = bsm_map(TESTPID2, vpno, TEST_BS2, NPAGES);
    if (result == OK) {
        kprintf("  bsm_map() successful: pid=%d, vpno=%d, store=%d, npages=%d\n", 
                TESTPID2, vpno, TEST_BS2, NPAGES);
    } else {
        kprintf("  ERROR: bsm_map() failed for second mapping\n");
    }
    
    kprintf("\n");
}

void test_bsm_lookup() {
    kprintf("Testing bsm_lookup()...\n");
    
    int store, pageth;
    long vaddr;
    
    // Test lookup for mapped address
    vaddr = 4096 * 4096 + 2000;  // Page 4096, offset 2000
    int result = bsm_lookup(TESTPID, vaddr, &store, &pageth);
    if (result == OK) {
        kprintf("  bsm_lookup() successful for vaddr 0x%08lX\n", vaddr);
        kprintf("  Found: store=%d, page offset=%d\n", store, pageth);
    } else {
        kprintf("  ERROR: bsm_lookup() failed for vaddr 0x%08lX\n", vaddr);
    }
    
    // Test lookup for unmapped address
    vaddr = 3000 * 4096;
    result = bsm_lookup(TESTPID, vaddr, &store, &pageth);
    if (result == SYSERR) {
        kprintf("  bsm_lookup() correctly failed for unmapped vaddr 0x%08lX\n", vaddr);
    } else {
        kprintf("  ERROR: bsm_lookup() should have failed for vaddr 0x%08lX\n", vaddr);
    }
    
    kprintf("\n");
}

void test_release_bs() {
    kprintf("Testing release_bs()...\n");
    
    // Release a mapped backing store (should fail)
    int result = release_bs(TEST_BS);
    if (result == SYSERR) {
        kprintf("  release_bs(%d) correctly failed for mapped BS\n", TEST_BS);
    } else {
        kprintf("  ERROR: release_bs(%d) should have failed but succeeded\n", TEST_BS);
    }
    
    // Unmap the backing store
    result = bsm_unmap(TESTPID, 4096, 0);
    if (result == OK) {
        kprintf("  bsm_unmap() successful for pid=%d, vpno=%d\n", TESTPID, 4096);
    } else {
        kprintf("  ERROR: bsm_unmap() failed for pid=%d, vpno=%d\n", TESTPID, 4096);
    }
    
    // Now try to release the backing store
    result = release_bs(TEST_BS);
    if (result == OK) {
        kprintf("  release_bs(%d) successful after unmapping\n", TEST_BS);
    } else {
        kprintf("  ERROR: release_bs(%d) failed after unmapping\n", TEST_BS);
    }
    
    // Try to release an invalid backing store
    result = release_bs(MAX_BS);
    if (result == SYSERR) {
        kprintf("  release_bs(%d) correctly failed for invalid BS ID\n", MAX_BS);
    } else {
        kprintf("  ERROR: release_bs(%d) should have failed but succeeded\n", MAX_BS);
    }
    
    kprintf("\n");
}
