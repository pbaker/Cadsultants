/*  NMENUS.C  Home menus*/



#include "my_globs.h"
#include "my_utils.h"
#include <string.h>
#include <pw_lib.h>
#include "expanded.h"
#include "mywindows.h"

#define BLACK_SUB0 0

/*cursor limits*/
#define X_MAX 2048
#define X_MIN -2047
#define Y_MAX 2048
#define Y_MIN -2047

#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1
#define RES_1024_768 3
#define RES_1280_1024 4



#define NO_PFK  0
#define OUR_PFK  1
#define MDC_PFK  2



#define MOUSE_2_BUTTON 1
#define MOUSE_3_BUTTON 2
#define A_JOYSTICK     3
#define A_DATA_TABLET  4


#define A_COLOR 1
#define A_MONOCHROME 2


#define VT100_MODE  0
#define MESS_MON  1




struct a_clipping
{
	int low_x,low_y,up_x,up_y;
};


extern struct a_clipping  the_host_clipping[];



extern float  actual_size;

extern int  count1;
extern int  exit_icu_flag;
extern int  vt100_mess_mon_flag;

int  pfk_interrupt = 5;
char  message_monitor_type = A_COLOR;
char  pointing_device_type = MOUSE_3_BUTTON;
extern char  printer_listings_use_cr;


extern struct a_window  my_windows[];


extern void  default_window_parameters();
extern void  scale_up_window_data();



/* digitizer stuff */
extern void  restore_int3();
extern char  digitizer_enabled;
extern char  digitizer_buttons;
extern int  digitizer_x,  digitizer_y;
extern char  use_the_mouse;
extern void  read_input_device();
extern void  close_digitizer();



extern OPCODE  io_masks[];
extern int  global_screen_color;
extern char  customer_license();



extern short  text_monitor_dl[];
extern short  *text_dl_pointer;



extern void  clear_text_monitor_dl();

extern void  change_page_gmark_undraw();

extern char  licensee[];
extern char  serial[];
char  location[80],  contact[80];

extern void  control_window();
extern char  stop_after_matrix_sub;

extern char  errors_happened;
extern void  clear_error_vars();


extern void  r_flag();
extern void  w_flag();
extern void  my_reset();
extern FILE  *com_save;
extern FILE  *com_read;




extern OPCODE unity_viewing_matrix[];
extern int  init_baud_rate;
extern char  which_high_res;
extern char  ems_mode;
extern char  version_str[];
extern int  rw_threshold;
extern void  hpgl_menu();
extern void  print_id();
extern int  number_of_errors;
extern int  atox();
extern void  read_menu_integer();
extern void  read_menu_string();
extern int  whose_pfk;
extern int  mdc_pfk_port;
extern int  initial_joy_count;
extern int  number_of_mouse_buttons;

extern int  the_protect_color, the_non_protect_color, the_light_color,
	 the_vt100_color;
extern int  num_unalloc_pages;


extern int gmark_color;
extern int xor_bar_color;



char  cursor_speed_set();
char  my_listing_control();

char  in_the_home_menu = 'f';
FILE  *listing_output_file = NULL;
char  listing_file_name[111];
char  temp_ems_mode;
OPCODE  my_status;

char  a_string[256];
char  another_string[256];



char  *invalid_file[2] =
{
	"Invalid File Name",
	"  "
};



char  *d135_colors[16] =
{
	"Black",	/* 0 */
	"Grey",		/* 1 */
	"Olive",  	/* 2 */
	"Blue",   	/* 3 */
	"Pink",   	/* 4 */
	"Green",  	/* 5 */
	"Brown",  	/* 6 */
	"Cyan",   	/* 7 */
	"Orange", 	/* 8 */
	"Red",		/* 9 */
	"Purple",	/* 10 */
	"Magenta",	/* 11 */
	"Dark Red",	/* 12 */
	"Yellow",	/* 13 */
	"Aquamarine",	/* 14 */
	"White"		/* 15 */
};



char  *d135_colors_part1[10] =
{
	"Black",	/* 0 */
	"Grey",		/* 1 */
	"Olive",  	/* 2 */
	"Blue",   	/* 3 */
	"Pink",   	/* 4 */
	"Green",  	/* 5 */
	"Brown",  	/* 6 */
	"Cyan",   	/* 7 */
	"                       ",
	"Next Page"
};


char  *d135_colors_part2[10] =
{
	"Orange", 	/* 8 */
	"Red",		/* 9 */
	"Purple",	/* 10 */
	"Magenta",	/* 11 */
	"Dark Red",	/* 12 */
	"Yellow",	/* 13 */
	"Aquamarine",	/* 14 */
	"White",	/* 15 */
	"                       ",
	"Previous Page"
};



int  select_d135_color(the_string,can_disable)
char  the_string[],can_disable;
{
int selection;

	while(99)
	{
colors_part1:
		sprintf(a_string,"%s Color",the_string);

		if(can_disable == 't')
		{
			strcpy(d135_colors_part1[8],"Disable");
		}
		else
			strcpy(d135_colors_part1[8],"\0");


		selection = choose_menu(a_string,d135_colors_part1,10,0);

		switch(selection)
		{
			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return(-1);

			case 9 :
			{
				if(can_disable == 't')
					return(-2);

				break;
			}

			case 10 :
				goto colors_part2;

			default :
				return(selection - 1);
		}
	}


	while(99)
	{
colors_part2:

		sprintf(a_string,"%s Color",the_string);

		if(can_disable == 't')
		{
			strcpy(d135_colors_part2[8],"Disable");
		}
		else
			strcpy(d135_colors_part2[8],"\0");



		selection = choose_menu(a_string,d135_colors_part2,10,0);

		switch(selection)
		{
			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return(-1);

			case 9 :
			{
				if(can_disable == 't')
					return(-2);

				break;
			}


			case 10 :
				goto colors_part1;

			default :
				return(selection - 1 + 8);
		}
	}
}





#ifdef fffffffffffff
char  *mouse_joystick_text[4] =
{
	"Pointing Device         :  Mouse/Joystick\0       ",
	"Mouse/Joystick Delay    :  10     \0              ",
	"Crosshair Speed         :  4   \0                 ",
	"Number Of Mouse Buttons :  3   \0                 "
};


char  mouse_joystick()
{
int selection;

	while(99)
	{
		if(use_the_mouse == 't')
		{
			sprintf(mouse_joystick_text[0],
				"Pointing Device         :  Mouse/Joystick");
		}
		else
			sprintf(mouse_joystick_text[0],
				"Pointing Device         :  Data Tablet");

		sprintf(mouse_joystick_text[1],"Mouse/Joystick Delay    :  %d",
			initial_joy_count);

		if(curser_ratio > 0)
		{
			sprintf(mouse_joystick_text[2],
				"Crosshair Speed         :  %d",curser_ratio);
		}
		else if(curser_ratio == 0)
		{
			sprintf(mouse_joystick_text[2],
				"Crosshair Speed         :  1");
		}
		else
			sprintf(mouse_joystick_text[2],
				"Crosshair Speed         :  %0.3f",1.0/
					(-curser_ratio));

		sprintf(mouse_joystick_text[3],"Number Of Mouse Buttons :  %d",
			number_of_mouse_buttons);


		selection = choose_menu("Pointing Device",mouse_joystick_text,
			4,0);

		switch(selection)
		{
			case 1 : /* Pointing Device mouse OR digitizer */
			{
				if(use_the_mouse == 't')
					use_the_mouse = 'f';
				else
					use_the_mouse = 't';

				break;
			}


			case 2 : /* Mouse/Joystick Delay */
			{
				read_menu_integer("Mouse/Joystick Delay",
					&initial_joy_count,5,10000);

				break;
			}


			case 3 : /* Crosshair Speed */
			{
				if(cursor_speed_set() == 'e')
					return('e');

				break;
			}


			case 4 : /* Number Of Mouse Buttons */
			{
				read_menu_integer("Number Of Mouse Buttons",
					&number_of_mouse_buttons,2,3);

				break;
			}


			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}

	}

}
#endif



#ifdef dassssssss
char  *pfk_address_text[2] = 
{
	"2E8 (hexadecimal)",
	"User Defined"
};



void  read_pfk_address()
{
int selection;


	while(999)
	{
		sprintf(another_string,"%x",mdc_pfk_port);
		sprintf(a_string,"Function Keyboard Base Address = %s",
			strupr(another_string));
			
		selection = choose_menu(a_string,pfk_address_text,2,0);

		switch(selection)
		{
			case 1 :  /* 2e8 */ 
			{
				mdc_pfk_port = 0x2e8;
				break;
			}


			case 2 : /* user defined */
			{
				sprintf(a_string,"%x",mdc_pfk_port);
				read_menu_string("Function Keyboard Base Address",
					strupr(a_string));

				if(a_string[0] == '\0')
					return;

				mdc_pfk_port = atox(a_string);

				break;
			}


			case ENTRY_COMPLETE :
			case TERMINATE :
			case REJECT :
				return;
		}
	}
}
#endif



char  *the_pfk_text[3] =
{
	"None ",
	"CADSULTANTS",
	"MDC"
};



char  pfk_settings()
{
int current,selection;


	while(9999)
	{
		switch(whose_pfk)
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


		selection = choose_menu("Function Keyboard Type",the_pfk_text,3,
			current);

		switch(selection)
		{
			case 1 : /*No PFK*/
			{
				whose_pfk = NO_PFK;
				break;
			}
	
			case 2 : /* Our PFK */
			{
				whose_pfk = OUR_PFK;
				break;
			}

			case 3 : /* MDC PFK */
			{
				whose_pfk = MDC_PFK;
				break;			
			}


			case TERMINATE :
				return('e');

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');
		}
	}
}



char  *function_keyboard_text[3] =
{
	"Type    > Cadsultants \0         ",
	"Address > 2e8     \0             ",
	"IRQ     > 5 \0                   "
};


char  function_keyboard()
{
int selection;


	while(99)
	{
		switch(whose_pfk)
		{
			case NO_PFK :
			{
				strcpy(function_keyboard_text[0],
					"Type    > None");
				break;
			}
	
			case OUR_PFK :
			{
				strcpy(function_keyboard_text[0],
					"Type    > CADSULTANTS");
				break;
			}

			case MDC_PFK :
			{
				strcpy(function_keyboard_text[0],
					"Type    > MDC");
				break;
			}
		}

		sprintf(function_keyboard_text[1],"Address > %x",
			mdc_pfk_port);

		sprintf(function_keyboard_text[2],"IRQ     > %d",
			pfk_interrupt);

		selection = choose_menu("Function Keyboard",
			function_keyboard_text,3,0);

		switch(selection)
		{
			case 1 : /* pfk type (none,cadsultants, or mdc) */
			{
				if(pfk_settings() == 'e')
					return('e');

				break;
			}

			case 2 : /* pfk address */
			{
				sprintf(a_string,"%x",
					mdc_pfk_port);
				read_menu_string("Base Address",a_string);

				mdc_pfk_port = atox(a_string);

				if(mdc_pfk_port <= 0)
					mdc_pfk_port = 0x2e8;

				break;
			}

			case 3 : /* irq */
			{
				read_menu_integer("Interrupt Level",
					&pfk_interrupt,1,9);
				break;
			}

			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}
	}

}




#ifdef kldklddkldk
char  *input_devices_text[2] =
{
	"Pointing Device",
	"PFK Parameters"
};


char  the_input_devices()
{
int selection;

	while(99)
	{
		selection = choose_menu("Input Devices",input_devices_text,2,0);

		switch(selection)
		{
			case 1 :
			{
				if(mouse_joystick() == 'e')
					return('e');

				break;
			}

			case 2 :
			{
				if(pfk_settings() == 'e')
					return('e');

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}

	}

}
#endif



char  *colors_text[7] =
{
	"Blue",
	"Green",
	"Cyan",
	"Red",
	"Magenta",
	"Yellow",
	"White"
};




void  pick_color_menu(the_variable,description)
int  *the_variable;
char  description[];
{
int selection,current_color;


	if((*the_variable) == 7)
		current_color = 15;
	else
		current_color = *the_variable;


	selection = choose_menu("Select Color",colors_text,7,current_color - 8);

	if(selection > 0)
		(*the_variable) = selection + 8;

}




char  *set_colors_text[4] = 
{
	"Menu Options > White\0         ",
	"Calculator   > Red \0          ",
	"UG Functions > Yellow\0        ",
	"VT-100       > Magenta\0       "
};





char  set_text_colors()
{
int selection;

	while(99)
	{
		sprintf(set_colors_text[0],"Menu Options > %s",
			colors_text[the_non_protect_color % 8 - 1]);
		sprintf(set_colors_text[1],"Calculator   > %s",
			colors_text[the_protect_color % 8 - 1]);
		sprintf(set_colors_text[2],"UG Functions > %s",
			colors_text[the_light_color % 8 - 1]);
		sprintf(set_colors_text[3],"VT-100       > %s",
			colors_text[the_vt100_color % 8 - 1]);

		selection = choose_menu("Message Monitor Text Colors",
			set_colors_text,4,0);

		switch(selection)
		{
			case 1 :  /* menu options */
			{
				pick_color_menu(&the_non_protect_color,
					"Menu Options");
				break;
			}


			case 2 : /*calculator*/
			{
				pick_color_menu(&the_protect_color,
					"Calculator");

				if(the_protect_color == 15)
					the_protect_color = 7;

				break;
			}

			case 3 : /* lights */
			{
				pick_color_menu(&the_light_color,
					"Lights");

				if(the_light_color == 15)
					the_light_color = 7;



				break;
			}

			case 4 : /* VT100 color */
			{
				pick_color_menu(&the_vt100_color,
					"VT100");
				break;
			}


			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}

	}

}





#ifdef dsdddddddddddd
char  *display_devices_text[4] = 
{
	"Screen Resolution  :    \0                   ",
	"Screen Update Mode :     \0                  ",
	"Listing Control              \0    " ,
	"Message Monitor Colors       \0    "
};



char  display_devices()
{
int selection;

	while(99)
	{
		if(which_high_res == RES_1024_768)
		{
			sprintf(display_devices_text[0],
				"Screen Resolution   :  1024 x 768");
		}
		else
			sprintf(display_devices_text[0],
				"Screen Resolution   :  1280 x 1024");

		if(rotate_mode == 'a')
		{
			sprintf(display_devices_text[1],
				"Screen Update Mode  :  Automatic");
		}
		else
			sprintf(display_devices_text[1],
				"Screen Update Mode  :  Manual");


		selection = choose_menu("Display Devices",display_devices_text,
			4,0);

		switch(selection)
		{
			case 1 : /* screen resolution */
			{
				if(which_high_res == RES_1024_768)
					which_high_res = RES_1280_1024;
				else
					which_high_res = RES_1024_768;

				break;
			}

			case 2 : /* screen update mode */
			{
				if(rotate_mode == 'a')
					rotate_mode = 'm';
				else
					rotate_mode = 'a';

				break;
			}

			case 3 : /* listing control */
			{
				if(my_listing_control() == 'e')
					return('e');

				break;
			}

			case 4 : /* message monitor colors */
			{
				if(set_text_colors() == 'e')
					return('e');

				break;
			}

			case REJECT :
			case ENTRY_COMPLETE :
				return('c');
	
			case TERMINATE :
				return('e');
		}
	}
}
#endif



char  *memory_usage_text[2] =
{
	"Available = xyzK  \0            ",
	"In Use    = abcK  \0            "
};


char  *the_memory_text[2] = 
{
	"EMS Type  :  Software      \0",
	"Show Memory Usage"
};


char  the_memory()
{
int selection,i,num_pages;

	while(99)
	{
		if(temp_ems_mode == SOFTWARE_EMS)
		{
			sprintf(the_memory_text[0],"EMS Type  :  Software");
		}
		else
			sprintf(the_memory_text[0],"EMS Type  :  Hardware");


		selection = choose_menu("Expanded Memory",the_memory_text,2,0);

		switch(selection)
		{
			case 1 :  /* EMS Type */
			{
				if(temp_ems_mode == SOFTWARE_EMS)
					temp_ems_mode = HARDWARE_EMS;
				else
					temp_ems_mode = SOFTWARE_EMS;

				break;
			}

			case 2 : /* show memory usage */
			{
				sprintf(memory_usage_text[0],"Available = %dK",
					num_unalloc_pages*16);

				num_pages = 0;
				for(i = 0 ; i < 256 ; i++)
					if(available_pages[i] != -1)
						num_pages++;

				sprintf(memory_usage_text[1],"In Use    = %dK",
					num_pages*16);

				selection = information_menu("Memory Usage",
					memory_usage_text,2,'t','f');

				if(selection == TERMINATE)
					return('e');

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}

	}

}





void  start_recording()
{
	if(com_save == NULL)
	{
		out_string[0] = '\0';
		read_menu_string("Enter Record File Name",
			out_string);
	
		if(out_string[0] == '\0')
			return;

		com_save = fopen(out_string,"w");
		if(com_save == NULL)
		{
			putchar(7);
			error_message(6,invalid_file,1);
		}
		else
			w_flag(1);

	}

}



void  stop_recording()
{
	if(com_save != NULL)
	{
		fclose(com_save);
		com_save = NULL;
		w_flag(0);
	}

}



void  start_playback()
{
	if(com_read == NULL)
	{
		out_string[0] = '\0';
		read_menu_string("Enter Playback File Name",
			out_string);
	
		if(out_string[0] == '\0')
			return;

		com_read = fopen(out_string,"r");
		if(com_read == NULL)
		{
			putchar(7);
			error_message(6,invalid_file,1);
		}
		else
		{
			r_flag(1);
			my_reset();
		}

	}

}



char  *stop_recording_text[2] =
{
	"Stop Recording",
	"aaa"
};



void  recording_menu()
{
int selection;

	selection = choose_menu("Record/Playback",stop_recording_text,1,1);

	switch(selection)
	{
		case ENTRY_COMPLETE :
		case 1 : /* close save file */
		{
			stop_recording();
			break;
		}

		case REJECT :
		case TERMINATE :
			break;
	}
}




char  *playing_text[3] =
{
	"Record/Playback",
	"\0",
	"   Playback In Progress"
};


void  playing_menu()
{

	error_message(1,playing_text,3);
}



char  *start_com_text[2] =
{
	"Start Recording",
	"Playback"
};



void  start_com_menu()
{
int selection;

	selection = choose_menu("Record/Playback",start_com_text,2,0);

	switch(selection)
	{
		case 1 : /* open save file */
		{
			start_recording();
			break;
		}

		case 2 : /* read saved host file */
		{
			start_playback();
			break;
		}


		case REJECT :
		case ENTRY_COMPLETE :
		case TERMINATE :
			break;
	}

}




void  handle_com_files()
{
	if((com_save == NULL) && (com_read == NULL))
		start_com_menu();
	else if((com_save != NULL) && (com_read == NULL))
		recording_menu();
	else if((com_save == NULL) && (com_read != NULL))
		playing_menu();
	else
	{
		putchar(7); putchar(7);
		printf("\n\nIMPOSSIBLE: Playback & Record at SAME TIME!!!\n\n");
		printf("Press Enter\n\n");
		getchar();
	}
}

char  temp_error_log_path[256];

char  *error_messages_text[4] =
{
	"List On Screen",
	"Print On LPT:",
	"Clear Error File",
	"Change Path"
};

char  error_messages()
{
int selection;

	while(99)
	{
		selection = choose_menu("Error Messages",error_messages_text,
			4,0);

		switch(selection)
		{
			case 1 : /*display it*/
			{
				sprintf(diag_string,"%serrorlog.dat",path_error_log);
				error_log_ptr = fopen(diag_string,"a");
				fprintf(error_log_ptr,"\nFile Listed:\n\n");
				fclose(error_log_ptr);

				rdstat(&my_status);
				if((my_status & 0x300) != 0)
					display(1);
	

				clear_display_page();

				sprintf(diag_string,"cat %serrorlog.dat | more",
					path_error_log);
				system(diag_string);
				printf("\n\nPress Enter To Continue\n");
				getchar();

				clear_error_vars();
				clear_display_page();


				if((my_status & 0x300) != 0)
					display(0);

				break;
			}

			case 2 : /*print it*/
			{
				sprintf(diag_string,"%serrorlog.dat",path_error_log);
				error_log_ptr = fopen(diag_string,"a");
				fprintf(error_log_ptr,"\nFile Printed:\n\n");
				fclose(error_log_ptr);

				rdstat(&my_status);
				if((my_status & 0x300) != 0)
					display(1);

				clear_display_page();

				sprintf(diag_string,"print %serrorlog.dat",
					path_error_log);
				system(diag_string);
				printf("\n\nPress Enter To Continue\n");
				getchar();

				clear_error_vars();
				clear_display_page();

				if((my_status & 0x300) != 0)
					display(0);

				break;
			}

			case 3 : /*clear it*/
			{
				number_of_errors = 0;

				sprintf(diag_string,"%serrorlog.dat",path_error_log);
				error_log_ptr = fopen(diag_string,"w");
				fprintf(error_log_ptr,"File Cleared:\n\n");
				fclose(error_log_ptr);

				clear_error_vars();

				break;
			}

			case 4 : /* error log path */
			{
				strcpy(temp_error_log_path,path_error_log);
				read_menu_string("Change Error File Path",
					temp_error_log_path);
				if(temp_error_log_path[0] != '\0')
					strcpy(path_error_log,temp_error_log_path);

				break;
			}
		
			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');	
		}
	}
}



char  *error_log_text[3] =
{
	"Record/Playback",
	"Error Messages",
	"Timer : on/off\0      "
};


char  error_log_status()
{
int selection;

	while(99)
	{
		if(time_redisplay == 't')
		{
			strcpy(error_log_text[2],"Timer : On");
		}
		else
			strcpy(error_log_text[2],"Timer : Off");


		selection = choose_menu("Diagnostics",error_log_text,3,0);

		switch(selection)
		{
			case 1 : /* record/playback */
			{
				handle_com_files();
				break;
			}

			case 2 : /* error messages */
			{
				if(error_messages() == 'e')
					return('e');

				break;
			}

			case 3 : /* timer */
			{
				if(time_redisplay == 't')
					time_redisplay = 'f';
				else
					time_redisplay = 't';

				break;
			}


			case ENTRY_COMPLETE :
			case REJECT :
				return('c');


			case TERMINATE :
				return('e');
		}
	}
}



#ifdef ggggggggggg
char  *error_log_text[6] = 
{
	"List Errors",
	"Print Errors",
	"Clear Errors",
	"Record/Playback",
	"Error Log Path            \0                  ",
	"Update Timer                 \0               "
};




char  error_log_status()
{
int selection;

error_log_again:



	sprintf(error_log_text[4],"Error Log Path : %s",path_error_log);

	if(time_redisplay == 't')
	{
		sprintf(error_log_text[5],"Update Timer   : Enabled");
	}
	else
		sprintf(error_log_text[5],"Update Timer   : Disabled");


	selection = choose_menu("Diagnostics",error_log_text,6,0);



	switch(selection)
	{
		case 1 : /*display it*/
		{
			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"a");
			fprintf(error_log_ptr,"\nFile Listed:\n\n");
			fclose(error_log_ptr);


			clear_display_page();
			goto_row_col(0,0);
			sprintf(diag_string,"cat %serrorlog.dat | more",
				path_error_log);
			system(diag_string);
			printf("\n\nPress Enter To Continue\n");
			getchar();


			clear_error_vars();
			clear_display_page();

			goto error_log_again;

			break;
		}

		case 2 : /*print it*/
		{
			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"a");
			fprintf(error_log_ptr,"\nFile Printed:\n\n");
			fclose(error_log_ptr);


			clear_display_page();
			goto_row_col(0,0);
			sprintf(diag_string,"print %serrorlog.dat",
				path_error_log);
			system(diag_string);
			printf("\n\nPress Enter To Continue\n");
			getchar();

			clear_error_vars();
			clear_display_page();

			goto error_log_again;

			break;
		}

		case 3 : /*clear it*/
		{
			number_of_errors = 0;

			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"w");
			fprintf(error_log_ptr,"File Cleared:\n\n");
			fclose(error_log_ptr);

			clear_error_vars();

			goto error_log_again;

			break;
		}


		case 4 : /*com files*/
		{
			handle_com_files();
			goto error_log_again;
			break;
		}

		case 5 : /* error log path */
		{
			strcpy(temp_error_log_path,path_error_log);
			read_menu_string("Enter Error Log Path",
				temp_error_log_path);
			if(temp_error_log_path[0] != '\0')
				strcpy(path_error_log,temp_error_log_path);
			goto error_log_again;

			break;
		}


		case 6 : /* update timer */
		{
			if(time_redisplay == 't')
				time_redisplay = 'f';
			else
				time_redisplay = 't';
			goto error_log_again;

			break;
		}



		case REJECT : 
		case ENTRY_COMPLETE : 
			return('c');

		case TERMINATE : 
			return('e');

	}


	return('c');
}
#endif




char  *config_text[7] = 
{
	"     PC WorkWindow HYPERFORMANCE 3D \0                       ",
	"\0                                                           ",
	"           Version : 1.35 date     \0                        ",
	"     Serial Number : 123454                                  ",
	"          Licensee :  anyone at all                          ",
	"          Location :               \0                        ",
	"          Contact  :                 \0                      "
};



char  emulation_status()
{
int ch,i;
int display_size;
int selection;




	sprintf(config_text[2],"           Version : %s",version_str);
	sprintf(config_text[3],"     Serial Number : %s",serial);
	sprintf(config_text[4],"          Licensee : %s",licensee);
	sprintf(config_text[5],"          Location : %s",location);
	sprintf(config_text[6],"          Contact  : %s",contact);


	selection = information_menu("Software Identification",config_text,7,
		't','f');


	if(selection == TERMINATE)
		return('e');
	else
		return('c');

}






#ifdef ddddddddddddddddd
char  *cursor_text[9] =
{
	"Press Mouse/Joystick Buttons  \0                          " ,
	" \0                                                       " ,
	"  1  Increase Cursor Speed  \0                            " ,
	"  2  Decrease Cursor Speed  \0                            " ,
	"  (F1 To Center Cursor)   \0                              " ,
	"\0                                                        " ,
	"\0                                                        " ,
	"\0                                                        " ,
	"\0                                                        " 
};




char  cursor_speed_set()
{
int ch,save_current_color;
int save_digi_enable;
int the_buttons;


	the_buttons = new_x = new_y = old_x = old_y = 0;


	my_set_viewport(-2048,-2048,2048,2048,which_page);
	pushvw3();    /*save current 3d viewing matrix*/
	vwmatx3(&unity_viewing_matrix[0]);  /*set 3d viewing matrix to unity*/


	save_current_color = current_color;
	my_color(7);

	sprintf(cursor_text[4],"    (F1 To Center Cursor)    ");
	information_menu("Crosshair Control:",cursor_text,9,'f','f');


	draw_mode(3);
	my_line_xy(2*(new_x - 50),2*new_y,2*(new_x + 50),2*new_y);
	my_line_xy(2*new_x,2*(new_y - 50),2*new_x,2*(new_y + 50));


	while((the_buttons & 2) == 0)
	{
		if(keyboard_status() != -1)  /*keyboard was touched*/
		{
			ch = getchar();
			if(ch == 0)    /*its an extended code*/
			{
				ch = getchar();
				if(ch == 0x3b)
				{
					sprintf(cursor_text[4],
						"   Cursor Will Be Reset To 0 0 Upon Return");
					information_menu("Crosshair Control:",
						cursor_text,9,'f','t');


					mouse_x = 0;
					mouse_y = 0;
					digitizer_x = 0;
					digitizer_y = 0;

				}
				else if((ch == 75) || (ch == 77))
				{    /*undraw cursor speed curser*/
					my_line_xy(2*(new_x - 50),2*new_y,
						2*(new_x + 50),2*new_y);
					my_line_xy(2*new_x,2*(new_y - 50),
						2*new_x,2*(new_y + 50));

					draw_mode(0);
					my_color(save_current_color);

					popvw3();   /*restore saved matrix*/

					stop_after_matrix_sub = 't';
					display_a_seg(which_cseg);
					stop_after_matrix_sub = 'f';

					return('c');
				}
				else if(ch == 80)  /*terminate*/
				{    /*undraw cursor speed curser*/
					my_line_xy(2*(new_x - 50),2*new_y,
						2*(new_x + 50),2*new_y);
					my_line_xy(2*new_x,2*(new_y - 50),
						2*new_x,2*(new_y + 50));


					draw_mode(0);
					my_color(save_current_color);

					popvw3();    /*restore saved matrix*/

					stop_after_matrix_sub = 't';
					display_a_seg(which_cseg);
					stop_after_matrix_sub = 'f';

					return('e');
				}
			}
		}



		if(curser_ratio > 0)
			sprintf(cursor_text[7],
				"Cursor Speed = %d            ",curser_ratio);
		else if(curser_ratio < 0)
			sprintf(cursor_text[7],
				"Cursor Speed = %f",-1.0/curser_ratio);
		else
			sprintf(cursor_text[7],
				"Cursor Speed = 1             ");

		information_menu("Crosshair Control:",cursor_text,9,'f','t');



		save_digi_enable = digitizer_enabled;

		if(use_the_mouse == 't')
			digitizer_enabled = 'f';
		else
			digitizer_enabled = 't';


		read_input_device();


		digitizer_enabled = save_digi_enable;

		if(use_the_mouse == 't')
			the_buttons = pen_press;
		else
			the_buttons = digitizer_buttons;



		if((new_x != old_x) || (new_y != old_y))
		{    /*undraw curser speed curser*/
			my_line_xy(2*(old_x - 50),2*old_y,2*(old_x + 50),
				2*old_y);
			my_line_xy(2*old_x,2*(old_y - 50),2*old_x,
				2*(old_y + 50));


		     /* redraw it  at new coords */
			my_line_xy(2*(new_x - 50),2*new_y,2*(new_x + 50),
				2*new_y);
			my_line_xy(2*new_x,2*(new_y - 50),2*new_x,
				2*(new_y + 50));



			old_x = new_x;
			old_y = new_y;
		}


		if(use_the_mouse == 't')
		{
			if((the_buttons & 1) != 0)
			{
				curser_ratio++;
				if(curser_ratio == 0)
					curser_ratio = 1;
			}
			else if((the_buttons & 4) != 0)
			{
				curser_ratio--;
				if(curser_ratio == 0)
					curser_ratio = -1;
			}
		}
		else
		{
			if((the_buttons & 8) != 0)
			{
				curser_ratio++;
				if(curser_ratio == 0)
					curser_ratio = 1;
			}
			else if((the_buttons & 4) != 0)
			{
				curser_ratio--;
				if(curser_ratio == 0)
					curser_ratio = -1;
			}

		}

	
	}   /*end while*/


    /*undraw cursor speed curser*/
	my_line_xy(2*(new_x - 50),2*new_y,2*(new_x + 50),2*new_y);
	my_line_xy(2*new_x,2*(new_y - 50),2*new_x,2*(new_y + 50));


	draw_mode(0);
	my_color(save_current_color);


	popvw3();   /*restore saved 3d viewing matrix & window/viewport*/

	stop_after_matrix_sub = 't';
	display_a_seg(which_cseg);
	stop_after_matrix_sub = 'f';

	return('c');

}
#endif





#ifdef llllllllllllllll
char  *list_file_closed[2] =
{
	"Listing File :  jewkewkfffeflfl                    ",
	"Has Been Closed"
};



void  handle_listing_file()
{
	if(listing_output_file == NULL)
	{
		listing_file_name[0] = '\0';
		read_menu_string("Enter Listing File Name",listing_file_name);
		if(listing_file_name[0] == '\0')
			return;

		listing_output_file = fopen(listing_file_name,"w");
		if(listing_output_file == NULL)
		{
			putchar(7);
			error_message(6,invalid_file,1);
		}

	}
	else
	{
		sprintf(list_file_closed[0],"Listing File  :  %s",
			listing_file_name);
		fclose(listing_output_file);
		listing_output_file = NULL;

		information_menu("Listing Control",list_file_closed,2,'t','f');
	}

}



char  *listing_text[3] = 
{
	"Listing Size \0                         ",
	"Echo To Printer  \0                     ",
	"Echo To File \0                                         "
};



char  my_listing_control()
{
int selection;

	while(99)
	{
		if(list_size == 0)
			sprintf(listing_text[0],
				"Listing Size        :  Normal");
		else
			sprintf(listing_text[0],
				"Listing Size        :  Double");


		if(print_list == 't')
			sprintf(listing_text[1],
				"Echo To Printer     :  Enabled");
		else
			sprintf(listing_text[1],
				"Echo To Printer     :  Disabled");


		if(listing_output_file == NULL)
			sprintf(listing_text[2],
				"Echo To File        :  Disabled");
		else
			sprintf(listing_text[2],
				"Echo To File        :  %s",listing_file_name);




		selection = choose_menu("Listing Control",listing_text,3,0);


		switch(selection)
		{
			case 1 : /*set size of listing characters*/
			{
			/* undraw listing & clear listing buffer */
				pw_dl_init(text_dl_pointer,0,NULL,0);
				dlend();
				pw_dl_init(NULL,0,NULL,0);

			        my_set_viewport(-2048,-2048,2048,2048,
					which_page);
				my_size_chars(list_size);

				draw_mode(3);
				dlrun(&text_monitor_dl[0]);
				draw_mode(0);

				clear_text_monitor_dl();


				if(list_size == 0)
					list_size = 1;
				else
					list_size = 0;

				break;
			}

			case 2 : /*send/don't send listings to the printer*/
			{
				if(print_list == 't')
					print_list = 'f';
				else
					print_list = 't';

				break;
			}

			case 3 : /* listing file */
			{
				handle_listing_file();
				break;
			}


			case TERMINATE :
				return('e');


			case ENTRY_COMPLETE :
			case REJECT :
				return('c');
		}

	}

}
#endif


char  *file_already_open[2] =
{
	"Listing File Already Open",
	"   "
};


char  *listing_control_text[4] =
{
	"Screen Only",
	"Screen & File",
	"Screen & Printer",
	"Screen, File & Printer"
};

int  listing_control_selection = 1;


char  my_listing_control()
{
int selection;

	while(99)
	{
		selection = choose_menu("Listing Control",listing_control_text,
			4,listing_control_selection);

		switch(selection)
		{
			case 1 : /* Screen Only */
			{
				print_list = 'f';
				if(listing_output_file != NULL)
				{
					fclose(listing_output_file);
					listing_output_file = NULL;
				}

				listing_control_selection = 1;

				break;
			}

			case 2 : /* Screen & File */
			{
				if(listing_output_file == NULL)
				{
					listing_file_name[0] = '\0';
					read_menu_string(
						"Enter Listing File Name",
						listing_file_name);

					if(listing_file_name[0] == '\0')
						break;

					listing_output_file = fopen(
						listing_file_name,"w");

					if(listing_output_file == NULL)
					{
						error_message(6,invalid_file,1);
						break;
					}
					else
					{
						listing_control_selection = 2;
						print_list = 'f';
					}
				}
				else
					error_message(8,file_already_open,1);

				break;
			}


			case 3 : /* Screen & Printer */
			{
				print_list = 't';

				if(listing_output_file != NULL)
				{
					fclose(listing_output_file);
					listing_output_file = NULL;
				}

				listing_control_selection = 3;

				break;
			}


			case 4 : /* Screen, File & Printer */
			{
				if(listing_output_file == NULL)
				{
					listing_file_name[0] = '\0';
					read_menu_string(
						"Enter Listing File Name",
						listing_file_name);

					if(listing_file_name[0] == '\0')
						break;

					listing_output_file = fopen(
						listing_file_name,"w");

					if(listing_output_file == NULL)
					{
						error_message(6,invalid_file,1);
						break;
					}
					else
					{
						listing_control_selection = 4;
						print_list = 't';
					}
				}
				else
					error_message(8,file_already_open,1);


				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}



char  *dos_commands_text[2] =
{
	"Yes (Type Exit To Return)",
	"No "
};



char  do_dos_commands()
{
int selection;


	selection = choose_menu("Run DOS Shell",dos_commands_text,2,0);

	switch(selection)
	{
		case 1 : 
			break;

		case 2 : 
		case ENTRY_COMPLETE :
		case REJECT :
			return('c');

		case TERMINATE :
			return('e');
	}

	rdstat(&my_status);
	if((my_status & 0x300) != 0)
		display(1);

	clear_display_page();

	system("command.com");

	printf("\n\nPress Enter To Continue\n");
	getchar();

	clear_display_page();



	if((my_status & 0x300) != 0)
		display(0);
}





char  *current_baud_rate[2] =
{
	"Baud Rate >  12344\0       ",
	"    ",
};


char  *the_baud_rates[] = 
{
	"1200",
	"2400",
	"4800",
	"9600",
	"19200"
};



char  enter_baud_rate()
{
int row,selection,baud_rate;


	switch(init_baud_rate)
	{
		case 96 :
		{
			baud_rate = 1200;
			break;
		}

		case 48 :
		{
			baud_rate = 2400;
			break;
		}

		case 24 :
		{
			baud_rate = 4800;
			break;
		}

		case 12 :
		{
			baud_rate = 9600;
			break;
		}

		case 6 :
		{
			baud_rate = 19200;
			break;
		}
	}


	sprintf(current_baud_rate[0],"Baud Rate > %d",baud_rate);
	selection = choose_menu("Communications",current_baud_rate,1,0);

	switch(selection)
	{
		case 1 : 
			break;

		case ENTRY_COMPLETE :
		case REJECT :
			return('c');

		case TERMINATE :
			return('e');
	}

	

baud_again:


	switch(init_baud_rate)
	{
		case 96 :
		{
			row = 1;
			break;
		}

		case 48 :
		{
			row = 2;
			break;
		}

		case 24 :
		{
			row = 3;
			break;
		}

		case 12 :
		{
			row = 4;
			break;
		}

		case 6 :
		{
			row = 5;
			break;
		}
	}



	selection = choose_menu("Select Baud Rate",the_baud_rates,5,row);

	switch(selection)
	{
		case 1 :  /*1200*/
		{
			init_baud_rate = 96;
			open_host(96);
			goto baud_again;
		}

		case 2 :  /*2400*/
		{
			init_baud_rate = 48;
			open_host(48);
			goto baud_again;
		}

		case 3 :  /*4800*/
		{
			init_baud_rate = 24;
			open_host(24);
			goto baud_again;
		}

		case 4 :  /*9600*/
		{
			init_baud_rate = 12;
			open_host(12);
			goto baud_again;
		}

		case 5 :  /*19200*/
		{
			init_baud_rate = 6;
			open_host(6);
			goto baud_again;
		}

		case REJECT : 
		case ENTRY_COMPLETE : 
			return('c');

		case TERMINATE : 
			return('e');


		default:
			goto baud_again;

	}
}





char  *cant_find_exe_text[3] = 
{
	"Can't Find D135.exe For Saving",
	"Enviroment. CD Back To Its ",
	"Directory First."
};


char  *save_environment_text[2] =
{
	"Yes",
	"No"
};


char  *environment_saved[2] =
{
	"Environment Has",
	"Been Saved"
};


char  save_environment()
{
int selection;


	selection = choose_menu("Save Enviroment",save_environment_text,2,0);

	switch(selection)
	{
		case 1 :  /* yes */
		{
			if(write_config() == 'f')
			{
				information_menu("Save Environment",
					cant_find_exe_text,3,'t','f');
			}
			else
				information_menu("   ",environment_saved,
					2,'t','f');

			break;
		}

		case 2 :  /* no */
			break;

		case ENTRY_COMPLETE :
		case REJECT :
			return('c');

		case TERMINATE :
			return('e');
	}
}



char  *the_config_text[2] =
{
	"Software Identification",
	"Save Environment"
};



char  the_configuration()
{
int selection;

	while(99)
	{
		selection = choose_menu("Configuration",the_config_text,2,0);

		switch(selection)
		{
			case 1 : /* software identification */
			{
				if(emulation_status() == 'e')
					return('e');

				break;
			}

			case 2 : /* save environment */
			{
				if(save_environment() == 'e')
					return('e');

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}




char  *graphic_controller_text[2] =
{
	"Resolution > 1280 x 1024 \0         ",
	"    "
};



char  *select_resolution_text[2] =
{
	"1024 x 768",
	"1280 x 1024"
};



char  graphic_controller()
{
int selection,menu_row;

	while(99)
	{
		if(which_high_res == RES_1024_768)
		{
			strcpy(graphic_controller_text[0],
				"Resolution > 1024 x 768");
		}
		else
			strcpy(graphic_controller_text[0],
				"Resolution > 1280 x 1024");

		selection = choose_menu("Graphic Controller Pixelworks Clipper",
			graphic_controller_text,1,0);

		switch(selection)
		{
			case 1 :
				break;

			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}


		if(which_high_res == RES_1024_768)
			menu_row = 1;
		else
			menu_row = 2;

		selection = choose_menu("Select Resolution",
			select_resolution_text,2,menu_row);

		switch(selection)
		{
			case 1 : /* 1024 x 768 */
			{
				which_high_res = RES_1024_768;
				break;
			}

			case 2 : /* 1280 x 1024 */
			{
				which_high_res = RES_1280_1024;
				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}


char  *device_type_text[4] =
{
	"2-Button Mouse",
	"3-Button Mouse",
	"Joystick",
	"Data Tablet"
};


char  the_device_type()
{
int selection;

	while(99)
	{
		selection = choose_menu("Device Type",device_type_text,4,
			pointing_device_type);

		switch(selection)
		{
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			{
				pointing_device_type = selection;
				break;
			}

			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}


char  *pointing_device_text[3] =
{
	"Device Type > mouse \0                   ",
	"Delay       > 2001 \0            ",
	"Ratio       > 4 \0               "
};


char  pointing_device()
{
int selection;

	while(99)
	{
		switch(pointing_device_type)
		{
			case MOUSE_2_BUTTON :
			case MOUSE_3_BUTTON :
			{
				strcpy(pointing_device_text[0],
					"Device Type > Mouse");
				break;
			}

			case A_JOYSTICK :
			{
				strcpy(pointing_device_text[0],
					"Device Type > Joystick");
				break;
			}

			case A_DATA_TABLET :
			{
				strcpy(pointing_device_text[0],
					"Device Type > Data Tablet");
			}
		}


		sprintf(pointing_device_text[1],"Delay       > %d",
			initial_joy_count);

		sprintf(pointing_device_text[2],"Ratio       > %d",
			curser_ratio);

		selection = choose_menu("Pointing Device",pointing_device_text,
			3,0);

		switch(selection)
		{
			case 1 : /* device type */
			{
				if(the_device_type() == 'e')
					return('e');

				break;
			}

			case 2 : /* delay */
			{
				read_menu_integer("Enter Delay",
					&initial_joy_count,5,10000);
				break;
			}

			case 3 : /* ratio */
			{
				read_menu_integer("Enter Ratio",&curser_ratio,
					1,100);
				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}




char  *message_monitor_text[2] =
{
	"Type : color     ",
	"Text Colors"
};

char  menu_message_monitor()
{
int selection;

	while(99)
	{
		if(message_monitor_type == A_COLOR)
		{
			strcpy(message_monitor_text[0],"Type : Color");
		}
		else
			strcpy(message_monitor_text[0],"Type : Mono");

		selection = choose_menu("Message Monitor",
			message_monitor_text,2,0);

		switch(selection)
		{
			case 1 : /* mess mon type */
			{
				if(message_monitor_type == A_COLOR)
				{
					message_monitor_type = A_MONOCHROME;

					the_protect_color = 7;
					the_non_protect_color = 15;
					the_light_color = 7;
					the_vt100_color = 15;
				}
				else
				{
					message_monitor_type = A_COLOR;

					the_protect_color = 12;
					the_non_protect_color = 15;
					the_light_color = 14;
					the_vt100_color = 13;
				}

				break;
			}

			case 2 : /* mess mon colors */
			{
				if(message_monitor_type == A_MONOCHROME)
					break;

				if(set_text_colors() == 'e')
					return('e');

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}


char  *printer_linefeed_text[2] =
{
	"Line Terminator : LF \0        ",
	"   "
};


char  the_printer_linefeed()
{
int selection;

	while(99)
	{
		if(printer_listings_use_cr == 't')
		{
			strcpy(printer_linefeed_text[0],
				"Line Terminator : LF/CR");
		}
		else
			strcpy(printer_linefeed_text[0],
				"Line Terminator : LF");

		selection = choose_menu("Printer",printer_linefeed_text,1,0);

		switch(selection)
		{
			case 1 : /* toggle */
			{
				if(printer_listings_use_cr == 't')
					printer_listings_use_cr = 'f';
				else
					printer_listings_use_cr = 't';

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}



char  *hardware_setup_text[5] =
{
	"Graphic Controller",
	"Pointing Device",
/*
	"Message Monitor",
*/
	"Printer",
	"Function Keyboard"
};


char  hardware_setup()
{
int selection;

	while(99)
	{
		selection = choose_menu("Hardware Setup",hardware_setup_text,
			4,0);

		switch(selection)
		{
			case 1 : /* graphic controller */
			{
				if(graphic_controller() == 'e')
					return('e');

				break;
			}

			case 2 : /* pointing device */
			{
				if(pointing_device() == 'e')
					return('e');

				break;
			}

#ifdef rrrrrrrrrr
			case 3 : /* message monitor */
			{
				if(menu_message_monitor() == 'e')
					return('e');

				break;
			}
#endif

			case 3 : /* printer */
			{
				if(the_printer_linefeed() == 'e')
					return('e');

				break;
			}

			case 4 : /* function keyboard */
			{
				if(function_keyboard() == 'e')
					return('e');

				break;
			}

			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}



char  *pick_color_text[2] =
{
	"Arrow    :                   ",
	"Text Bar :                   "
};


char  pick_cursor_colors()
{
int selection,temp;

	while(99)
	{
		sprintf(pick_color_text[0],"Arrow     :  %s",
			d135_colors[gmark_color & 0x0f]);

		sprintf(pick_color_text[1],"Text Bar  :  %s",
			d135_colors[xor_bar_color & 0x0f]);


		selection = choose_menu("Cursor Colors",pick_color_text,2,0);

		switch(selection)
		{
			case 1 : /* gmark color */
			{
				temp = select_d135_color("Arrow",'f');
				if(temp == -1)
					break;

				gmark_color = (temp << 4) | temp;

				break;
			}

			case 2 : /* xor bar color */
			{
				temp = select_d135_color("Text Bar",'f');
				if(temp == -1)
					break;

				xor_bar_color = (temp << 4) | temp;

				break;
			}

			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}




void  update_matrix_subs_clipping()
{
int subroutine_num,segment_number,i;


	if(vt100_mess_mon_flag == MESS_MON)
	{
		restore_window_blt(HOME_MENU);

		if(which_high_res == RES_1024_768)
		{
			viewport(0,1023,0,767);
		}
		else
			viewport(0,1279,0,1023);

		wrmask(0xff);

		flood((short)((global_screen_color << 4) | 
			global_screen_color));

		wrmask(io_masks[which_page]);


		in_the_home_menu = 'f';
		
		if(is_window_array_empty(MAIN_MENU)
		      == 'f')
			display_pop_up_window(MAIN_MENU);

		if(is_window_array_empty(LIGHTS)
		      == 'f')
			display_pop_up_window(LIGHTS);
	
		if(is_window_array_empty(CALCULATOR)
		      == 'f')
			display_pop_up_window(CALCULATOR);

		if(is_window_array_empty(PART_INFO)
		      == 'f')
			display_pop_up_window(PART_INFO);

		in_the_home_menu = 't';


/* update clipping in all subs that have clipping */
		for(subroutine_num = 0 ; subroutine_num < NUMBER_OF_SUBS ;
			subroutine_num++)
		{
			for(i = 0 ; sub_host_to_pix_ptrs[sub_nums[
			    subroutine_num]][i] != NULL ; i++)
			/* if subroutine entry == the window() command */
				if(*sub_host_to_pix_ptrs[sub_nums[
					subroutine_num]][i] == 0xb3)
				{
					pw_dl_init(sub_host_to_pix_ptrs[
						sub_nums[subroutine_num]][i],0,
						NULL,0);

					my_set_viewport(the_host_clipping[
						sub_nums[subroutine_num]].low_x,
						the_host_clipping[sub_nums[
							subroutine_num]].low_y,
						the_host_clipping[sub_nums[
							subroutine_num]].up_x,
						the_host_clipping[sub_nums[
							subroutine_num]].up_y,
						0);

					pw_dl_init(NULL,0,NULL,0);

					break;
				}

		}


/* update viewport in each segments header */
		for(segment_number = 1 ; (segment_number < NUMBER_OF_SEGS) &&
			(segment_nums[segment_number] != -1) ; segment_number++)
		{
			save_map_handle_page = 't';
			map_handle_page(start_segment_ptrs[segment_number].
				logical_page_num,0,handle);
			pw_dl_init(physical_pages_pointer,0,NULL,0);

			my_set_viewport(-2048,-2048,2048,2048,0);

			pw_dl_init(NULL,0,NULL,0);
		}

	/* copy last segments physical page back to ems page array */
		save_map_handle_page = 't';
		map_handle_page(current_logical_page,0,handle);


		ins_matrix_flag = 't';
		megatek_update();
		ins_matrix_flag = 'f';

		save_window_blt(HOME_MENU);
	}
}




char  *graphics_window_text[4] =
{
	"Size   :           ",
	"Color  :           ",
	"Row    :           ",
	"Column :           "
};



char  graphics_window_parameters()
{
int selection,temp;

	while(99)
	{
		temp = my_windows[GRAPHICS_WINDOW].full_size.background;

		sprintf(graphics_window_text[0],"Size   : %d",my_windows[
			GRAPHICS_WINDOW].full_size.screen_width);
		sprintf(graphics_window_text[1],"Color  : %s",d135_colors[temp]);
		sprintf(graphics_window_text[2],"Row    : %d",my_windows[
			GRAPHICS_WINDOW].full_size.screen_row);
		sprintf(graphics_window_text[3],"Column : %d",my_windows[
			GRAPHICS_WINDOW].full_size.screen_col);

		selection = choose_menu("Graphics Window",graphics_window_text,
			4,0);

		switch(selection)
		{
			case 1 : /* size */
			{
				read_menu_integer("Graphics Window Size",
					&(my_windows[GRAPHICS_WINDOW].full_size.
						screen_width),100,1023);

				actual_size = (float)my_windows[
					GRAPHICS_WINDOW].full_size.screen_width;

				break;
			}

			case 2 : /* color */
			{
				temp = select_d135_color("Graphic Window",'f');
				if(temp == -1)
					break;

				my_windows[GRAPHICS_WINDOW].full_size.
					background = temp;
	
				pw_dl_init(&(sub_struct_ptr ->
					our_versions_of_subs[
					BLACK_SUB0][0]),0,NULL,0);
				color((temp << 4) | temp);
				ret();
				pw_dl_init(NULL,0,NULL,0);

				break;
			}

			case 3 : /* row */
			{
				read_menu_integer("Graphics Window Row",
					&(my_windows[GRAPHICS_WINDOW].full_size.
						screen_row),0,1000);

				break;
			}

			case 4 : /* column */
			{
				read_menu_integer("Graphics Window Column",
					&(my_windows[GRAPHICS_WINDOW].full_size.
						screen_col),0,1000);

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
			{
				update_matrix_subs_clipping();
				change_page_gmark_undraw();
				return('c');
			}

			case TERMINATE :
			{
				update_matrix_subs_clipping();
				change_page_gmark_undraw();
				return('e');
			}
		}
	}
}




char  *window_parameters_text[7] =
{
	"Options",
	"PFK Functions",
	"Calculator",
	"Part Info",
	"Graphics",
	"VT100",
	"Home Menu",
};



char  *retracted_expanded_text[2] =
{
	"Expanded Position",
	"Retracted Position"
};


char  *window_fields_text[9] =
{
	"Text Size        :                ",
	"Left Column      :                ",
	"Width            :                ",
	"Top Row          :                ",
	"Height           :                ",
	"Foreground Color :                ",
	"Background Color :                ",
	"Shadow Color     :                ",
	"Border Color     :                "
};



char  enter_expanded_pos(which_one)
int which_one;
{
int selection,temp;

	while(99)
	{
		sprintf(a_string,"%s Window Expanded Position",
			window_parameters_text[which_one]);

		sprintf(window_fields_text[0],"Text Size        : %d",
			my_windows[which_one].full_size.text_size);
		sprintf(window_fields_text[1],"Left Column      : %d",
			my_windows[which_one].full_size.screen_col);
		sprintf(window_fields_text[2],"Width            : %d",
			my_windows[which_one].full_size.screen_width);
		sprintf(window_fields_text[3],"Top Row          : %d",
			my_windows[which_one].full_size.screen_row);
		sprintf(window_fields_text[4],"Height           : %d",
			my_windows[which_one].full_size.screen_height);
		sprintf(window_fields_text[5],"Foreground Color : %s",
			d135_colors[my_windows[which_one].full_size.foreground]);
		sprintf(window_fields_text[6],"Background Color : %s",
			d135_colors[my_windows[which_one].full_size.background]);

		if(my_windows[which_one].full_size.shadow >= 0)
		{
			sprintf(window_fields_text[7],"Shadow Color     : %s",
				d135_colors[my_windows[which_one].full_size.shadow]);
		}
		else
			strcpy(window_fields_text[7],"Shadow Color     : Disabled");

		if(my_windows[which_one].full_size.border >= 0)
		{
			sprintf(window_fields_text[8],"Border Color     : %s",
				d135_colors[my_windows[which_one].full_size.border]);
		}
		else
			strcpy(window_fields_text[8],"Border Color     : Disabled");

		selection = choose_menu(a_string,window_fields_text,9,0);

		switch(selection)
		{
			case 1 : /* text size */
			{
				read_menu_integer("Text Size",&(my_windows[
					which_one].full_size.text_size),3,30);

			my_windows[which_one].full_size.scroll_height =
				my_windows[which_one].full_size.screen_height/
				(my_windows[which_one].full_size.text_size)
					*4/5;

				break;
			}

			case 2 : /* left column */
			{
				read_menu_integer("Left Column",&(my_windows[
					which_one].full_size.screen_col),0,1000);
				break;
			}

			case 3 : /* width */
			{
				read_menu_integer("Width",&(my_windows[
					which_one].full_size.screen_width),20,
						1000);
				break;
			}

			case 4 : /* top row */
			{
				read_menu_integer("Top Row",&(my_windows[
					which_one].full_size.screen_row),10,1023);
				break;
			}

			case 5 : /* height */
			{
				read_menu_integer("Height",&(my_windows[
					which_one].full_size.screen_height),20,
					1000);

			my_windows[which_one].full_size.scroll_height =
				my_windows[which_one].full_size.screen_height/
				(my_windows[which_one].full_size.text_size)
					*4/5;

				break;
			}

			case 6 : /* foreground */
			{
				temp = select_d135_color("Foreground",'f');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.foreground =
					temp;

				break;
			}

			case 7 : /* background */
			{
				temp = select_d135_color("Background",'f');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.background =
					temp;

				break;
			}

			case 8 : /* shadow */
			{
				temp = select_d135_color("Shadow",'t');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.shadow = temp;

				break;
			}

			case 9 : /* border */
			{
				temp = select_d135_color("Border",'t');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.border = temp;

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');

		}
	}
}



char  enter_retracted_pos(which_one)
int which_one;
{
int selection,temp;

	while(99)
	{
		sprintf(a_string,"%s Window Retracted Position",
			window_parameters_text[which_one]);

		sprintf(window_fields_text[0],"Text Size        : %d",
			my_windows[which_one].retracted.text_size);
		sprintf(window_fields_text[1],"Left Column      : %d",
			my_windows[which_one].retracted.screen_col);
		sprintf(window_fields_text[2],"Width            : %d",
			my_windows[which_one].retracted.screen_width);
		sprintf(window_fields_text[3],"Top Row          : %d",
			my_windows[which_one].retracted.screen_row);
		sprintf(window_fields_text[4],"Height           : %d",
			my_windows[which_one].retracted.screen_height);
		sprintf(window_fields_text[5],"Foreground Color : %s",
			d135_colors[my_windows[which_one].retracted.foreground]);
		sprintf(window_fields_text[6],"Background Color : %s",
			d135_colors[my_windows[which_one].retracted.background]);

		if(my_windows[which_one].retracted.shadow >= 0)
		{
			sprintf(window_fields_text[7],"Shadow Color     : %s",
				d135_colors[my_windows[which_one].retracted.shadow]);
		}
		else
			strcpy(window_fields_text[7],"Shadow Color     : Disabled");

		if(my_windows[which_one].retracted.border >= 0)
		{
			sprintf(window_fields_text[8],"Border Color     : %s",
				d135_colors[my_windows[which_one].retracted.border]);
		}
		else
			strcpy(window_fields_text[8],"Border Color     : Disabled");

		selection = choose_menu(a_string,window_fields_text,9,0);

		switch(selection)
		{
			case 1 : /* text size */
			{
				read_menu_integer("Text Size",&(my_windows[
					which_one].retracted.text_size),3,30);

			my_windows[which_one].retracted.scroll_height =
				my_windows[which_one].retracted.screen_height/
				(my_windows[which_one].retracted.text_size)
					*4/5;

				break;
			}

			case 2 : /* left column */
			{
				read_menu_integer("Left Column",&(my_windows[
					which_one].retracted.screen_col),0,1000);
				break;
			}

			case 3 : /* width */
			{
				read_menu_integer("Width",&(my_windows[
					which_one].retracted.screen_width),20,
						1000);
				break;
			}

			case 4 : /* top row */
			{
				read_menu_integer("Top Row",&(my_windows[
					which_one].retracted.screen_row),-20,1023);
				break;
			}

			case 5 : /* height */
			{
				read_menu_integer("Height",&(my_windows[
					which_one].retracted.screen_height),20,
					1000);

			my_windows[which_one].retracted.scroll_height =
				my_windows[which_one].retracted.screen_height/
				(my_windows[which_one].retracted.text_size)
					*4/5;

				break;
			}

			case 6 : /* foreground */
			{
				temp = select_d135_color("Foreground",'f');

				if(temp == -1)
					break;

				my_windows[which_one].retracted.foreground =
					temp;

				break;
			}

			case 7 : /* background */
			{
				temp = select_d135_color("Background",'f');

				if(temp == -1)
					break;

				my_windows[which_one].retracted.background =
					temp;

				break;
			}

			case 8 : /* shadow */
			{
				temp = select_d135_color("Shadow",'t');

				if(temp == -1)
					break;

				my_windows[which_one].retracted.shadow = temp;

				break;
			}

			case 9 : /* border */
			{
				temp = select_d135_color("Border",'t');

				if(temp == -1)
					break;

				my_windows[which_one].retracted.border = temp;

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');

		}
	}
}


char  enter_retractable_window(which_one)
int which_one;
{
int selection;

	while(99)
	{
		sprintf(a_string,"%s Window",window_parameters_text[which_one]);
		selection = choose_menu(a_string,retracted_expanded_text,2,0);

		switch(selection)
		{
			case 1 : /* expanded pos */
			{
				if(enter_expanded_pos(which_one) == 'e')
					return('e');

				break;
			}

			case 2 : /* retracted pos */
			{
				if(enter_retracted_pos(which_one) == 'e')
					return('e');

				break;
			}

			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}



char  enter_window_parameters(which_one)
int which_one;
{
int selection,temp;

	while(99)
	{
		sprintf(a_string,"%s Window",
			window_parameters_text[which_one]);

		sprintf(window_fields_text[0],"Text Size        : %d",
			my_windows[which_one].full_size.text_size);
		sprintf(window_fields_text[1],"Left Column      : %d",
			my_windows[which_one].full_size.screen_col);
		sprintf(window_fields_text[2],"Width            : %d",
			my_windows[which_one].full_size.screen_width);
		sprintf(window_fields_text[3],"Top Row          : %d",
			my_windows[which_one].full_size.screen_row);
		sprintf(window_fields_text[4],"Height           : %d",
			my_windows[which_one].full_size.screen_height);
		sprintf(window_fields_text[5],"Foreground Color : %s",
			d135_colors[my_windows[which_one].full_size.foreground]);
		sprintf(window_fields_text[6],"Background Color : %s",
			d135_colors[my_windows[which_one].full_size.background]);

		if(my_windows[which_one].full_size.shadow >= 0)
		{
			sprintf(window_fields_text[7],"Shadow Color     : %s",
				d135_colors[my_windows[which_one].full_size.shadow]);
		}
		else
			strcpy(window_fields_text[7],"Shadow Color     : Disabled");

		if(my_windows[which_one].full_size.border >= 0)
		{
			sprintf(window_fields_text[8],"Border Color     : %s",
				d135_colors[my_windows[which_one].full_size.border]);
		}
		else
			strcpy(window_fields_text[8],"Border Color     : Disabled");

		selection = choose_menu(a_string,window_fields_text,9,0);

		switch(selection)
		{
			case 1 : /* text size */
			{
				read_menu_integer("Text Size",&(my_windows[
					which_one].full_size.text_size),3,30);

		if(which_one == VT100_WINDOW)
			my_windows[which_one].full_size.scroll_height =
				my_windows[which_one].full_size.screen_height/
				(my_windows[which_one].full_size.text_size)
					*2/5;
		else
			my_windows[which_one].full_size.scroll_height =
				my_windows[which_one].full_size.screen_height/
				(my_windows[which_one].full_size.text_size)
					*4/5;


				break;
			}

			case 2 : /* left column */
			{
				read_menu_integer("Left Column",&(my_windows[
					which_one].full_size.screen_col),0,1023);
				break;
			}

			case 3 : /* width */
			{
				read_menu_integer("Width",&(my_windows[
					which_one].full_size.screen_width),20,
						1000);
				break;
			}

			case 4 : /* top row */
			{
				read_menu_integer("Top Row",&(my_windows[
					which_one].full_size.screen_row),10,1023);
				break;
			}

			case 5 : /* height */
			{
				read_menu_integer("Height",&(my_windows[
					which_one].full_size.screen_height),20,
					1000);

		if(which_one == VT100_WINDOW)
			my_windows[which_one].full_size.scroll_height =
				my_windows[which_one].full_size.screen_height/
				(my_windows[which_one].full_size.text_size)
					*2/5;
		else
			my_windows[which_one].full_size.scroll_height =
				my_windows[which_one].full_size.screen_height/
				(my_windows[which_one].full_size.text_size)
					*4/5;


				break;
			}

			case 6 : /* foreground */
			{
				temp = select_d135_color("Foreground",'f');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.foreground =
					temp;

				break;
			}

			case 7 : /* background */
			{
				temp = select_d135_color("Background",'f');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.background =
					temp;

				break;
			}

			case 8 : /* shadow */
			{
				temp = select_d135_color("Shadow",'t');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.shadow = temp;

				break;
			}

			case 9 : /* border */
			{
				temp = select_d135_color("Border",'t');

				if(temp == -1)
					break;

				my_windows[which_one].full_size.border = temp;

				break;
			}


			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');

		}
	}
}



char  *restore_defaults_text[2] =
{
	"Yes",
	"No"
};


char  *windows_restored[3] =
{
	"The Windows Have Been",
	"Restored To Their ",
	"Default Attributes"
};


char  restore_the_defaults()
{
int selection,temp;

	selection = choose_menu("Restore Window Defaults?",
		restore_defaults_text,2,0);

	switch(selection)
	{
		case 1 :
		{
			default_window_parameters();
			if(which_high_res == RES_1280_1024)
				scale_up_window_data();


			temp = my_windows[GRAPHICS_WINDOW].full_size.background;
	
			pw_dl_init(&(sub_struct_ptr ->
				our_versions_of_subs[BLACK_SUB0][0]),0,NULL,0);
			color((temp << 4) | temp);
			ret();
			pw_dl_init(NULL,0,NULL,0);

			actual_size = (float)my_windows[GRAPHICS_WINDOW].
				full_size.screen_width;

			update_matrix_subs_clipping();

			information_menu("  ",windows_restored,3,'t','f');

			change_page_gmark_undraw();

			break;
		}

		case 2 :
			return('c');

		case ENTRY_COMPLETE :
		case REJECT :
			return('c');

		case TERMINATE :
			return('e');
	}
}



char  *the_window_parameters_text[9] =
{
	"Options",
	"PFK Functions",
	"Calculator",
	"Part Info",
	"Graphics",
	"VT100",
	"Home Menu",
	"Global Screen Color",
	"Restore Window Defaults"
};




char  window_parameters()
{
int selection,temp;

	while(99)
	{
		selection = choose_menu("Select Window",
			the_window_parameters_text,9,0);

		switch(selection)
		{
			case ENTRY_COMPLETE :
			case REJECT :
				return('c');

			case TERMINATE :
				return('e');

			case 1 : /* main menu options */
			{
				if(enter_retractable_window(MAIN_MENU) == 'e')
					return('e');

				break;
			}

			case 4 : /* part info */
			{
				if(enter_retractable_window(PART_INFO) == 'e')
					return('e');

				break;
			}

			case 5 :
			{
				if(graphics_window_parameters() == 'e')
					return('e');

				break;
			}


			case 8 :
			{
				temp = select_d135_color("Global Screen Color",
					'f');

				if(temp == -1)
					break;

				global_screen_color = temp;
				update_matrix_subs_clipping();
				change_page_gmark_undraw();

				break;
			}

			case 9 : 
			{
				if(restore_the_defaults() == 'e')
					return('e');

				break;
			}

			default :
			{
				if(enter_window_parameters(selection - 1) == 'e')
					return('e');
			}
		}
	}
}



char  *display_control_text[5] =
{
	"Auto Update  : on/off   \0     ",
	"Text Size    : small/large \0      ",
	"Listing Ctrl > some combo \0                            ",
	"Cursor Colors",
	"Window Parameters"
};


char  display_control()
{
int selection;

	while(99)
	{
		if(rotate_mode == 'a')
		{
			strcpy(display_control_text[0],"Auto Update  : On");
		}
		else
			strcpy(display_control_text[0],"Auto Update  : Off");

		if(list_size == 0)
		{
			strcpy(display_control_text[1],"Text Size    : Small");
		}
		else
			strcpy(display_control_text[1],"Text Size    : Large");

		switch(listing_control_selection)
		{
			case 1 : 
			{
				strcpy(display_control_text[2],
					"Listing Ctrl > Screen Only");

				break;
			}

			case 2 : 
			{
				strcpy(display_control_text[2],
					"Listing Ctrl > Screen & File");

				break;
			}

			case 3 : 
			{
				strcpy(display_control_text[2],
					"Listing Ctrl > Screen & Printer");

				break;
			}

			case 4 : 
			{
				strcpy(display_control_text[2],
					"Listing Ctrl > Screen, File & Printer");

				break;
			}


		}


		selection = choose_menu("Display Control",display_control_text,
			5,0);

		switch(selection)
		{
			case 1 :
			{
				if(rotate_mode == 'a')
					rotate_mode = 'm';
				else
					rotate_mode = 'a';

				break;
			}

			case 2 :
			{
			/* undraw listing & clear listing buffer */
				pw_dl_init(text_dl_pointer,0,NULL,0);
				dlend();
				pw_dl_init(NULL,0,NULL,0);

			        my_set_viewport(-2048,-2048,2048,2048,
					which_page);
				my_size_chars(list_size);

				draw_mode(3);
				dlrun(&text_monitor_dl[0]);
				draw_mode(0);

				clear_text_monitor_dl();


				if(list_size == 0)
					list_size = 1;
				else
					list_size = 0;

				break;
			}

			case 3 :
			{
				if(my_listing_control() == 'e')
					return('e');

				break;
			}

			case 4 : /* pick cursor colors */
			{
				if(pick_cursor_colors() == 'e')
					return('e');

				break;
			}

			case 5 :
			{
				if(window_parameters() == 'e')
					return('e');

				break;
			}


			case REJECT :
			case ENTRY_COMPLETE :
				return('c');

			case TERMINATE :
				return('e');
		}
	}
}




char  *home_menu_text[] =
{
	"Configuration",
	"Communications",
	"Hardware Setup",
	"Diagnostics",
	"Memory",
	"Display Control",
	"Hardcopy",
	"DOS Shell",
	"\0",
	"EXIT TO DOS"
};


char  name_plot_file[100];


void  menu()
{
int selection,i,subroutine_num,sub_end,sub_ptr,baud_rate,auto_manual,
	list_print,redis_time;
unsigned long id,displacement,temp,num_blocks;
int seg_start,ch;
char save_temps;


/* if graphics segment is open then do not display home menu */
	if(is_a_seg_open == 't')
		return;

/* if host buffer not empty then do not display home menu 
	if(count1 != 0)
		return;
*/

/* 7/26/90     retract the two movable windows */
	if(my_windows[MAIN_MENU].blt_status != RESTORED_GRAPHICS)
	{		
		restore_window_blt(PART_INFO);
		my_windows[PART_INFO].is_retracted = 't';

		restore_window_blt(MAIN_MENU);
		my_windows[MAIN_MENU].is_retracted = 't';
		display_pop_up_window(MAIN_MENU);
	}	


	in_the_home_menu = 't';
	change_page_gmark_undraw();
	save_window_blt(HOME_MENU);


	selection = 0;

	while(selection != 0x44)
	{
display_main_menu:

		selection = choose_menu("System Control Functions",
			home_menu_text,10,0);


		switch(selection)
		{
			case 1 : /*status information*/
			{
				if(the_configuration() == 'e')
					goto exit_main_menu;

				break;
			}


			case 2 : /*Enter baud rate*/
			{
				if(enter_baud_rate() == 'e')
					goto exit_main_menu;


				break;
			}


			case 3 : /* input devices */
			{
				if(hardware_setup() == 'e')
					goto exit_main_menu;

				break;
			}

#ifdef rlrlrrlrlrl
			case 4 : /* display_devices */
			{
				if(display_devices() == 'e')
					goto exit_main_menu;

				break;
			}
#endif
			case 4 : /* diagnostics */
			{
				if(error_log_status() == 'e')
					goto exit_main_menu;

				break;
			}


			case 5 : /* memory */
			{
				if(the_memory() == 'e')
					goto exit_main_menu;

				break;
			}

			case 6 : /* display control */
			{
				if(display_control() == 'e')
					goto exit_main_menu;

				break;
			}


			case 7 :  /*HPGL Output*/
			{
				hpgl_menu();

				break;
			}


			case 8 :  /* dos commands */
			{
				if(do_dos_commands() == 'e')
					goto exit_main_menu;

				break;
			}


			case 10 : /*exit this program*/
			{
				exit_icu_flag = 't';
				goto exit_main_menu;
			}



			case REJECT :
			case ENTRY_COMPLETE :
			case TERMINATE :
			case 0x77 :  /* ctrl home */
			case 0x47 :  /*return to caller*/
			{
		exit_main_menu:

				in_the_home_menu = 'f';
				restore_window_blt(HOME_MENU);
				change_page_gmark_undraw();

				return;
			}

		}   /*end switch*/

	}   /*end while*/

}


