#include <stdio.h>
#include "encoding.h"

 
extern int  handle;
extern void  exit_dealloc_pages();

extern void  close_host();

extern char  location[], contact[];


unsigned char  encoded_block[NUM_ROWS][NUM_COLS];


long  data_pos;

char  licensee[80];
char  serial[80];

char  exe_file_name[256] = "d135.exe";
FILE  *exe_file = NULL;
int  read_header[3];




void  no_good_exit(where)
int where;
{
	printf("\n\ndecoding failed at %d\n\n",where);

	exit_dealloc_pages(handle);

	putchar(7);


	exit(0);
}


void  new_read_lock()
{
int i,j;

#ifdef asdfghjklkjhgfdsadfghjkjhgfd

	fseek(exe_file,data_pos,SEEK_SET);

	j = fread(&(encoded_block[0][0]),sizeof(encoded_block),1,exe_file);

/*
	if(j < 1)
	{
		putchar(7);
		printf("\read in %d blocks of encoded data\n",j);
	}
*/

/*
	fclose(exe_file);	
*/

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


	for(i = 0 ; i <= encoded_block[2][STRING_LENGTH] ; i++)
		location[i] = encoded_block[2][insert_array[i]];

	location[i] = '\0';


	for(i = 0 ; i <= encoded_block[3][STRING_LENGTH] ; i++)
		contact[i] = encoded_block[3][insert_array[i]];

	contact[i] = '\0';


/*
printf("\nInside read_lock() :\nlicensee = %s    serial = %s\n",licensee,serial);
*/
	for(i = 0 ; (i <= encoded_block[0][STRING_LENGTH]) &&
		(i < NUM_CHECKS) ; i++)
	{
		if(encoded_block[0][check_byte_pos[i]] != 
			(unsigned char)(licensee[i] + licensee[i + 1]))
				no_good_exit(1);
	}


	for(i = 0 ; (i <= encoded_block[1][STRING_LENGTH]) &&
		(i < NUM_CHECKS) ; i++)
	{
		if(encoded_block[1][check_byte_pos[i]] != 
			(unsigned char)(serial[i] + serial[i + 1]))
				no_good_exit(2);
	}


	if(encoded_block[0][STRING_LENGTH] < 3)
		no_good_exit(3);

	if(encoded_block[1][STRING_LENGTH] < 3)
		no_good_exit(4);


	for(i = 0 ; (i < encoded_block[1][STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(encoded_block[1][check_byte_pos[i]] < 64)
			no_good_exit(5);
			

	for(i = 0 ; (i < encoded_block[0][STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(encoded_block[0][check_byte_pos[i]] < 64)
			no_good_exit(6);

#endif
}


read_lock()
{

#ifdef ldllddlldldldlld

	exe_file = fopen(exe_file_name,"rb");

	if(exe_file == NULL)
	{
		putchar(7);
		printf("\nCan't Open %s File: Aborting to DOS\n",
			exe_file_name);

		exit_dealloc_pages(handle);
		close_host();
		restore_int9();
		exit(0);
	}
	
	if(fread(read_header,sizeof(int),3,exe_file) != 3)
		printf("cant read header of %s file",exe_file_name);


	data_pos = 512L*(long)(read_header[2] - 1) + (long)read_header[1];

/*
	printf("data position = %ld\n",data_pos);
*/
	new_read_lock();

/*
	printf("\ncust name = %s\n serial = %s\n\n",licensee,serial);
*/

#endif

	strcpy(licensee,"A Company");
	strcpy(serial,"1234");
	strcpy(location,"Anywhere, USA");
	strcpy(contact,"Joe Blow");
}

