#ifndef INSTRUCTION_STRUCTURE_H
#define INSTRUCTION_STRUCTURE_H

#define SIZE_BYTE_D		255

#define SIZE_DESTINATION	3
#define SIZE_SOURCE		3
#define SIZE_WIDTH		1

#define	SIZE_OFFSET_TC		13
#define SIZE_OPCODE_TC		3

#define SIZE_COUNT_FALSE	3
#define SIZE_COUNT_TRUE		3
#define SIZE_CODE		4
#define SIZE_OPCODE_CE		6

#define SIZE_OPCODE_DO		8
#define SIZE_SOURCE_TYPE	1

#define SIZE_OPCODE_RE		9

#define SIZE_CLEAR		3
#define SIZE_OPCODE_SO		9

#define SIZE_DATA		8
#define SIZE_OPCODE_RI		5

#define SIZE_OFFSET_RA		7
#define SIZE_OPCODE_RA		2

#define SIZE_CARRY		1
#define SIZE_ZERO		1
#define SIZE_NEGATIVE		1
#define SIZE_SLEEP		3
#define SIZE_OVERFLOW		1
#define SIZE_PRIORITY		3
#define SIZE_FLT		1
#define SIZE_NUL		4

#define SIZE_BIT		1

#define SIZE_BYTE		8

enum SOURCE{REGISTER, CONSTANT};

typedef union
{	
/*
* transferControl is used for:
* BL, BRA
*/
	struct
	{
		unsigned short offset      : SIZE_OFFSET_TC;
		unsigned short opcode      : SIZE_OPCODE_TC;
	}transfer_control;	
/*
* conditionalExecution is used for:
* CEX
*/
	struct
	{
		unsigned short count_false : SIZE_COUNT_FALSE;
		unsigned short count_true  : SIZE_COUNT_TRUE;
		unsigned short cexcode     : SIZE_CODE;
		unsigned short opcode      : SIZE_OPCODE_CE;
	}conditional_execution;	
/*
* twoOperand is used for:
* ADD, ADDC, SUB, SUBC, XOR, AND, BIT, BIC, BIS
*/	
	struct
	{
		unsigned short destination : SIZE_DESTINATION;
		unsigned short source      : SIZE_SOURCE;
		unsigned short data_width  : SIZE_WIDTH;
		unsigned short source_type : SIZE_SOURCE_TYPE;
		unsigned short opcode      : SIZE_OPCODE_DO;
	}double_operand;	
/*
* registerExchange is used for:
* MOV, SWAP
*/	
	struct
	{
		unsigned short destination : SIZE_DESTINATION;
		unsigned short source      : SIZE_SOURCE;
		unsigned short data_width  : SIZE_WIDTH;
		unsigned short opcode      : SIZE_OPCODE_RE;
	}register_exchange;	
/*
* noOperand is used for:
* SRA, RRC, SWPB, SXT
*/	
	struct
	{
		unsigned short destination : SIZE_DESTINATION;
		unsigned short clear       : SIZE_CLEAR;
		unsigned short data_width  : SIZE_WIDTH;
		unsigned short opcode      : SIZE_OPCODE_SO;
	}single_operand;
/*
* registerInitialization is used for:
* MOVL, MOVLZ, MOVLS, MOVH
*/
	struct
	{
		unsigned short destination : SIZE_DESTINATION;
		unsigned short data        : SIZE_DATA;
		unsigned short opcode      : SIZE_OPCODE_RI;
	}register_initialization;
/*
* relativeAddressing is used for:
* LDR, STR
*/
	struct
	{
		unsigned short destination : SIZE_DESTINATION;
		unsigned short source      : SIZE_SOURCE;
		unsigned short data_width  : SIZE_WIDTH;
		unsigned short offset      : SIZE_OFFSET_RA;
		unsigned short opcode      : SIZE_OFFSET_RA;
	}relative_addressing;
	
	unsigned short record;	
}type_record_xm;

typedef union
{
	struct
	{
		unsigned short zero	    : SIZE_BIT;
		unsigned short one	    : SIZE_BIT;
		unsigned short two	    : SIZE_BIT;
		unsigned short three	    : SIZE_BIT;
		unsigned short four 	    : SIZE_BIT;
		unsigned short five         : SIZE_BIT;
		unsigned short six	    : SIZE_BIT;
		unsigned short seven	    : SIZE_BIT;
		unsigned short eight	    : SIZE_BIT;
		unsigned short nine	    : SIZE_BIT;
		unsigned short ten 	    : SIZE_BIT;
		unsigned short eleven 	    : SIZE_BIT;
		unsigned short twelve	    : SIZE_BIT;
		unsigned short thirteen	    : SIZE_BIT;
		unsigned short fourteen	    : SIZE_BIT;
		unsigned short fifteen	    : SIZE_BIT;
	}bit;
	
	struct
	{
		unsigned short LSB          : SIZE_BYTE;
		unsigned short MSB          : SIZE_BYTE;
	}byte;
	
	unsigned short word;
	unsigned short data;
}bitwise;

typedef union
{
	struct
	{
		unsigned short C                 : SIZE_CARRY;
		unsigned short Z                 : SIZE_ZERO;
		unsigned short N                 : SIZE_NEGATIVE;
		unsigned short SLP               : SIZE_SLEEP;
		unsigned short V                 : SIZE_OVERFLOW;
		unsigned short priority_current  : SIZE_PRIORITY;
		unsigned short FLT               : SIZE_FLT;
		unsigned short NUL               : SIZE_NUL;
		unsigned short priority_previous : SIZE_PRIORITY;
	}status_bit;
	
	unsigned short program_status;

}type_program_status;

bitwise fetch(unsigned short program_counter);

void BL   (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void BRA  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void CEX  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void ADD  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void ADDC (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void SUB  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void SUBC (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void XOR  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void AND  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void BIT  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void BIC  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void BIS  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void MOV  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void SWAP (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void SRA  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void RRC  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void SWPB (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void SXT  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void MOVL (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void MOVLZ(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void MOVLS(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void MOVH (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void LDR  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);
void STR  (type_record_xm record_xm, unsigned short* registers, type_program_status* PSW);

typedef void(*instruction_name)(type_record_xm, unsigned short*, type_program_status*);
instruction_name decode(bitwise srec_data);

void set_PSW(bitwise destin_pre, bitwise destin_post, type_program_status* PSW, enum SIZE w);

#endif













