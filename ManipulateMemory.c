#include "../include/Declaration.h"

/*
* The main while loop reads each record from the input file. Each file is 
* scanned for its type, count, address, data, and checksum. If the record is
* of type 1, it will be passed to the bus function and relayed into memory.
* If type 9, the address will be displayed as the starting address. If a record
* has an incorrect checksum or an incorrect record type, it will not be entered
* into the memory and an error will appear. 
*/

void srec_load(FILE* filestream_srec, unsigned short* address_pair)
{	
	char type[SIZE_SREC_TYPE];
	char record_srec[SIZE_SREC_MAX];
	unsigned int address_high, address_low, count, srecord_number;
	unsigned short mar, data, checksum, endpoint;
	int DATA_END;
	
	while(fgets(record_srec, SIZE_SREC_MAX, filestream_srec) != NULL)
	{
		srecord_number++;
		
		sscanf(record_srec, "%2s%2X%2X%2X", type, &count, &address_high, &address_low);
		
		if(!strstr(type, "S0") && !strstr(type, "S1") && !strstr(type, "S9"))
		{
			printf("Incompatible S-Record Type: %s", type);
			printf("on line %d\n", srecord_number);
			exit(EXIT_FAILURE);
		}
		
		mar = address_high << SIZE_BYTE | address_low;
		
		if(strstr(type, "S1"))
			endpoint = count - 3;
		
		address_pair[START] = mar;
		
		checksum = count + address_high + address_low;
		
		DATA_END = SIZE_ASCII_PAIR*count;
		
		for(int i = DATA_START; i <= DATA_END + SIZE_CHECKSUM; i += SIZE_ASCII_PAIR)
		{
			sscanf(record_srec + i, "%2hX", &data);
			checksum += data;
		}

		if(checksum & 0x00FF) //comback to remove CHECKSUM_VALID
		{
			if(strstr(type, "S0")) printf("Source filename: ");
			
			for(int i = DATA_START; i <= DATA_END; i += SIZE_ASCII_PAIR)
			{
				sscanf(record_srec + i, "%2hX", &data);
				if(strstr(type, "S0")) printf("%c", data);
				bus(mar++, &data, WRITE, BYTE);
			}
			
			if(strstr(type, "S0")) printf("\n");
			if(strstr(type, "S9"))
			{
				printf("File read - no errors detected. Starting address: %04X\n", address_pair[START]);
				address_pair[STOP] = endpoint + address_pair[START];
			}
		}
		else
		{
			printf("Error with SREC %s on line %d: Invalid checksum!\n", record_srec, srecord_number);
			exit(EXIT_FAILURE);
		}	
	}
	fclose(filestream_srec);
}
/*
* The user enters a range of address to peek into.
*/

void srec_dump(FILE* filestream_user, bool script_on)
{
	unsigned short mbr, mar;
	unsigned int end, start;
	char ascii[SIZE_WORD];
	
	fscanf(filestream_user, "%04X %04X", &start, &end); //needs a failure condition...
	
	if(script_on) printf("%04X %04X\n", start, end);
	
	while(start < end)
	{
		printf("%04X: ", start);
	
		for(mar = start; mar < start + SIZE_WORD - INDEX_OFFSET; mar++)
		{
			bus(mar, &mbr, READ, BYTE);
			printf("%02X ", mbr);
			if(mbr < ASCII_SPACE || mbr > ASCII_DEL)
			{
				mbr = '.';
			}
			ascii[mar - start] = mbr;
		}
		
		printf("%s\n", ascii);
		start += SIZE_WORD;
	}
}



