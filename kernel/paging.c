#include "paging.h"
#include "mem.h"
#include "../drivers/screen.h"
#define DIRECTORY_SIZE 1024
#define ENTRY_SIZE 1024

extern int load_page_directory(PageDirectoryEntry *pageDirectory);
extern int enable_paging();

PageDirectoryEntry *pageDirectory;
PageTableEntry *firstPageEntryTable;

void init_paging() {
    pageDirectory = (PageDirectoryEntry *)  kmalloc(sizeof(PageDirectoryEntry) * DIRECTORY_SIZE, 1);
    firstPageEntryTable = (PageTableEntry *) kmalloc(sizeof(PageTableEntry) * ENTRY_SIZE, 1);

    // Initialize the page directory to be empty
    for ( int i = 0 ; i < DIRECTORY_SIZE ; i++) {
        pageDirectory[i].page_table_address = 0;

        // Not present
        pageDirectory[i].present = 0;
        pageDirectory[i].rw = 0;
        // Only kernel mode can access
        pageDirectory[i].user = 0;
        pageDirectory[i].write_through = 0;
        pageDirectory[i].disabled_cache = 0;
        pageDirectory[i].accessed = 0;
        pageDirectory[i].size = 0;
        pageDirectory[i].g = 0;
        pageDirectory[i].available = 0;
    }

    for (int i = 0 ; i < ENTRY_SIZE ; i++) {
        firstPageEntryTable[i].present = 1;
        firstPageEntryTable[i].rw = 1;
        // User can access
        firstPageEntryTable[i].user = 1;
        firstPageEntryTable[i].write_through = 0;
        firstPageEntryTable[i].disabled_cache = 0;
        firstPageEntryTable[i].accessed = 0;
        firstPageEntryTable[i].available = 0;
        firstPageEntryTable[i].page_address = (i * 0x1000) >> 11;
    }

    // Update the page directory to point to the page entries
    pageDirectory[0].page_table_address =  ((uint32_t) (firstPageEntryTable) >> 11);
    pageDirectory[0].present = 1;
    pageDirectory[0].rw = 1;
    pageDirectory[0].user = 1;

    int has_loaded_page_directory = load_page_directory(pageDirectory);
    if (has_loaded_page_directory != 1) {
        print_string("Did not set up page directory correctly\n");
        return;
    }
    int has_enabled_paging = enable_paging();
    if (has_enabled_paging != 1) {
        print_string("Did not set up paging correctly\n");
        return;
    }

    uint32_t *ptr = (uint32_t*) 0xA0000000;
    uint32_t do_page_fault = *ptr;
}
