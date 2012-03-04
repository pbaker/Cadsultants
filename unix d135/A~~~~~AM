#include <stdio.h>
#include "mywindows.h"
#include <pw_lib.h>



#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3

extern void wait_read_keyboard();
extern int type_char_read,value_char_read;

extern struct a_window  my_windows[];


char  controls[3][50] =
{
	{26,' ','E','n','t','r','y',' ','C','o','m','p','l','e','t','e','\0'},
	{27,' ','R','e','j','e','c','t',' ',' ',' ',' ',' ',' ',' ',' ','\0'},
	{25,' ','T','e','r','m','i','n','a','t','e',' ',' ',' ',' ',' ','\0'}
};



int  control_window_background = 15;
int  control_window_foreground = 0;


void  control_window()
{
int i,screen_row,screen_col,width,height,text_size;

	return;


	linpat(0);

	text_size = my_windows[HOME_MENU].full_size.text_size;

	height = text_size*5 * 5/4;
	width = text_size*17;

	screen_row = my_windows[HOME_MENU].full_size.screen_row -
		my_windows[HOME_MENU].full_size.screen_height + height/2;

	screen_col = my_windows[HOME_MENU].full_size.screen_col +
		my_windows[HOME_MENU].full_size.screen_width - width*3/4;
	

	window(screen_col,screen_col + width,screen_row - height,screen_row);
	viewport(screen_col,screen_col + width,screen_row - height,screen_row);

	flood((control_window_background << 4) | control_window_background);
	color((control_window_foreground << 4) | control_window_foreground);

	gtsize(text_size,text_size);


	for(i = 0 ; i < 3 ; i++)
	{
		move2(screen_col + text_size,screen_row - text_size*(i + 2)
			*5/4);
		gtext(controls[i]);
	}


	linpat(1);
}




int  choose_menu(header_text,text_options,num_options,the_default)
char  *header_text;
char  *text_options[];
int num_options,the_default;
{
int i,ch;
char  *temp_ptr;




	while(2345)
	{

	/*  clear text array out */
		for(i = 0 ; i < MAX_WINDOW_ROWS - 2 ; i++)
		{
			*(my_windows[HOME_MENU].text_array[i]) = '\0';
			*(my_windows[HOME_MENU].text_array[i] + 1) = '\0';
		}



		temp_ptr = my_windows[HOME_MENU].text_array[0];
		sprintf(temp_ptr," %s",header_text);

		for(i = 0 ; i < num_options ; i++)
		{
			if(*(text_options[i]) != '\0')
			{
				temp_ptr = my_windows[HOME_MENU].
					text_array[i + 2];

				if(i + 1 < 10)
				{
					sprintf(temp_ptr,"    F%d %s",i + 1,
						text_options[i]);
				}
				else
					sprintf(temp_ptr,"   F%d %s",i + 1,
						text_options[i]);
			}
		}

		if(the_default > 0)
		{
			temp_ptr = my_windows[HOME_MENU].
				text_array[1 + the_default];
			*(temp_ptr + 6) = '>';
		}


		display_pop_up_window(HOME_MENU);
		control_window();


		wait_read_keyboard();

		if(type_char_read != 0)
			continue;

		switch(value_char_read)
		{
			case 75 :
				return(REJECT);

			case 77 :
				return(ENTRY_COMPLETE);

			case 80 :
				return(TERMINATE);

			case 0x77 :  /* ctrl home key */
				return(0x77);

			case 0x47 :  /* home key */
				return(0x47);


			default :
			{
				ch = value_char_read - 0x3b;

				if((ch >= 0) && (ch < num_options))
					return(ch + 1);
			}
		}
	}
}







int  information_menu(header_text,text_array,num_rows,do_read,in_a_loop)
char  *header_text;
char  *text_array[];
int num_rows;
char do_read,in_a_loop;
{
int i,ch;
char  *temp_ptr;


/*  clear text array out */
	for(i = 0 ; i < MAX_WINDOW_ROWS - 2 ; i++)
	{
		*(my_windows[HOME_MENU].text_array[i]) = '\0';
		*(my_windows[HOME_MENU].text_array[i] + 1) = '\0';
	}


	temp_ptr = my_windows[HOME_MENU].text_array[0];
	sprintf(temp_ptr,"  %s",header_text);

	for(i = 0 ; i < num_rows ; i++)
	{
		temp_ptr = my_windows[HOME_MENU].text_array[i + 2];
		sprintf(temp_ptr,"  %s",text_array[i]);
	}


	display_pop_up_window(HOME_MENU);
	control_window();


	if(do_read == 'f')
		return(0);


	while(879)
	{
		wait_read_keyboard();

		if(type_char_read != 0)
			continue;


		switch(value_char_read)
		{
			case 75 :
				return(REJECT);

			case 77 :
				return(ENTRY_COMPLETE);

			case 80 :
				return(TERMINATE);
		}
	}
}




void  error_message(window_row,the_text,num_rows)
int window_row;
char  *the_text[];
int num_rows;
{
int i;
char  *temp_ptr;


	for(i = 0 ; i < num_rows ; i++)
	{
		temp_ptr = my_windows[HOME_MENU].text_array[window_row + 1 + i];
		sprintf(temp_ptr,"  %s",the_text[i]);
	}

	temp_ptr = my_windows[HOME_MENU].text_array[window_row + i + 2];
	sprintf(temp_ptr,"Press Enter To Continue");

	display_pop_up_window(HOME_MENU);
	control_window();

	wait_read_keyboard();
}

