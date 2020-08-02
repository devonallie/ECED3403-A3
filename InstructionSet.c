#include "../include/Declaration.h"
#include "../include/InstructionStructure.h"

unsigned short encoded_constant[] = {0, 1, 2, 3, 4, 8, 16, 32, -1};

/* See introduction to XM3 ISA */

/*
* This instruction stores the program counter to the link register then
* the counter is shifted by the decoded offset.
*/

void BL(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise offset;
	offset.data = record_xm.transfer_control.offset;
	
	if(offset.bit.thirteen)
	{
		offset.bit.fifteen 	= BIT_SET;
		offset.bit.fourteen = BIT_SET;
	}
	else
	{
		offset.bit.fifteen 	= BIT_CLEAR;
		offset.bit.fourteen = BIT_CLEAR;
	}
	offset.data <<= 1;
	
	type_record_xm store;
	store.record = 0b110000000101110;
	STR(store, registers, PSW); //stores the content of the link register in memory pointed by the stack pointer
	registers[R5] = registers[R7];
	registers[R7] += offset.data;
}
/*
* im gonna have a heart attack if I comment the rest of these...
*/

void BRA(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise offset;
	offset.data = record_xm.transfer_control.offset;
	
	if(offset.bit.ten)
	{
		offset.bit.fifteen 	= BIT_SET;
		offset.bit.fourteen = BIT_SET;
		offset.bit.thirteen = BIT_SET;
		offset.bit.twelve	= BIT_SET;
		offset.bit.eleven	= BIT_SET;
	}
	else
	{
		offset.bit.fifteen 	= BIT_CLEAR;
		offset.bit.fourteen = BIT_CLEAR;
		offset.bit.thirteen = BIT_CLEAR;
		offset.bit.twelve	= BIT_CLEAR;
		offset.bit.eleven	= BIT_CLEAR;
	}
	offset.data <<= 1;
	
	registers[R7] += offset.data;
}

void CEX(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	cex_set = true;	
	bitwise cex_code;
	
	cex_code.data = record_xm.conditional_execution.cexcode; 
	
	if(cex_code.bit.three)
	{
		if(cex_code.bit.two)
		{
			if(cex_code.bit.zero)
			{
				if(PSW->status_bit.Z && (PSW->status_bit.N != PSW->status_bit.V))
					cex_state = true;
				else
					cex_state = false;
			}
			else
			{
				if(!PSW->status_bit.Z && (PSW->status_bit.N == PSW->status_bit.V))
					cex_state = true;
				else
					cex_state = false;
			}
		}
		else
		{
			if(cex_code.bit.one)
			{
				if(cex_code.bit.zero)
				{
					if((PSW->status_bit.N != PSW->status_bit.V))
						cex_state = true;
					else
						cex_state = false;
				}
				else
				{
					if((PSW->status_bit.N == PSW->status_bit.V))
						cex_state = true;
					else
						cex_state = false;
				}
			}
			else
			{
				if(cex_code.bit.zero)
				{
					if((!PSW->status_bit.C && PSW->status_bit.Z))
						cex_state = true;
					else
						cex_state = false;
				}
				else
				{
					if((PSW->status_bit.C && !PSW->status_bit.Z))
						cex_state = true;
					else
						cex_state = false;
				}
			}
		}
	}
	else
	{
		if(cex_code.bit.two)
		{
			if(cex_code.bit.one)
			{
				if(cex_code.bit.zero)
				{
					if(!PSW->status_bit.V)
						cex_state = true;
					else
						cex_state = false;
				}
				else
				{
					if(PSW->status_bit.V)
						cex_state = true;
					else
						cex_state = false;
				}
			}
			else
			{
				if(cex_code.bit.zero)
				{
					if(!PSW->status_bit.N)
						cex_state = true;
					else
						cex_state = false;
				}
				else
				{
					if(PSW->status_bit.N)
						cex_state = true;
					else
						cex_state = false;
				}
			}
		}
		else
		{
			if(cex_code.bit.one)
			{
				if(cex_code.bit.zero)
				{
					if(!PSW->status_bit.C)
						cex_state = true;
					else
						cex_state = false;
				}
				else
				{
					if(PSW->status_bit.C)
						cex_state = true;
					else
						cex_state = false;
				}
			}
			else
			{
				if(cex_code.bit.zero)
				{
					if(!PSW->status_bit.Z)
						cex_state = true;
					else
						cex_state = false;
				}
				else
				{
					if(PSW->status_bit.Z)
					{
						cex_state = true;
					}
					else
						cex_state = false;
				}
			}
		}
	}
}

void ADD(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{	
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word + source.word;
		}
		else
		{	
			destin_post.word = destin_pre.word + constant.word;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB + source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB + constant.byte.LSB;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}	
}

void ADDC(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{	
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word + source.word + PSW->status_bit.C;
		}
		else
		{	
			destin_post.word = destin_pre.word + constant.word + PSW->status_bit.C;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB + source.byte.LSB + PSW->status_bit.C;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB + constant.byte.LSB + PSW->status_bit.C;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}
}

void SUB(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word + ~source.word + 1;
		}
		else
		{	
			destin_post.word = destin_pre.word + ~constant.word + 1;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB + ~source.byte.LSB + 1;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB + ~constant.byte.LSB + 1;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}
}

void SUBC(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{	
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word + ~source.word + PSW->status_bit.C;
		}
		else
		{	
			destin_post.word = destin_pre.word + ~constant.word + PSW->status_bit.C;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB + ~source.byte.LSB + PSW->status_bit.C;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB + ~constant.byte.LSB + PSW->status_bit.C;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}
}

void XOR(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word ^ source.word;
		}
		else
		{	
			destin_post.word = destin_pre.word ^ constant.word;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB ^ source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB ^ constant.byte.LSB;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}
}

void AND(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word & source.word;
		}
		else
		{	
			destin_post.word = destin_pre.word & constant.word;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB & source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB & constant.byte.LSB;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}

}

void BIT(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word & source.word;
		}
		else
		{	
			destin_post.word = destin_pre.word & constant.word;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB & source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB & constant.byte.LSB;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
	}	
}

void BIC(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word & ~source.word;
		}
		else
		{	
			destin_post.word = destin_pre.word & ~constant.word;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB & ~source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB & ~constant.byte.LSB;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}
}

void BIS(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = destin_pre.word | source.word;
		}
		else
		{	
			destin_post.word = destin_pre.word | constant.word;
		}
		set_PSW(destin_pre, destin_post, PSW, WORD);
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = destin_pre.byte.LSB | source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = destin_pre.byte.LSB | constant.byte.LSB;
		}
		set_PSW(destin_pre, destin_post, PSW, BYTE);
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}
}

void MOV(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destin_pre, destin_post, source, constant;
	source.data = registers[record_xm.double_operand.source];
	destin_pre.data = registers[record_xm.double_operand.destination];
	constant.data = encoded_constant[record_xm.double_operand.source];
	
	if(record_xm.double_operand.data_width == WORD)
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.word = source.word;
		}
		else
		{	
			destin_post.word = constant.word;
		}
		registers[record_xm.double_operand.destination] = destin_post.word;
	}
	else
	{
		if(record_xm.double_operand.source_type == REGISTER)
		{
			destin_post.byte.LSB = source.byte.LSB;
		}
		else
		{	
			destin_post.byte.LSB = constant.byte.LSB;
		}
		registers[record_xm.double_operand.destination] = destin_post.byte.LSB;
	}	
}
void SWAP(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	unsigned short temp = registers[record_xm.register_exchange.destination];
	registers[record_xm.register_exchange.destination] = registers[record_xm.register_exchange.source];
	registers[record_xm.register_exchange.source] = temp;
}
void SRA(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.single_operand.destination];
	destination.bit.zero = PSW->status_bit.C;
	bool MSB;
	
	if(record_xm.double_operand.data_width == WORD)
	{
		MSB = destination.bit.fifteen;
		destination.data >>= 1;
		if(MSB)
			destination.bit.fifteen = BIT_SET;
		else
			destination.bit.fifteen = BIT_CLEAR;
			
		destination.bit.fourteen = BIT_CLEAR;
	}	
	else
	{
		MSB = destination.bit.seven;
		destination.data >>= 1;
		if(MSB)
			destination.bit.seven = BIT_SET;
		else
			destination.bit.seven = BIT_CLEAR;
			
		destination.bit.six = BIT_CLEAR;
	}
	
	registers[record_xm.single_operand.destination] = destination.data;
}
void RRC(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.single_operand.destination];
	destination.bit.zero = PSW->status_bit.C;
	bool MSB = PSW->status_bit.C;
	
	if(record_xm.double_operand.data_width == WORD)
	{
		destination.data >>= 1;
		destination.bit.fifteen = MSB;	
	}	
	else
	{
		destination.data >>= 1;
		destination.bit.seven = MSB;
	}
	
	registers[record_xm.single_operand.destination] = destination.data;

}
void SWPB(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{	
	bitwise destination;
	destination.data = registers[record_xm.single_operand.destination];
	registers[record_xm.single_operand.destination] = destination.byte.LSB << SIZE_BYTE | destination.byte.MSB;	
}
void SXT(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.single_operand.destination];
	if(destination.bit.seven)
		destination.byte.MSB = BYTE_SET;
	else
		destination.byte.MSB = BYTE_CLEAR;
	
	registers[record_xm.single_operand.destination] = destination.data;
}
void MOVL(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.register_initialization.destination];
	destination.byte.LSB = record_xm.register_initialization.data;
	registers[record_xm.register_initialization.destination] = destination.data;
}

void MOVLZ(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.register_initialization.destination];
	destination.byte.LSB = record_xm.register_initialization.data;
	destination.byte.MSB = BYTE_CLEAR;
	registers[record_xm.register_initialization.destination] = destination.data;
}

void MOVLS(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.register_initialization.destination];
	destination.byte.LSB = record_xm.register_initialization.data;
	destination.byte.MSB = BYTE_SET;
	registers[record_xm.register_initialization.destination] = destination.data;
}

void MOVH(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise destination;
	destination.data = registers[record_xm.register_initialization.destination];
	destination.byte.MSB = record_xm.register_initialization.data;
	registers[record_xm.register_initialization.destination] = destination.data;
}

void LDR(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise offset, source;
	offset.data = record_xm.relative_addressing.offset;
	
	source.data = registers[record_xm.relative_addressing.source];

	if(record_xm.relative_addressing.data_width == WORD)
	{
		if(offset.bit.six)
		{
			offset.byte.MSB = BYTE_SET;
			offset.bit.seven = BIT_SET;
		}
		else
		{
			offset.byte.MSB = BYTE_CLEAR;
			offset.bit.seven = BIT_CLEAR;
		}
	}
	else
	{
		if(offset.bit.six)
		{
			offset.bit.seven = BIT_SET;
		}
		else
		{
			offset.bit.seven = BIT_CLEAR;
		}
	}
	unsigned short data, effective_address;
	effective_address = source.data + offset.data;
	bus(effective_address, &data, READ, WORD);
	registers[record_xm.relative_addressing.destination] = data;
	
	sys_clock += MEMORY_ACCESS_CLOCK_COUNT;
}

void STR(type_record_xm record_xm, unsigned short* registers, type_program_status* PSW)
{
	bitwise offset, destination;
	offset.data = record_xm.relative_addressing.offset;
	
	destination.data = registers[record_xm.relative_addressing.destination];

	if(record_xm.relative_addressing.data_width == WORD)
	{
		if(offset.bit.six)
		{
			offset.byte.MSB = BYTE_SET;
			offset.bit.seven = BIT_SET;
		}
		else
		{
			offset.byte.MSB = BYTE_CLEAR;
			offset.bit.seven = BIT_CLEAR;
		}
	}
	else
	{
		if(offset.bit.six)
		{
			offset.bit.seven = BIT_SET;
		}
		else
		{
			offset.bit.seven = BIT_CLEAR;
		}
	}
	unsigned short data, effective_address;
	effective_address = destination.data + offset.data;
	data = registers[record_xm.relative_addressing.source];
	bus(effective_address, &data, WRITE, WORD);
	
	sys_clock += MEMORY_ACCESS_CLOCK_COUNT;
}

void set_PSW(bitwise destin_pre, bitwise destin_post, type_program_status* PSW, enum SIZE bw)
{
	switch(bw)
	{
		case WORD:
		if(destin_post.bit.fifteen)
			PSW->status_bit.N = BIT_SET;
		else
			PSW->status_bit.N = BIT_CLEAR;
		
		if(destin_pre.bit.fifteen != destin_post.bit.fifteen) 
			PSW->status_bit.C = BIT_SET;
		else
			PSW->status_bit.C = BIT_CLEAR;
			
		if(destin_pre.bit.fifteen != destin_post.bit.fifteen)
			PSW->status_bit.V = BIT_SET;
		else
			PSW->status_bit.V = BIT_CLEAR;
			
		case BYTE:
		if(destin_post.bit.seven)
			PSW->status_bit.N = BIT_SET;
		else
			PSW->status_bit.N = BIT_CLEAR;
			
		if(destin_post.data > SIZE_BYTE_D)
			PSW->status_bit.C = BIT_SET;
		else
			PSW->status_bit.C = BIT_CLEAR;

		if(destin_pre.bit.seven != destin_post.bit.seven)
			PSW->status_bit.V = BIT_SET;
		else
			PSW->status_bit.V = BIT_CLEAR;
	}

	if(destin_post.data == BIT_CLEAR)
		PSW->status_bit.Z = BIT_SET;
	else
		PSW->status_bit.Z = BIT_CLEAR;

}

























