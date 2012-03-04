#include "stdio.h"
#include "graph.h"
#include "dos.h"
#include "string.h"
#include "stdlib.h"


#define TRUE 1
#define FALSE 0

#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1

#define NO_PFK  0
#define OUR_PFK  1
#define MDC_PFK  2



#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3




/* choose.c functions */
extern void far control_window(void);
extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);



/*my C versions of BIOS calls*/
extern void far make_win(int,int,int,int);
extern void far clear_win(int);
extern void far save_win(void);
extern void far restore_win(void);
extern void far goto_row_col(int,int);
extern void far read_curser_pos(void);
extern void far blinking_string(char far *,int,int);
extern void far normal_string(char far *,int,int);
extern void far turn_on_curser(void);
extern void far turn_off_curser(void);



union REGS far my_input_regs,far my_output_regs;
char far char_save_page[25][80],far attr_save_page[25][80];

int far win_x_low,far win_y_low,far win_x_up,far win_y_up;

int far row_curser,far col_curser;
int far selection = 0;
int far previous_switch = 0;
int far last_main_selection = 0;
int far last_baud_selection = 4;
char far a_string[111];
char far another_string[111];


FILE far *config_file, far *test_file;


struct config
{
	int baud_rate,cursor_speed;
	char listing_size,screen_update_mode,
		print_listings,update_timer,ems_mode;
	char error_path[80];
	char screen_resolution;
	char have_display_list;
	char whose_pfk;
	int pfk_port_address;
	int joystick_delay;
	unsigned long wait_time;
	int update_threshold;
	int protect_color,non_protect_color,light_color,vt100_color;
};




struct config far config_info;
int far size_config_file = sizeof(struct config);
char far input_string[80],far temp_string[80];




char far *controllers[] =
{
	"EGA",
	"VGA",
	"Artist10 640 x 480",
	"Artist10 800 x 560",
	"Artist10 1024 x 768",
	"Clipper 1024 x 768",
	"Clipper 1280 x 1024",
	"Metheus 1104",
	"Vectrix Pepe"
};



int far num_controllers = sizeof(controllers) / sizeof(char far *);
int far which_controller = 0;





int far char_is_hex(the_char)
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



int far atox(the_string)
char far the_string[];
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





void far enforce_integer_range(value,bottom_range,top_range)
int far *value,bottom_range,top_range;
{
	if(*value < bottom_range)
		*value = bottom_range;

	if(*value > top_range)
		*value = top_range;
}




void far read_menu_integer(header_string,value,bottom_range,top_range)
char far header_string[];
int far *value;
int bottom_range,top_range;
{
int i,ch;
char the_string[33],temp_string[80];
int row,col;

	make_win(10,3,60,17);

	sprintf(temp_string,"Enter New Value [%d..%d] : ?",bottom_range,
		top_range);

	row = win_y_low + 3;
	col = win_x_low + 1 + strlen(temp_string); 

start_over:
	make_win(10,3,60,17);
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 2);
	printf("%s = %d",header_string,*value);
	goto_row_col(win_y_low + 3,win_x_low + 2);
	printf("%s",temp_string);


	control_window();
	make_win(10,3,60,17);

	ch = getch();

	if(ch == 0)
	{
		ch = getch();

		switch(ch)
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
		if((ch >= '0') && (ch <= '9'))
		{
			goto_row_col(row,col);
			putchar(ch);
			the_string[0] = ch;
		}
		else
			goto start_over;

	}



	i = 1;
	for(ch = getch() ; (ch != 13) /*enter key*/
		 && (ch != 0) ; ch = getch())
	{
		if(ch != 8)
		{
			if((ch >= '0') && (ch <= '9'))
			{
				putchar(ch);
				the_string[i] = ch;
				i++;
			}
		}
		else
		{
			if(i > 0)
			{
				i--;

				goto_row_col(row,col + i);
				putchar(' ');
				goto_row_col(row,col + i);
			}
		}
	}


	the_string[i] = '\0';


	if(ch == 0)
	{	ch = getch();

		switch(ch)
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

		goto start_over;
	}
}







void far read_menu_string(header_string,return_string)
char far header_string[];
char far return_string[];
{
int i,ch;
int row,col;

	make_win(10,3,60,17);

	row = win_y_low + 4;
	col = win_x_low + 7;

start_over:
	make_win(10,3,60,17);
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 2);
	printf("%s",header_string);


	if(return_string[0] != '\0')
	{
		goto_row_col(win_y_low + 2,win_x_low + 3);
		printf("Current Value = %s",return_string);
	}


	control_window();
	make_win(10,3,60,17);
	goto_row_col(win_y_low + 4,col);

	ch = getch();

	if(ch == 0)
	{
		ch = getch();

		switch(ch)
		{
			case 75 : 
			case 77 : 
			case 80 :
			{
				return_string[0] = '\0';
				return;
			}

			default :
				goto start_over;
		}
	}
	else
	{
		return_string[0] = ch;
		putchar(ch);
	}



	i = 1;
	for(ch = getch() ; (ch != 13) /*enter key*/
		 && (ch != 0) ; ch = getch())
	{
		if(ch != 8)
		{
			putchar(ch);
			return_string[i] = ch;
			i++;
		}
		else
		{
			if(i > 0)
			{
				i--;

				goto_row_col(row,col + i);
				putchar(' ');
				goto_row_col(row,col + i);
			}
		}
	}


	return_string[i] = '\0';


	if(ch == 0)
	{	ch = getch();

		switch(ch)
		{
			case 75 : /*reject*/
			{
				goto start_over;
			}

			case 77 : /*entry complete*/
				break;
	
			case 80 : /*terminate*/
			{
				return_string[0] = '\0';
				break;
			}
		}
	}
}






make_default_data()
{
	config_info.baud_rate = 12;
	config_info.cursor_speed = 4;
	config_info.listing_size = 'n';   /*normal size*/
	config_info.screen_update_mode = 'm';    /*manual*/
	config_info.print_listings = FALSE;
	config_info.update_timer = FALSE;
	config_info.ems_mode = 's';   /*software EMS*/
	strcpy(config_info.error_path,"c:\\");  /* c:\ as the path*/
	config_info.screen_resolution = 0;
	config_info.have_display_list = TRUE;
	config_info.whose_pfk = NO_PFK;
	config_info.pfk_port_address = 0x2e8;
	config_info.joystick_delay = 10;
	config_info.wait_time = 700000L;
	config_info.update_threshold = 0;

	config_info.protect_color = 12;
	config_info.non_protect_color = 15;
	config_info.vt100_color = 13;
	config_info.light_color = 14;
}





char far *colors_text[7] =
{
	"Blue",
	"Green",
	"Cyan",
	"Red",
	"Magenta",
	"Yellow",
	"White"
};




void far pick_color_menu(the_variable,description)
int far *the_variable;
char far description[];
{
int selection,current_color;


	if((*the_variable) == 7)
		current_color = 15;
	else
		current_color = *the_variable;


	selection = choose_menu(description,colors_text,7,current_color - 8);

	if(selection > 0)
		(*the_variable) = selection + 8;

}




char far *set_colors_text[4] = 
{
	"Menu Options",
	"Calculator",
	"Lights",
	"VT100"
};





void far  set_text_colors()
{
int selection;

	while(99)
	{
		selection = choose_menu("Text Colors",set_colors_text,4,0);

		switch(selection)
		{
			case 1 :  /* menu options */
			{
				pick_color_menu(&config_info.non_protect_color,
					"Menu Options");
				break;
			}


			case 2 : /*calculator*/
			{
				pick_color_menu(&config_info.protect_color,
					"Calculator");

				if(config_info.protect_color == 15)
					config_info.protect_color = 7;

				break;
			}

			case 3 : /* lights */
			{
				pick_color_menu(&config_info.light_color,
					"Lights");

				if(config_info.light_color == 15)
					config_info.light_color = 7;



				break;
			}

			case 4 : /* VT100 color */
			{
				pick_color_menu(&config_info.vt100_color,
					"VT100");
				break;
			}


			case REJECT :
			case ENTRY_COMPLETE :
			case TERMINATE :
				return;
		}

	}

}





int far the_baud_rate()
{
	switch(config_info.baud_rate)
	{
		case 6 :
			return(19200);

		case 12 :
			return(9600);

		case 24 :
			return(4800);

		case 48 :
			return(2400);

		case 96 :
			return(1200);
	}
}




char far *switches_text[7] =
{
	"Expanded Memory Mode \0                                  ",
	"Listing Size \0                                          ",
	"Printer Control \0                                       ",
	"Screen Update Mode \0                                    ",
	"Screen Update Timer \0                                   ",
	"D90 Display List \0                                      ",
	" \0 "
};




void far switches_menu()
{
int selection;


	while(2001)
	{
		if(config_info.ems_mode == 's')
			sprintf(switches_text[0],
				"Expanded Memory Mode         :  Software");
		else
			sprintf(switches_text[0],
				"Expanded Memory Mode         :  Hardware");


		if(config_info.listing_size == 'n')
			sprintf(switches_text[1],
				"Listing Size                 :  Normal");
		else
			sprintf(switches_text[1],
				"Listing Size                 :  Double");



		if(config_info.print_listings == TRUE)
			sprintf(switches_text[2],
				"Printer Control              :  Enabled");
		else	
			sprintf(switches_text[2],
				"Printer Control              :  Disabled");



		if(config_info.screen_update_mode == 'm')
			sprintf(switches_text[3],
				"Screen Update Mode           :  Manual");
		else
			sprintf(switches_text[3],
				"Screen Update Mode           :  Automatic");
	

		if(config_info.update_timer == TRUE)
			sprintf(switches_text[4],
				"Screen Update Timer          :  Enabled");
		else
			sprintf(switches_text[4],
				"Screen Update Timer          :  Disabled");


		if(config_info.have_display_list == TRUE)
			sprintf(switches_text[5],
				"D90 Display List             :  Enabled");
		else	
			sprintf(switches_text[5],
				"D90 Display List             :  Disabled");


		selection = choose_menu("Switches Menu",switches_text,6,
			previous_switch);


		if(selection > 0)
			previous_switch = selection;


		switch(selection)
		{
			case 1 : /*ems mode*/
			{
				if(config_info.ems_mode == 's')  /*software*/
					config_info.ems_mode = 'h';  /*make hardware*/
				else  
					config_info.ems_mode = 's';  /*make software*/


				break;
			}

			case 2 : /*listing size*/
			{
				if(config_info.listing_size == 'n')  /*normal*/
					config_info.listing_size = 'd';  /*make double*/
				else
					config_info.listing_size = 'n';  /*make normal*/


				break;
			}

			case 3 : /*printer control*/
			{
				if(config_info.print_listings == TRUE)
					config_info.print_listings = FALSE;
				else
					config_info.print_listings = TRUE;


				break;
			}



			case 4 : /*screen update mode*/
			{
				if(config_info.screen_update_mode == 'm')  /*manual*/
					config_info.screen_update_mode = 'a'; 
				else
					config_info.screen_update_mode = 'm';


				break;
			}

			case 5 : /*update timer*/
			{
				if(config_info.update_timer == TRUE)
					config_info.update_timer = FALSE;
				else
					config_info.update_timer = TRUE;
	


				break;
			}


			case 6 : /*d90 display list use*/
			{
				if(config_info.have_display_list == TRUE)
					config_info.have_display_list = FALSE;
				else
					config_info.have_display_list = TRUE;



				break;
			}


			case TERMINATE : 
			case ENTRY_COMPLETE : 
			case REJECT : 
				return;

		}
	
	}

}



char far *baud_rate_text[5] =
{
	"1200",
	"2400",
	"4800",
	"9600",
	"19200"
};



void far change_baud_rate()
{
int selection;


	while(99)
	{ 
		selection = choose_menu("Change Baud Rate",baud_rate_text,5,
			last_baud_selection);

		if(selection > 0)
			last_baud_selection = selection;


		switch(selection)
		{
			case 1 : /*1200*/
			{	
				config_info.baud_rate = 96;
				break;
			}

			case 2 : /*2400*/
			{
				config_info.baud_rate = 48;
				break;
			}

			case 3 : /*4800*/
			{
				config_info.baud_rate = 24;
				break;
			}

			case 4 : /*9600*/
			{
				config_info.baud_rate = 12;
				break;
			}

			case 5 : /*19200*/
			{
				config_info.baud_rate = 6;
				break;
			}


			case ENTRY_COMPLETE : 
			case REJECT : 
			case TERMINATE : 
				return;


		}
	}
}





void far change_cursor_speed()
{
	read_menu_integer("Cursor Speed",&(config_info.cursor_speed),1,50);

}




char far *no_such_path[2] =
{
	"Path Does Not Exist",
	"   "
};



void far error_file_path()
{
int first_char,i,j;


path_again:


	strcpy(input_string,config_info.error_path);
	read_menu_string("Error Logging File Path",input_string);

	if(input_string[0] == '\0')
		return;


/*skip leading blanks and tabs*/
	for(i = 0 ; (input_string[i] == ' ') || (input_string[i] == '\t') ;
		 i++)
			;

/*check for no error log file case*/
	strcpy(temp_string,input_string);  /*copy it (strupr changes its arg)*/

	if(strcmp(strupr(temp_string),"NONE") == 0)
	{
		strcpy(config_info.error_path,temp_string);
		return;
	}


	if((input_string[i] == 'a') || (input_string[i] == 'A') ||
		(input_string[i] == 'b') || (input_string[i] == 'B'))
	{
		if(input_string[i + 1] == ':')
		{
			config_info.error_path[0] = input_string[i];
			config_info.error_path[1] = ':';
			config_info.error_path[2] = '\0';
			return;
		}
	}



/*find end of input string*/
	for(j = i ; input_string[j] != '\0' ; j++)
		;

	if(input_string[j - 1] != '\\')
	{
		input_string[j] = '\\';
		input_string[j + 1] = '\0';
	}



	sprintf(temp_string,"%spathtest",input_string + i);
	test_file = fopen(temp_string,"w");

	if(test_file == NULL)
	{
		error_message(7,no_such_path,1);
	}
	else
	{
		fclose(test_file);
		remove(temp_string);
		strcpy(config_info.error_path,input_string + i);
	}

}



char far *exit_text[2] = 
{
	"Exit And Save Changes",
	"Exit And Ignore Changes"
};




void far exit_code()
{
int exit_mode;



	exit_mode = choose_menu("EXIT TO DOS",exit_text,2,0);


	if(exit_mode == 1)
	{
		fseek(config_file,0L,SEEK_SET);
		fwrite(&config_info,size_config_file,1,config_file);
		fclose(config_file);
	}
	else if(exit_mode == 2)
		fclose(config_file);
	else
		return;



	make_win(0,0,79,24);
	clear_win(NORMAL_VIDEO);
	turn_on_curser();
	goto_row_col(0,0);
	exit(0);
}




char far *pfk_address_text[2] = 
{
	"2E8 (hexadecimal)",
	"User Defined"
};



void far read_pfk_address()
{
int selection;


	while(999)
	{
		sprintf(another_string,"%x",config_info.pfk_port_address);
		sprintf(a_string,"Function Keyboard Base Address = %s",
			strupr(another_string));
			
		selection = choose_menu(a_string,pfk_address_text,2,0);

		switch(selection)
		{
			case 1 :  /* 2e8 */ 
			{
				config_info.pfk_port_address = 0x2e8;
				break;
			}


			case 2 : /* user defined */
			{
				sprintf(a_string,"%x",config_info.pfk_port_address);
				read_menu_string("Function Keyboard Base Address",
					strupr(a_string));

				if(a_string[0] == '\0')
					return;

				config_info.pfk_port_address = atox(a_string);

				break;
			}


			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}





char far *the_pfk_text[3] =
{
	"None ",
	"Without Lights",
	"With Lights"
};



void far get_pfk_info()
{
int current,selection;


	while(9999)
	{
		switch(config_info.whose_pfk)
		{
			case NO_PFK :
			{
				current = 1;
				break;
			}
	
			case OUR_PFK :
			{
				current = 2;
				break;
			}

			case MDC_PFK :
			{
				current = 3;
				break;
			}


			default :
				current = 0;
		}


		selection = choose_menu("Function Keyboard",the_pfk_text,3,
			current);

		switch(selection)
		{
			case 1 : /*No PFK*/
			{
				config_info.whose_pfk = NO_PFK;
				break;
			}
	
			case 2 : /* Our PFK */
			{
				config_info.whose_pfk = OUR_PFK;
				read_pfk_address();
				break;
			}

			case 3 : /* MDC PFK */
			{
				config_info.whose_pfk = MDC_PFK;
				read_pfk_address();
				break;			
			}


			case TERMINATE :
			case ENTRY_COMPLETE :
			case REJECT :
				return;
		}
	}
}





void far controller_board()
{
int selection;

	selection = choose_menu("Graphics Controller",controllers,9,
		config_info.screen_resolution + 1);


	switch(selection)
	{
		case ENTRY_COMPLETE :
		case REJECT :
		case TERMINATE :
			return;


		default :
			config_info.screen_resolution = selection - 1;
	}	
}






char far *main_menu_text[10] = 
{
	"Baud Rate                   :   xxxxx \0                    ",
	"Cursor Speed                :   xxx \0                      ",
	"Update Threshold            :  #  \0                        ",
	"Error Log File Path         :      \0                       ",
	"Joystick Delay              :   xxxxx \0                    ",
	"Function Keyboard           :  xxxxx xxxxxxx                ",
	"Graphics Controller         :  any board  \0                ",
	"Switches    \0                                              ",
	"Text Colors \0                                              ",
	"EXIT TO DOS  \0                                             "
};





void far main_menu()
{
int selection;


	sprintf(main_menu_text[0],
		"Baud Rate           : %d",the_baud_rate());

	if(config_info.cursor_speed >= 1)
		sprintf(main_menu_text[1],
			"Cursor Speed        : %d",
				config_info.cursor_speed);
	else
		sprintf(main_menu_text[1],
			"Cursor Speed        : %f",
				-1.0/config_info.cursor_speed);


	sprintf(main_menu_text[2],
		"Update Threshold    : %d",config_info.update_threshold);

	sprintf(main_menu_text[3],
		"Error Log File Path : %s",
			config_info.error_path);

	sprintf(main_menu_text[4],
		"Joystick Delay      : %d",
			config_info.joystick_delay);



	switch(config_info.whose_pfk)
	{
		case NO_PFK :
		{
			sprintf(main_menu_text[5],
				"Function Keyboard   : None");
			break;
		}

		case OUR_PFK :
		{
			sprintf(main_menu_text[5],
				"Function Keyboard   : Without Lights");
			break;
		}

		case MDC_PFK :
		{
			sprintf(main_menu_text[5],
				"Function Keyboard   : With Lights");
			break;
		}
	}


	sprintf(main_menu_text[6],
		"Graphics Controller : %s",controllers[config_info.
			screen_resolution]);


	selection = choose_menu("Configuration File Editor",main_menu_text,10,
		last_main_selection);


	if(selection > 0)
		last_main_selection = selection;


	switch(selection)
	{
		case 1 : /*Change Baud Rate*/
		{
			change_baud_rate();
			break;
		}

		case 2 : /*change cursor speed*/
		{
			change_cursor_speed();
			break;
		}

		case 3 : /*update threshold*/
		{
			read_menu_integer("Update Threshold",&(config_info.
				update_threshold),0,1);
			break;
		}

		case 4 : /*error log file path*/
		{
			error_file_path();
			break;
		}


		case 5 :  /* joystick delay */
		{
			read_menu_integer("Joystick Delay",
				&(config_info.joystick_delay),3,10000);
			break;
		}


		case 6 :  /* PFK INFO */
		{
			get_pfk_info();
			break;
		}


		case 7 : /*graphic controller board*/
		{
			controller_board();
			break;
		}


		case 8 : /*switches menu*/
		{
			switches_menu();
			break;
		}


		case 9 : /* text colors*/
		{
			set_text_colors();
			break;
		}

		case 10 : /*exit this program*/
		{
			exit_code();
			break;
		}

	}
}








main()
{
	config_file = fopen("work.cfg","r+b");

	if(config_file == NULL)
	{
		config_file = fopen("work.cfg","w+b");
		make_default_data();
		fwrite(&config_info,size_config_file,1,config_file);
		fseek(config_file,0L,SEEK_SET);  /*go to beginning of file*/
	}


	fread(&config_info,size_config_file,1,config_file);


	turn_off_curser();

	make_win(0,0,79,24);
	clear_win(NORMAL_VIDEO);

	make_win(10,3,60,17);


	while(99)
		main_menu();

}
