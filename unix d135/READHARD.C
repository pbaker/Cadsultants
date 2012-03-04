#include <stdio.h>





#define NUM_ROWS 4
#define NUM_COLS 77
#define NUM_CHECKS 7
#define MAX_NUM_USED 30

#define STRING_LENGTH 44


extern int  handle;
extern char  serial[];
extern char  use_hardcopy_colors;
extern char  the_version[];



int  hard_insert_array[MAX_NUM_USED] = {66,61,34,18,42,33,64,56,8,17,4,51,39,26,
	14,50,15,11,69,7,28,46,55,38,40,22,37,65,60,21};

int  hard_check_byte_pos[NUM_CHECKS] = {29,13,47,3,63,35,52};


char  hard_serial[33];





struct hard_info
{
	char the_version[45];
	char encoded_block[222];
	char menu_text[3][111];
	char dos_commands[3][222];
	char user_file_names[3][111];
	char use_colors[4];
	int default_target;
	int size_of_paper[4],type_of_plotter[4],type_of_7580b[4];
};





struct hard_info  hardcopy_info;



FILE  *hard_config_file;

char  config_file_name[222] = "hardcopy.cfg";
char  found_hardcopy_file = 'f';






char  decode_serial_id()
{
int i;


	for(i = 0 ; i <= hardcopy_info.encoded_block[STRING_LENGTH] ; i++)
		hard_serial[i] = hardcopy_info.
			encoded_block[hard_insert_array[i]];



	hard_serial[i] = '\0';
	hard_serial[i + 1] = '\0';
	hard_serial[i + 2] = '\0';



	for(i = 0 ; (i <= hardcopy_info.encoded_block[STRING_LENGTH]) &&
		(i < NUM_CHECKS) ; i++)
	{
		if(hardcopy_info.encoded_block[hard_check_byte_pos[i]] != 
			(char)(hard_serial[i] + hard_serial[i + 1]))
				return('f');
	}




	if(hardcopy_info.encoded_block[STRING_LENGTH] < 3)
		return('f');



	for(i = 0 ; (i < hardcopy_info.encoded_block[STRING_LENGTH]) &&
	   (i < NUM_CHECKS - 1) ; i++)
		if(hardcopy_info.encoded_block[hard_check_byte_pos[i]] < 64)
			return('f');
			

		
	return('t');
}






void  read_hardcopy_info()
{

	hard_config_file = fopen(config_file_name,"r+b");

	if(hard_config_file == NULL)
	{
		found_hardcopy_file = 'f';
		return;
	}
	else
	{
		fread(&hardcopy_info,sizeof(struct hard_info),1,
			hard_config_file);

/*
		if(strcmp(hardcopy_info.the_version,the_version) != 0)
		{
			exit_dealloc_pages(handle);
			printf("\nOut Of Date HARDCOPY.CFG Configuration File\n");
			printf("Can't Continue \n\n Please Run  HARDCFE  %s\n\n",
				the_version);
			close_host();
			restore_int9();
			exit(0);
		}
*/

/*
		use_hardcopy_colors = hardcopy_info.use_colors;
*/

		found_hardcopy_file = 't';
	}

}





void  compare_serial_ids()
{
#ifdef ksdkdksdddksk
	if(strncmp(serial,hard_serial,4) == 0)
	{
		found_hardcopy_file = 't';

		if(strcmp(hardcopy_info.the_version,the_version) != 0)
		{
			exit_dealloc_pages(handle);
			printf("\nOut Of Date HARDCOPY.CFG Configuration File\n");
			printf("Can't Continue \n\n Please Run  HARDCFE  %s\n\n",
				the_version);
			close_host();
			restore_int9();
			exit(0);
		}
	}
	else
		found_hardcopy_file = 'f';
#endif
}




/*

main()
{
	printf("TRYING to read hardcopy.cfg.....\n");
	read_hardcopy_info();
	printf(" decode serial ID = %c  \n\n",
		found_hardcopy_file);

	printf("SERIAL ID = %s\n",hard_serial);
}

*/
