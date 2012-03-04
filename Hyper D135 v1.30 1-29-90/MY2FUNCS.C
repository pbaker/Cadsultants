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


/*#define HAVE_DIAGS*/

#define ENFORCE_PAL

#include "conio.h"
#include "stdio.h"
#include "math.h"
#include "dos.h"
#include "io.h"
#include "string.h"
#include "time.h"

#include "c:\emulate\clipper\expanded.h"
#include "c:\emulate\clipper\pw_lib.h"



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



extern char far licensee[];


/*NEW in comio.4c */
extern void far r_flag(int);
extern void far w_flag(int);


extern void far make_our_subs();  /*my7.c*/
extern void far my_reset();
extern char far ems_mode;

char far use_the_debug_mode = 'f';
int far the_check_handle;
int far current_color;
extern int far unity_viewing_matrix[];



extern char far which_high_res;

extern void far menu(void);    /* Home menus */
extern void far megatek_device(int);   /* Host interpretor */


/*keyboard revectoring functions*/
extern void far set_int9();
extern void far restore_int9();


extern int far keyboard_status();
extern void far error_logger(char *);
extern int far init_artist10();
extern void far close_board();
extern void far open_host();
extern void far close_host();
extern void far ugout();
extern int far read_host_buffer();
extern void far icu();
extern void far message_monitor();
extern void far set_joystick();
extern void far switch_display_page();
extern void far clear_display_page();
extern void far get_sub_value();


/*my BIOS call routines used here*/
extern void far make_win();
extern void far clear_win();
extern void far save_win();
extern void far restore_win();
extern void far goto_row_col();
extern void far read_curser_pos();
extern void far draw_box();
extern void far turn_on_curser();
extern void far turn_off_curser();
extern void far normal_string();


/*generic graphics board externs*/
extern void far my_size_chars();
extern void far my_set_viewport(int,int,int,int,int);
extern void far my_lock_viewport();
extern void far my_scroll();
extern void far my_color();
extern void far color_reg();
extern void far my_dash_pattern();
extern void far my_dash_enable();
extern void far my_line_xy();
extern void far place_char();
extern void far clear_page();
extern void far clear_screen();
extern void far draw_mode();
extern void far my_line3();
extern void far my_char();
extern void far test_board();


extern void far my_pckb();


extern char far pen_press;
extern int far x;
extern int far y;


char far which_terminal = VT100_TERMINAL;


short far color_sub_table[50][10];


struct header_data
{
	char visibility,Xform_enable;
	int viewing_3d_matrix[16];
	int origin[3];
	int my_set_viewport_values[4];
	unsigned int dash_pattern;
	char dash_enable;
};

struct header_data far update_header_data;




char far segment_visibility[NUMBER_OF_SEGS];

/* tells length of megatek imitation files  (like ftell at end of file) */
long far megatek_ends[50];
/* tells sequence of megatek logical pages for each segment */
unsigned char far megatek_locate[50][64];
short far *current_megatek_pointer = NULL;
long far current_megatek_insert_pos = 0L;
int far current_megatek_page = 0;




int far which_insert_mode = 2;
int far available_pages[256];
int far insert_sub_x_Xlate[NUMBER_OF_SUBS];
int far insert_sub_y_Xlate[NUMBER_OF_SUBS];
int far handle;
int far physical_page;
int far logical_page;
int far num_unalloc_pages;
int far page_frame_segment_address;
int far current_logical_page;
int far current_physical_page;
int far current_offset;
short far *physical_pages_pointer;
short far *start_of_page_frame_0;
int far *interrupt_reason_ptr;
long far current_word[256];
long far last_word[NUMBER_OF_SEGS];


short far sub_calling_table[64][10];
short far our_versions_of_subs[10][100];


struct ems_position
{
	int logical_page_num,offset;
};

struct ems_position far start_segment_ptrs[NUMBER_OF_SEGS] =
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


struct ems_position far end_segment_ptrs[NUMBER_OF_SEGS] =
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

struct our_element far an_element;


struct dl_pointer
{
	char op_code;
	int logical_page_num,offset;
};


struct dl_pointer far dl_pointer_element,far read_element;

int far size_dl_ptr_element = sizeof(struct dl_pointer);

int far my_3d_viewing_matrix[16]  = 
	{
		0x800,0,0,0,
		0,0x800,0,0,
		0,0,0x800,0,
		0,0,0,1
	};

int far my_3d_modelling_matrix[16] =
	{
		0x2000,0,0,0,
		0,0x2000,0,0,
		0,0,0x2000,0,
		0,0,0,1
	};


short far *current_pointer_to_dl;
int far segment_nums[NUMBER_OF_SEGS];
unsigned char far megatek_data[50];


FILE *fopen(),far *out_ptr = NULL;
FILE far *display_file = NULL,far *save_file_ptr;
FILE far *error_log_ptr;
FILE far *com_save = NULL,far *com_read = NULL;


char far path_error_log[80] = "d:\\";
char far errors_happened = 'f';


/*subroutine segments*/
struct our_element
	huge *ptr_sub,
	huge our_sub3[SUB_NUM_ELS],
	huge our_sub6[SUB_NUM_ELS],
	huge our_sub7[SUB7_NUM_ELS],
	huge my_sub6[SUB7_NUM_ELS];



short far sub_display_lists[NUMBER_OF_SUBS][2*SUB_NUM_ELS];

short far *sub_ends[NUMBER_OF_SUBS];
short far *sub_host_to_pix_ptrs[NUMBER_OF_SUBS][SUB_NUM_ELS]; 
short far *current_sub_pointer = &(sub_display_lists[0][0]);


int far sub_nums[NUMBER_OF_SUBS + 1];

char far fill_sub = 'f',far our_call = 'f';
int far my_sub6_ptr = -1;

int far sub_insert_ptrs[NUMBER_OF_SUBS + 1];

int far which_cseg = -1;  /*tells which  segment you're using*/
int far which_sub = -1;  /*tells which subroutine you're using*/


struct a_word
{
	unsigned int most_sig,least_sig;
};

struct a_word far vm512 = {64387,0},far vm345 = {12336,12358},far vm33536 = {0,0},
	far vm49536 = {0,0},
	far vm1170 = {0,0},far vm1166 = {0,0},far vm69 = {0,0},
	far vm7 = {0,0};


char far segment_full[NUMBER_OF_SEGS];

int far size_an_element;

long far seg_starts[NUMBER_OF_SEGS] = 
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
union REGS far my_input_regs,far my_output_regs;
char far char_save_page[25][80],far attr_save_page[25][80];
int far win_x_low,far win_y_low,far win_x_up,far win_y_up;
int far row_curser,far col_curser;


char far quick_redisplay = 'f';
int far update_level = 0;
int far mouse_x = 0;
int far mouse_y = 0;
int far curser_ratio = 1;
char far depress_flag = 'f';
int far button_byte = 0;
int far mouse_phase = 0;
char far type_char_read;
char far value_char_read;
char far doing_redisplay = 'f';
char far version_str[] = "1.30   1/29/90";
char far board_string[70];
char far save_write_error = 'f';
char far plot_write_error = 'f';
char far time_redisplay = 'f';
long far num_words_in_segs = 2496L;
char far write_error = 'f';
char far print_list = 'f';
int far list_size = 0;
char far is_a_seg_open = 'f';
char far joy_off = 'f';
char far pen_press2 = 'f';
int far pen2_x;
int far pen2_y;
int far num_op_code_4s = 0;
int far num_op_code_0s = 0;
int far send_textmode_to = GRAPHICS;
char far did_scroll_text = 'f';
int far ins_matrix_seg;
int far ins_matrix_sub;
char far rw_sub_w_0_1 = 'f';
int far rw_which_seg;
int far redis_old_x;
int far redis_old_y;
int far last_sub_rewritten;
int far check_resp = 1; /*default of fixed checksums*/
char far rewrite_color = 'f';
char far button1 = 'f';
char far rotate_mode = 'm';

int far where_called[NUMBER_OF_SUBS][20];
int far segment_calls[NUMBER_OF_SEGS];

int far version = 3;
char far models[4] = {'!','!','!','!'};
char far term_types[4] = {'#','%',39,39};
char far rev[4] = {'$','$','$','&'};

int far size_header = 14;


int far sub_starts[NUMBER_OF_SUBS] = 
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



int far init_baud_rate = 12;
char far quit_segment = 'f';
int far which_changed = -1;
char far draw_black = 'f';
int far num_pages = 2;
int far segm1_start = 2496;
int far segm2_start = 2560;
int far char_y_off = -7,far char_x_off = -2;
int far my_y_max = 4100;
char far ins_matrix_flag = 'f';
int far which_redis = 3;
char far quit_redis = 'f';
int far text_row,far text_col;
int far graphics_text = TEXT;
int far save_joy_x = 0,far save_joy_y = 0;
char far locate_called = 'f';
char far locate_1st_draw = 'u';
int far locate_seg = -1;
char far change_sub3_6_7 = 'f';
int far save_line_type;
int far save_my_x,far save_my_y;
int far my_sub6_ptr;
char far curser_displayed = 'f';
int far which_page = 0;
char far clear_seg_flag = 'f';
char far num_dims = '2';
int far new_x;
int far new_y;
char far read_joystick = 'f';
char far joy_enable = 'f';
char far rw_op_code;
int far sub1_0_seg;
char far black = 'f';
char far rw_sub1_0_flag = 'f';
long far ptr_rw_sub1_0 = 0L;
char far save_color;
int far joy_x = 0,far joy_y = 0;
int far save_insert_ptr = -2;
int far save_sub_insert_ptr = -2;
char far print_flag = 'f';
int far num_transform_segs = 0;
int far num_matrixes_ins = 0;
char far rewrite_flag = 'f';
char far sub_rewrite = 'f';
char far diag_flag = 'f';
char far draw_flag;
char far no_draw_flag = 'f';
int far line_type;
unsigned char far command;
char far out_string[1000],far diag_string[200];
int far xx,far yy,far zz,far my_x,far my_y,far my_z,far old_segment,far new_segment,
	far x_origin,far y_origin,far z_origin,far a_segment;
unsigned char far flag,far size,far short_x,far short_y,far most_x,far least_x,
	far most_y,far least_y,far temp;
unsigned char far a_char,far arg2,far arg3;
unsigned int far dash_pat = 0x25b8,far pick_label,far num_millis,far id,
	far switches_flag;
int far displacement,far a_subroutine,far old_subroutine,far new_subroutine;
int far horz_lower,far vert_lower,far horz_upper,far vert_upper,far dx,far dy,
	far pick_width,far pick_height;
int far diag_num;
int far mode_flag; /*tells whether you're in graphics or text mode*/

int far sub_level = 0;

int far old_x = 0,far old_y = 0;

/* contains diagnostic handler (must appear at this position) */
#include "my9.h"

char a_line[80];


void far mouse_reset()
{
	if(use_the_debug_mode == 't')
		printf("Mouse reset BEFORE INT86\n");

	my_input_regs.x.ax = 0;
	int86(0x33,&my_input_regs,&my_output_regs);

	if(use_the_debug_mode == 't')
		printf("Mouse reset AFTER INT86\n");

	if(my_output_regs.x.ax == 0)
		printf("Joystick/Mouse Not Installed\n");

}




void far my_ugout(i)
int i;
{
#ifdef HAVE_DIAGS
	sprintf(diag_string,"****************UGOUT(%x)\n",i);
	my_diag(diag_string);
#endif
	ugout(i);
}


void far which_icu()
{
	ugout(0x1e);
	ugout('o');
	ugout(models[version]);
	ugout(term_types[version]);
	ugout(rev[version]);
}



void far check_com_status(status_value)
int status_value;
{
int error_count;
int save_row,save_col;

	if((status_value & 30) != 0)
	{
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
	}

}





int far read_a_file()
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



void far write_a_file(value)
int value;
{
	if(value != -1)
		putc(value,com_save);
}





void far read_more(num_chars)
int num_chars;
{
int i,j,ttt;
int ch;
long start_time,current_time,dummy;

	ttt = diag_num;
	diag_num = 300;


	for(i = 0 ; i < num_chars ; i++)
	{
		ch = read_host_buffer();
		my_pckb();


cntrl_p:	if((ch & 0x7f) == 0x10)
		{
			ch = read_host_buffer();

                        while(ch == -1)
                        {
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
			my_pckb();

			ch = read_host_buffer();
		}

		

		if((ch & 0x7f) == 0x10)
			goto cntrl_p;

		megatek_data[i] = (char)(ch & 0x7f);

	}


	diag_num = ttt;
}  /* end read_more */


void far asterisk(x,y)
int x,y;
{
        my_line_xy(x - 4,y,x+4,y);
        my_line_xy(x,y - 4,x,y + 4);
        my_line_xy(x - 3,y - 3,x + 3,y + 3);
        my_line_xy(x - 3,y + 3,x + 3,y - 3);
}


void far dot(x,y)
int x,y;
{
	my_line_xy(x,y,x + 1,y);
	my_line_xy(x,y + 1,x + 1,y + 1);
}


void far plus(x,y)
int x,y;
{
	my_line_xy(x,y - 4,x,y + 4);
	my_line_xy(x - 4,y,x + 4,y);
}



main(argc,argv)
int argc;
char far *argv[];
{
int i,j,subs,from_segs;
int ch;





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
		getch();
	}


	if(use_the_debug_mode == 't')
		printf("\nInstalling Keyboard Interrupt Routine...\n");


	set_int9();


	if(use_the_debug_mode == 't')
		printf("  Keyboard Interrupt Routine Is Installed.\n\n");



	if(use_the_debug_mode == 'f')
	{
		clear_display_page();
		goto_row_col(0,0);
	}


	make_win(10,3,60,17);


	FP_SEG(interrupt_reason_ptr) = 0x40;
	FP_OFF(interrupt_reason_ptr) = 0x0004;


	if(use_the_debug_mode == 't')
		printf("Before Initializing Expanded Memory\n");


	get_board_status();
	page_frame_segment_address = get_page_frame_address();
	num_unalloc_pages = get_unalloc_page_count();
	the_check_handle = handle = allocate_pages(num_unalloc_pages);
	physical_pages_pointer = (short far *)((long)
		page_frame_segment_address << 16);
	start_of_page_frame_0 = physical_pages_pointer;


	if(use_the_debug_mode == 't')
		printf("After Initializing Expanded Memory\n");





/* mark every logical page with its number */
	for(i = 0 ; i < num_unalloc_pages ; i++)
	{
		map_handle_page(i,0,handle);

		for(j = 8180 ; j < 8190 ; j++)
			*(physical_pages_pointer + j) = i;
	}






	if(use_the_debug_mode == 't')
	{
		printf("After Adding Debugging Data To Expanded Memory\n");
		printf("Before Initializing Clipper Graphics Board\n");
	}



	i = init_artist10();


	if(use_the_debug_mode == 't')
		printf("After Initializing Clipper Graphics Board\n");


	clear_screen();
	test_board();


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


	if(strcmp(strupr(path_error_log),"NONE") == 0)
		error_log_ptr = NULL;
	else
	{
		sprintf(diag_string,"%serrorlog.dat",path_error_log);
		error_log_ptr = fopen(diag_string,"w");
		fprintf(error_log_ptr,"No Errors:\n\n");
		fclose(error_log_ptr);
	}


	if(use_the_debug_mode == 't')
		printf("After Creating ERROR Log File\n");


	make_our_subs();  /*my7.c*/


/* twice is needed here */

	if(use_the_debug_mode == 't')
		printf("Initializing Host Communications Port\n");


	open_host(init_baud_rate);
	open_host(init_baud_rate);

	if(use_the_debug_mode == 't')
	{
		printf("Host Port Initialized Successfully\n");
		printf("Entering Interactive Terminal Mode.......\n");
	} 


	icu();
}

