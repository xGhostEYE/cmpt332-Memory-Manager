/* CMPT 332
* Assignment 4
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#include <stdio.h>
#include <list.h>
#include <rtthreads.h>
#include <RttCommon.h>
#include <A1.h>
#include <reader-writer-monitor.h>
#include <math.h>
#include <time.h>

#define MAX_SLEEP 5

PhysicalMemFrame physical_memory[NUM_FRAMES];
int clock_hand;
int second_chance_clock_hand;
RttSem sem_counter;

int ClockAlgo();
int SecondChanceAlgo();

int RandomRange(int min, int max){
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

int GetPageNumber(int virtual_address) {
    return virtual_address / PAGE_SIZE;
}

void PageFault(Process *process, int page_number, int operation_type){
    int i;
    int frame_num;
    int removed_page_num;
    double start_time;
    double time_dif;    
    PhysicalMemFrame *frame;
    PageTableEntry *page_table_entry;
    Process *removed_process;
    process->page_fault_count++;
    start_time = ((double)clock())/CLOCKS_PER_SEC;
    frame_num = -1;
    for (i = 0; i < NUM_FRAMES; i++) {
        if (physical_memory[i].is_occupied == 0) {
            frame_num = i;
            printf("Process %lu found a free spot in mem\n", \
            process->thread_id.lid);
            break;
        }
    }
    if (frame_num == -1){
        /*Going to see if making this ranodm is a good idea for testing, but 1
        for now while testing clock algo*/
        printf("Process %lu no free spot found, using algo\n", 
            process->thread_id.lid);
        if (process->clock_or_second) {
            printf("Process %lu using ClockAlgo\n", process->thread_id.lid);
            frame_num = ClockAlgo();
        } else {
            printf("Process %lu using SecondChanceAlgo\n", 
                process->thread_id.lid);
            frame_num = SecondChanceAlgo();
        }
        frame = &physical_memory[frame_num];
        removed_process = frame->occupied_by;
        removed_page_num = frame->page_number;     
        removed_process->page_table[removed_page_num].valid_bit = 0;
        removed_process->page_table[removed_page_num].frame_number = -1;
        printf("Done replacing\n");
    }

    physical_memory[frame_num].is_occupied = 1;
    frame = &physical_memory[frame_num];
    frame->occupied_by = process;
    frame->page_number = page_number;
    frame->reference_bit = 1;
    if (operation_type == WRITE){
        frame->modify_bit = 1;
    }
    else{
        frame->modify_bit = 0;
    }

    page_table_entry = &process->page_table[page_number];
    page_table_entry->frame_number = frame_num;
    page_table_entry->valid_bit = 1;
    page_table_entry->reference_bit = 1;
    page_table_entry->modify_bit = frame->modify_bit;
    time_dif = (((double)clock())/CLOCKS_PER_SEC) - start_time;
    process->total_page_fault_time += time_dif;
    printf("Process %lu done pagefault\n", process->thread_id.lid);
}

void ReadMemory(Process *process, ReferenceStringEntry *virtual_address) {
    PageTableEntry *page_table_entry;
    PhysicalMemFrame *physical_mem_frame;
    int page_number;
    
    page_number = GetPageNumber(virtual_address->virtual_address);

    page_table_entry = &process->page_table[page_number];

    if (page_table_entry->valid_bit == 1) {
        printf("Process %lu reading VALID_BIT\n", process->thread_id.lid);
        page_table_entry->reference_bit = 1;
        physical_mem_frame = &physical_memory[page_table_entry->frame_number];
        physical_mem_frame->reference_bit = 1;
    }
    else {
        printf("Process %lu reading PAGEFAULT\n", process->thread_id.lid);
        PageFault(process, page_number, virtual_address->operation_type);
    }

    
}

void WriteMemory(Process *process, ReferenceStringEntry *virtual_address) {
    int page_number;
    PageTableEntry *page_table_entry;
    PhysicalMemFrame *physical_mem_frame;

    page_number = GetPageNumber(virtual_address->virtual_address);
    page_table_entry = &process->page_table[page_number];
    
    if (page_table_entry->valid_bit == 1) {
        printf("Process %lu writing VALID_BIT\n", process->thread_id.lid);
        page_table_entry->reference_bit = 1;
        page_table_entry->modify_bit = 1;

        physical_mem_frame = &physical_memory[page_table_entry->frame_number];
        physical_mem_frame->reference_bit = 1;
        physical_mem_frame->modify_bit = 1;
    }
    else {
        printf("Process %lu writing PAGEFAULT\n", process->thread_id.lid);
        PageFault(process, page_number, virtual_address->operation_type);      
    }
}

void ProcessSimulation(void* arg){
    Process *process;
    ReferenceStringEntry *RS_entry;
    int i;
    double execution_start_time;
    double execution_time_dif;
    double start_time;
    double time_dif;

    process = (Process*)arg;
    process->thread_id = RttMyThreadId();
    printf("Process %lu is in its own thread\n", process->thread_id.lid);
    /*this is set to 100 only to get results faster*/
    execution_start_time = ((double)clock())/CLOCKS_PER_SEC;
    for (i = 0; i < NUM_SIMULATIONS; i++) {
        process->total_mem_accesses++;
        RS_entry = &process->reference_string[i];

        if (RS_entry->operation_type == READ) {
            printf("Process %lu reading\n", process->thread_id.lid);
            start_time = ((double)clock())/CLOCKS_PER_SEC;
            StartRead();
            process->num_reads++;
            ReadMemory(process, RS_entry);
            StopRead();
            time_dif = (((double)clock())/CLOCKS_PER_SEC) - start_time;
            process->total_read_time += time_dif;
        } else if (RS_entry->operation_type == WRITE) {
            printf("Process %lu writing\n", process->thread_id.lid);
            start_time = ((double)clock())/CLOCKS_PER_SEC;
            StartWrite();
            process->num_writes++;
            WriteMemory(process, RS_entry);
            StopWrite();
            time_dif = (((double)clock())/CLOCKS_PER_SEC) - start_time;
            process->total_write_time += time_dif;
        }
        printf("Process %lu sleeping after read or write\n", 
            process->thread_id.lid);
        RttSleep(RandomRange(1,MAX_SLEEP));
        printf("Process %lu woke up!\n", process->thread_id.lid);
    }
    execution_time_dif = (((double)clock())/CLOCKS_PER_SEC) - 
      execution_start_time;
    process->total_execution_time = execution_time_dif;
    printf("Process %lu finished doing tasks.\n", process->thread_id.lid);
    RttV(sem_counter);
}

unsigned int GenerateVirtualAddress(Process *process){
    /*need to use Zipf*/
    int code_segment;
    int data_segment;
    int code_segment_page;
    int data_segment_page;
    int code_segment_boundary_start;
    int code_segment_boundary_end;
    int data_segment_boundary_start;
    int data_segment_boundary_end;
    int random_num;
    int chosen_segment_start;
    int chosen_segment_end;
    int i;
    double normalization_constant;
    double zipf_exponent;
    double rank_probability;
    double *rank_probabilities;
    double rank;
    int page_number;
    int offset;
    unsigned int virtual_address;
    int num_pages_segment;

    code_segment = 75;
    data_segment = 25;
    normalization_constant = 0;
    rank_probability = 0;
   
    code_segment_page = (process->num_pages * code_segment)/100;
    data_segment_page = (process->num_pages * data_segment)/100;

    code_segment_boundary_start = 0;
    code_segment_boundary_end = code_segment_boundary_start \
    + code_segment_page - 1;
    
    data_segment_boundary_start = code_segment_boundary_end + 1;
    data_segment_boundary_end = data_segment_boundary_start \
    + data_segment_page - 1;

    random_num = RandomRange(1,100);
    if (random_num < 25){
        chosen_segment_start = data_segment_boundary_start;
        chosen_segment_end = data_segment_boundary_end;
        zipf_exponent = 1.5;
    }
    else{
        chosen_segment_start = code_segment_boundary_start;
        chosen_segment_end = code_segment_boundary_end;
        zipf_exponent = 0.75;       
    }

    num_pages_segment = chosen_segment_end - chosen_segment_start + 1;
    rank_probabilities = (double*)malloc((num_pages_segment + 1) * 
        sizeof(double));
    rank_probabilities[0] = 0.0;

    for (i = 1; i < num_pages_segment; i++){
        normalization_constant += 1/pow(i, zipf_exponent);
    }

    for (i = 1; i < num_pages_segment; i++){
        rank_probability += (1/pow(i, zipf_exponent))/normalization_constant;
        rank_probabilities[i] = rank_probability;
    }

    random_num = RandomRange(0,1);

    for (i = 0; i < num_pages_segment; i++){
        if (random_num <= rank_probabilities[i]){
            rank = i;
            break;
        }
    }
    page_number = chosen_segment_start + (rank - 1);
    
    offset = RandomRange(0, PAGE_SIZE - 1);
    virtual_address = (page_number * PAGE_SIZE) + offset;
    free(rank_probabilities);
    return virtual_address;
}

int SecondChanceAlgo(){
    /*Reminder: need feedback on this!!!*/
    PhysicalMemFrame *memory_frame;
    while (1){
        if (second_chance_clock_hand >= NUM_FRAMES){
            printf("Resetting clock hand\n");
            second_chance_clock_hand = 0;
        }
        memory_frame = &physical_memory[second_chance_clock_hand];
        if (memory_frame->reference_bit == 0 && 
            memory_frame->modify_bit == 0){
            printf("SecondChance algo found a free spot\n");
            return second_chance_clock_hand;
        }
        else if (memory_frame->reference_bit == 0 && 
            memory_frame->modify_bit == 1){
            memory_frame->modify_bit = 0;
            second_chance_clock_hand ++;
        }
        else{
            memory_frame->reference_bit = 0;
            second_chance_clock_hand ++;
        }
    }
    return 0;
}

int ClockAlgo(){
    PhysicalMemFrame *memory_frame;
    while (1){
        if (clock_hand >= NUM_FRAMES){
            printf("Resetting clock hand\n");
            clock_hand = 0;
        }
        memory_frame = &physical_memory[clock_hand];
        if (memory_frame->reference_bit == 0){
            printf("Clock algo found a free spot\n");
            return clock_hand;
        }
        else{
            memory_frame->reference_bit = 0;
            clock_hand ++;
        }
    }
    return 0;
}

int mainp(int argc , char *argv[]) { 
    int i;
    int j;
    int x;
    int status;
    int num_clock;
    int num_second;
    Process processes[NUM_THREADS];
    RttSchAttr myattr;
    RttThreadId temp;
    double average_read_time_clock;
    double average_write_time_clock;
    double average_execution_time_clock;
    double average_page_fault_time_clock;
    double average_read_time_second;
    double average_write_time_second;
    double average_execution_time_second;
    double average_page_fault_time_second;
    int page_fault_rate_clock;
    int page_fault_rate_second;
    int total_mem_accesses_clock;
    int total_mem_accesses_second;
    
    /*here to get rid of the warninig for them*/
    (void)argc;
    (void)argv;

    clock_hand = 0;
    second_chance_clock_hand = 0;
    num_clock = 0;
    num_second = 0;
    average_read_time_clock = 0;
    average_write_time_clock = 0;
    average_execution_time_clock = 0;
    average_page_fault_time_clock = 0;
    average_read_time_second = 0;
    average_write_time_second = 0;
    average_execution_time_second = 0;
    average_page_fault_time_second = 0;
    page_fault_rate_clock = 0;
    page_fault_rate_second = 0;
    total_mem_accesses_clock = 0;
    total_mem_accesses_second = 0;
    
    srand(SRAND);
    printf("Setting up Monitor and counting semaphore\n");
    Initialize();
    RttAllocSem(&sem_counter, 0, RTTFCFS);

    printf("Setting up thread attributes\n");
    myattr.startingtime = RTTZEROTIME;
    myattr.priority = RTTNORM;
    myattr.deadline = RTTNODEADLINE; 

    printf("Setting up physical memory\n");
    for (i = 0; i < NUM_FRAMES; i++) {
        physical_memory[i].frame_number = i;
        physical_memory[i].is_occupied = 0;
        physical_memory[i].reference_bit = 0;
        physical_memory[i].modify_bit = 0;
        physical_memory[i].page_number = -1;
        physical_memory[i].occupied_by = NULL;
    }



    for (i = 0; i < NUM_THREADS; i++) {

        processes[i].num_pages = NUM_PAGES;
        processes[i].page_table = malloc(sizeof(PageTableEntry) * 
            processes[i].num_pages);
        processes[i].page_fault_count = 0;
        processes[i].num_reads = 0;
        processes[i].num_writes = 0;
        processes[i].clock_or_second = i%2;
        processes[i].total_page_fault_time = 0;
        processes[i].total_read_time = 0;
        processes[i].total_write_time = 0;
        processes[i].total_execution_time = 0;
        processes[i].total_mem_accesses = 0;
        if (processes[i].clock_or_second){
            num_clock++;
        }
        else{
            num_second++;
        }
        printf("Setting up page table\n");
        for (j = 0; j < processes[i].num_pages; j++) {
            processes[i].page_table[j].frame_number = -1;
            processes[i].page_table[j].valid_bit = 0;
            processes[i].page_table[j].reference_bit = 0;
            processes[i].page_table[j].modify_bit = 0;
        }

        printf("Generating reference string\n");
        processes[i].reference_string_length = 1000;
        processes[i].reference_string = malloc(sizeof(ReferenceStringEntry) \
        * processes[i].reference_string_length);

        for (x = 0; x < processes[i].reference_string_length; x++) {
            switch (RandomRange(0,1))
            {
            case WRITE:
                processes[i].reference_string[x].operation_type = WRITE;
                break;
            case READ:
                processes[i].reference_string[x].operation_type = READ;
                break;
            default:
                printf("============>This shouldn't be printing<======\n");
                break;
            }
            
            processes[i].reference_string[x].virtual_address = 
              GenerateVirtualAddress(&processes[i]);
        }

    }
    printf("Creating threads\n");
    for (i = 0; i < NUM_THREADS; i++){
        status = RttCreate(&temp, ProcessSimulation, 16000,"a", 
            &processes[i], myattr, RTTUSR);
        if (status == RTTFAILED) {
            printf("Failed to create thread\n");
            exit(1);
        }        
    }
    printf("Parent waiting\n");
    for (i = 0; i < NUM_THREADS; i++) {
        RttP(sem_counter);
        if (processes[i].clock_or_second){
            average_execution_time_clock += processes[i].total_execution_time;
            average_page_fault_time_clock += 
              processes[i].total_page_fault_time;
            average_read_time_clock += processes[i].total_read_time;
            average_write_time_clock += processes[i].total_write_time;
            page_fault_rate_clock += processes[i].page_fault_count;
            total_mem_accesses_clock += processes[i].total_mem_accesses;
        }
        else{
            average_execution_time_second += 
              processes[i].total_execution_time;
            average_page_fault_time_second += 
              processes[i].total_page_fault_time;
            average_read_time_second += processes[i].total_read_time;
            average_write_time_second += processes[i].total_write_time;
            page_fault_rate_second += processes[i].page_fault_count;
            total_mem_accesses_second += processes[i].total_mem_accesses;
        }
    }

    printf("ALL PROCESSES DONE!\n");
    for (i = 0; i < NUM_THREADS; i++){
        printf("Process %lu stats:\n", processes[i].thread_id.lid);
        if (processes[i].clock_or_second){
            printf("-ALGO: Clock\n");
            printf("  -TOTALS: Process %lu\n", processes[i].thread_id.lid);
            printf("    -Total process exectuion time: %0.6f\n", 
                processes[i].total_execution_time);
            printf("    -Total pagefault time: %0.6f\n", 
                processes[i].total_page_fault_time);
            printf("    -Total read time: %0.6f\n", 
                processes[i].total_read_time);
            printf("    -Total write time: %0.6f\n", 
                processes[i].total_write_time);
            printf("    -Total number of reads: %d\n", 
                processes[i].num_reads);
            printf("    -Total number of writes: %d\n", 
                processes[i].num_writes);
            printf("    -Total number of pagefaults: %d\n", 
                processes[i].page_fault_count);
        }
        else{
            printf("-ALGO: SecondChance\n");
            printf("  -TOTALS: Process %lu\n", processes[i].thread_id.lid);
            printf("    -Total process exectuion time: %0.6f\n", 
                processes[i].total_execution_time);
            printf("    -Total pagefault time: %0.6f\n", 
                processes[i].total_page_fault_time);
            printf("    -Total read time: %0.6f\n", 
                processes[i].total_read_time);
            printf("    -Total write time: %0.6f\n", 
                processes[i].total_write_time);
            printf("    -Total number of reads: %d\n", 
                processes[i].num_reads);
            printf("    -Total number of writes: %d\n", 
                processes[i].num_writes);
            printf("    -Total number of pagefaults: %d\n", 
                processes[i].page_fault_count);
        }
    }
    printf("-ALGO: Clock\n");    
    printf("  -TOTALS:\n");
    printf("    -Total execution time: %f\n", average_execution_time_clock);
    printf("    -Total pagefault time: %f\n", average_page_fault_time_clock);
    printf("    -Total read time: %f\n", average_read_time_clock);
    printf("    -Total write time: %f\n", average_write_time_clock);
    printf("    -Total pagefaults: %d\n", page_fault_rate_clock);
    printf("    -Total memory accesses: %d\n", total_mem_accesses_clock);
    printf("    -Pagefault rate: %f\n", 
        (double)page_fault_rate_clock/total_mem_accesses_clock);    
    printf("-ALGO: SecondChance\n");
    printf("  -TOTALS:\n");    
    printf("    -Total execution time: %f\n", average_execution_time_second);
    printf("    -Total pagefault time: %f\n", average_page_fault_time_second);
    printf("    -Total read time: %f\n", average_read_time_second);
    printf("    -Total write time: %f\n", average_write_time_second);
    printf("    -Total pagefaults: %d\n", page_fault_rate_second);
    printf("    -Total memory accesses: %d\n", total_mem_accesses_second);
    printf("    -Pagefault rate: %f\n", 
        (double)page_fault_rate_second/total_mem_accesses_second);

    average_execution_time_clock = average_execution_time_clock/num_clock;
    average_page_fault_time_clock = average_page_fault_time_clock/num_clock;
    average_read_time_clock = average_read_time_clock/num_clock;
    average_write_time_clock = average_write_time_clock/num_clock;

    average_execution_time_second = average_execution_time_second/num_second;
    average_page_fault_time_second = average_page_fault_time_second/
      num_second;
    average_read_time_second = average_read_time_second/num_second;
    average_write_time_second = average_write_time_second/num_second; 
    printf("-ALGO: Clock\n");  
    printf("  -AVERAGES:\n");
    printf("    -Average execution time: %f\n", average_execution_time_clock);
    printf("    -Average pagefault time: %f\n", 
        average_page_fault_time_clock);
    printf("    -Average read time: %f\n", average_read_time_clock);
    printf("    -Average write time: %f\n", average_write_time_clock);
    printf("-ALGO: SecondChance\n");
    printf("  -AVERAGES:\n");
    printf("    -Average execution time: %f\n", average_execution_time_second);
    printf("    -Average pagefault time: %f\n", 
        average_page_fault_time_second);
    printf("    -Average read time: %f\n", average_read_time_second);
    printf("    -Average write time: %f\n", average_write_time_second);    
    return 0;
}

