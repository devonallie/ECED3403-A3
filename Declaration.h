#ifndef DECLARATION_H
#define DECLARATION_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define SIZE_SREC_MAX 78
#define SIZE_CHECKSUM 2
#define SIZE_BYTE 8
#define SIZE_WORD 16
#define ASCII_SPACE 32
#define ASCII_DEL 127
#define SIZE_ASCII_PAIR 2
#define DATA_START 8
#define INDEX_OFFSET 1

#define SIZE_MEMORY_MAX 65536
#define SIZE_FILENAME_MAX 80
#define SIZE_SREC_TYPE 2

#define SIZE_REGISTER_SET 8

#define MEMORY_INCREMENT 1
#define SIZE_ADDRESS_PAIR 2

#define BYTE_CLEAR 0x00
#define BYTE_SET 0xFF

#define MEMORY_ACCESS_CLOCK_COUNT 3
#define FETCH_CLOCK_COUNT 1
#define DECODE_CLOCK_COUNT 1
#define EXECUTE_CLOCK_COUNT 1

enum CONDITION_BIT{BIT_CLEAR, BIT_SET};
enum ACTION{READ, WRITE};
enum SIZE{WORD, BYTE};
enum REGISTER{R0, R1, R2, R3, R4, R5, R6, R7};
enum ADDRESS{START, STOP};

volatile sig_atomic_t ctrl_c_fnd;
void sigint_hdlr();

void debug(FILE* filestream_user, bool script_on);
void srec_load(FILE* filestream_srec, unsigned short* address_pair);
void srec_dump(FILE* filestream_user, bool script_on);
void srec_run(unsigned short* address_pair, unsigned short* registers, unsigned short breakpoint);
void display_registers(unsigned short* registers);
unsigned short set_breakpoint(FILE* filestream_user, bool script_on, unsigned short breakpoint_old);

FILE* script(void);
FILE* xme(FILE* filestream_user, bool stript_on);

void bus(unsigned short mar, unsigned short *mbr, enum ACTION rw, enum SIZE bw);

bool cex_state, cex_set;

unsigned short sys_clock;

#endif




















