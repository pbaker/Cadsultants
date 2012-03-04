#include "stdio.h"
#include "mywindows.h"


#define NUM_ROWS 4
#define NUM_COLS 77
#define NUM_CHECKS 7
#define MAX_NUM_USED 30

#define STRING_LENGTH 44


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


extern int far global_screen_color;
extern struct a_window far my_windows[16];


extern int far pfk_interrupt;
extern char far message_monitor_type;
extern char far pointing_device_type;


extern unsigned char far encoded_block[NUM_ROWS][NUM_COLS];
extern long far data_pos;
extern FILE far *exe_file;
extern char far exe_file_name[];


extern int far number_of_mouse_buttons;
extern char far vt100_attribute;

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
extern char far the_version[];


extern char far printer_listings_use_cr;
extern char far use_the_mouse;

extern int far the_protect_color,far the_non_protect_color,far the_light_color,
	far the_vt100_color;

extern int far whose_pfk;
extern char far temp_ems_mode;
extern int far gmark_color, far xor_bar_color;




write_out_values()
{
int i;

	putc(op_the_version,exe_file);
	for(i = 0 ; i < 40 ; i++)
		putc(the_version[i],exe_file);


	putc(op_baud_rate,exe_file);
	putc((init_baud_rate >> 8) & 0xff,exe_file);
	putc(init_baud_rate & 0xff,exe_file);

	putc(op_curser_speed,exe_file);
	putc((curser_ratio >> 8) & 0xff,exe_file);
	putc(curser_ratio & 0xff,exe_file);

	putc(op_listing_size,exe_file);
	if(list_size == 0)
	{
		putc('n',exe_file);
	}
	else
		putc('d',exe_file);


	putc(op_screen_update_mode,exe_file);
	putc(rotate_mode,exe_file);

	putc(op_print_listings,exe_file);
	putc(print_list,exe_file);

	putc(op_update_timer,exe_file);
	putc(time_redisplay,exe_file);

	putc(op_ems_mode,exe_file);
	if(temp_ems_mode == SOFTWARE_EMS)
	{
		putc('s',exe_file);
	}
	else
		putc('h',exe_file);

	putc(op_error_path,exe_file);
	for(i = 0 ; i < 80 ; i++)
		putc(path_error_log[i],exe_file);

	putc(op_screen_resolution,exe_file);
	putc(which_high_res + 2,exe_file);

	putc(op_have_display_list,exe_file);
	putc(have_display_list,exe_file);

	putc(op_whose_pfk,exe_file);
	putc(whose_pfk,exe_file);

	putc(op_pfk_port_address,exe_file);
	putc((mdc_pfk_port >> 8) & 0xff,exe_file);
	putc(mdc_pfk_port & 0xff,exe_file);

	putc(op_joystick_delay,exe_file);
	putc((initial_joy_count >> 8) & 0xff,exe_file);
	putc(initial_joy_count & 0xff,exe_file);

	putc(op_update_threshold,exe_file);
	putc(rw_threshold,exe_file);

	putc(op_protect_color,exe_file);
	putc(the_protect_color,exe_file);

	putc(op_non_protect_color,exe_file);
	putc(the_non_protect_color,exe_file);

	putc(op_vt100_color,exe_file);
	putc(the_vt100_color,exe_file);

	putc(op_light_color,exe_file);
	putc(the_light_color,exe_file);

/*	putc(op_use_mouse,exe_file);
	putc(use_the_mouse,exe_file);
*/
	putc(op_listings_use_cr,exe_file);
	putc(printer_listings_use_cr,exe_file);

/*	putc(op_number_of_mouse_buttons,exe_file);
	putc(number_of_mouse_buttons,exe_file);
*/
	putc(op_gmark_color,exe_file);
	putc(gmark_color,exe_file);

	putc(op_xor_bar_color,exe_file);
	putc(xor_bar_color,exe_file);

	putc(op_pfk_interrupt,exe_file);
	putc(pfk_interrupt,exe_file);

	putc(op_message_monitor_type,exe_file);
	putc(message_monitor_type,exe_file);

	putc(op_pointing_device_type,exe_file);
	putc(pointing_device_type,exe_file);


	putc(op_end_of_data,exe_file);
	putc(op_end_of_data,exe_file);
	putc(op_end_of_data,exe_file);
	putc(op_end_of_data,exe_file);


	putc(global_screen_color,exe_file);
	fwrite(&my_windows[0],sizeof(my_windows),1,exe_file);
}




char far write_config()
{
	exe_file = fopen(exe_file_name,"r+b");

	if(exe_file == NULL)
		return(FALSE);

	fseek(exe_file,data_pos + (long)sizeof(encoded_block),SEEK_SET);

	write_out_values();

	fclose(exe_file);

	return(TRUE);
}

