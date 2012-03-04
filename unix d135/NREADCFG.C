#include <stdio.h>
#include "mywindows.h"


#define  TRUE 't'
#define  FALSE 'f'
#define  END_OF_DATA 'e'

#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1

#define NO_PFK  0
#define OUR_PFK  1
#define MDC_PFK  2


#define MOUSE_2_BUTTON 1
#define MOUSE_3_BUTTON 2
#define A_JOYSTICK     3
#define A_DATA_TABLET  4


#define A_COLOR 1
#define A_MONOCHROME 2





enum op_codes 
{
	op_end_of_data,op_the_version,op_baud_rate,op_curser_speed,
	op_listing_size,op_screen_update_mode,op_print_listings,
	op_update_timer,op_ems_mode,op_error_path,op_screen_resolution,
	op_have_display_list,op_whose_pfk,op_pfk_port_address,
	op_joystick_delay,op_update_threshold,op_protect_color,
	op_non_protect_color,op_light_color,op_vt100_color,op_use_mouse,
	op_listings_use_cr,op_number_of_mouse_buttons,op_gmark_color,
	op_xor_bar_color,op_pfk_interrupt,op_message_monitor_type,
	op_pointing_device_type
 };



extern int  global_screen_color;
extern struct a_window  my_windows[16];


extern int  pfk_interrupt;
extern char  message_monitor_type;
extern char  pointing_device_type;


extern char  temp_ems_mode;

extern char  vt100_attribute;

/* for EXITING when it CAN'T find work.cfg */
extern int  handle;
extern void  exit_dealloc_pages();
extern void  close_host();
extern void  restore_int9();



extern char  mdc_pfk_in_use;
extern int  mdc_pfk_port;
extern int  initial_joy_count;

extern char  have_display_list;
extern char  ems_mode;
extern int  init_baud_rate;

extern int  curser_ratio;
extern char  rotate_mode;
extern int  list_size;
extern char  print_list;
extern int  time_redisplay;
extern int  init_baud_rate;
extern char  which_high_res;
extern char  path_error_log[];
extern int  rw_threshold;
extern unsigned long  max_wait_time;
extern char  the_version[];


char  printer_listings_use_cr;
char  use_the_mouse;

int  the_protect_color, the_non_protect_color, the_light_color,
	 the_vt100_color;

int  whose_pfk;
int  number_of_mouse_buttons = 3;
int  gmark_color = 0x77;
int  xor_bar_color = 0xaa;


unsigned char  config_data[256];
char  end_config_file = FALSE;


extern FILE  *exe_file;




void  config_read_more(number)
int number;
{
int i,ch;

	for(i = 0 ; i < number ; i++)
	{
		ch = getc(exe_file);
		if(ch == EOF)
		{
			end_config_file = TRUE;
			return;
		}

		config_data[i] = ch;
	}

}


int  make_config_integer(high_byte,low_byte)
int high_byte,low_byte;
{
	return((high_byte << 8) | low_byte);
}



char  parse_config()
{
int i;

	config_read_more(1);

	switch(config_data[0])
	{
		case op_the_version :
		{
			config_read_more(40);
			break;
		}

		case op_baud_rate :
		{
			config_read_more(2);

			init_baud_rate = make_config_integer(config_data[0],
				config_data[1]);

			break;
		}

		case op_curser_speed :
		{
			config_read_more(2);

			curser_ratio = make_config_integer(config_data[0],
				config_data[1]);

			break;
		}

		case op_listing_size :
		{
			config_read_more(1);

			if(config_data[0] == 'n')
				list_size = 0;
			else
				list_size = 1;

			break;
		}

		case op_screen_update_mode :
		{
			config_read_more(1);
			rotate_mode = config_data[0];      /* 'm' or 'a' */

			break;
		}

		case op_print_listings :
		{
			config_read_more(1);
			print_list = config_data[0];     /* 't' or 'f' */

			break;
		}

		case op_update_timer :
		{
			config_read_more(1);
			time_redisplay = config_data[0];   /* 't' or 'f' */

			break;
		}

		case op_ems_mode :
		{
			config_read_more(1);

			if(config_data[0] == 's')
				ems_mode = SOFTWARE_EMS;
			else
				ems_mode = HARDWARE_EMS;

			temp_ems_mode = ems_mode;

			break;
		}

		case op_error_path :
		{
			config_read_more(80);
			config_data[80] = '\0';
			strcpy(path_error_log,config_data);

			break;
		}

		case op_screen_resolution :
		{
			config_read_more(1);

			if((config_data[0] == 5) || (config_data[0] == 6))
				which_high_res = config_data[0] - 2;
			else
				which_high_res = 3;

			break;
		}

		case op_have_display_list :
		{
			config_read_more(1);
			have_display_list = config_data[0];   /* 't' or 'f' */

			break;
		}

		case op_whose_pfk :
		{
			config_read_more(1);

			whose_pfk = config_data[0];

			switch(config_data[0])
			{
				case NO_PFK :
				{
					mdc_pfk_in_use = 'f';
					break;
				}

				case OUR_PFK :
				{
					mdc_pfk_in_use = 'f';
					break;
				}

				case MDC_PFK :
				{
					mdc_pfk_in_use = 't';
					break;
				}
			}


			break;
		}


		case op_pfk_port_address :
		{
			config_read_more(2);
			mdc_pfk_port = make_config_integer(config_data[0],
				config_data[1]);

			break;
		}

		case op_joystick_delay :
		{
			config_read_more(2);
			initial_joy_count = make_config_integer(config_data[0],
				config_data[1]);

			break;
		}

		case op_update_threshold :
		{
			config_read_more(1);
			rw_threshold = config_data[0];

			break;
		}

		case op_protect_color :
		{
			config_read_more(1);
			the_protect_color = config_data[0];

			break;
		}

		case op_non_protect_color :
		{
			config_read_more(1);
			the_non_protect_color = config_data[0];

			break;
		}

		case op_light_color :
		{
			config_read_more(1);
			the_light_color = config_data[0];

			break;
		}

		case op_vt100_color :
		{
			config_read_more(1);
			the_vt100_color = config_data[0];
			vt100_attribute = the_vt100_color;

			break;
		}

		case op_use_mouse :
		{
			config_read_more(1);

/*
			use_the_mouse = config_data[0];    't' or 'f'
 */

			break;
		}

		case op_listings_use_cr :
		{
			config_read_more(1);
			printer_listings_use_cr = config_data[0]; /* 't' 'f' */

			break;
		}

		case op_number_of_mouse_buttons :
		{
			config_read_more(1);
/*
			number_of_mouse_buttons = config_data[0];
*/

			break;
		}

		case op_gmark_color :
		{
			config_read_more(1);
			gmark_color = config_data[0];

			break;
		}

		case op_xor_bar_color :
		{
			config_read_more(1);
			xor_bar_color = config_data[0];

			break;
		}

		case op_pfk_interrupt :
		{
			config_read_more(1);
			pfk_interrupt = config_data[0];

			break;
		}

		case op_message_monitor_type :
		{
			config_read_more(1);
			message_monitor_type = config_data[0];

			break;
		}

		case op_pointing_device_type :
		{
			config_read_more(1);
			pointing_device_type = config_data[0];

			break;
		}

		case op_end_of_data :
		{
			return(END_OF_DATA);
		}


		default :
		{
			printf("\n\nInvalid op code in New Read Config File Processor\n\n");
			return(FALSE);
		}

	}


	return(TRUE);
}




void  set_up_defaults()
{
	printer_listings_use_cr = FALSE;
	use_the_mouse = TRUE;
	the_protect_color = 12;
	the_non_protect_color = 15;
	the_light_color = 14;
	the_vt100_color = 13;

	mdc_pfk_in_use = FALSE;
	mdc_pfk_port = 0x2e8;
	initial_joy_count = 10;
	have_display_list = TRUE;
	temp_ems_mode = ems_mode = SOFTWARE_EMS;
	init_baud_rate = 12;
	curser_ratio = 4;
	rotate_mode = 'a';
	list_size = 0;
	print_list = FALSE;
	time_redisplay = FALSE;
	which_high_res = 3;
	strcpy(path_error_log,"/usr2/james/d135ss/");
	rw_threshold = 0;
	max_wait_time = 700000L;
}


void  new_read_config()
{
int ch;

	set_up_defaults();


/*
	while((parse_config() != END_OF_DATA) && (end_config_file == FALSE))
		;

	for(ch = getc(exe_file) ; ch == op_end_of_data ; ch = getc(exe_file))
		;

	if(ch != EOF)
	{
		global_screen_color = ch;
		fread(&my_windows[0],sizeof(my_windows),1,exe_file);
	}

	fclose(exe_file);
*/
}


