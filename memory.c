#include "types.h"
#include "memory.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BACKING_STORE_FILE "BACKING_STORE"

void frame_block_init(FrameBlock** block) {
	int i;
	for (i = 0; i < FRAME_SIZE; i++) {
		// (*block)->table[i] = NULL;
	}
}

void memory_init(PhysicalMemory** memory) {
	(*memory) = malloc(sizeof(PhysicalMemory));
	(*memory)->backing_store_pointer = fopen(BACKING_STORE_FILE, "rb");
  if ((*memory)->backing_store_pointer == NULL) {
  	printf("Backing store '%s' file is invalid\n", BACKING_STORE_FILE);
  	exit(-1);
  }
	(*memory)->backing_store_fd = fileno((*memory)->backing_store_pointer);
	int i;
	for (i = 0; i < FRAME_COUNT; i++) {
		(*memory)->table[i] = NULL;
	}
}

int memory_load(PhysicalMemory* memory, Offset offset, FrameNumber* frame_number) {
	//Offset is an n bit value. This can only represent 2^n values.
	//That's fine. We can map the 2^n values to the 2^(2*n) values by doing
	//FRAME_SIZE * offset 
	if (lseek(memory->backing_store_fd, offset * FRAME_SIZE, SEEK_SET) == -1) {
		return SOMETHING_IS_WRONG;
	}
	char byte[FRAME_SIZE];
	if (fread(byte, 1, FRAME_SIZE, memory->backing_store_pointer) != FRAME_SIZE) {
		return SOMETHING_IS_WRONG;
	}
	int i;
	for (i = 0; i < FRAME_COUNT; i++) {
		if (memory->table[i] != NULL) break;
	}
	*frame_number = i;
	FrameBlock* block = malloc(sizeof(FrameBlock));
	frame_block_init(&block);
	for (i = 0; i < FRAME_SIZE; i++) {
		block->table[i] = byte[i];
	}
	memory->table[*frame_number] = block;
	return 0;
}

int memory_get(PhysicalMemory* memory, FrameNumber frame_number, Offset offset, FrameValue* frame_value) {
	if(memory->table[frame_number] != NULL) {
		*frame_value = memory->table[frame_number]->table[offset]; 
		return 0;
	} else {
		printf("SOMTHING WRONG trying to access frame num %u\n", frame_number);
		return SOMETHING_IS_WRONG;
	} 
}