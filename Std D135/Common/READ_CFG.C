#include "stdio.h"



#define TRUE 1
#define FALSE 0
#define OUTPUT_TRUE 't'
#define OUTPUT_FALSE 'f'


#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1



#define NO_PFK  0
#define OUR_PFK  1
#define MDC_PFK  2



extern char far vt100_attribute;




/* for EXITING when it CAN'T find work.cfg */
extern int far handle;
extern void far exit_dealloc_pages(int);



extern void far close_host();
extern void far restore_int9();




extern char far mdc_pfk_in_use;
extern int far mdc_pfk_port;
extern int far initial_joy_count;

extern char far have_display_list;
extern char far ems_mode;
extern int far init_baud_rate;

extern int far curser_ratio;
extern char far rotate_mode;
extern int far list_size;
extern char far print_list;
extern int far time_redisplay;
extern int far init_baud_rate;
extern char far which_high_res;
extern char far path_error_log[];
extern int far rw_threshold;

extern unsigned long far max_wait_time;


FILE far *config_file_ptr;



int far the_protect_color,far the_non_protect_color,far the_light_color,
	far the_vt100_color;



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






void far read_config_file()
{
	config_file_ptr = fopen("work.cfg","r+b");

	if(config_file_ptr == NULL)
	{
		exit_dealloc_pages(handle);
		printf("Couldn't Open WORK.CFG Configuration File\n");
		close_host();
		restore_int9();
		exit();
	}

	fread(&config_info,size_config_file,1,config_file_ptr);

	fclose(config_file_ptr);


	rw_threshold = config_info.update_threshold;
	init_baud_rate = config_info.baud_rate;
	strcpy(path_error_log,config_info.error_path);
	curser_ratio = config_info.cursor_speed;

	if(config_info.print_listings == TRUE)
		print_list = OUTPUT_TRUE;
	else
		print_list = OUTPUT_FALSE;

	if(config_info.listing_size == 'n')
		list_size = 0;
	else
		list_size = 1;

	rotate_mode = config_info.screen_update_mode;

	if(config_info.update_timer == TRUE)
		time_redisplay = OUTPUT_TRUE;
	else
		time_redisplay = OUTPUT_FALSE;


	if(config_info.ems_mode == 's')
		ems_mode = SOFTWARE_EMS;
	else
		ems_mode = HARDWARE_EMS;


	if((config_info.screen_resolution == 5) || 
	    (config_info.screen_resolution == 6))
		which_high_res = config_info.screen_resolution - 2;
	else
		which_high_res = 3;


	if(config_info.have_display_list == TRUE)
		have_display_list = OUTPUT_TRUE;
	else
		have_display_list = OUTPUT_FALSE;


	max_wait_time = 700000;

	initial_joy_count = config_info.joystick_delay;


	switch(config_info.whose_pfk)
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


	mdc_pfk_port = config_info.pfk_port_address;



	the_protect_color = config_info.protect_color;
	the_non_protect_color = config_info.non_protect_color;
	the_light_color = config_info.light_color;
	the_vt100_color = config_info.vt100_color;


	vt100_attribute = the_vt100_color;
}

