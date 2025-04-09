/* testmain.c - Test backing store management and memory mapping */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

#define TESTPID1 20
#define TESTPID2 21
#define TEST_BS1 4
#define TEST_BS2 5
#define NPAGES1 20
#define NPAGES2 30
#define VPAGE1 4096
#define VPAGE2 6000
#define VPAGE3 7000

void test_bsm_init();
void test_get_bs();
void test_bsm_map();
void test_xmmap();
void test_bsm_lookup();
void test_xmunmap();
void test_release_bs();
void write_read_test();

int main() {
    kprintf("\n\n====== BACKING STORE AND MEMORY MAPPING TEST ======\n\n");
    
    test_bsm_init();
    test_get_bs();
    test_bsm_map();
    test_xmmap();
    test_bsm_lookup();
    write_read_test();
    test_xmunmap();
    test_release_bs();
    
    return 0;
}

void test_bsm_init() {
    kprintf("===== Testing init_bsm() =====\n");
    
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
    kprintf("===== Testing get_bs() =====\n");
    
    // Test valid allocation
    int pages = get_bs(TEST_BS1, NPAGES1);
    if (pages == NPAGES1) {
        kprintf("  get_bs(%d, %d) successful, returned %d pages\n", TEST_BS1, NPAGES1, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", TEST_BS1, NPAGES1, pages);
    }
    
    // Test allocation of second backing store
    pages = get_bs(TEST_BS2, NPAGES2);
    if (pages == NPAGES2) {
        kprintf("  get_bs(%d, %d) successful, returned %d pages\n", TEST_BS2, NPAGES2, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", TEST_BS2, NPAGES2, pages);
    }
    
    // Test allocation of existing backing store
    pages = get_bs(TEST_BS1, 50);
    if (pages == NPAGES1) {
        kprintf("  get_bs(%d, 50) correctly returned existing size of %d pages\n", TEST_BS1, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, 50) failed, returned %d instead of %d\n", TEST_BS1, pages, NPAGES1);
    }
    
    // Test invalid parameters
    pages = get_bs(MAX_BS, 10);
    if (pages == SYSERR) {
        kprintf("  get_bs(%d, 10) correctly failed for invalid BS ID\n", MAX_BS);
    } else {
        kprintf("  ERROR: get_bs(%d, 10) should have failed but returned %d\n", MAX_BS, pages);
    }
    
    pages = get_bs(10, 129);
    if (pages == SYSERR) {
        kprintf("  get_bs(10, 129) correctly failed for too many pages\n");
    } else {
        kprintf("  ERROR: get_bs(10, 129) should have failed but returned %d\n", pages);
    }
    
    pages = get_bs(11, 0);
    if (pages == SYSERR) {
        kprintf("  get_bs(11, 0) correctly failed for zero pages\n");
    } else {
        kprintf("  ERROR: get_bs(11, 0) should have failed but returned %d\n", pages);
    }
    
    kprintf("\n");
}

void test_bsm_map() {
    kprintf("===== Testing bsm_map() =====\n");
    
    // Map a backing store to a process's virtual memory
    int result = bsm_map(TESTPID1, VPAGE1, TEST_BS1, NPAGES1);
    if (result == OK) {
        kprintf("  bsm_map() successful: pid=%d, vpno=%d, store=%d, npages=%d\n", 
                TESTPID1, VPAGE1, TEST_BS1, NPAGES1);
    } else {
        kprintf("  ERROR: bsm_map() failed\n");
    }
    
    // Try to map the same backing store to another process (should fail)
    result = bsm_map(TESTPID2, VPAGE1, TEST_BS1, NPAGES1);
    if (result == SYSERR) {
        kprintf("  bsm_map() correctly failed when mapping same BS to another process\n");
    } else {
        kprintf("  ERROR: bsm_map() should have failed but succeeded\n");
    }
    
    // Map another backing store to another process
    result = bsm_map(TESTPID2, VPAGE1, TEST_BS2, NPAGES2);
    if (result == OK) {
        kprintf("  bsm_map() successful: pid=%d, vpno=%d, store=%d, npages=%d\n", 
                TESTPID2, VPAGE1, TEST_BS2, NPAGES2);
    } else {
        kprintf("  ERROR: bsm_map() failed for second mapping\n");
    }
    
    // Display the mapping table entries
    kprintf("  Checking mapping table entries:\n");
    kprintf("  bs_tab[%d]: status=%d, pid=%d, vpno=%d, npages=%d\n", 
            TEST_BS1, bsm_tab[TEST_BS1].bs_status, bsm_tab[TEST_BS1].bs_pid, 
            bsm_tab[TEST_BS1].bs_vpno, bsm_tab[TEST_BS1].bs_npages);
    kprintf("  bs_tab[%d]: status=%d, pid=%d, vpno=%d, npages=%d\n", 
            TEST_BS2, bsm_tab[TEST_BS2].bs_status, bsm_tab[TEST_BS2].bs_pid, 
            bsm_tab[TEST_BS2].bs_vpno, bsm_tab[TEST_BS2].bs_npages);
    
    kprintf("\n");
}

void test_xmmap() {
    kprintf("===== Testing xmmap() =====\n");
    
    // Allocate a new backing store for xmmap testing
    int bs_id = 6;
    int npages = 25;
    int pages = get_bs(bs_id, npages);
    if (pages == npages) {
        kprintf("  get_bs(%d, %d) successful for xmmap test\n", bs_id, npages);
    } else {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", bs_id, npages, pages);
        return;
    }
    
    // Test xmmap with valid parameters
    int result = xmmap(VPAGE2, bs_id, npages);
    if (result == OK) {
        kprintf("  xmmap() successful: virtpage=%d, source=%d, npages=%d\n", 
                VPAGE2, bs_id, npages);
    } else {
        kprintf("  ERROR: xmmap() failed\n");
    }
    
    // Test xmmap with invalid parameters
    result = xmmap(100, bs_id, npages); // virtpage < 4096
    if (result == SYSERR) {
        kprintf("  xmmap() correctly failed for virtpage < 4096\n");
    } else {
        kprintf("  ERROR: xmmap() should have failed but succeeded\n");
    }
    
    result = xmmap(VPAGE3, MAX_BS, npages); // invalid bs_id
    if (result == SYSERR) {
        kprintf("  xmmap() correctly failed for invalid bs_id\n");
    } else {
        kprintf("  ERROR: xmmap() should have failed but succeeded\n");
    }
    
    result = xmmap(VPAGE3, bs_id, 200); // npages too large
    if (result == SYSERR) {
        kprintf("  xmmap() correctly failed for npages > backing store size\n");
    } else {
        kprintf("  ERROR: xmmap() should have failed but succeeded\n");
    }
    
    // Verify the mapping was created
    kprintf("  Checking mapping after xmmap:\n");
    kprintf("  bs_tab[%d]: status=%d, pid=%d, vpno=%d, npages=%d\n", 
            bs_id, bsm_tab[bs_id].bs_status, bsm_tab[bs_id].bs_pid, 
            bsm_tab[bs_id].bs_vpno, bsm_tab[bs_id].bs_npages);
    
    kprintf("\n");
}

void test_bsm_lookup() {
    kprintf("===== Testing bsm_lookup() =====\n");
    
    int store, pageth;
    long vaddr;
    
    // Test lookup for mapped address
    vaddr = VPAGE1 * 4096 + 2000; // Page VPAGE1, offset 2000
    int result = bsm_lookup(TESTPID1, vaddr, &store, &pageth);
    if (result == OK) {
        kprintf("  bsm_lookup() successful for vaddr 0x%08lX\n", vaddr);
        kprintf("  Found: store=%d, page offset=%d\n", store, pageth);
    } else {
        kprintf("  ERROR: bsm_lookup() failed for vaddr 0x%08lX\n", vaddr);
    }
    
    // Test lookup for address at end of mapped region
    vaddr = VPAGE1 * 4096 + (NPAGES1-1) * 4096;
    result = bsm_lookup(TESTPID1, vaddr, &store, &pageth);
    if (result == OK) {
        kprintf("  bsm_lookup() successful for vaddr 0x%08lX (end of region)\n", vaddr);
        kprintf("  Found: store=%d, page offset=%d\n", store, pageth);
    } else {
        kprintf("  ERROR: bsm_lookup() failed for vaddr 0x%08lX\n", vaddr);
    }
    
    // Test lookup for unmapped address
    vaddr = 3000 * 4096;
    result = bsm_lookup(TESTPID1, vaddr, &store, &pageth);
    if (result == SYSERR) {
        kprintf("  bsm_lookup() correctly failed for unmapped vaddr 0x%08lX\n", vaddr);
    } else {
        kprintf("  ERROR: bsm_lookup() should have failed for vaddr 0x%08lX\n", vaddr);
    }
    
    // Test lookup for address beyond mapped region
    vaddr = VPAGE1 * 4096 + NPAGES1 * 4096 + 100;
    result = bsm_lookup(TESTPID1, vaddr, &store, &pageth);
    if (result == SYSERR) {
        kprintf("  bsm_lookup() correctly failed for out-of-bounds vaddr 0x%08lX\n", vaddr);
    } else {
        kprintf("  ERROR: bsm_lookup() should have failed for vaddr 0x%08lX\n", vaddr);
    }
    
    kprintf("\n");
}

void write_read_test() {
    kprintf("===== Testing write_bs and read_bs =====\n");
    
    // Create test data
    char testdata[NBPG];
    char readbuf[NBPG];
    int i;
    
    // Initialize test data
    for (i = 0; i < NBPG; i++) {
        testdata[i] = (char)(i % 256);
    }
    
    // Write test data to backing store
    int result = write_bs(testdata, TEST_BS1, 0);
    if (result == OK) {
        kprintf("  write_bs() successful: wrote page 0 to store %d\n", TEST_BS1);
    } else {
        kprintf("  ERROR: write_bs() failed\n");
        return;
    }
    
    // Read back the data
    result = read_bs(readbuf, TEST_BS1, 0);
    if (result == OK) {
        kprintf("  read_bs() successful: read page 0 from store %d\n", TEST_BS1);
    } else {
        kprintf("  ERROR: read_bs() failed\n");
        return;
    }
    
    // Verify data integrity
    int errors = 0;
    for (i = 0; i < NBPG; i++) {
        if (testdata[i] != readbuf[i]) {
            errors++;
            if (errors <= 5) {
                kprintf("  Data mismatch at offset %d: wrote %d, read %d\n", 
                        i, testdata[i], readbuf[i]);
            }
        }
    }
    
    if (errors == 0) {
        kprintf("  Data verification successful: all %d bytes match\n", NBPG);
    } else {
        kprintf("  ERROR: Data verification failed with %d mismatches\n", errors);
    }
    
    // Test invalid parameters
    result = read_bs(readbuf, MAX_BS, 0);
    if (result == SYSERR) {
        kprintf("  read_bs() correctly failed for invalid store ID\n");
    } else {
        kprintf("  ERROR: read_bs() should have failed but succeeded\n");
    }
    
    result = write_bs(testdata, TEST_BS1, 200);
    if (result == SYSERR) {
        kprintf("  write_bs() correctly failed for invalid page number\n");
    } else {
        kprintf("  ERROR: write_bs() should have failed but succeeded\n");
    }
    
    kprintf("\n");
}

void test_xmunmap() {
    kprintf("===== Testing xmunmap() =====\n");
    
    // Test xmunmap with valid parameters
    int result = xmunmap(VPAGE2);
    if (result == OK) {
        kprintf("  xmunmap() successful for virtpage=%d\n", VPAGE2);
    } else {
        kprintf("  ERROR: xmunmap() failed for virtpage=%d\n", VPAGE2);
    }
    
    // Verify the mapping was removed
    int store, pageth;
    long vaddr = VPAGE2 * 4096;
    result = bsm_lookup(currpid, vaddr, &store, &pageth);
    if (result == SYSERR) {
        kprintf("  Verification successful: mapping was removed\n");
    } else {
        kprintf("  ERROR: mapping still exists after xmunmap\n");
    }
    
    // Test xmunmap with invalid parameters
    result = xmunmap(-1);
    if (result == SYSERR) {
        kprintf("  xmunmap() correctly failed for invalid virtpage\n");
    } else {
        kprintf("  ERROR: xmunmap() should have failed but succeeded\n");
    }
    
    // Test xmunmap on already unmapped page
    result = xmunmap(VPAGE2);
    if (result == SYSERR) {
        kprintf("  xmunmap() correctly failed for already unmapped virtpage\n");
    } else {
        kprintf("  ERROR: xmunmap() should have failed but succeeded\n");
    }
    
    kprintf("\n");
}

void test_release_bs() {
    kprintf("===== Testing release_bs() =====\n");
    
    // Release a mapped backing store (should fail)
    int result = release_bs(TEST_BS1);
    if (result == SYSERR) {
        kprintf("  release_bs(%d) correctly failed for mapped BS\n", TEST_BS1);
    } else {
        kprintf("  ERROR: release_bs(%d) should have failed but succeeded\n", TEST_BS1);
    }
    
    // Unmap the backing store
    result = bsm_unmap(TESTPID1, VPAGE1, 1);
    if (result == OK) {
        kprintf("  bsm_unmap() successful for pid=%d, vpno=%d\n", TESTPID1, VPAGE1);
    } else {
        kprintf("  ERROR: bsm_unmap() failed for pid=%d, vpno=%d\n", TESTPID1, VPAGE1);
    }
    
    // Now try to release the backing store
    result = release_bs(TEST_BS1);
    if (result == OK) {
        kprintf("  release_bs(%d) successful after unmapping\n", TEST_BS1);
    } else {
        kprintf("  ERROR: release_bs(%d) failed after unmapping\n", TEST_BS1);
    }
    
    // Unmap and release the second backing store
    result = bsm_unmap(TESTPID2, VPAGE1, 1);
    if (result == OK) {
        kprintf("  bsm_unmap() successful for pid=%d, vpno=%d\n", TESTPID2, VPAGE1);
    } else {
        kprintf("  ERROR: bsm_unmap() failed for pid=%d, vpno=%d\n", TESTPID2, VPAGE1);
    }
    
        result = release_bs(TEST_BS2);
    if (result == OK) {
        kprintf("  release_bs(%d) successful after unmapping\n", TEST_BS2);
    } else {
        kprintf("  ERROR: release_bs(%d) failed after unmapping\n", TEST_BS2);
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

