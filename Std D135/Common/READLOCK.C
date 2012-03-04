#include "stdio.h"
#include "encoding.h"



FILE far *lock_file;

char far licensee[80];
char far serial[80];


unsigned char far encoded_block[NUM_ROWS][NUM_COLS];
char far lock_file_name[80];





char far  read_lock()
{
int i;


	lock_file = fopen(lock_file_name,"r+b");

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
				return('f');
	}


	for(i = 0 ; (i <= encoded_block[1][STRING_LENGTH]) &&
		(i < NUM_CHECKS) ; i++)
	{
		if(encoded_block[1][check_byte_pos[i]] != 
			(char)(serial[i] + serial[i + 1]))
				return('f');
	}





	if(encoded_block[0][STRING_LENGTH] < 3)
		return('f');

	if(encoded_block[1][STRING_LENGTH] < 3)
		return('f');




	for(i = 0 ; (i < encoded_block[1][STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(encoded_block[1][check_byte_pos[i]] < 64)
			return('f');
			

	for(i = 0 ; (i < encoded_block[0][STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(encoded_block[0][check_byte_pos[i]] < 64)
			return('f');


		
	return('t');
}



main()
{
	system("dir lock*");

	printf("\nEnter Lock file name:  ");
	scanf("%s",lock_file_name);

	printf("\nUnlock Success = %c \n",read_lock());
	printf("\nLicensee =   :%s:  \n",licensee);
	printf("serial =  :%s: \n\n",serial);
}

