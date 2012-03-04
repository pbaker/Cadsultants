#include "stdio.h"

#define TRUE 1
#define FALSE 0

extern int far read_host_buffer();
extern void far goto_row_col();
extern void far read_curser_pos();
extern void far normal_string();
extern void far turn_on_curser();
extern void far turn_off_curser();

extern int far row_curser;
extern int far col_curser;

char far display_alt_menu = TRUE;

struct light_line
{
	int col,row;
	char is_on;
	char light_message[80];
};


struct light_line far light_lines[32] =
	{
		{36,8,'f', "ALT M acros"},
		{36,13,'f',"ALT 1 Change Module"},
		{36,6,'f', "ALT F ile/Terminate"},
		{36,9,'f', "ALT R edisplay"},

		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},
		{36,0,'f',"\0"},

		{36,2,'f', "ALT B lank/Unblank"},
		{36,3,'f', "ALT C alculator"},
		{36,15,'f',"ALT 3 Module Parameters"},
		{36,10,'f',"ALT S ystem Parameters"},
		{36,11,'f',"ALT W CS Control"},
		{36,14,'f',"ALT 2 Module Function"},
		{36,12,'f',"ALT Z oom/Pan/Regen"},
		{36,7,'f', "ALT L ayer Control"},
		{36,4,'f', "ALT D isplay Control"},
		{36,5,'f', "ALT E dit Work View"},

		{61,14,'f',{26,' ','E','n','t','r','y',' ','C','o','m','p','l','e','t','e','\0'}},
		{61,15,'f',{27,' ','R','e','j','e','c','t',' ',' ',' ',' ',' ',' ',' ',' ','\0'}},
		{61,16,'f',{25,' ','T','e','r','m','i','n','a','t','e',' ',' ',' ',' ',' ','\0'}},
		{61,13,'f',{24,' ','A','l','t','.',' ','A','c','t','i','o','n',' ',' ',' ','\0'}}
	};



char far *column36_blank = "                       ";
char far *column61_blank = "                ";




void far all_lights_on()
{
int i;


	for(i = 0 ; i < 32 ; i++)
	{
		light_lines[i].is_on = TRUE;

		if(display_alt_menu == TRUE)
		{
			if(light_lines[i].light_message[0] != '\0')
			{
				if(light_lines[i].col < 60)
				{
					goto_row_col(light_lines[i].row,
						light_lines[i].col);
					printf("%s",light_lines[i].light_message);
				}
				else
					normal_string(light_lines[i].light_message,
						light_lines[i].row,
						light_lines[i].col);
			}
		}
	}
}


void far all_lights_off()
{
int i;

	for(i = 0 ; i < 28 ; i++)   /*column 36 stuff*/
	{
		light_lines[i].is_on = FALSE;

		if(display_alt_menu == TRUE)
		{
			if(light_lines[i].light_message[0] != '\0')
			{
				goto_row_col(light_lines[i].row,
					light_lines[i].col);
				printf("%s",column36_blank);
			}
		}
	}

	for(i = 28 ; i < 32 ; i++)   /*column 61 stuff*/
	{
		light_lines[i].is_on = FALSE;

		if(display_alt_menu == TRUE)
		{
			if(light_lines[i].light_message[0] != '\0')
			{
				goto_row_col(light_lines[i].row,
					light_lines[i].col);
				printf("%s",column61_blank);
			}
		}
	}
}













void far alt_menu_on()
{
int i,save_row_curser,save_col_curser;

	save_row_curser = row_curser;
	save_col_curser = col_curser;
	read_curser_pos();
	turn_off_curser();

	display_alt_menu = TRUE;

	for(i = 0 ; i < 32 ; i++)
	{
		if(light_lines[i].is_on == TRUE)
		{
			if(light_lines[i].light_message[0] != '\0')
			{
				if(light_lines[i].col < 60)
				{
					goto_row_col(light_lines[i].row,
						light_lines[i].col);
					printf("%s",light_lines[i].light_message);
				}
				else
					normal_string(light_lines[i].light_message,
						light_lines[i].row,
						light_lines[i].col);
			}
		}
	}

	goto_row_col(row_curser,col_curser);
	row_curser = save_row_curser;
	col_curser = save_col_curser;
	turn_on_curser();
}




void far alt_menu_off()
{
int i,save_row_curser,save_col_curser;

	save_row_curser = row_curser;
	save_col_curser = col_curser;
	read_curser_pos();
	turn_off_curser();

	display_alt_menu = FALSE;

	for(i = 0 ; i < 28 ; i++)   /*column 36 stuff*/
	{
		if(light_lines[i].light_message[0] != '\0')
		{
			goto_row_col(light_lines[i].row,light_lines[i].col);
			printf("%s",column36_blank);
		}
	
	}

	for(i = 28 ; i < 32 ; i++)   /*column 61 stuff*/
	{
		if(light_lines[i].light_message[0] != '\0')
		{
			goto_row_col(light_lines[i].row,light_lines[i].col);
			printf("%s",column61_blank);
		}
	}

	goto_row_col(row_curser,col_curser);
	row_curser = save_row_curser;
	col_curser = save_col_curser;
	turn_on_curser();
}






void far alt_functions()
{
int the_char,row,i,save_row_curser,save_col_curser;

/*save the curser position*/
	save_row_curser = row_curser;
	save_col_curser = col_curser;
	read_curser_pos();
	turn_off_curser();

	the_char = ' ';
	while(the_char != 13 /*cr*/)
	{
		the_char = read_host_buffer();

		while(the_char == -1)
		{
			for(i = 0 ; i < 1000 ; i++)
				;

			the_char = read_host_buffer();
		}

		if(the_char == 24)  /*CAN*/
		{
			all_lights_off();
		}
		else if(the_char == 25)  /*EM*/
		{
			all_lights_on();
		}
		else if((the_char >= '@') && (the_char <= 95))  /*turn off line*/
		{
			row = the_char - 64;

			light_lines[row].is_on = FALSE;

			if(display_alt_menu == TRUE)
			{
				if(light_lines[row].light_message[0] != '\0')
				{
					goto_row_col(light_lines[row].row,
						light_lines[row].col);
	
					if(light_lines[row].col > 60)
					{
						printf("%s",column61_blank);
					}
					else
						printf("%s",column36_blank);
				}
			}
		}
		else if((the_char >= ' ') && (the_char <= '?')) /*turn on line*/
		{
			row = the_char - 32;

			light_lines[row].is_on = TRUE;

			if(display_alt_menu == TRUE)
			{
				if(light_lines[row].light_message[0] != '\0')
				{
					if(light_lines[row].col < 60)
					{
						goto_row_col(light_lines[row].row,
							light_lines[row].col);
		
						printf("%s",light_lines[row].
							light_message);
					}
					else
						normal_string(light_lines[row].
							light_message,
							light_lines[row].row,
							light_lines[row].col);
				}
			}
		}
			
	}

/*restore the cursor*/
	goto_row_col(row_curser,col_curser);
	row_curser = save_row_curser;
	col_curser = save_col_curser;
	turn_on_curser();
}


