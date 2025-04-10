#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

#define TEST_BS1 1
#define TEST_BS2 2
#define NPAGES1 20
#define NPAGES2 30

void test_backing_store();
void test_direct_bs_access();
void test_bs_mapping();

int main() {
    kprintf("\n\n====== BACKING STORE AND MEMORY MAPPING TEST ======\n\n");
    
    test_backing_store();
    test_direct_bs_access();
    test_bs_mapping();
    
    return 0;
}

void test_backing_store() {
    kprintf("===== Testing Backing Store Management =====\n");
    
    // Test get_bs
    kprintf("Testing get_bs()...\n");
    int pages = get_bs(TEST_BS1, NPAGES1);
    if (pages == NPAGES1) {
        kprintf("  get_bs(%d, %d) successful, returned %d pages\n", TEST_BS1, NPAGES1, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", TEST_BS1, NPAGES1, pages);
    }
    
    // Test get_bs with existing backing store
    pages = get_bs(TEST_BS1, 50);
    if (pages == NPAGES1) {
        kprintf("  get_bs(%d, 50) correctly returned existing size of %d pages\n", TEST_BS1, pages);
    } else {
        kprintf("  ERROR: get_bs(%d, 50) failed, returned %d instead of %d\n", TEST_BS1, pages, NPAGES1);
    }
    
    // Test invalid parameters
    pages = get_bs(16, 10);
    if (pages == SYSERR) {
        kprintf("  get_bs(16, 10) correctly failed for invalid BS ID\n");
    } else {
        kprintf("  ERROR: get_bs(16, 10) should have failed but returned %d\n", pages);
    }
    
    // Test release_bs
    kprintf("\nTesting release_bs()...\n");
    int result = release_bs(TEST_BS1);
    if (result == OK) {
        kprintf("  release_bs(%d) successful\n", TEST_BS1);
    } else {
        kprintf("  ERROR: release_bs(%d) failed\n", TEST_BS1);
    }
    
    // Test release_bs with invalid ID
    result = release_bs(16);
    if (result == SYSERR) {
        kprintf("  release_bs(16) correctly failed for invalid BS ID\n");
    } else {
        kprintf("  ERROR: release_bs(16) should have failed but succeeded\n");
    }
    
    kprintf("\n");
}

void test_direct_bs_access() {
    kprintf("===== Testing Direct Backing Store Access =====\n");
    
    // Allocate a backing store
    int bs_id = TEST_BS2;
    int npages = NPAGES2;
    int pages = get_bs(bs_id, npages);
    if (pages != npages) {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", bs_id, npages, pages);
        return;
    }
    
    // Create test data
    char testdata[NBPG];
    char readbuf[NBPG];
    int i;
    
    // Initialize test data with recognizable pattern
    for (i = 0; i < NBPG; i++) {
        testdata[i] = (char)((i + 65) % 256); // ASCII values starting from 'A'
    }
    
    // Write test data to backing store
    kprintf("Writing test data to backing store %d, page 0...\n", bs_id);
    int result = write_bs(testdata, bs_id, 0);
    if (result == OK) {
        kprintf("  write_bs() successful\n");
    } else {
        kprintf("  ERROR: write_bs() failed\n");
        return;
    }
    
    // Modify test data to ensure we're reading from backing store
    for (i = 0; i < NBPG; i++) {
        testdata[i] = 0;
    }
    
    // Read back the data
    kprintf("Reading data from backing store %d, page 0...\n", bs_id);
    result = read_bs(readbuf, bs_id, 0);
    if (result == OK) {
        kprintf("  read_bs() successful\n");
    } else {
        kprintf("  ERROR: read_bs() failed\n");
        return;
    }
    
    // Verify data integrity
    int errors = 0;
    for (i = 0; i < 100; i++) { // Check first 100 bytes to keep output manageable
        if (readbuf[i] != (char)((i + 65) % 256)) {
            errors++;
            if (errors <= 5) {
                kprintf("  Data mismatch at offset %d: expected %d, got %d\n", 
                        i, (i + 65) % 256, readbuf[i]);
            }
        }
    }
    
    if (errors == 0) {
        kprintf("  Data verification successful: first 100 bytes match\n");
    } else {
        kprintf("  ERROR: Data verification failed with %d mismatches in first 100 bytes\n", errors);
    }
    
    // Write to multiple pages
    kprintf("\nWriting to multiple pages...\n");
    for (i = 1; i < 3; i++) {
        // Modify first byte to identify the page
        testdata[0] = (char)(i + 65);  // 'B', 'C', etc.
        
        result = write_bs(testdata, bs_id, i);
        if (result == OK) {
            kprintf("  write_bs() successful for page %d\n", i);
        } else {
            kprintf("  ERROR: write_bs() failed for page %d\n", i);
        }
    }
    
    // Read back and verify
    for (i = 1; i < 3; i++) {
        result = read_bs(readbuf, bs_id, i);
        if (result == OK) {
            kprintf("  read_bs() successful for page %d\n", i);
            
            if (readbuf[0] == (char)(i + 65)) {
                kprintf("  Page %d data verification successful\n", i);
            } else {
                kprintf("  ERROR: Page %d data verification failed: expected %c, got %c\n", 
                        i, (char)(i + 65), readbuf[0]);
            }
        } else {
            kprintf("  ERROR: read_bs() failed for page %d\n", i);
        }
    }
    
    // Clean up
    release_bs(bs_id);
    kprintf("\n");
}

void test_bs_mapping() {
    kprintf("===== Testing Backing Store Mapping Functions =====\n");
    
    // Allocate a backing store
    int bs_id = 3;
    int npages = 10;
    int pages = get_bs(bs_id, npages);
    if (pages != npages) {
        kprintf("  ERROR: get_bs(%d, %d) failed, returned %d\n", bs_id, npages, pages);
        return;
    }
    
    // Test xmmap
    kprintf("Testing xmmap()...\n");
    int vpage = 4096;
    int result = xmmap(vpage, bs_id, npages);
    if (result == OK) {
        kprintf("  xmmap() successful: mapped BS %d to virtual page %d\n", bs_id, vpage);
    } else {
        kprintf("  ERROR: xmmap() failed\n");
        return;
    }
    
    // Verify mapping in bsm_tab
    kprintf("Verifying mapping in bsm_tab...\n");
    if (bsm_tab[bs_id].bs_status == BSM_MAPPED && 
        bsm_tab[bs_id].bs_pid == currpid &&
        bsm_tab[bs_id].bs_vpno == vpage) {
        kprintf("  Mapping verification successful\n");
        kprintf("  bs_tab[%d]: status=%d, pid=%d, vpno=%d, npages=%d\n", 
                bs_id, bsm_tab[bs_id].bs_status, bsm_tab[bs_id].bs_pid, 
                bsm_tab[bs_id].bs_vpno, bsm_tab[bs_id].bs_npages);
    } else {
        kprintf("  ERROR: Mapping verification failed\n");
        kprintf("  bs_tab[%d]: status=%d, pid=%d, vpno=%d, npages=%d\n", 
                bs_id, bsm_tab[bs_id].bs_status, bsm_tab[bs_id].bs_pid, 
                bsm_tab[bs_id].bs_vpno, bsm_tab[bs_id].bs_npages);
    }
    
    // Test bsm_lookup
    kprintf("\nTesting bsm_lookup()...\n");
    int store, pageth;
    long vaddr = vpage * NBPG + 100;  // Address in the mapped region
    result = bsm_lookup(currpid, vaddr, &store, &pageth);
    if (result == OK) {
        kprintf("  bsm_lookup() successful for vaddr 0x%08lX\n", vaddr);
        kprintf("  Found: store=%d, page offset=%d\n", store, pageth);
        
        if (store == bs_id && pageth == 0) {
            kprintf("  Lookup verification successful\n");
        } else {
            kprintf("  ERROR: Lookup verification failed, expected store=%d, pageth=0\n", bs_id);
        }
    } else {
        kprintf("  ERROR: bsm_lookup() failed for vaddr 0x%08lX\n", vaddr);
    }
    
    // Test xmunmap
    kprintf("\nTesting xmunmap()...\n");
    result = xmunmap(vpage);
    if (result == OK) {
        kprintf("  xmunmap() successful for vpage %d\n", vpage);
    } else {
        kprintf("  ERROR: xmunmap() failed\n");
    }
    
    // Verify unmapping
    kprintf("Verifying unmapping...\n");
    result = bsm_lookup(currpid, vaddr, &store, &pageth);
    if (result == SYSERR) {
        kprintf("  Unmapping verification successful: address no longer mapped\n");
    } else {
        kprintf("  ERROR: Unmapping verification failed: address still mapped\n");
    }
    
    // Clean up
    release_bs(bs_id);
    kprintf("\n");
}
