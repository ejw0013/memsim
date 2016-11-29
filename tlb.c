#include "tlb.h"
#include "types.h"
#include "parser.h"
#include "memory.h"
#include "page.h"

#include <limits.h>

int tlb_scan(TLB* tlb, Address address, FrameNumber* frame_number) {
	printf("TLB scan called\n");
	PageNumber search = address.page_number;
	int i;
	for (i = 0; i < TLB_ENTRIES && tlb->table[i] != NULL; i++) {
		if (tlb->table[i]->page_number == search) {
			*frame_number = tlb->table[i]->frame_number; 
			tlb->table[i]->last_used = time(NULL);
			return 0;
		}
	}
	printf("TLB scan finished\n");
	return TLB_MISS;
}

int tlb_get(TLB* tlb, Address address, int mode, FrameValue* frame_value) {
	printf("TLB get called with address %u, %u\n", address.page_number, address.offset);
	FrameNumber frame_number;
	if (tlb_scan(tlb, address, &frame_number) == TLB_MISS) {
		page_get(tlb->page_table, address, &frame_number);
		if (mode == FIFO) {
			tlb_replace_fifo(tlb, address, frame_number);
		} else if (mode == LRU) {
			tlb_replace_lru(tlb, address, frame_number);
		} else {
			printf("Operation mode isn't available\n");
			exit(-1);
		}
		tlb_misses++;
		printf("TLB MISS\n");
		exit(-1);
		printf("TLB get finished\n");
	}
	//Go to memory
	if (memory_get(tlb->main_memory, frame_number, address.offset, frame_value) == SOMETHING_IS_WRONG) {
		printf("Undefined physical memory issue\n");
		exit(-1);
	}
	return 0;
}

int tlb_replace_fifo(TLB* tlb, Address address, FrameNumber frame_number) {
	//TLB first in is initialized to 0 at start.
	//So each time a new value comes in we want to increment it by one and then
	//Replace the value at it this gives us the effect of FIFO.  
	printf("tlb repalce fifo called\n");
	tlb_entry_init(tlb->table[tlb->first_in], address.page_number, frame_number);
	tlb->first_in = ((tlb->first_in + 1) % TLB_ENTRIES); 
	printf("tlb replace fifo finished\n");
	return 0;
}

int tlb_replace_lru(TLB* tlb, Address address, FrameNumber frame_number) {
	time_t lowest_time = UINT_MAX;
	int oldest_tlb = 0;
	int i;
	for (i = 0; i < TLB_ENTRIES && tlb->table[i] != NULL; i++) {
		if (lowest_time > tlb->table[i]->last_used) {
			oldest_tlb = i;
			lowest_time = tlb->table[i]->last_used;
		}
	}
	tlb_entry_init(tlb->table[oldest_tlb], oldest_tlb, frame_number);
}

void tlb_init(TLB* tlb) {
	printf("TLB init started\n");
	tlb = malloc(sizeof(TLB));
	int i;
	for (i = 0; i < TLB_ENTRIES; i++) {
		tlb->table[i] = NULL;
	}
	tlb->first_in = 0;
	memory_init(tlb->main_memory);
	page_init(tlb->page_table, tlb->main_memory);
	printf("TLB init ended\n");
}

void tlb_entry_init(TLBEntry* tlb_entry, PageNumber page_number, FrameNumber frame_number) {
	tlb_entry->page_number = page_number;
	tlb_entry->frame_number = frame_number;
	tlb_entry->last_used = time(NULL);
}