#include "stdio.h"
#include "encoding.h"     /* has encoding arrays & #defines */



FILE far *lock_file;

char far customer_name[80];
char far customer_id[80];


char far encoded_block[NUM_ROWS][NUM_COLS];
char far out_file_name[121];

unsigned int far seed;



void far read_a_line(the_string) /*which often contains blanks, so no scanfs*/
char far the_string[];
{
int i,ch;

	for(i = 0,ch = getchar() ; ch != '\n' ; i++,ch = getchar())
	{
		if(ch == 8)
		{
			if(i > 0)
			{
				i--;
				ch = getchar();
			}
		}

		the_string[i] = ch;
	}


	the_string[i] = '\0';
	the_string[i + 1] = '\0';
	the_string[i + 2] = '\0';

	putchar('\n');
}






void far encode_row(row,the_string)
int row;
char far the_string[];
{
int i;

	for(i = 0 ; (i < MAX_NUM_USED) && (the_string[i] != '\0') ; i++)
		encoded_block[row][insert_array[i]] = the_string[i];
	
	encoded_block[row][STRING_LENGTH] = i - 1;


	for(i = 0 ; (i < NUM_CHECKS) && (the_string[i] != '\0') ; i++)
		encoded_block[row][check_byte_pos[i]] = 
			the_string[i] + the_string[i + 1];
}




main()
{
int row,col,i;


	printf("Enter seed for random # generator:  ");
	scanf("%u",&seed);
	getchar();
	srand(seed);



try_again:

	for(row = 0 ; row < NUM_ROWS ; row++)
		for(col = 0 ; col < NUM_COLS ; col++)
		{
			switch(rand() % 3)
			{
				case 0 :
				{
					encoded_block[row][col] = 
						rand() % 26 + 2;
					break;

				}

				case 1 :
				{
					encoded_block[row][col] = 
						rand() % 26 + 'a';
					break;
				}

				case 2 :
				{
					encoded_block[row][col] = 
						rand() % 26 + 'A';
					break;
				}
			}
		}



	printf("\n\nEnter customer name:  ");
	read_a_line(customer_name);


	if(strlen(customer_name) > MAX_NUM_USED)
	{
		putchar(7);
		printf("Warning: Customer name truncated to:\n\n");

		for(i = 0 ; i < MAX_NUM_USED ; i++)
			putchar(customer_name[i]);

		printf("\n\n");
	}


	printf("\nEnter serial ID:  ");
	read_a_line(customer_id);


	encode_row(0,customer_name);
	encode_row(1,customer_id);


	customer_id[4] = '\0';
	sprintf(out_file_name,"c:\\emulate\\releases\\lock%s",customer_id);
	printf("\n\nCustomer Lock file written to:\n\t %s\n\n",out_file_name);
	lock_file = fopen(out_file_name,"w+b");

	if(lock_file == NULL)
	{
		printf("Invalid File Name\n\n");
		goto try_again;
	}


	fwrite(&(encoded_block[0][0]),sizeof(encoded_block),1,lock_file);

}