#include "../include/Declaration.h"
#include "../include/InstructionStructure.h"

/*
* This function takes the start and end address of the data along with the breakpoint and points to the register file.
* Each instruction is fetched, decoded and executed so long as the program counter does not match the breakpoint
*/

void srec_run(unsigned short* address_pair, unsigned short* registers, unsigned short breakpoint)
{
	bitwise srec_data;
	type_record_xm record_xm;
	instruction_name to_execute;	
	unsigned short run, skip;	
	type_program_status PSW;
	
	registers[R7] = address_pair[START];
	
	if(address_pair[STOP] < breakpoint)
		breakpoint = address_pair[STOP];
		
	printf("Start: PC: %04X, PSW: %04X, Brkpt: %04X Clk: %04X\n", registers[R7], PSW.program_status, breakpoint, sys_clock);
	
	while((registers[R7] < breakpoint) && !ctrl_c_fnd)
	{
		srec_data = fetch(registers[R7]);
		record_xm.record = srec_data.data;
		to_execute = decode(srec_data);
		to_execute(record_xm, registers, &PSW);
		sys_clock += EXECUTE_CLOCK_COUNT;
		if(cex_set)
		{
			if(cex_state)
			{
				run	 = record_xm.conditional_execution.count_true;
				skip = record_xm.conditional_execution.count_false;
			}
			else
			{
				skip = record_xm.conditional_execution.count_true;
				run = record_xm.conditional_execution.count_false;
			}
			while(skip)
			{
				srec_data = fetch(registers[R7]);
				record_xm.record = srec_data.data;
				to_execute = decode(srec_data);
				
				if(run = 0)
				{	
					skip--;
					continue;
				}
				else
				{	
					run--;
					to_execute(record_xm, registers, &PSW);
					sys_clock += EXECUTE_CLOCK_COUNT;
				}
				
				registers[R7] += SIZE_ASCII_PAIR;
			}
			cex_state = false;
		}
		
		registers[R7] += SIZE_ASCII_PAIR;
	}
	printf("End: PC: %04X, PSW: %04X, Brkpt: %04X Clk: %04X\n", registers[R7], PSW.program_status, breakpoint, sys_clock);
}

/*
* Instruction is fetched from memory and system clock is incremented.
*/
bitwise fetch(unsigned short program_counter)
{
	unsigned short memory_data;
	bitwise srec_data;
	
	bus(program_counter, &memory_data, READ, WORD);
	srec_data.data = memory_data;
	sys_clock += MEMORY_ACCESS_CLOCK_COUNT;
	sys_clock += FETCH_CLOCK_COUNT;
	return srec_data;
}

/*
* Register contents are displayed
*/
void display_registers(unsigned short* registers)
{
	printf("R0: %04X\n", registers[R0]);
	printf("R1: %04X\n", registers[R1]);
	printf("R2: %04X\n", registers[R2]);
	printf("R3: %04X\n", registers[R3]);
	printf("R4: %04X\n", registers[R4]);
	printf("R5: %04X\n", registers[R5]);
	printf("R6: %04X\n", registers[R6]);
	printf("R7: %04X\n", registers[R7]);
}

/*
* breakpoint is changed from default to what user sets
*/
unsigned short set_breakpoint(FILE* filestream_user, bool script_on, unsigned short breakpoint_old)
{
	unsigned short breakpoint_new;

	fscanf(filestream_user, "%04hX", &breakpoint_new);
	if(script_on) printf("%04hX", breakpoint_new);
	printf("Current breakpoint: %04hx\n", breakpoint_old);
	printf("Set breakpoint address\n");
	return breakpoint_new;
}

/*
* Binary if-else search tree compares each bit of the srecord data. If a match
* to the opcode is found, a function pointer pointing to the relevent intruction
* is returned
*/
instruction_name decode(bitwise srec_data)
{
	instruction_name instruction;

	if(srec_data.bit.fifteen)
	{
		if(srec_data.bit.fourteen)
			return instruction = STR;
		else
			return instruction = LDR;
	}
	else
	{
		if(srec_data.bit.fourteen)
		{
		    if(srec_data.bit.thirteen)
		    {
		    	if(srec_data.bit.twelve)
		   		{
		   			if(srec_data.bit.eleven)
						return instruction = MOVH;
					else
						return instruction = MOVLS;
				}
				else
				{
					if(srec_data.bit.eleven)
						return instruction = MOVLZ;
					else
						return instruction = MOVL;
				}
		    }
		    else
		    {
		    	if(srec_data.bit.eleven)
		    	{
		    		if(srec_data.bit.ten)
		    		{
		                if(srec_data.bit.nine)
		                {
		                    if(srec_data.bit.three)
		                    	return instruction = SXT;
		                    else
		                    	return instruction = SWPB;
		                }
		                else
		                {
		                    if(srec_data.bit.eight)
		                    {
		                        if(srec_data.bit.seven)
		                        	return instruction = RRC;
		                        else
		                        	return instruction = SRA;
		                    }
		                    else
		                    {
		                        if(srec_data.bit.seven)
		                        {
		                            return instruction = SWAP;
		                        }
		                        else
		                        {
		                            return instruction = MOV;
		                        }
		                    }
		                }
		            }
		            else
		            {
		                if(srec_data.bit.nine)
		               		return instruction = BIS;
		               	else
		               	{
		               		if(srec_data.bit.eight)
		               			return instruction = BIC;
		               		else
		               			return instruction = BIT;
		               	}
		            }
		        }  
		        else
		        {
		            if(srec_data.bit.ten)
		            {
	            		if(srec_data.bit.eight)
	            			return instruction = AND;
	            		else
	            			return instruction = XOR;
		            }
		            else
		            {
		            	if(srec_data.bit.nine)
		            	{
		            		if(srec_data.bit.eight)
		            			return instruction = SUBC;
		            		else
		            			return instruction = SUB;
		            	}
		            	else
		            	{
		            		if(srec_data.bit.eight)
		            			return instruction = ADDC;
		            		else
		            			return instruction = ADD;
		            	}
		            }
		        }
		    }
		}        
		else
		{
			if(srec_data.bit.thirteen)
			{
				if(srec_data.bit.ten)
				{
					return instruction = CEX;
				}
				else
				{
					return instruction = BRA;
				}
			}
			else
			{
				return instruction = BL;
			}
		}
	}
	sys_clock += DECODE_CLOCK_COUNT;
}









