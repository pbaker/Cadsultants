#include "stdio.h"
#include "encoding.h"

extern int far handle;
extern void far exit_dealloc_pages(int);

extern void far close_host();
extern void far restore_int9();


FILE far *lock_file;

char far licensee[80];
char far serial[80];


unsigned char far encoded_block[NUM_ROWS][NUM_COLS];
char far lock_file_name[80] = "customer.id";


void far no_good_exit()
{
	exit_dealloc_pages(handle);
	close_host();
	restore_int9();

	putchar(7);
	printf("\n\n%s File Is Currupted: Aborting to DOS\n\n",lock_file_name);

	exit();
}



void far read_lock()
{
int i;


	lock_file = fopen(lock_file_name,"r+b");

	if(lock_file == NULL)
	{
		putchar(7);
		printf("\nCan't Open %s File: Aborting to DOS\n",
			lock_file_name);

		exit_dealloc_pages(handle);
		close_host();
		restore_int9();
		exit();
	}


	fread(&(encoded_block[0][0]),sizeof(encoded_block),1,lock_file);
	fclose(lock_file);	


	for(i = 0 ; i <= encoded_block[0][STRING_LENGTH] ; i++)
		licensee[i] = encoded_block[0][insert_array[i]];

	licensee[i] = '\0';
	licensee[i + 1] = '\0';
	licensee[i + 2] = '\0';


	for(i = 0 ; i <= encoded_block[1][STRING_LENGTH] ; i++)
		serial[i] = encoded_block[1][insert_array[i]];

	serial[i] = '\0';
	serial[i + 1] = '\0';
	serial[i + 2] = '\0';


	for(i = 0 ; (i <= encoded_block[0][STRING_LENGTH]) &&
		(i < NUM_CHECKS) ; i++)
	{
		if(encoded_block[0][check_byte_pos[i]] != 
			(char)(licensee[i] + licensee[i + 1]))
				no_good_exit();
	}


	for(i = 0 ; (i <= encoded_block[1][STRING_LENGTH]) &&
		(i < NUM_CHECKS) ; i++)
	{
		if(encoded_block[1][check_byte_pos[i]] != 
			(char)(serial[i] + serial[i + 1]))
				no_good_exit();
	}





	if(encoded_block[0][STRING_LENGTH] < 3)
		no_good_exit();

	if(encoded_block[1][STRING_LENGTH] < 3)
		no_good_exit();




	for(i = 0 ; (i < encoded_block[1][STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(encoded_block[1][check_byte_pos[i]] < 64)
			no_good_exit();
			

	for(i = 0 ; (i < encoded_block[0][STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(encoded_block[0][check_byte_pos[i]] < 64)
			no_good_exit();


}

