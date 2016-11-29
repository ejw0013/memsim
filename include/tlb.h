#ifndef _TLB_H
#define _TLB_H

#include "types.h"
#include "memory.h"
#include "page.h"

#include <stdio.h>
#include <time.h>


typedef struct {
	PageNumber page_number;
	FrameNumber frame_number;
	ValidBit valid_bit;
	time_t last_used;
} TLBEntry;

typedef struct {
	TLBEntry* table[TLB_ENTRIES];
	PhysicalMemory* main_memory;
	Page* page_table;
	int first_in;
} TLB;

void tlb_init(TLB* tlb);

int tlb_scan(TLB* tlb, Address address, FrameNumber* frame_number);

int tlb_get(TLB* tlb, Address address, int mode, FrameValue* frame_value);

int tlb_replace_fifo(TLB* tlb, Address address, FrameNumber frame_number);

int tlb_replace_lru(TLB* tlb, Address address, FrameNumber frame_number);

void tlb_entry_init(TLBEntry* tlb_entry, PageNumber page_number, FrameNumber frame_number);

#endif
