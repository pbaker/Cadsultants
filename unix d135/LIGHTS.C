/*March 29, 1989 replaced read_host_buffer() calls with read_more(1) calls
for empty buffer handling, masking off host data, etc */


#include <stdio.h>
#include <string.h>
#include <pw_lib.h>
#include "mywindows.h"


#define W_BORDER_INDENT  9
#define V_BORDER_INDENT  4

#define CHAR_WIDTH  50
#define CHAR_HEIGHT  60

#define W_LEFT  8000
#define W_RIGHT  9900
#define W_TOP  2048
#define W_BOTTOM  -1000

#define V_LEFT  0
#define V_RIGHT 255
#define V_TOP  767
#define V_BOTTOM 0

#define FORE_COLOR  0x77
#define BACK_COLOR  0
#define BORDER_COLOR  0x55
#define CHAR_X_SHIFT  3*CHAR_WIDTH
#define CHAR_Y_SHIFT  2*CHAR_HEIGHT

#define LIGHT_COL  6



extern char  buffer_non_protect;
extern char  last_blt_flag;

extern struct a_window  my_windows[];
extern void  update_the_windows();


extern void  display_non_protect_row();
extern void  set_message_viewport();
extern void  restore_graphics_viewport();

extern void  read_more();
extern unsigned char  megatek_data[];




char  lights_wiped = 'f';
char  light_is_on[35] = "fffffffffffffffffffffffffffffffffff";


char  display_alt_menu = 't';

struct light_line
{
	int col,row;
	char is_on;
	char light_message[80];
};


struct light_line  light_lines[32] =
	{
		{36,8,'f', "M MACROS"},
		{36,14,'f',"1 CHANGE MODULE"},
		{36,6,'f', "F FILE/TERMINATE"},
		{36,9,'f', "R REDISPLAY"},

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

		{36,2,'f', "B BLANK/UNBLANK"},
		{36,3,'f', "C CALCULATOR"},
		{36,16,'f',"3 MODULE PARAMS"},
		{36,10,'f',"S SYSTEM PARAMS"},
		{36,11,'f',"W WCS CONTROL"},
		{36,15,'f',"2 MODULE FUNCTION"},
		{36,12,'f',"Z ZOOM/PAN/REGEN"},
		{36,7,'f', "L LAYER CONTROL"},
		{36,4,'f', "D DISPLAY CONTROL"},
		{36,5,'f', "E EDIT WORK VIEW"},

		{61,19,'f',{26,' ','E','N','T','R','Y',' ','C','O','M','P','L','E','T','E','\0'}},
		{61,20,'f',{27,' ','R','E','J','E','C','T',' ',' ',' ',' ',' ',' ',' ',' ','\0'}},
		{61,21,'f',{25,' ','T','E','R','M','I','N','A','T','E',' ',' ',' ',' ',' ','\0'}},
		{61,18,'f',{24,' ','A','L','T','.',' ','A','C','T','I','O','N',' ',' ',' ','\0'}}
	};





void  pixel_light(the_string,row,col)
char  *the_string;
int row,col;
{
int i;


	strcpy(my_windows[LIGHTS].text_array[row - 2],the_string);

	return;





	set_message_viewport();
	row += 18;

	if(the_string[0] == '\0')
	{
	/* clear light line */
		color(BACK_COLOR);

		for(i = 0 ; i < 20 ; i++)
		{
			move2((col + i)*CHAR_WIDTH + W_LEFT - 10,
				W_TOP - row*CHAR_HEIGHT - 10);
			boxr(CHAR_WIDTH,CHAR_HEIGHT);
		}

		color(FORE_COLOR);
	}
	else
	{
	/* display the light line */
		color(FORE_COLOR);
		move2(col*CHAR_WIDTH + W_LEFT,W_TOP - row*CHAR_HEIGHT);
		gtext(the_string);
	}


	restore_graphics_viewport();
}




void  all_lights_on()
{
int i;

	for(i = 0 ; i < 32 ; i++)
	{
		light_lines[i].is_on = 't';

		if(display_alt_menu == 't')
		{
			if(light_lines[i].light_message[0] != '\0')
				pixel_light(light_lines[i].light_message,
					light_lines[i].row,LIGHT_COL);
		}
	}
}



void  all_lights_off()
{
int i;

	lights_wiped = 't';


	for(i = 0 ; i < 32 ; i++) 
	{
		light_lines[i].is_on = 'f';

		if(display_alt_menu == 't')
		{
			if(light_lines[i].light_message[0] != '\0')
				pixel_light("\0",light_lines[i].row,LIGHT_COL);
		}
	}
}






void  alt_menu_on()
{
int i;


	display_alt_menu = 't';

	for(i = 0 ; i < 32 ; i++)
	{
		if(light_lines[i].is_on == 't')
		{
			if(light_lines[i].light_message[0] != '\0')
				pixel_light(light_lines[i].light_message,
					light_lines[i].row,LIGHT_COL);
		}
	}
}




void  alt_menu_off()
{
int i;


	display_alt_menu = 'f';

	for(i = 0 ; i < 32 ; i++)  
	{
		if(light_lines[i].light_message[0] != '\0')
			pixel_light("\0",light_lines[i].row,LIGHT_COL);
	
	}

}






void  alt_functions()
{
int the_char,row,i;


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

			light_lines[row].is_on = 'f';

			if(display_alt_menu == 't')
			{
				if(light_lines[row].light_message[0] != '\0')
					pixel_light("\0",light_lines[row].row,
						LIGHT_COL);
			}
		}
		else if((the_char >= ' ') && (the_char <= '?')) /*turn on line*/
		{
			row = the_char - 32;

			light_lines[row].is_on = 't';

			if(display_alt_menu == 't')
			{
				if(light_lines[row].light_message[0] != '\0')
					pixel_light(light_lines[row].
						light_message,
						light_lines[row].row,
						LIGHT_COL);
			}
		}
			
	}


	update_the_windows();

	buffer_non_protect = 'f';
}

