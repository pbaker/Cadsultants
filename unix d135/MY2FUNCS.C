/*   MY2FUNCS.C     contains main()  & most globel variable declarations */


/*October 12, 1988 added a clear_display_page() to the exit d1etc part of
menu()*/

/*October 13, 1988 use conditional compilation to decide if diagnostic
stuff should be compiled at all (change my_globs.h each time too)
(Use HAVE_DIAGS in the #ifdefs)*/

/*October 14, 1988 Made baud rate and drive letter be set by init_artist10()
by reading them from the artist10.set file.
Made switch to artist10/16 set which_page to 0 and call scrola(0,0)*/

/*October 17, 1988 10:50 added long far seg_starts[#segs] and
long far num_words_in_segs = 2496L; */

/*October 18, 1988 10:08 added int far list_size = 0 variable and menu option
to set it*/

/*October 18, 1988 11:11 added char far print_list = 'f' and menu option to
set it*/

/*October 20, 1988 added code to fill in segment 0's inside-the-file header*/

/*October 31, 1988 added char far board_string[50]; to hold which artist board
you're using & board test code to set board_string or else exit() if its
an invalid board*/

/*October 31, 1988 Made menu() use function keys for all input*/

/*November 1, 1988 Made menu() use bios calls in "bios.c" file*/

/*November 2, 1988 added a call to turn off the curser at the start of menu()
 and a call to turn on the curser when menu() returns */


/*November 11, 1988 Added calls to my_pckb() to waiting loops in read_more()*/

/*November 15, 1988 Made sub7 10 times bigger*/

/*November 15, 1988 Replaced artlib calls with generic graphics board calls*/

/*November 15, 1988 took out my_line3() and my_char() and put them into
art10brd*/

/*November 16, 1988 took out artist board test code and put it into art10brd*/

/*November 17, 1988 Worked on read_more() to mask ch with 0x7f in its ifs
and when it copies ch into megatek_data[] & made read_host_buffer() an int*/


/*November 18, 1988 added extern of void far close_board() &
 call it when exiting d1etc.exe*/

/*December 13,1988 added  char far quick_redisplay = 'f';*/

/*January 30, 1989 added FILE far *error_log_ptr & other error logging stuff*/

/*February 7, 1989 updated all my menus for arrow keys as input & removed
my menu erroneous input messages (replaced them with a blink of the display)*/

/*February 8, 1989 added "cursor speed control" to home menu*/

/*February 9, 1989 added call to close_host() when exiting to DOS*/

/*February 10, 1989 rearranged Home menu(put switches into their own menu)*/

/*February 14, 1989 Added FILE far *test_results & Home Menu Option to
test the host's response to del key & ASCII characters*/

/*March 24, 1989 added variables for non-interpreted subroutines*/

/*March 28, 1989 added struct, array of it, and code to use 
change-something-command recording headers for each segment*/

/*March 28, 1989 added time-out code to read_more() so it will return if
nothing comes in for some number of seconds (defined in READ_MORE_TIME_OUT
constant)*/

/*March 29, 1989 paul made a new version of comio.obj (comio329.obj), that
has read_host_buffer() call check_com_status() (in this file), to see if
there was a communication problem and call error_logger() to report it*/

/*March 30, 1989 commented out calls to time() in read_more()*/




/*
   March 28, 1991   removed those delay loops because keyboard was getting
	lost  
*/


/*
#define HAVE_DIAGS
*/



#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "my_utils.h"
#include "expanded.h"
#include <pw_lib.h>
#include "mywindows.h"


#define READ_MORE_TIME_OUT 1L
#define PI 3.141592654
#define GRAPHICS 1
#define TEXT 0
#define SUB_NUM_ELS 50
#define NULLCODE (char)100
#define NUMBER_OF_SEGS 30
#define SUB7_NUM_ELS 500
#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1
#define SVWPT_PAGE_1  4
#define NUMBER_OF_SUBS 30

#include "my_subs.h"

/*cursor limits*/
#define X_MAX 2048
#define X_MIN -2047
#define Y_MAX 2048
#define Y_MIN -2047

#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1
#define RES_1024_768 3
#define RES_1280_1024 4

#define PAGE1_SUB1 2
#define PAGE2_SUB1 3
#define NO_DRAW_SUB1 4
#define VT100_TERMINAL  0

#define RES_1024_768 3
#define RES_1280_1024 4


/* BLT   defines    blt */

/* pass to get_next_free_page() to identify the use of the new page */
#define BLT_LIST 2 

/* add to page number inside of available_pages[] to mark it as a BLT EMS page */
#define BLT_CODE 300    

/* the MAX number of BLT lists Allowed */
#define NUMBER_OF_BLT_LISTS 30

#define MOUSE_2_BUTTON 1
#define MOUSE_3_BUTTON 2
#define A_JOYSTICK     3
#define A_DATA_TABLET  4


#define VT100_MODE  0
#define MESS_MON  1



extern int  vt100_mess_mon_flag;
extern int  global_screen_color;
extern OPCODE  io_masks[];
extern int  exit_icu_flag;

extern char  pointing_device_type;


extern struct a_window  my_windows[];

extern int pw_kbd_setraw();
extern void pw_kbd_restore();

extern void  default_window_parameters();

extern void add_to_page_frame_ptr();

extern void  flood_draw_gmark();
extern void  clear_whole_screen();
extern void  calculate_graphics_viewport();



/* digitizer stuff */
extern void  save_int3();
extern char  use_the_mouse;
extern void  open_digitizer();
extern void  digitizer_out();
extern int  read_digitizer_buffer();


extern char  licensee[];


/*  NEW in license.c  */
extern void  init_some_colors();
extern char  customer_license();




/*NEW in comio.4c */
extern void  r_flag();
extern void  w_flag();


extern void  make_our_subs();  /*my7.c*/
extern void  my_reset();
extern char  ems_mode;

char  use_the_debug_mode = 'f';
int  the_check_handle;
int  current_color;
extern OPCODE  unity_viewing_matrix[];



extern char  which_high_res;

extern void  menu();    /* Home menus */
extern void  megatek_device();   /* Host interpretor */


/*keyboard revectoring functions*/
extern void  set_int9();
extern void  restore_int9();


extern int  keyboard_status();
extern void  error_logger();
extern int  init_artist10();
extern void  close_board();
extern void  open_host();
extern void  close_host();
extern void  ugout();
extern int  read_host_buffer();
extern void  icu();
extern void  message_monitor();
extern void  set_joystick();
extern void  switch_display_page();
extern void  clear_display_page();
extern void  get_sub_value();


/*my BIOS call routines used here*/
extern void  make_win();
extern void  clear_win();
extern void  save_win();
extern void  restore_win();
extern void  goto_row_col();
extern void  read_curser_pos();
extern void  draw_box();
extern void  turn_on_curser();
extern void  turn_off_curser();
extern void  normal_string();


/*generic graphics board externs*/
extern void  my_size_chars();
extern void  my_set_viewport();
extern void  my_lock_viewport();
extern void  my_scroll();
extern void  my_color();
extern void  color_reg();
extern void  my_dash_pattern();
extern void  my_dash_enable();
extern void  my_line_xy();
extern void  place_char();
extern void  clear_page();
extern void  clear_screen();
extern void  draw_mode();
extern void  my_line3();
extern void  my_char();
extern void  test_board();


extern void  my_pckb();

extern char  pen_press;
extern int  x;
extern int  y;
extern int  my_status;



/* points to physical memory buffer as returned by pw_phys_map() call */
struct sub_holder *sub_struct_ptr; 
unsigned long unix_memory_phys_addr,size_unix_memory_buffer;


/* BLT variables    blt */

int  which_blt_list = 0;   /* is the Active One */

char  *blt_page_frame_ptr;

char do_not_dlrun_flag = 'f';

int  number_of_times_floppy = 0;
int  clipper_status_register;
char  cga_emulation_enabled = 'f';
char  read_more_flag = 'f';
char  which_terminal = VT100_TERMINAL;

/*
short  color_sub_table[50][10];
*/

struct header_data
{
	char visibility,Xform_enable;
	int viewing_3d_matrix[16];
	int origin[3];
	int my_set_viewport_values[4];
	unsigned int dash_pattern;
	char dash_enable;
};

struct header_data  update_header_data;




char  segment_visibility[NUMBER_OF_SEGS];

/* tells length of megatek imitation files  (like ftell at end of file) */
long  megatek_ends[50];
/* tells sequence of megatek logical pages for each segment */
unsigned char  megatek_locate[50][64];
short  *current_megatek_pointer = NULL;
long  current_megatek_insert_pos = 0L;
int  current_megatek_page = 0;




int  which_insert_mode = 2;
int  available_pages[256];
int  insert_sub_x_Xlate[NUMBER_OF_SUBS];
int  insert_sub_y_Xlate[NUMBER_OF_SUBS];
int  handle;
int  physical_page;
int  logical_page;
int  num_unalloc_pages;
int  page_frame_segment_address;
int  current_logical_page;
int  current_physical_page;
int  current_offset;
short  *physical_pages_pointer;
short  *start_of_page_frame_0;
long  current_word[256];
long  last_word[NUMBER_OF_SEGS];

/*
short  sub_calling_table[64][10];
short  our_versions_of_subs[10][100];
*/

struct ems_position
{
	int logical_page_num,offset;
};

struct ems_position  start_segment_ptrs[NUMBER_OF_SEGS] =
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0}
	};


struct ems_position  end_segment_ptrs[NUMBER_OF_SEGS] =
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0}
	};


struct our_element
{
	char op_code,flag1,flag2;
	int arg1,arg2,arg3,arg4;
};

struct our_element  an_element;


struct dl_pointer
{
	char op_code;
	int logical_page_num,offset;
};


struct dl_pointer  dl_pointer_element, read_element;

int  size_dl_ptr_element = sizeof(struct dl_pointer);

OPCODE  my_3d_viewing_matrix[16]  = 
	{
		0x800,0,0,0,
		0,0x800,0,0,
		0,0,0x800,0,
		0,0,0,1
	};

OPCODE  my_3d_modelling_matrix[16] =
	{
		0x2000,0,0,0,
		0,0x2000,0,0,
		0,0,0x2000,0,
		0,0,0,1
	};


short  *current_pointer_to_dl;
int  segment_nums[NUMBER_OF_SEGS];
unsigned char  megatek_data[50];


FILE *fopen(), *out_ptr = NULL;
FILE  *display_file = NULL, *save_file_ptr;
FILE  *error_log_ptr;
FILE  *com_save = NULL, *com_read = NULL;


char  path_error_log[80] = "d:\\";
char  errors_happened = 'f';


/*subroutine segments*/
struct our_element
	 *ptr_sub,
	 our_sub3[SUB_NUM_ELS],
	 our_sub6[SUB_NUM_ELS],
	 our_sub7[SUB7_NUM_ELS],
	 my_sub6[SUB7_NUM_ELS];


/*
short  sub_display_lists[NUMBER_OF_SUBS][2*SUB_NUM_ELS];
*/

short  *sub_ends[NUMBER_OF_SUBS];
short  *sub_host_to_pix_ptrs[NUMBER_OF_SUBS][SUB_NUM_ELS]; 
short  *current_sub_pointer; 


int  sub_nums[NUMBER_OF_SUBS + 1];

char  fill_sub = 'f', our_call = 'f';
int  my_sub6_ptr = -1;

int  sub_insert_ptrs[NUMBER_OF_SUBS + 1];

int  which_cseg = -1;  /*tells which  segment you're using*/
int  which_sub = -1;  /*tells which subroutine you're using*/


struct a_word
{
	unsigned int most_sig,least_sig;
};

struct a_word  vm512 = {64387,0}, vm345 = {12336,12358}, vm33536 = {0,0},
	 vm49536 = {0,0},
	 vm1170 = {0,0}, vm1166 = {0,0}, vm69 = {0,0},
	 vm7 = {0,0};


char  segment_full[NUMBER_OF_SEGS];

int  size_an_element;

long  seg_starts[NUMBER_OF_SEGS] = 
{
 	2496L,
 	2496L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L,
 	2560L
};



/*my BIOS variables*/
char  char_save_page[25][80], attr_save_page[25][80];
int  win_x_low, win_y_low, win_x_up, win_y_up;
int  row_curser, col_curser;


char  quick_redisplay = 'f';
int  update_level = 0;
int  mouse_x = 512;
int  mouse_y = 383;
int  curser_ratio = 1;
char  depress_flag = 'f';
int  button_byte = 0;
int  mouse_phase = 0;
char  doing_redisplay = 'f';
char  version_str[] = "2.00   1/25/91";
char  the_version[45] = "Version  1.33";
char  board_string[70];
char  save_write_error = 'f';
char  plot_write_error = 'f';
char  time_redisplay = 'f';
long  num_words_in_segs = 2496L;
char  write_error = 'f';
char  print_list = 'f';
int  list_size = 0;
char  is_a_seg_open = 'f';
char  joy_off = 'f';
char  pen_press2 = 'f';
int  pen2_x;
int  pen2_y;
int  num_op_code_4s = 0;
int  num_op_code_0s = 0;
int  send_textmode_to = GRAPHICS;
char  did_scroll_text = 'f';
int  ins_matrix_seg;
int  ins_matrix_sub;
char  rw_sub_w_0_1 = 'f';
int  rw_which_seg;
int  redis_old_x;
int  redis_old_y;
int  last_sub_rewritten;
int  check_resp = 1; /*default of fixed checksums*/
char  rewrite_color = 'f';
char  button1 = 'f';
char  rotate_mode = 'm';

int  where_called[NUMBER_OF_SUBS][20];
int  segment_calls[NUMBER_OF_SEGS];

int  version = 3;
char  models[4] = {'!','!','!','!'};
char  term_types[4] = {'#','%',39,39};
char  rev[4] = {'$','$','$','&'};

int  size_header = 14;


int  sub_starts[NUMBER_OF_SUBS] = 
{
 	1793,
 	1857,
 	1921,
 	1985,
 	2049,
 	2113,
 	2177,
 	2241,
 	2305,
 	2369,
 	1001,
 	2433,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
 	1001,
	1001,
	1001,
	1001
};



int  init_baud_rate = 12;
char  quit_segment = 'f';
int  which_changed = -1;
char  draw_black = 'f';
int  num_pages = 2;
int  segm1_start = 2496;
int  segm2_start = 2560;
int  char_y_off = -7, char_x_off = -2;
int  my_y_max = 4100;
char  ins_matrix_flag = 'f';
int  which_redis = 3;
char  quit_redis = 'f';
int  text_row, text_col;
int  graphics_text = TEXT;
int  save_joy_x = 0, save_joy_y = 0;
char  locate_called = 'f';
char  locate_1st_draw = 'u';
int  locate_seg = -1;
char  change_sub3_6_7 = 'f';
int  save_line_type;
int  save_my_x, save_my_y;
int  my_sub6_ptr;
char  curser_displayed = 'f';
int  which_page = 0;
char  clear_seg_flag = 'f';
char  num_dims = '2';
int  new_x = 512;
int  new_y = 383;
char  read_joystick = 'f';
char  joy_enable = 'f';
char  rw_op_code;
int  sub1_0_seg;
char  black = 'f';
char  rw_sub1_0_flag = 'f';
long  ptr_rw_sub1_0 = 0L;
char  save_color;
int  joy_x = 0, joy_y = 0;
int  save_insert_ptr = -2;
int  save_sub_insert_ptr = -2;
char  print_flag = 'f';
int  num_transform_segs = 0;
int  num_matrixes_ins = 0;
char  rewrite_flag = 'f';
char  sub_rewrite = 'f';
char  diag_flag = 'f';
char  draw_flag;
char  no_draw_flag = 'f';
int  line_type;
unsigned char  command;
char  out_string[1000], diag_string[200];
int  xx, yy, zz, my_x, my_y, my_z, old_segment, new_segment,
	 x_origin, y_origin, z_origin, a_segment;
unsigned char  flag, size, short_x, short_y, most_x, least_x,
	 most_y, least_y, temp;
unsigned char  a_char, arg2, arg3;
unsigned int  dash_pat = 0x25b8, pick_label, num_millis, id,
	 switches_flag;
int  displacement, a_subroutine, old_subroutine, new_subroutine;
int  horz_lower, vert_lower, horz_upper, vert_upper, dx, dy,
	 pick_width, pick_height;
int  diag_num;
int  mode_flag; /*tells whether you're in graphics or text mode*/

int  sub_level = 0;

int  old_x = 512, old_y = 383;

/* contains diagnostic handler (must appear at this position) */
#include "my9.h"

char a_line[80];


void  mouse_reset()
{
#ifdef lllllllllllllssssssssss
	if(use_the_debug_mode == 't')
		printf("Mouse reset BEFORE INT86\n");

	my_input_regs.x.ax = 0;
	int86(0x33,&my_input_regs,&my_output_regs);

	if(use_the_debug_mode == 't')
		printf("Mouse reset AFTER INT86\n");

	if(my_output_regs.x.ax == 0)
		printf("Joystick/Mouse Not Installed\n");

#endif
}




void  my_ugout(i)
int i;
{
#ifdef HAVE_DIAGS
	sprintf(diag_string,"****************UGOUT(%x)\n",i);
	my_diag(diag_string);
#endif
	ugout(i);
}


void  which_icu()
{
	ugout(0x1e);
	ugout('o');
	ugout(models[version]);
	ugout(term_types[version]);
	ugout(rev[version]);
}



void  check_com_status(status_value)
int status_value;
{
int error_count;
int save_row,save_col;

	if((status_value & 30) != 0)
	{

#ifdef lswlslslslslsl
		putchar(7);

		error_count = 0;
		save_row = row_curser;
		save_col = col_curser;

		read_curser_pos();


		if((status_value & 2) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Overrun Error");
		}


		if((status_value & 4) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Parity Error");
		}


		if((status_value & 8) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Framing Error");
		}


		if((status_value & 16) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Break Detect");
		}


		if(error_count > 1)
		{
			goto_row_col(24,60);
			printf("Com Errors = %x    ",status_value & 30);
		}


		row_curser = save_row;
		col_curser = save_col;
		goto_row_col(row_curser,col_curser);
#endif

		sprintf(diag_string,"Com Errors = %x    ",status_value & 30);
		error_logger(diag_string);
	}

}





int  read_a_file()
{
int ch;

	ch = getc(com_read);

	if(ch == EOF)
	{
		r_flag(0);
		fclose(com_read);
		com_read = NULL;
		return(-1);
	}
	else
		return(ch);
}



void  write_a_file(value)
int value;
{
	if(value != -1)
		putc(value,com_save);
}





void  read_more(num_chars)
int num_chars;
{
int i,j,ttt,jj;
int ch;
long start_time,current_time,dummy;

	ttt = diag_num;
	diag_num = 300;

	read_more_flag = 't';

	for(i = 0 ; i < num_chars ; i++)
	{
		ch = read_host_buffer();
		my_pckb();


cntrl_p:	if((ch & 0x7f) == 0x10)
		{

			ch = read_host_buffer();

                        while(ch == -1)
                        {
		/*
				for(jj = 0 ; jj < 90000 ; jj++)
					;
		*/

				my_pckb();

                                ch = read_host_buffer();
                        }


			if((ch & 0x7f) == 0x0d)
			{
#ifdef HAVE_DIAGS
					my_diag("SWITCH TO MESSAGE MONITOR\n");
#endif

				message_monitor();

				ch = read_host_buffer();

			}
		}


		while(ch == -1)
		{
	/*
			for(jj = 0 ; jj < 90000 ; jj++)
				;
	*/

			my_pckb();

			ch = read_host_buffer();
		}

		

		if((ch & 0x7f) == 0x10)
			goto cntrl_p;

		megatek_data[i] = (char)(ch & 0x7f);

	}


	read_more_flag = 'f';

	diag_num = ttt;
}  /* end read_more */


void  asterisk(x,y)
int x,y;
{
        my_line_xy(x - 4,y,x+4,y);
        my_line_xy(x,y - 4,x,y + 4);
        my_line_xy(x - 3,y - 3,x + 3,y + 3);
        my_line_xy(x - 3,y + 3,x + 3,y - 3);
}


void  dot(x,y)
int x,y;
{
	my_line_xy(x,y,x + 1,y);
	my_line_xy(x,y + 1,x + 1,y + 1);
}


void  plus(x,y)
int x,y;
{
	my_line_xy(x,y - 4,x,y + 4);
	my_line_xy(x - 4,y,x + 4,y);
}



main(argc,argv)
int argc;
char  *argv[];
{
int i,j,subs,from_segs;
int ch;
unsigned int pw_phys_buf_size;


	if(pw_kbd_setraw() == 0)
	{
		printf("\n\nUnable to set keyboard to raw mode\n\n");
		exit(0);
	}


	diag_flag = 'f';


/*
	printf("\n\nENTER RECORD FILE NAME : ");
	scanf("%s",diag_string);
	printf("\n\n");

	com_read = fopen(diag_string,"rb");

	if(com_read == NULL)
	{
		printf("Invalid file name!!!!!\n\nBye!!\n\n");
		exit(0);
	}
*/

	if(argc == 1)
		use_the_debug_mode = 'f';
	else
	{
		if(strncmp(strupr(argv[1]),"/T",2) == 0)
			use_the_debug_mode = 't';
		else
			use_the_debug_mode = 'f';
	}



	if(use_the_debug_mode == 't')
	{
		putchar(7);
		printf("\n\n\n\n\n\t\t\tDEBUG MODE ENABLED  \n\n");
		printf("\t\t\tD135 Emulator Version 1.30\n\n");
		printf("\t\t\tPress Enter To Continue...\n\n\n\n\n\n");
		getchar();
	}



	if(use_the_debug_mode == 'f')
	{
		clear_display_page();
/*
		goto_row_col(0,0);
*/
	}


/*
	make_win(10,3,60,17);
*/


/*  What's a good address ????????
	FP_SEG(interrupt_reason_ptr) = 0x40;
	FP_OFF(interrupt_reason_ptr) = 0x0004;
*/


	if(use_the_debug_mode == 't')
		printf("Before Initializing Expanded Memory\n");


	get_board_status();
	page_frame_segment_address = get_page_frame_address();
	num_unalloc_pages = get_unalloc_page_count();
	the_check_handle = handle = allocate_pages(num_unalloc_pages);
	physical_pages_pointer = (short  *)&my_ems_pages[0][0];


	/* blt_page_frame_ptr  points to page frame 1 */
	blt_page_frame_ptr = (char  *)physical_pages_pointer;  




	if(use_the_debug_mode == 't')
		printf("After Initializing Expanded Memory\n");





/* mark every logical page with its number

 map_handle_page NOW memcpy()'s from physical memory buffer, which is not
initialized at this point

	for(i = 0 ; i < num_unalloc_pages ; i++)
	{
		map_handle_page(i,0,handle);

		for(j = 8180 ; j < 8190 ; j++)
			*(physical_pages_pointer + j) = i;
	}
*/

	if(use_the_debug_mode == 't')
	{
		printf("After Adding Debugging Data To Expanded Memory\n");
		printf("Before Initializing Clipper Graphics Board\n");
	}


	default_window_parameters();


	i = init_artist10();

	rdstat(&my_status);
	if((my_status & 0x300) != 0)
		display(0);

	if(use_the_debug_mode == 't')
		printf("After Initializing Clipper Graphics Board\n");


	test_board();


	size_unix_memory_buffer = (unsigned long)pw_phys_map(
		(OPCODE **)&sub_struct_ptr,&unix_memory_phys_addr,
		&pw_phys_buf_size);
	printf("\n\nsize of unix physical buffer = %u\n\n",
		pw_phys_buf_size);
	printf("sizeof(struct subholder) = %d\n\n",sizeof(struct sub_holder));


	if(pw_phys_buf_size <= sizeof(struct sub_holder))
	{
		putchar(7);
		printf("\nunix physical buffer is too small\n");
		exit(0);
	}


	if(use_the_debug_mode == 't')
		printf("Before Initializing Mouse/Joystick\n");


	mouse_reset();

	if(use_the_debug_mode == 't')
		printf("After Initializing Mouse/Joystick\n");


	size_an_element = sizeof(struct our_element);


	if(use_the_debug_mode == 't')
		printf("Intializing Emulator Functions...\n");

	my_reset();  

	if(use_the_debug_mode == 't')
		printf("  Emulator Functions Initialized Successfully.\n\n");


/*open error log file if wanted (path has been tested in init_artist10())*/


	if(use_the_debug_mode == 't')
		printf("Before Creating ERROR Log File\n");

/*
	if(strcmp(strupr(path_error_log),"NONE") == 0)
		error_log_ptr = NULL;
	else

	{

		sprintf(diag_string,"%serrorlog.dat",path_error_log);
*/
		error_log_ptr = fopen("error_log.dat","w");
		fprintf(error_log_ptr,"No Errors:\n\n");
		fclose(error_log_ptr);
/*
	}
*/

	if(use_the_debug_mode == 't')
		printf("After Creating ERROR Log File\n");

	make_our_subs();  /*my7.c*/
	init_some_colors();



	clear_whole_screen();



	my_windows[MAIN_MENU].is_retracted = 't';
	my_windows[PART_INFO].is_retracted = 't';



	clear_pop_up_window(VT100_WINDOW,return_window_color(VT100_WINDOW,
		BACKGROUND),EXPANDED_POS);



	if(use_the_debug_mode == 't')
		printf("Initializing Host Communications Port\n");

#ifdef lllllllllllllllllllllllllll
	if(pointing_device_type == A_DATA_TABLET)
	{
		save_int3();  /* save mouse/joystick interrupt 3 handler */

		open_digitizer(12);
		open_digitizer(12);

		digitizer_out(17);   /* X_ON */
	}
#endif

	open_host(init_baud_rate);


	if(use_the_debug_mode == 't')
	{
		printf("Host Port Initialized Successfully\n");
		printf("Entering Interactive Terminal Mode.......\n");
	} 


	calculate_graphics_viewport();
	flood_draw_gmark();

	while(99)
	{
		if(number_of_times_floppy == 0)
		{
			which_page = !which_page;
			rdmask(io_masks[which_page]);
			wrmask(io_masks[which_page]);

			if(which_high_res == RES_1024_768)
			{
				viewport(0,1023,0,767);
			}
			else
				viewport(0,1279,0,1023);

			flood((short)((global_screen_color << 4) | 
				global_screen_color));
		}


		number_of_times_floppy++;

		ch = customer_license('t');

		if(ch == 'e')
		{
			if(which_high_res == RES_1024_768)
			{
				viewport(0,1023,0,767);
			}
			else
				viewport(0,1279,0,1023);

			wrmask(0xff);
			flood(0);

			pw_kbd_restore();
			close_board();
			close_host();  /*closes host's com port*/

			rdstat(&my_status);
			if((my_status & 0x300) != 0)
				display(1);	

			exit(0);
		}
		else if(ch == 'h')
		{
			menu();
		}
		else
		{
			number_of_times_floppy = 0;

			if(which_high_res == RES_1024_768)
			{
				viewport(0,1023,0,767);
			}
			else
				viewport(0,1279,0,1023);

			flood((short)((global_screen_color << 4) | 
				global_screen_color));

			if(vt100_mess_mon_flag == MESS_MON)
			{
				if(is_window_array_empty(MAIN_MENU) == 'f')
					display_pop_up_window(MAIN_MENU);

				if(is_window_array_empty(LIGHTS) == 'f')
					display_pop_up_window(LIGHTS);
	
				if(is_window_array_empty(CALCULATOR) == 'f')
					display_pop_up_window(CALCULATOR);

				if(is_window_array_empty(PART_INFO) == 'f')
					display_pop_up_window(PART_INFO);
			}


			my_set_viewport(-2048,-2048,2048,2048,0);
			linpat(1);
			my_size_chars(list_size);

			which_page = !which_page;
			rdmask(io_masks[which_page]);
			wrmask(io_masks[which_page]);

			exit_icu_flag = 'f';
			icu();
		}
	}



/*
	icu();

	getchar();
	getchar();

	fclose(com_read);

	viewport(0,1023,0,767);
	flood(0);

	exit(0);
*/

}


