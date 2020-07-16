#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

/**
 *
 * Paging is implemented as:
 *
 *  [ Paging Directory ] -> [Page Directory Entry] -> [Page Table Entry]
 *
 **/

/**
 * The topmost paging structure
 */
struct page_directory_entry {
    /**
     * 1 if the page is present in physical memory, 0 otherwise.
     * If this page is called, and this is zero, then a page fault will occurr.
     **/
    uint32_t present: 1;

    /**
     * 0 = read only, 1 if read/write. This does not apply when running kernel mode.
     **/
    uint32_t rw : 1;

    /**
     * 1 = user mode, 0 if supervisor (kernel) mode.
     * When 1, this page can be accessed by all If 0, only the supervisor may access it.
     **/
    uint32_t user: 1;

    /**
     * 1 = write through caching is enabled, 0 = disabled.
     **/
    uint32_t write_through: 1;

    /**
    * 1 if cache is disabled, 0 otherwise
    **/
    uint32_t disabled_cache: 1;

    /**
    * 1 if the page has been read or written to, 0 otherwise.
    * This bit will not be cleared by the CPU, it should be maintained by the OS.
    **/
    uint32_t accessed: 1;

    /**
     * Zero bit
     **/
    uint32_t : 1;

    /**
     * 1 if the page size is 4MiB in size, otherwise they are 4KiB.
     * 4MiB pages require PSE enabled - https://en.wikipedia.org/wiki/Page_Size_Extension
     * Which is not enabled.
     **/
    uint32_t size: 1;

    /**
     * G bit, ignored
     **/
    uint32_t g: 1;

    /**
     * Available/free bits, not used.
     **/
    uint32_t available: 2;

    /**
    * The physical page table address, shifted right 11 bits
    **/
    uint32_t page_table_address: 20;

};

/**
 * The topmost paging structure
 */
struct page_table_entry {
    /**
     * 1 if the page is present in physical memory, 0 otherwise.
     * If this page is called, and this is zero, then a page fault will occurr.
     **/
    uint32_t present: 1;

    /**
     * 0 = read only, 1 if read/write. This does not apply when running kernel mode.
     **/
    uint32_t rw : 1;

    /**
     * 1 = user mode, 0 if supervisor (kernel) mode.
     * When 1, this page can be accessed by all If 0, only the supervisor may access it.
     **/
    uint32_t user: 1;

    /**
     * 1 = write through caching is enabled, 0 = disabled.
     **/
    uint32_t write_through: 1;

    /**
    * 1 if cache is disabled, 0 otherwise
    **/
    uint32_t disabled_cache: 1;

    /**
    * 1 if the page has been read or written to, 0 otherwise.
    * This bit will not be cleared by the CPU, it should be maintained by the OS.
    **/
    uint32_t accessed: 1;

    /**
     * 1 indicates that the page has been written to. No
     **/
    uint32_t dirty : 1;

    /**
     * 1 if the page size is 4MiB in size, otherwise they are 4KiB.
     * 4MiB pages require PSE enabled - https://en.wikipedia.org/wiki/Page_Size_Extension
     * Which is not enabled.
     **/
    uint32_t size: 1;

    /**
     * G bit, ignored
     **/
    uint32_t : 1;

    /**
     * Available/free bits, not used.
     **/
    uint32_t available: 2;

    /**
    * The physical page address, shifted right 11 bits
    **/
    uint32_t page_address: 20;

};

void init_paging();

#endif
