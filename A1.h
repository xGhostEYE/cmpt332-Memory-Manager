/* CMPT 332
* Assignment 4
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/
#ifndef A1_H
#define A1_H

#include <rtthreads.h>
#include <RttCommon.h>
#include <stdio.h>
#include <list.h>


#define PAGE_SIZE 4096
#define NUM_FRAMES 2
#define NUM_THREADS 2
#define NUM_SIMULATIONS 5
#define NUM_PAGES 20
#define SRAND 32
#define READ 0
#define WRITE 1

typedef struct{
    /*Physical frame number*/
    int frame_number;
    /*1 if the page is in physical memory, 0 otherwise*/
    int valid_bit;
    /*Used by the algos*/
    int reference_bit;
    /* 1 if the page has been modified, 0 otherwise*/
    int modify_bit;

}PageTableEntry;

typedef struct{
    /*1 for write, 0 for read*/
    int operation_type;
    /*Virtual address being accessed*/
    unsigned int virtual_address;
}ReferenceStringEntry;

typedef struct{
    RttThreadId thread_id;
    PageTableEntry *page_table;
    ReferenceStringEntry *reference_string;
    int reference_string_length;
    int page_fault_count;
    int num_reads;
    int num_writes;
    int num_pages;
    int clock_or_second;
    int total_mem_accesses;
    double total_page_fault_time;
    double total_read_time;
    double total_write_time;
    double total_execution_time;
}Process;

typedef struct{
    /*Physical frame number*/
    int frame_number;
    /*Pointer to the owning process*/
    Process *occupied_by;
    /*Is occupied or not 1 yes, 0 no*/
    int is_occupied;
    /*Used by the algos*/
    int reference_bit;
    /*1 if the page has been modified*/
    int modify_bit;
    /*Virtual page number stored in this frame*/
    int page_number;
}PhysicalMemFrame;

#endif