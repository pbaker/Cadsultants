#include <stdio.h>





/*
#define LOG_COMMANDS
*/


#define SHIFT_IN  0
#define SHIFT_OUT  1


#define TRUE 't'
#define FALSE 'f'

#define ESCAPE 0x1b


#define VT100_TERMINAL 0
#define ASCII_TERMINAL 1



#define APPLICATIONS_MODE 0
#define NUMERIC_MODE 1
#define CURSER_MODE 2

#define VT100_MODE  0
#define MESS_MON  1





int atoi();


extern int  the_vt100_color;



extern char  my_test_flag;


extern void  command_logger();
extern char  command_log_string[];

extern int  read_host_buffer();
extern void  vt100_pckb();
extern void  ugout();

extern char  which_terminal;



int  vt100_mess_mon_flag = VT100_MODE;
char curser_key_mode = CURSER_MODE;
char  keypad_key_mode = NUMERIC_MODE;

char  shift_out_chars[256];
char  shift_flag = SHIFT_IN;

char  escape_push_back = '\0';


char  escape_parameter[22];
char  escape_host_data[100];
char  vt100_attribute = 15;

char  parameters[25][9];


char  escape_error[100];
char  escape_output[50];

int  num_parameters = 0; 

char  escape_buffer[1000];


int top_of_scroll_region = 0;
int  bottom_of_scroll_region = 24;


int  curser_row_save = 0;
int  curser_col_save = 0;


char  the_error_message[222];



char  is_letter(ch)
int ch;
{
	if((ch >= 'a') && (ch <= 'z'))
		return(TRUE);

	if((ch >= 'A') && (ch <= 'Z'))
		return(TRUE);


	return(FALSE);
}







void  escape_error_handler(a_string)
char  a_string[];
{
	sprintf(the_error_message,"VT100 Escape Sequence Error: %s\n",
		a_string);
	error_logger(the_error_message);
}






void  display_host_char(ch)
int ch;
{
int save_row,save_col;


	if(my_test_flag == 't')
	{
		if(vt100_mess_mon_flag == MESS_MON)
			vt100_attribute = the_vt100_color;

		vt100_mess_mon_flag = VT100_MODE;
		my_test_flag = 'f';
	}


	if(ch == 14)
	{
		shift_flag = SHIFT_OUT;
		return;
	}


	if(ch == 15)
	{
		shift_flag = SHIFT_IN;
		return;
	}


/*	save_row = row_curser;
	save_col = col_curser;
*/
/*
	read_curser_pos();


	if(ch == 13)
		goto_row_col(row_curser,0);
	else
*/
 if(ch == 10)
	{
		if(vt100_mess_mon_flag == VT100_MODE)
		{
/*
			if(row_curser >= bottom_of_scroll_region)
			{
				scroll_up(1,
					0,top_of_scroll_region,
					79,bottom_of_scroll_region,
					15);

				goto_row_col(bottom_of_scroll_region,0);
			}
			else
				goto_row_col(row_curser + 1,col_curser);
*/
		}
		else
		{
/*			if(row_curser > 23)
			{
				scroll_up(1,
					0,0,
					79,23,
					15);
				goto_row_col(23,0);
			}
			else
				goto_row_col(row_curser + 1,0);
*/
		}
	}
	else if(ch == 8)
	{
#ifdef shsdajhdsj
		if(col_curser > 0)
		{
			goto_row_col(row_curser,col_curser - 1);

	/*      	output_character(' ',vt100_attribute);
	*/
		}
#endif
	}
	else if(ch == 7)
	{
		putchar(7);
	}
	else
	{
#ifdef ldldlldfdflfelfel
		if(shift_flag == SHIFT_IN)
			output_character(ch,vt100_attribute);
		else
		{
			if(ch == '`')
				output_character(0x4,vt100_attribute);
			else
			{
		/*  use the shift out array */

				switch(ch)
				{
					case '^' :
					{
						output_character('^',
							vt100_attribute);
						break;
					}


					case '>' :
					{
						output_character('>',
							vt100_attribute);
						break;
					}


					case '<' :
					{
						output_character('<',
							vt100_attribute);
						break;
					}


					case ' ' :
					{
						output_character(' ',
							vt100_attribute);
						break;
					}


					case 'j' :
					{
						output_character(0xd9,
							vt100_attribute);
						break;
					}


					case 'k' :
					{
						output_character(0xbf,
							vt100_attribute);
						break;
					}

					
					case 'l' :
					{
						output_character(0xda,
							vt100_attribute);
						break;
					}

					case 'm' :
					{
						output_character(0xc0,
							vt100_attribute);
						break;
					}

					case 'n' :
					{
						output_character(0xc5,
							vt100_attribute);
						break;
					}


					case 'q' :
					{
						output_character(0xc4,
							vt100_attribute);

						break;
					}

					case 't' :
					{
						output_character(0xc3,
							vt100_attribute);
						break;
					}


					case 'u' :
					{
						output_character(0xb4,
							vt100_attribute);
						break;
					}


					case 'v' :
					{
						output_character(0xc1,
							vt100_attribute);
						break;
					}


					case 'w' :
					{
						output_character(0xc2,
							vt100_attribute);
						break;
					}

					case 'x' :
					{
						output_character(0xb3,
							vt100_attribute);

						break;
					}


				}
			}   /*end else use shifted out */
		}



		col_curser++;


		if(col_curser > 80)
		{
			if(row_curser > bottom_of_scroll_region - 1)
			{
				scroll_up(1,
					0,top_of_scroll_region,
					79,bottom_of_scroll_region,
					15);

				goto_row_col(bottom_of_scroll_region,0);
			}
			else
				goto_row_col(row_curser + 1,0);
		}
		else
			goto_row_col(row_curser,col_curser);

#endif
	}


	
/*
	row_curser = save_row;
	col_curser = save_col;
*/
}



void  escape_read_more(number)
{
int i,ch,j;

	for(i = 0 ; i < number ; i++)
	{
		vt100_pckb();

		if(escape_push_back == '\0')
		{
			ch = read_host_buffer();

			while(ch == -1)
			{
				vt100_pckb();

			/*
				for(j = 0 ; j < 90000 ; j++)
					;
			*/

				ch = read_host_buffer();
			}
		}
		else
		{
			ch = escape_push_back;
			escape_push_back = '\0';
		}


		escape_host_data[i] = ch & 0x7f;
	}
}





char  is_parameter_character(the_char)
int the_char;
{
	if((escape_host_data[0] >= '0') && (escape_host_data[0] <= '9'))
		return(TRUE);
	else if(escape_host_data[0] == '?')
		return(TRUE);
	else if(escape_host_data[0] == '<')
		return(TRUE);
	else
		return(FALSE);
}






void  read_escape_parameter(the_string)
char  the_string[];
{
int i;

	for(i = 0,escape_read_more(1) ; 
		is_parameter_character(escape_host_data[0]) == TRUE ;
			 i++,escape_read_more(1))
				the_string[i] = escape_host_data[0];

	the_string[i] = '\0';

	escape_push_back = escape_host_data[0];
}







void  parse_parameters()
{
int i,row,col;


	for(i = 0 ; i < 20 ; i++)
	{
		parameters[i][0] = '0';
		parameters[i][1] = '\0';
	}


	num_parameters = 1;

	i = 0;

	for(row = 0 ; (row < 20) && (escape_buffer[i] != '\0') ; row++)
	{
		for(col = 0 ; (escape_buffer[i] != ';') &&
			(escape_buffer[i] != '\0') ; col++)
		{
			parameters[row][col] = escape_buffer[i];
			i++;
		}	

		if(col > 0)
			parameters[row][col] = '\0';

		if(escape_buffer[i] != '\0')
		{
			num_parameters++;

			i++;   /*skip over semicolon*/	
		}
	}


	parameters[row][0] = '0';
	parameters[row][1] = '\0';
}





void  erase_line(which_way)
int which_way;
{
int i;

#ifdef leleelelel

	switch(which_way)
	{
		case 0 :  /*from cursor to end of line*/
		{
			for(i = col_curser ; i < 80 ; i++)
			{
				goto_row_col(row_curser,i);
				output_character(' ',15);
			}

			goto_row_col(row_curser,col_curser);

			break;
		}

		case 1 :  /*from begin line to cursor*/
		{
			goto_row_col(row_curser,0);

			for(i = 0 ; i < col_curser ; i++)
			{
				goto_row_col(row_curser,i);
				output_character(' ',15);
			}

			break;
		}

		case 2 :  /*the entire line*/
		{
			goto_row_col(row_curser,0);

			for(i = 0 ; i < 80 ; i++)
			{
				goto_row_col(row_curser,i);
				output_character(' ',15);
			}

			goto_row_col(row_curser,col_curser);

			break;
		}

	}
#endif
}




void  erase_screen(which_way)
int which_way;
{
	switch(which_way)
	{
		case 0 :  /*curser line down to bottom*/
		{
/*
			scroll_up(0,
				0,row_curser,
				79,bottom_of_scroll_region,
				15);

			goto_row_col(row_curser,col_curser);
*/
			break;
		}


		case 1 :  /*top of screen to curser line*/
		{
/*
			scroll_up(0,
				0,top_of_scroll_region,
				79,row_curser,
				15);

			goto_row_col(row_curser,col_curser);
*/

			break;
		}

		case 2 : /*entire screen*/
		{
/*
			scroll_up(0,
				0,top_of_scroll_region,
				79,bottom_of_scroll_region,
				15);

			goto_row_col(row_curser,col_curser);

*/
			break;
		}
	}
}




void  reset_question_param(which_one)
int which_one;
{
	switch(parameters[which_one][1])
	{
		case '1' :  /*curser key mode*/
		{
			curser_key_mode == CURSER_MODE;
			break;
		}


		case '2' :  /*set to use VT52 syntax*/
		{

			break;
		}


		case '3' :  /*set dialog area to 80 column*/
		{
#ifdef lsjdjjdjdjddj
			scroll_up(0,      /* clear whole region */
				0,0,      /* upper left corner */
				79,24,    /* lower right corner */
				0x0f);    /* Normal attributes */
#endif

			break;
		}


		case '6' :  /*set origin to absolute*/
		{

			break;
		}

		case '7' :  /*disable auto-wrap*/
		{

			break;
		}

		case '8' :  /*disable auto repeat of keys*/
		{

			break;
		}
	}
}





void  set_question_param(which_one)
int which_one;
{
	switch(parameters[which_one][1])
	{
		case '1' :  /*curser key mode*/
		{
			curser_key_mode == APPLICATIONS_MODE;
			break;
		}

	/*where's the  ?2  in the big book???*/

		case '3' :  /*set dialog width to 132*/
		{
#ifdef kskskskssk
			scroll_up(0,      /* clear whole region */
				0,0,      /* upper left corner */
				79,24,    /* lower right corner */
				0x0f);    /* Normal attributes */
#endif

			break;
		}

		case '6' :  /*set origin-mode to relative*/
		{

			break;
		}

		case '7' :  /*enable auto-wrap*/
		{

			break;
		}

		case '8' :  /*enable auto-repeat of keys*/
		{

			break;
		}
	}
}




void  reset_mode()
{
int i;


	for(i = 0 ; i < num_parameters ; i++)
	{
		switch(parameters[i][0])  /*first char of parameter*/
		{
			case '1' :
			{
				if(parameters[i][1] == '2')  /*send/receive*/
				{

				}

				break;
			}


			case '2' : 
			{
				if(parameters[i][1] == '\0') /*unlock keyboard*/
				{

				}
				else  /*linefeed/newline behavior*/
				{

				}

				break;
			}

			case '4' :   /*insertion replacement mode*/
			{

				break;
			}


			case '?' :
			{
				reset_question_param(i);

				break;
			}

			case '<' :  /*overstrike/replace mode*/
			{

				break;
			}

		}
	}
}







void  set_mode()
{
int i;



	for(i = 0 ; i < num_parameters ; i++)
	{
		switch(parameters[i][0])  /*first char of parameter*/
		{
			case '1' :
			{
				if(parameters[i][1] == '2') /*send/receive*/
				{

				}

				break;
			}


			case '2' : 
			{
				if(parameters[i][1] == '\0') /*lock keyboard*/
				{

				}
				else   /*linefeed/newline behavior*/
				{

				}

				break;
			}

			case '4' :   /*insert/replace*/
			{

				break;
			}

			case '?' :
			{
				set_question_param(i);

				break;
			}

			case '<' :  /*overstrike/replace*/
			{

				break;
			}

		}
	}
}





void  insert_n_chars(number)
int number;
{

}




void  tabulation_clear(which_way)
char which_way;
{

}




void  select_graphic_rendition()
{
int i;
int a_param;

	for(i = 0 ; i < num_parameters ; i++)
	{
		a_param = atoi(&(parameters[i][0]));

		switch(a_param)
		{
			case 0 :  /*primary rendition*/
			{
				vt100_attribute = 15;

				break;
			}

			case 1 :  /*bold*/
			{
				
				break;
			}

			case 4 :  /*underscore*/
			{

				break;
			}

			case 5 :  /*slow blink*/
			{

				break;
			}


			case 7 :  /*reverse video*/
			{
				vt100_attribute = 0x70;

				break;
			}

					
			case 24 : /*no underscore*/
			{


				break;
			}

			case 25 : /*no blink*/
			{

				break;
			}

			case 27 :  /*positive video text*/
			{
				vt100_attribute = 15;

				break;
			}

			case 30 :  /*black on white*/
			{

				break;
			}

			case 31 :  /*red on transparent*/
			{

				break;
			}

			case 32 :  /*green on transparent*/
			{

				break;
			}

			case 33 :  /*yellow on transparent*/
			{
	
				break;
			}

			case 34 :  /*blue on transparent*/
			{

				break;
			}

			case 35 :  /*magenta on transparent*/
			{

				break;
			}

			case 36 :  /*cyan on transparent*/
			{

				break;
			}

			case 37 :  /*white on transparent*/
			{

				break;
			}

			case 39 :  /*default color (white on transparent)*/
			{

				break;
			}

		}  /*end switch*/
	}

}





void  which_left_bracket()
{
int save_col,save_row;
int i,j,k,l,m;
int row,col;

/*
	save_row = row_curser;
	save_col = col_curser;
*/

/*
	read_curser_pos();
*/


	escape_read_more(1);


#ifdef LOG_COMMANDS
		sprintf(command_log_string,"VT100 ESCAPE [ %s %c",
			escape_buffer,escape_host_data[0]);
		command_logger(command_log_string);
#endif



	switch(escape_host_data[0])
	{
		case 'f' :  /*position cursor*/
		case 'H' :
		{

			parse_parameters();

			row = atoi(&(parameters[0][0]));
			col = atoi(&(parameters[1][0]));

			if(row == 0)
				row = 1;

			if(col == 0)
				col = 1;

		/*check for outside of scroll region, etc*/

/*
			goto_row_col(row - 1,col - 1);
*/

			break;
		}


		case 'D' :  /*cursor left*/
		{
			parse_parameters();

			if(parameters[0][0] == '0')
				parameters[0][0] = '1';

/*
			if(col_curser >= atoi(&(parameters[0][0])))
				goto_row_col(row_curser,col_curser -
					atoi(&(parameters[0][0])));
			else
			{
				sprintf(escape_error,
				    "Cursor Left by: %s  col_cursor = %d",
					&(parameters[0][0]),col_curser);
				escape_error_handler(escape_error);
			}
*/

			break;
		}

		case 'B' :  /*cursor down*/
		{
			parse_parameters();

			if(parameters[0][0] == '0')
				parameters[0][0] = '1';



/*
			if(bottom_of_scroll_region - row_curser >= 
				atoi(&(parameters[0][0])))
				     goto_row_col(row_curser + 
					atoi(&(parameters[0][0])),
					col_curser);
			else
			{
				sprintf(escape_error,
				    "Cursor Down by: %s  row_cursor = %d",
					&(parameters[0][0]),row_curser);
				escape_error_handler(escape_error);
			}
*/
			break;
		}

		case 'C' :  /*cursor right*/
		{
			parse_parameters();

			if(parameters[0][0] == '0')
				parameters[0][0] = '1';


/*
			if(79 - col_curser >= atoi(&(parameters[0][0])))
				goto_row_col(row_curser,col_curser +
					atoi(&(parameters[0][0])));
			else
			{
				sprintf(escape_error,
				    "Cursor Right by: %s  col_cursor = %d",
					&(parameters[0][0]),col_curser);
				escape_error_handler(escape_error);
			}
*/
			break;
		}

		case 'A' :  /*cursor up*/
		{
			parse_parameters();

			if(parameters[0][0] == '0')
				parameters[0][0] = '1';


/*
			if(row_curser >= atoi(&(parameters[0][0])))
				goto_row_col(row_curser - 
					atoi(&(parameters[0][0])),
						col_curser);
			else
			{
				sprintf(escape_error,
				    "Cursor Up by: %s  row_cursor = %d",
					&(parameters[0][0]),row_curser);
				escape_error_handler(escape_error);
			}
*/

			break;
		}


		case 'r' :  /*scroll region margins*/
		{
	/*sets the region of the screen that can be scrolled*/

			parse_parameters();

/*
			goto_row_col(0,0);
*/

			top_of_scroll_region = atoi(&(parameters[0][0]));
			bottom_of_scroll_region = atoi(&(parameters[1][0]));


			if(top_of_scroll_region == 0)
				top_of_scroll_region = 1;


			top_of_scroll_region--;
			bottom_of_scroll_region--;

			break;
		}

		case 'm' :   /*select graphic rendition*/
		{
			parse_parameters();

			select_graphic_rendition();

			break;
		}

	
		case 'K' :  /*erase line*/
		{
			parse_parameters();


			erase_line(atoi(&(parameters[0][0])));

			break;
		}

		case 'J' :   /*erase screen*/
		{
			parse_parameters();

			erase_screen(atoi(&(parameters[0][0])));

			break;
		}

		case 'q' :  /*light emitting diode control*/
		{

			break;
		}

		case 'n' :  /*device status report*/
		{
			parse_parameters();


			if(parameters[0][0] == '5')  /*status report*/
			{
			/*terminal OK*/
				ugout(ESCAPE);
				ugout('[');
				ugout('0');
				ugout('n');
			}
			else if(parameters[0][0] == '6')  /*report cursor pos*/
			{
/*
				sprintf(escape_output,"%c[%d;%dR",ESCAPE,
					row_curser,col_curser);

				for(i = 0 ; escape_output[i] != '\0' ; i++)
					ugout(escape_output[i]);
*/
			}


			break;
		}

		case 'g' :   /*clear tab(s)*/
		{
			parse_parameters();

			tabulation_clear(parameters[0][1]);

			break;
		}

		case 'c' :  /*what are you report*/
		{
			if(which_terminal == VT100_TERMINAL)
			{
				ugout(ESCAPE);
				ugout('[');
				ugout('?');
				ugout('1');
				ugout(';');
				ugout('2');
				ugout('c');
			}



			break;
		}

		case 'l' : /*reset mode*/
		{
			parse_parameters();

			reset_mode();

			break;
		}

		case 'h' : /*set mode*/
		{
			parse_parameters();

			set_mode();

			break;
		}

		case 'Z' :  /*curser backward tab*/
		{

			break;
		}

		case 'I' : /*curser horizontal tab*/
		{

			break;
		}

		case 'P' : /*delete characters from a line*/
		{

			break;
		}

		case 'M' : /*delete n lines*/
		{

			break;
		}

		case 'X' :  /*erase characters*/
		{

			break;
		}

		case '@' :  /*insert characters*/
		{
			parse_parameters();

			insert_n_chars(atoi(&(parameters[0][0])));

			break;
		}

		case 'L' :  /*insert lines*/
		{

			break;
		}



		case 'T' :  /*scroll down*/
		{

			break;
		}


		case 'S' :  /*scroll up*/
		{

			break;
		}


		default :
		{
			sprintf(escape_error,
			    "Unknown Sequence: ESC [ .... %c",
				escape_host_data[0]);
			escape_error_handler(escape_error);
		}
	}
}





void  left_square_bracket()
{
int i;


	if(vt100_mess_mon_flag == MESS_MON)
		vt100_attribute = the_vt100_color;


	vt100_mess_mon_flag = VT100_MODE;



	escape_read_more(1);

	i = 0;

	while((is_letter(escape_host_data[0]) == FALSE) &&
		(escape_host_data[0] != '@'))
	{
		escape_buffer[i] = escape_host_data[0];
		escape_read_more(1);
		i++;
	}

	escape_push_back = escape_host_data[0];

	escape_buffer[i] = '\0';

		
	which_left_bracket();
	
}




void  host_escape(first_char)
int first_char;
{
int save_row,save_col;


	escape_push_back = first_char;


/*
	save_row = row_curser;
	save_col = col_curser;
*/
/*
	read_curser_pos();
*/

	escape_read_more(1);


#ifdef LOG_COMMANDS

		if(escape_host_data[0] != '[')
		{
			sprintf(command_log_string,"VT100 ESCAPE %c",
				escape_host_data[0]);
			command_logger(commandç_log_string);
		}
#endif



	switch(escape_host_data[0])
	{
		case '[' :
		{
			left_square_bracket();

			break;
		}

		case '8' :  /*restore saved curser pos*/
		{
/*
			goto_row_col(curser_row_save,curser_col_save);
*/

			break;
		}

		case '7' :  /*save curser pos*/
		{
/*			curser_row_save = row_curser;
			curser_col_save = col_curser;
*/
			break;
		}

		case 'D' :  /*index (move curser down)*/
		{
/*
			if(row_curser >= bottom_of_scroll_region)
				scroll_up(1,
					0,top_of_scroll_region,
					79,bottom_of_scroll_region,
					15);
			else
				goto_row_col(row_curser + 1,col_curser);

*/
			break;
		}

		case 'M' :  /*reverse index*/
		{
/*
			if(row_curser <= top_of_scroll_region)
				scroll_down(1,
					0,top_of_scroll_region,
					79,bottom_of_scroll_region,
					15);
			else
				goto_row_col(row_curser - 1,col_curser);

*/
			break;
		}

		case 'E' :  /*next line*/
		{

			break;
		}

		case '=' :
		{
			keypad_key_mode = APPLICATIONS_MODE;

			break;
		}

		case '>' :
		{
			keypad_key_mode = NUMERIC_MODE;

			break;
		}

		case '<' :   /*ansi/vt52 mode (ignored)*/
		{

			break;
		}


		case 'c' :  /*reset to initial state*/
		{

			break;
		}

		case 'H' :  /*horizontal tab set*/
		{

			break;
		}

		case '`' :  /*disable manual input*/
		{

			break;
		}

		case 'b' :  /*enable manual input*/
		{

			break;
		}

		case '#' :
		{
			escape_read_more(1);

			if(escape_host_data[0] == '!')  /*report syntax mode*/
			{
				read_escape_parameter();

			}

			break;
		}



		case '%' :
		{
			escape_read_more(1);

			if(escape_host_data[0] == '!') /*select operating mode*/
			{
				read_escape_parameter();

			}

			break;
		}


		case '(' :  /*select characrter set G0*/
		{

			break;
		}


		case ')' :  /*select character set G1*/
		{

			break;
		}

	}
}


