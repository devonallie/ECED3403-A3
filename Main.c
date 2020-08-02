#include "../include/Declaration.h"

/*
* File: Main.c
* Author: Devon Allie
* School: Dalhousie
* Dept: Electrical and Computer Engineering
* Course: ECED3403 Computer Engineering
* 
* Purpose: This programme loads into emulated memory, a set of s-records.
* The contents of the s-records can be dumped by the user for evaluation.
* The contents of the s-records can be executed to implement the XM-3 ISA.
* The user can set a breakpoint to limit how many instruction are executed.
* The user can display the contents of each CPU register.
*/

/*
* The main function reads command line arguments. If the argument is -d, the
* argument following will be the script that runs through the program for the
* purposes of debugging. Instead of reading user-input, commands will be taken
* from the script.
*/

int main(int argc, char** argv)
{	
	char option;
	FILE* filestream_user = stdin;
	bool script_on = false;
	
	while((option = getopt(argc, argv, "d:")) != -1)
	{
		switch(option)
		{
			case 'd':
			filestream_user = script();
			script_on = true;
			break;
		}
	}	
	debug(filestream_user, script_on);
	fclose(filestream_user);	
	return EXIT_SUCCESS;
}

/*
* The debug function will loop through the valid commands. If the command is 
* not valid an error will be displayed. If the command is to dump a loop will
* call the bus function to print the data in memory for each location called.
* If the command is to load, the relevant file will be loaded and each S1
* record will be loaded into memory. The default case is for the commands that
* are invalid.
*/
void debug(FILE* filestream_user, bool script_on)
{

	unsigned short address_pair[SIZE_ADDRESS_PAIR];
	unsigned short breakpoint;
	FILE* filestream_srec;
	unsigned short registers[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

	while(true)
	{
		ctrl_c_fnd = false;
		signal(SIGINT, sigint_hdlr);
		
		printf("Option: ");
		char command = fgetc(filestream_user);
		if(script_on) printf("%c", command);
		
		switch(command)
		{
			case 'X': case 'x':
			return;
			
			case 'L': case 'l':
			filestream_srec = xme(filestream_user, script_on);
			srec_load(filestream_srec, address_pair);
			breakpoint = address_pair[STOP];
			break;
			
			case 'D': case 'd':
			srec_dump(filestream_user, script_on);
			break;
			
			case 'B': case 'b':
			breakpoint = set_breakpoint(filestream_user, script_on, breakpoint);
			break;
			
			case 'G': case 'g':
			srec_run(address_pair, registers, breakpoint);
			break;
			
			case 'R': case 'r':
			display_registers(registers);
			break;
			
			default:
			printf("Invalid command!\n");
			
		}	
		(void)fgetc(filestream_user);
	}
}
/*
* Takes the write direction and byte size from the callee. The data is 
* converted to its ascii equivalent and then saved to the emulated memory.
* If data is being, mbr is set to the address of the first integer in the 
* memory array.
*/

void bus(unsigned short mar, unsigned short *mbr, enum ACTION rw, enum SIZE bw)
{	
	static unsigned char memory[SIZE_MEMORY_MAX];
	unsigned short mar_next;

	switch(rw)
	{
		case WRITE:
		memory[mar] = *mbr;
		break;
		
		case READ:
		switch(bw)
		{
			case BYTE:
			*mbr = memory[mar];
			break;
			
			case WORD:
			mar_next = mar + MEMORY_INCREMENT;
			*mbr = memory[mar_next] << SIZE_BYTE | memory[mar];
			break;
		}
		break;
	}
}

/*
* Loads the debug script
*/

FILE* script(void)
{
	FILE* filestream;
	if((filestream = fopen(optarg, "r")) != NULL)
	{
		return filestream;	
	}
	else
	{
		printf("Not a valid file!\n");
		exit(EXIT_FAILURE);
	}
}

/*
* Loads the XME file.
*/

FILE* xme(FILE* filestream_user, bool script_on)
{
	char filename[SIZE_FILENAME_MAX];
	FILE* filestream_srec;
	
	printf("Enter .XME file to load\n");
	fscanf(filestream_user, "%s", filename);
	if(script_on) printf("%s\n", filename);
	
	if((filestream_srec = fopen(filename, "r")) != NULL)
	{
		return filestream_srec;
	}
	else
	{
		printf("Not a valid file\n");
		exit(EXIT_FAILURE);
	}
}

void sigint_hdlr()
{
/*
- Invoked when SIGINT (control-C) is detected
- changes state of waiting_for_signal
- signal must be reinitialized
*/
ctrl_c_fnd = true;
signal(SIGINT, sigint_hdlr); /* Reset SIGINT */
}






























