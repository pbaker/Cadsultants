/*  ICU.C  */

/*
  march 28, 1991  removed those delay loops Because they made my_pckb()
	miss characters  Raw keyboard mode has no buffering 
*/



#include <stdio.h>

#define cr 13
#define ctrl_P 16
#define escape 27
#define EXIT_MAX 1000


char js_flag = 0;
int character_count = 0,exit_flag = 0,run_vt_flag = 0,exit_icu_flag = 'f',
	exit_counter = 0;

extern void call_single_update();
extern void  alt_functions();
extern void my_pckb();
extern void megatek_device();
extern void megatek_update();
extern void which_icu();
extern void input_device();
extern int read_host_buffer();
extern void ugout();
extern void mm_filter();
extern void matrix_update();
extern void run_vt();



void send_to_host(a_char)
char a_char;
{
	ugout(a_char);
}

void set_joystick(an_int)
int an_int;
{
	js_flag = 0;
}


void message_monitor()
{
int a_char,j;

	call_single_update();

mm_top:
	if(exit_icu_flag == 't')
	{
		exit_flag = 0;
		character_count = 0;
		return;
	}

	input_device();	

check_kb1:
	if(run_vt_flag == 1)
		run_vt();

	my_pckb();
	a_char = read_host_buffer();
	if(a_char == -1)
	{
/*
		for(j = 0 ; j < 9000 ; j++)
			;
*/

		goto mm_top;
	}

mm:
	character_count++;

	mm_filter(a_char);

	if(exit_flag == 1)
	{
		exit_flag = 0;
		character_count = 0;
		return;
	}

	if(character_count > 1)
		goto check_kb1;
	megatek_update();
	goto check_kb1;
}


void icu()
{
int a_char,j;

begin_mm:
	message_monitor();
	if(exit_icu_flag == 't')
		return;
begin_tdata:
	input_device();
check_kb:
	my_pckb();
check_host:
	a_char = read_host_buffer();
	if(a_char == -1)
	{
/*
		for(j = 0 ; j < 9000 ; j++)
			;
*/

		goto begin_tdata;
	}


	if(a_char == ctrl_P)
	{
		for(a_char = read_host_buffer() ; a_char == -1 ;
			a_char = read_host_buffer())
		{
/*
			for(j = 0 ; j < 9000 ; j++)
				;
*/
		}

		if(a_char == cr)
			goto begin_mm;

		megatek_device(a_char);
		goto begin_tdata;
	}

	megatek_device(a_char);
	goto check_kb;
}

