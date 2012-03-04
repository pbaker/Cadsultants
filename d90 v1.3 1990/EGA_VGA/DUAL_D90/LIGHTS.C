#include "stdio.h"


#define TRUE  1
#define FALSE  0


#define USE_COLOR_CARD  1
#define USE_MONO_CARD  0



extern int far read_host_buffer();
extern void far clear_display_page();
extern void far enter_graphics_mode();


extern char far my_text_mode;
extern char far *mono_ptr;


char far *column36_blank = "                       ";
char far *column61_blank = "                ";


unsigned char far megatek_data[33];



char far display_alt_menu = TRUE;

struct light_line
{
	int col,row;
	char is_on;
	char light_message[80];
};


struct light_line far light_lines[32] =
	{
		{36,8,FALSE, "ALT M acros"},
		{36,13,FALSE,"ALT 1 Change Module"},
		{36,6,FALSE, "ALT F ile/Terminate"},
		{36,9,FALSE, "ALT R edisplay"},

		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},
		{36,0,FALSE,"\0"},

		{36,2,FALSE, "ALT B lank/Unblank"},
		{36,3,FALSE, "ALT C alculator"},
		{36,15,FALSE,"ALT 3 Module Parameters"},
		{36,10,FALSE,"ALT S ystem Parameters"},
		{36,11,FALSE,"ALT W CS Control"},
		{36,14,FALSE,"ALT 2 Module Function"},
		{36,12,FALSE,"ALT Z oom/Pan/Regen"},
		{36,7,FALSE, "ALT L ayer Control"},
		{36,4,FALSE, "ALT D isplay Control"},
		{36,5,FALSE, "ALT E dit Work View"},

		{61,14,FALSE,{26,' ','E','n','t','r','y',' ','C','o','m','p','l','e','t','e','\0'}},
		{61,15,FALSE,{27,' ','R','e','j','e','c','t',' ',' ',' ',' ',' ',' ',' ',' ','\0'}},
		{61,16,FALSE,{25,' ','T','e','r','m','i','n','a','t','e',' ',' ',' ',' ',' ','\0'}},
		{61,13,FALSE,{24,' ','A','l','t','.',' ','A','c','t','i','o','n',' ',' ',' ','\0'}}
	};





void far read_more(number)
int number;
{
int i,j,ch;


	for(i = 0 ; i < number ; i++)
	{
		ch = read_host_buffer();

		while(ch == -1)
		{
			for(j = 0 ; j < 3333 ; j++)
				;

			ch = read_host_buffer();
		}


		megatek_data[i] = ch & 0x7f;
	}

}






void far normal_string(the_string,row,col)
char far the_string[];
int row,col;
{
int i;

	for(i = 0 ; the_string[i] != '\0' ; i++)
	{
		*(mono_ptr + row*160 + ((col + i) << 1)) = the_string[i];
		*(mono_ptr + row*160 + ((col + i) << 1) + 1) = 0x0f;
	}

}







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
				normal_string(column36_blank,
					light_lines[i].row,
					light_lines[i].col);
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
				normal_string(column61_blank,
					light_lines[i].row,
					light_lines[i].col);
			}
		}
	}
}






void far alt_menu_on()
{
int i;


	display_alt_menu = TRUE;

	for(i = 0 ; i < 32 ; i++)
	{
		if(light_lines[i].is_on == TRUE)
		{
			if(light_lines[i].light_message[0] != '\0')
			{
				if(light_lines[i].col < 60)
				{
					normal_string(light_lines[i].
						light_message,
						light_lines[i].row,
						light_lines[i].col);
				}
				else
					normal_string(light_lines[i].
						light_message,
						light_lines[i].row,
						light_lines[i].col);
			}
		}
	}

}




void far alt_menu_off()
{
int i;


	display_alt_menu = FALSE;

	for(i = 0 ; i < 28 ; i++)   /*column 36 stuff*/
	{
		if(light_lines[i].light_message[0] != '\0')
		{
			normal_string(column36_blank,
				light_lines[i].row,
				light_lines[i].col);

		}
	
	}

	for(i = 28 ; i < 32 ; i++)   /*column 61 stuff*/
	{
		if(light_lines[i].light_message[0] != '\0')
		{
			normal_string(column61_blank,
				light_lines[i].row,
				light_lines[i].col);

		}
	}

}






void far alt_functions()
{
int the_char,row,i;


/*
fprintf(stdprn,"HOST LIGHT DATA\n");
fflush(stdprn);
*/

	if(my_text_mode == USE_COLOR_CARD)
	{
		clear_display_page();
		my_text_mode = USE_MONO_CARD;
		enter_graphics_mode();
	}





	the_char = ' ';
	while(the_char != 13 /*cr*/)
	{
		read_more(1);

		the_char = megatek_data[0];


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
					if(light_lines[row].col > 60)
					{
						normal_string(column61_blank,
							light_lines[row].row,
							light_lines[row].col);
					}
					else
						normal_string(column36_blank,
							light_lines[row].row,
							light_lines[row].col);

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
					normal_string(light_lines[row].
						light_message,
						light_lines[row].row,
						light_lines[row].col);
				}
			}
		}
			
	}

}

