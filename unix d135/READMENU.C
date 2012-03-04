#include <stdio.h>
#include <string.h>
#include "mywindows.h"


#define TRUE 1
#define FALSE 0

#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1


#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3


extern int  win_y_low;
extern int  win_x_low;

extern struct a_window  my_windows[];



/* choose.c functions */
extern void  control_window();
extern int  choose_menu();
extern int  information_menu();
extern void  error_message();



/*my C versions of BIOS calls*/
extern void  make_win();
extern void  clear_win();
extern void  save_win();
extern void  restore_win();
extern void  goto_row_col();
extern void  read_curser_pos();
extern void  blinking_string();
extern void  normal_string();
extern void  turn_on_curser();
extern void  turn_off_curser();

extern void wait_read_keyboard();
extern int type_char_read,value_char_read;


char  read_menu_save_string[222];




int  char_is_hex(the_char)
char the_char;
{
	if((the_char >= '0') && (the_char <= '9'))
		return(TRUE);

	if((the_char >= 'a') && (the_char <= 'f'))
		return(TRUE);

	if((the_char >= 'A') && (the_char <= 'F'))
		return(TRUE);


	return(FALSE);
}



int  atox(the_string)
char  the_string[];
{
int i,result;

	result = 0;
	strupr(the_string);


	for(i = 0 ; (the_string[i] != '\0') &&
	        (char_is_hex(the_string[i]) == TRUE) ; i++)
	{
		if((the_string[i] >= '0') && (the_string[i] <= '9'))
			result = (result << 4) + the_string[i] - '0';
		else
			result = (result << 4) + the_string[i] - 'A' + 10;
	}


	return(result);
}





void  enforce_integer_range(value,bottom_range,top_range)
int  *value,bottom_range,top_range;
{
	if(*value < bottom_range)
		*value = bottom_range;

	if(*value > top_range)
		*value = top_range;
}




void  read_menu_integer(header_string,value,bottom_range,top_range)
char  header_string[];
int  *value;
int bottom_range,top_range;
{
int i,ch;
char the_string[33],temp_string[80], *temp_ptr;
int row,col;


start_over:


/*  clear text array out */
	for(i = 0 ; i < MAX_WINDOW_ROWS - 2 ; i++)
	{
		*(my_windows[HOME_MENU].text_array[i]) = '\0';
		*(my_windows[HOME_MENU].text_array[i] + 1) = '\0';
	}


	row = 2;

	temp_ptr = my_windows[HOME_MENU].text_array[0];
	sprintf(temp_ptr,"%s = %d",header_string,*value);

	temp_ptr = my_windows[HOME_MENU].text_array[row];
	sprintf(temp_ptr," Enter New Value [%d..%d] : ?",
		bottom_range,top_range);
	col = strlen(temp_ptr) + 1; 

	display_pop_up_window(HOME_MENU);
	control_window();



	wait_read_keyboard();

	if(type_char_read == 0)
	{
		switch(value_char_read)
		{
			case 75 : 
			case 77 : 
			case 80 :
				return;

			default :
				goto start_over;
		}
	}
	else
	{
		ch = type_char_read;

		if((ch >= '0') && (ch <= '9'))
		{
			temp_ptr = my_windows[HOME_MENU].text_array[row];
			temp_string[0] = ch;
			temp_string[1] = '\0';
			strcat(temp_ptr,temp_string);

			the_string[0] = ch;
			display_pop_up_window(HOME_MENU);
			control_window();
		}
		else
			goto start_over;

	}



	i = 1;

/*
	for(ch = getch() ; (ch != 13)
		 && (ch != 0) ; ch = getch())
*/
	for(wait_read_keyboard() ; (type_char_read != 13) && 
		(type_char_read != 10) && (type_char_read != 0) ;
		wait_read_keyboard())
	{
		ch = type_char_read;

		if(ch != 8)
		{
			if((ch >= '0') && (ch <= '9'))
			{
			/*	putchar(ch);
			*/

				temp_ptr = my_windows[HOME_MENU].
					text_array[row];
				temp_string[0] = ch;
				temp_string[1] = '\0';
				strcat(temp_ptr,temp_string);

				the_string[i] = ch;
				i++;

				display_pop_up_window(HOME_MENU);
				control_window();
			}
		}
		else
		{
			if(i > 0)
			{
				i--;

				*(my_windows[HOME_MENU].
					text_array[row] + col + i - 1) = '\0';
				*(my_windows[HOME_MENU].
					text_array[row] + col + i) = '\0';
				display_pop_up_window(HOME_MENU);
				control_window();
			}
		}
	}


	the_string[i] = '\0';


	if(type_char_read == 0)
	{	
		switch(value_char_read)
		{
			case 75 : /*reject*/
			{
				goto start_over;
			}

			case 77 : /*entry complete*/
			{
				*value = atoi(the_string);
				enforce_integer_range(value,bottom_range,
					top_range);

				break;
			}


			case 80 : /*terminate*/
				return;
		}
	}
	else
	{
		*value = atoi(the_string);
		enforce_integer_range(value,bottom_range,top_range);

	}

}







void  read_menu_string(header_string,return_string)
char  header_string[];
char  return_string[];
{
int i,ch;
int row,col;
char  *temp_ptr,temp_string[80];


	row = 4;
	col = 3;

	strcpy(read_menu_save_string,return_string);


start_over:


/*  clear text array out */
	for(i = 0 ; i < MAX_WINDOW_ROWS - 2 ; i++)
	{
		*(my_windows[HOME_MENU].text_array[i]) = '\0';
		*(my_windows[HOME_MENU].text_array[i] + 1) = '\0';
	}


	temp_ptr = my_windows[HOME_MENU].text_array[0];
	sprintf(temp_ptr,"%s",header_string);


	if(return_string[0] != '\0')
	{
		temp_ptr = my_windows[HOME_MENU].text_array[2];
		sprintf(temp_ptr,"Current Value = %s",return_string);
	}


	display_pop_up_window(HOME_MENU);
	control_window();


	wait_read_keyboard();

	if(type_char_read == 0)
	{
		ch = value_char_read; 

		switch(ch)
		{
			case 75 : 
			case 77 : 
			case 80 :
			{
				strcpy(return_string,read_menu_save_string);

				return;
			}

			default :
				goto start_over;
		}
	}
	else
	{
		ch = type_char_read;

		return_string[0] = ch;
		temp_ptr = my_windows[HOME_MENU].text_array[row];
		strcpy(temp_ptr,"   ");
		temp_string[0] = ch;
		temp_string[1] = '\0';
		strcat(temp_ptr,temp_string);
		display_pop_up_window(HOME_MENU);
		control_window();
	}



	i = 1;

/* 
	for(ch = getch() ; (ch != 13)
		 && (ch != 0) ; ch = getch())
*/
	for(wait_read_keyboard() ; (type_char_read != 13) &&
		(type_char_read != 10) && (type_char_read != 0) ;
		wait_read_keyboard())
	{
		ch = type_char_read;

		if(ch != 8)
		{
		/*
			putchar(ch);
		*/
			
			temp_ptr = my_windows[HOME_MENU].text_array[row];
			temp_string[0] = ch;
			temp_string[1] = '\0';
			strcat(temp_ptr,temp_string);

			return_string[i] = ch;
			i++;

			display_pop_up_window(HOME_MENU);
			control_window();
		}
		else
		{
			if(i > 0)
			{
				i--;

				*(my_windows[HOME_MENU].
					text_array[row] + col + i) = '\0';

				display_pop_up_window(HOME_MENU);
				control_window();
			}
		}
	}


	return_string[i] = '\0';


	if(type_char_read == 0)
	{	
		ch = value_char_read;

		switch(ch)
		{
			case 75 : /*reject*/
			{
				strcpy(return_string,read_menu_save_string);
				goto start_over;
			}

			case 77 : /*entry complete*/
				break;
	
			case 80 : /*terminate*/
			{
				strcpy(return_string,read_menu_save_string);
				break;
			}
		}
	}
}


