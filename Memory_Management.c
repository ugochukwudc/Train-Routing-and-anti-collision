/*********************************

Assignment_1.c

ECED 4402 - Real Time systems
Assignment 1: Memory Management
Vignesh Ramasamy
22nd September 2014

The following program manages the 80 KiB heap memory

/*********************************

/*********** #include files ***********/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

/*********** #define files ***********/

#define ERROR 						0
#define NO_ERROR 					1
#define ERROR_CHAR					'0'
#define NO_ERROR_CHAR				'1'
#define MEMORY_BLOCK_ALLOCATED 		1
#define MEMORY_BLOCK_DEALLOCATED 	1

#define TOTAL_NUM_BLOCKS 			248
#define NUM_128_BYTE_BLOCKS 		128
#define NUM_256_BYTE_BLOCKS 		64
#define NUM_512_BYTE_BLOCKS 		32
#define NUM_1024_BYTE_BLOCKS 		16
#define NUM_2048_BYTE_BLOCKS 		8

#define SIZE_128_BYTE_ENTRY			0
#define SIZE_256_BYTE_ENTRY			1
#define SIZE_512_BYTE_ENTRY			2
#define SIZE_1024_BYTE_ENTRY		3
#define SIZE_2048_BYTE_ENTRY		4

#define NOT_IN_USE 					0
#define IN_USE 						1

#define FIRST_128_BYTE_BLOCK 	   	1
#define LAST_128_BYTE_BLOCK 	 	128
#define FIRST_256_BYTE_BLOCK  	 	129
#define LAST_256_BYTE_BLOCK 	 	192
#define FIRST_512_BYTE_BLOCK 	 	193
#define LAST_512_BYTE_BLOCK 	 	224
#define FIRST_1024_BYTE_BLOCK 	 	225
#define LAST_1024_BYTE_BLOCK 	 	240
#define FIRST_2048_BYTE_BLOCK 	 	241
#define LAST_2048_BYTE_BLOCK 	 	248

#define LAST_128_BYTE_BLOCK_ADDRESS 	0x20007fff
#define LAST_256_BYTE_BLOCK_ADDRESS  	0x2000bfff
#define LAST_512_BYTE_BLOCK_ADDRESS  	0x2000ffff
#define LAST_1024_BYTE_BLOCK_ADDRESS  	0x20013fff
#define LAST_2048_BYTE_BLOCK_ADDRESS  	0x20017fff

/*********** global variables ***********/

// The following variable is to indicate the end of first 16 KiB memory used for other purposes
// The following varilable + 1 indicates the start of heap memory blocks

uint32_t LAST_DATA_ADDRESS = 0x20003fff;

// structure block contains the memory block information.
// structure contains pointer to first and last address of a block, block size and block usage

struct block{
		char *block_first_address;
		char *block_last_address;
		int block_size;
		int usage;
	};

	// an array of memory blocks indicates the memory blocks to be managed
	struct block blocks[TOTAL_NUM_BLOCKS+1];

	// the following arrray is to keep track of available memory blocks of different size
	int num_blocks_available [5] = {NUM_128_BYTE_BLOCKS,NUM_256_BYTE_BLOCKS,NUM_512_BYTE_BLOCKS,NUM_1024_BYTE_BLOCKS,NUM_2048_BYTE_BLOCKS};

/*********** initialization routine ***********/

void initialize ()
{
	int i=1;
	int j=i-1;

	// the first memory block entry (block[0]) is to indicate the last data memory.
	// this block is not a part of heap memory and is only used to as reference to start the heap memory blocks
	blocks[0].block_last_address =(char *) LAST_DATA_ADDRESS;

	// the following for loop initializes first 128 blocks of size 0x80 (128) bytes.
	// different for loops are used for different block sizes to avoid if statement inside the for loop
	for (i;i<=LAST_128_BYTE_BLOCK;i++)
	{
		blocks[i].block_first_address = blocks[j].block_last_address+1;
		blocks[i].block_size = 128;
		blocks[i].block_last_address = blocks[i].block_first_address+blocks[i].block_size-1;
		blocks[i].usage = NOT_IN_USE;
		j++;
	}

	// the following for loop initializes the next 64 blocks of size 0x100 (256) bytes.
	for (i;i<=LAST_256_BYTE_BLOCK;i++)
	{
		blocks[i].block_first_address = blocks[j].block_last_address+1;
		blocks[i].block_size = 256;
		blocks[i].block_last_address = blocks[i].block_first_address+blocks[i].block_size-1;
		blocks[i].usage = NOT_IN_USE;
		j++;
	}

	// the following for loop initializes the next 32 blocks of size 0x200 (512) bytes.
	for (i;i<=LAST_512_BYTE_BLOCK;i++)
	{
		blocks[i].block_first_address = blocks[j].block_last_address+1;
		blocks[i].block_size = 512;
		blocks[i].block_last_address = blocks[i].block_first_address+blocks[i].block_size-1;
		blocks[i].usage = NOT_IN_USE;
		j++;
	}

	// the following for loop initializes the next 16 blocks of size 0x400 (1024) bytes.
	for (i;i<=LAST_1024_BYTE_BLOCK;i++)
	{
		blocks[i].block_first_address = blocks[j].block_last_address+1;
		blocks[i].block_size = 1024;
		blocks[i].block_last_address = blocks[i].block_first_address+blocks[i].block_size-1;
		blocks[i].usage = NOT_IN_USE;
		j++;

	}

	// the following for loop initializes the next 8 blocks of size 0x800 (2048) bytes.
	for (i;i<=LAST_2048_BYTE_BLOCK;i++)
	{
		blocks[i].block_first_address = blocks[j].block_last_address+1;
		blocks[i].block_size = 2048;
		blocks[i].block_last_address = blocks[i].block_first_address+blocks[i].block_size-1;
		blocks[i].usage = NOT_IN_USE;
		j++;
	}

}

/*********** allocation routine ***********/
char* allocate (int size)
{
	int i;

	//char result=NO_ERROR_CHAR;
	char *result=(char *)NO_ERROR_CHAR;
	// variable to identify where to start the search from
	int first_block;

	// variables to indicate the allocated memory block information
	int allocated_block_number;
	char *allocated_first_address;
	char *allocated_last_address;
	int allocated_block_size;

	// if the size is out of range, then output an error message
	if (size > 2048 || size <1)
	{
		result = (char *)ERROR_CHAR;
#ifdef PRINT
		printf("\nWARNING: Entered size is out of range");
#endif
	}

	// if the size is between 0x400 to 0x800 then start the search from block 241 (this is the first 0x800 (2048) byte block)
	else if (size > 1024 && size <= 2048)
		first_block = FIRST_2048_BYTE_BLOCK;
	// if the size is between 0x200 to 0x400 then start the search from block 225 (this is the first 0x400 (1024) byte block)
	else if (size > 512 && size <= 1024)
		first_block = FIRST_1024_BYTE_BLOCK;
	// if the size is between 0x100 to 0x200 then start the search from block 193 (this is the first 0x200 (512) byte block)
	else if (size > 256 && size <= 512)
		first_block = FIRST_512_BYTE_BLOCK;
	// if the size is between 0x80 to 0x100 then start the search from block 129 (this is the first 0x100 (256) byte block)
	else if (size > 128 && size <= 256)
		first_block = FIRST_256_BYTE_BLOCK;
	// if the size is between 0x01 to 0x80 then start the search from block 1
	else if (size >= 1 && size <= 128)
		first_block = FIRST_128_BYTE_BLOCK;
	// return error if the size is invalid
	else
		result = (char *)ERROR_CHAR;

	// enter to search for memory block only if the size is valid
	if (result != (char *)ERROR_CHAR)
	{
		//printf("I am here\n");
		result = (char *)ERROR_CHAR;

		for (i=first_block;i<=TOTAL_NUM_BLOCKS && result == (char *)ERROR_CHAR;i++)
		{
			// look for first available free memory block
			if(blocks[i].usage == NOT_IN_USE)
			{
			// set the block as unavailable for other processes
			blocks[i].usage = IN_USE;

			// update the allocated block information for output
			allocated_first_address=blocks[i].block_first_address;
			allocated_last_address=blocks[i].block_last_address;
			allocated_block_size=blocks[i].block_size;
			allocated_block_number=i;

			// set the return value to PASS
			//result=MEMORY_BLOCK_ALLOCATED;
			result=allocated_first_address;
			//printf("\n\n\n the output of allocation routine is %lx\n\n\n",result);

			// print the allocated memory block information
			//printf("Information of the allocated memory block is as follows:\n\n");
			//printf("Block number        = %i\n",allocated_block_number);
			//printf("Block start address = %lx\n",allocated_first_address);
			//printf("Block last address  = %lx\n",allocated_last_address);
			//printf("Block size          = %i\n\n",allocated_block_size);
			}
		}

	// print error message if the requested size is not available
	if (result == (char *)ERROR_CHAR)
	{
#ifdef PRINT
		printf("\nWARNING: Entered size is greater than the maximum available memory block");
#endif
	}
	// following else if statements update the available memory block entries depending on the allocated memory block
	else if (allocated_block_size == 2048)
		num_blocks_available [SIZE_2048_BYTE_ENTRY]-=1;
	else if (allocated_block_size == 1024)
		num_blocks_available [SIZE_1024_BYTE_ENTRY]-=1;
	else if (allocated_block_size == 512)
		num_blocks_available [SIZE_512_BYTE_ENTRY]-=1;
	else if (allocated_block_size == 256)
		num_blocks_available [SIZE_256_BYTE_ENTRY]-=1;
	else if (allocated_block_size == 128)
		num_blocks_available [SIZE_128_BYTE_ENTRY]-=1;

	return result;
	}
}

/*********** deallocation routine ***********/

int deallocate (char *address)
{
	int result=NO_ERROR,i;

	// variable to indicate the first and last block for the linear search
	int first_block, last_block;
	int block_size_entry;

	// if the address is outside the heap memory, then return the error message
	if (address > (char *)LAST_2048_BYTE_BLOCK_ADDRESS || address <= (char *)LAST_DATA_ADDRESS)
	{
		result = ERROR;
		//printf("\nWARNING: Entered address is outside the heap memory");
	}

	// if the address is part of a 0x800 (2048) byte size block, then search only in the last 8 blocks
	else if (address > (char *)LAST_1024_BYTE_BLOCK_ADDRESS && address <= (char *)LAST_2048_BYTE_BLOCK_ADDRESS)
	{
		first_block = FIRST_2048_BYTE_BLOCK;
		last_block = LAST_2048_BYTE_BLOCK;
		block_size_entry=SIZE_2048_BYTE_ENTRY;
	}

	// if the address is part of a 0x400 (1024) byte size block, then search only in blocks 225-240
	else if (address > (char *)LAST_512_BYTE_BLOCK_ADDRESS && address <= (char *)LAST_1024_BYTE_BLOCK_ADDRESS)
	{
		first_block = FIRST_1024_BYTE_BLOCK;
		last_block = LAST_1024_BYTE_BLOCK;
		block_size_entry=SIZE_1024_BYTE_ENTRY;
	}

	// if the address is part of a 0x200 (512) byte size block, then search only in blocks 193-224
	else if (address > (char *)LAST_256_BYTE_BLOCK_ADDRESS && address <= (char *)LAST_512_BYTE_BLOCK_ADDRESS)
	{
		first_block = FIRST_512_BYTE_BLOCK;
		last_block = LAST_512_BYTE_BLOCK;
		block_size_entry=SIZE_512_BYTE_ENTRY;
	}

	// if the address is part of a 0x100 (256) byte size block, then search only in blocks 129-192
	else if (address > (char *)LAST_128_BYTE_BLOCK_ADDRESS && address <= (char *)LAST_256_BYTE_BLOCK_ADDRESS)
	{
		first_block = FIRST_256_BYTE_BLOCK;
		last_block = LAST_256_BYTE_BLOCK;
		block_size_entry=SIZE_256_BYTE_ENTRY;
	}

	// if the address is part of a 0x80 (128) byte size block, then search only in blocks 1-128
	else if (address > (char *)LAST_DATA_ADDRESS && address <= (char *)LAST_128_BYTE_BLOCK_ADDRESS)
	{
		first_block = FIRST_128_BYTE_BLOCK;
		last_block = LAST_128_BYTE_BLOCK;
		block_size_entry=SIZE_128_BYTE_ENTRY;
	}

	// return error if the size is invalid
	else
		result = ERROR;

	// enter to search for memory block only if the address is part of heap memory
	if (result != ERROR)
	{
		result = ERROR;

		for (i=first_block;i<=last_block;i++)
		{
			// if the given address is between the first and last address of a block, then deallocate the block
			if(address >= blocks[i].block_first_address && address <= blocks[i].block_last_address && blocks[i].usage == IN_USE)
			{
				blocks[i].usage = NOT_IN_USE;
				result = MEMORY_BLOCK_DEALLOCATED;

				// print the deallocated memory block information
				/*printf("Information of the deallocated memory block is as follows:\n\n");
				printf("Block number        = %i\n",i);
				printf("Block start address = %lx\n",blocks[i].block_first_address);
				printf("Block last address  = %lx\n",blocks[i].block_last_address);
				printf("Block size          = %i\n\n",blocks[i].block_size);*/
			}
		}

		// if the give address is a heap memory address and is not allocated to any process then output an error
		if (result == ERROR)
			printf("\nWARNING: Entered memory address block is not in use");

		// if a block is deallocated then increment the available block entry depending on its size
		else
			num_blocks_available [block_size_entry]+=1;
	}

	return result;
}

/*********** show function ***********/

void show(void)
{
	// print the available number of memory blocks with their sizes
#ifdef PRINT

	printf("\n+---------------+------------------+");
	printf("\n| SIZE (BYTES)  | AVAILABLE BLOCKS |");
	printf("\n+---------------+------------------+");
	printf("\n| 128  (0x80 )  | %3i              |",num_blocks_available[SIZE_128_BYTE_ENTRY]);
	printf("\n| 256  (0x100)  | %3i              |",num_blocks_available[SIZE_256_BYTE_ENTRY]);
	printf("\n| 512  (0x200)  | %3i              |",num_blocks_available[SIZE_512_BYTE_ENTRY]);
	printf("\n| 1024 (0x400)  | %3i              |",num_blocks_available[SIZE_1024_BYTE_ENTRY]);
	printf("\n| 2048 (0x800)  | %3i              |",num_blocks_available[SIZE_2048_BYTE_ENTRY]);
	printf("\n+---------------+------------------+",num_blocks_available[SIZE_2048_BYTE_ENTRY]);
#endif

}

/*********** help function ***********/

void help (void)
{
#ifdef PRINT
	// The following console output is to help user on the commands he/she can type in
	printf ("The accepted commands are as follows:\n");
	printf ("\nTo allocate   - a (return) [Size in decimal bytes] (return) [Y - For iteration/N- For no iteration] (return) <Number of iterations>");
	printf ("\nTo deallocate - d (return) [address in hex] (return)");
	printf ("\nTo show       - s (return)");
	printf ("\nFor help      - h (return)");
	printf ("\nTo quit       - q\n\n");
#endif
}


/*********** main loop ***********/
/*
int main()
{
	// Variables for user input
	char Input[1],iteration[1];
	int Size,iteration_count,i;
	char *Address;

	int Result;
	char *Result_char;

	initialize();
	help();

	// continue to execute the user command until the command q is entered to quit

	while(Input[0] != 'q')
	{
		printf ("\nPlease enter a command: ");
		scanf ("%1s", &Input);

		switch(Input[0])
		{
			// command a to allocate a memory block
			case 'a':

				//Input size
				printf ("\nEnter size of memory required in decimal bytes: ");
				scanf ("%i",&Size);

				// iterate is to repeat the allocation process for defined number of times
				printf ("\nEnter 'y' for iterate and 'n' for no iteration:");
				scanf ("%1s",&iteration);

				// if user likes to repeat the allocation process then get the number of times to iterate
				if(iteration[0] == 'y')
				{
					printf("\nEnter number of iterations:");
					scanf("%i",&iteration_count);

					//repeat calling the allocation routine for user entered number of iterations
					for(i=1;i<=iteration_count;i++)
					{
						Result_char = allocate (Size);

						// output error message if the allocation failed
						if (Result_char == (char*)ERROR_CHAR)
						printf ("\nERROR: Allocation failed");
					}
				}

				// if user do not want to repeat allocation process, then call allocation routine only once
				else if(iteration[0] == 'n')
				{
					Result_char = allocate (Size);
					printf("\n\n\n the output of allocation routine is %lx\n\n\n",Result_char);
					// output error message if the allocation failed
					if (Result_char == (char*)ERROR_CHAR)
					printf ("\nERROR: Allocation failed");
				}

				// error message for invalid user input
				else
					printf ("\nERROR: invalid input");
				break;

			// command d to deallocate a memory block
			case 'd':

				//Input address
				printf ("\nEnter the memory address to be deallocated: ");
				scanf ("%lx", &Address);

				// call deallocation routine
				Result = deallocate (Address);

				// output error message if the deallocation failed
				if (Result == ERROR)
					printf ("\nERROR: Deallocation failed");
				break;

			// command s to show the available number of memory block
			case 's':
				show();
				break;

			// command h to show the valid commands to enter with description
			case 'h':
				help();
				break;

			default:
				printf ("\ninvalid input");
		}
	}
	exit (EXIT_SUCCESS);
	return 0;
}*/


