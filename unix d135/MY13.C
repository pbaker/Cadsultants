/* MY13.C  reads the keyboard & sends codes to host or invokes our stuff */

/*contains "C" versions of Paul's stuff*/
/*nov 8, 1988 changed my_pckb()'s normal charactor code to send the char
to the host only if doing_redisplay == 'f'*/



#include "my_globs.h"
#include <pw_lib.h>
#include "expanded.h"
#include "mywindows.h"


#define VT100_MODE  0
#define MESS_MON  1



#define RES_1024_768 3
#define RES_1280_1024 4



extern void  change_page_gmark_undraw();
extern void  clear_screen();
extern void  toggle_retraction();


extern void  vt100_pckb();
extern int  vt100_mess_mon_flag;
extern int run_vt_flag;


extern void  my_size_chars();
extern void  read_curser_pos();
extern void  goto_row_col();
extern void  clear_text_monitor_dl();
extern void  my_set_viewport();
extern void  my_ugout();
extern int  keyboard_status();
extern void  megatek_device();
extern void  clear_page();
extern void  alt_menu_on();
extern void  alt_menu_off();
extern char  display_alt_menu;
extern short  dave_test[];
extern OPCODE io_masks[];
extern void  display_a_seg();
extern char  stop_after_matrix_sub;
extern char  display_list_enabled;
extern void menu();

extern void read_keyboard();



int  which_logical_page, where_to_start, num_to_show;



extern char  which_high_res;
char  doing_an_alt_u = 'f';
char  doing_an_alt_r = 'f';
char  do_ugouts_only = 'f';




void  clear_graphics_totally()
{
short temp;




	if(which_high_res == RES_1024_768)
		viewport(0,1023,0,767);  /*full screen*/
	else		/*full high res screen*/
		viewport(160,1120,0,1023); 


	wrmask(0xff);  /* both pages */
	rdmask(io_masks[which_page]);  /* one page */

	temp = return_window_color(GRAPHICS_WINDOW,BACKGROUND);

	flood((temp << 4) | temp);   /*clear screen*/

	wrmask(io_masks[which_page]);  /*return to*/
	rdmask(io_masks[which_page]);  /*previous*/
					/*page*/
	change_page_gmark_undraw();

/*restore last saved window & viewport*/
	stop_after_matrix_sub = 't';
	display_a_seg(which_cseg);
	stop_after_matrix_sub = 'f';


	clear_text_monitor_dl();
}





void  my_send_ext(ch)
int ch;
{

	my_ugout(31);
	my_ugout(97);
	my_ugout(ch);
}


#ifdef lflfflflflfllf
void  read_keyboard()
{
	my_input_regs.h.ah = 0;

	int86(0x16,&my_input_regs,&my_output_regs);

	type_char_read = my_output_regs.h.al;
	value_char_read = my_output_regs.h.ah;
}
#endif






void  my_pckb()
{
int i,j;
unsigned long temp;
int save_row,save_col;
unsigned short *short_ptr;


/*
	if(vt100_mess_mon_flag == VT100_MODE)
	{
		vt100_pckb();
		return;
	}
*/

/*
   while(keyboard_status() != -1)    key was pressed
   {
*/

	read_keyboard();

	if((type_char_read == 0) && (value_char_read == 0))
		return;


	if(type_char_read != 0)   /*normal key*/
	{
		if(type_char_read == 8)
			type_char_read = 127;
		else if(type_char_read == 127)
			type_char_read = 8;

		if(type_char_read == 10)
			type_char_read = 13;

#ifdef HAVE_DIAGS
		if(type_char_read == 0x01)
			megatek_device(1);
		else if(type_char_read == 0x02)
			megatek_device(2);
		else
#endif

		my_ugout(type_char_read);


		if(vt100_mess_mon_flag == VT100_MODE)
			run_vt_flag = 1;

	}
	else          /*extended code key*/
	{
		switch(value_char_read)
		{

			case 59 : /*F1*/
			{
				my_send_ext(36);
				break;
			}

			case 60 : /*F2*/
			{
				my_send_ext(37);
				break;
			}

			case 61 : /*F3*/
			{
				my_send_ext(38);
				break;
			}

			case 62 : /*F4*/
			{
				my_send_ext(39);
				break;
			}
			case 63 : /*F5*/
			{
				my_send_ext(40);
				break;
			}

			case 64 : /*F6*/
			{
				my_send_ext(41);
				break;
			}

			case 65 : /*F7*/
			{
				my_send_ext(42);
				break;
			}

			case 66 : /*F8*/
			{
				my_send_ext(43);
				break;
			}

			case 67 : /*F9*/
			{
				my_send_ext(44);
				break;
			}

			case 68 : /*F10*/
			{
				my_send_ext(45);
				break;
			}


			case 22 :  /*alt U (calls megatek_update)*/
			{
				if(display_list_enabled == 'f')
					break;

				if(do_ugouts_only == 't')
					break;

				restore_graphics_if_needed();

				if(doing_an_alt_u == 'f')  
				{
					doing_an_alt_u = 't';
					megatek_device(4);
					doing_an_alt_u = 'f';

					clear_text_monitor_dl();
				}

				break;
			}



			case 0x77 :  /* ctrl - home */
			case 71 :  /*home*/
			{
				if(do_ugouts_only == 't')
					break;


			/*clear dlrun error messages*/

/*
				save_row = row_curser;
				save_col = col_curser;

				read_curser_pos();
				goto_row_col(24,59);
				printf("                    ");
				goto_row_col(row_curser,col_curser);

				row_curser = save_row;
				col_curser = save_col;
*/

				menu();

				break;
			}

			case 82 : /*Ins key  (clear graphics monitor)*/
			{
				if(do_ugouts_only == 't')
					break;

				restore_graphics_if_needed();

				wrmask(0xff);
				clear_screen();
				wrmask(io_masks[which_page]);

				break;
			}



			case 83 : /*Del key*/
			{
				my_ugout(127);
				break;
			}

			case 104 : /*alt F1*/
			{
				my_send_ext(46);
				break;
			}

			case 105 : /*alt F2*/
			{
				my_send_ext(47);
				break;
			}

			case 106 : /*alt F3*/
			{
				my_send_ext(48);
				break;
			}

			case 107 : /*alt F4*/
			{
				my_send_ext(49);
				break;
			}

			case 108 : /*alt F5*/
			{
				printf("\n\nhere is a list of all subs\n\n");

				for(j = 0 ; j < 16 ; j++)
				{
					short_ptr = (unsigned short *)
						&(sub_struct_ptr ->
					 	sub_display_lists[sub_nums[j]]
						[0]);
					printf("SUB %d =  ",sub_nums[j]);

					for(i = 0 ; i < 25 ; i++)
						printf("%x ",*(short_ptr + i));

					printf("\n");
				}



				short_ptr = (unsigned short *)
					&(sub_struct_ptr ->
					our_versions_of_subs[PAGE1_SUB1][0]);
				printf("\nPAGE1_SUB1 =  ");

				for(i = 0 ; i < 25 ; i++)
					printf("%x ",*(short_ptr + i));

				printf("\n");



				short_ptr = (unsigned short *)
					&(sub_struct_ptr ->
					our_versions_of_subs[PAGE2_SUB1][0]);
				printf("\nPAGE2_SUB1 =  ");

				for(i = 0 ; i < 25 ; i++)
					printf("%x ",*(short_ptr + i));

				printf("\n\n");


				break;
			}


#ifdef ekekefklefleleflefl
			case 109 : /*alt F6 (display piece of a page)*/
			{

				printf("Enter which segment's 1st page: ");
				scanf("%d",&which_logical_page);
				printf("Enter offset to start at: ");
				scanf("%d",&where_to_start);
				printf("Enter # of integers to print: ");
				scanf("%d",&num_to_show);

				save_map_handle_page = 't';
				map_handle_page(start_segment_ptrs[
					which_logical_page].logical_page_num,
					current_physical_page,handle);

				fprintf(stdprn,
				    "\n\nthis is segment %d's 1st page , starting at offset %d\n\n",
					which_logical_page,
						where_to_start);

				for(i = where_to_start ; (i < 8180) &&
					(i - where_to_start < num_to_show) ;
						i++)
				{
					if((i % 15) == 0)
						fprintf(stdprn,"\n");

					fprintf(stdprn,"%x  ",
						*(physical_pages_pointer + i));
					fflush(stdprn);
				}

				save_map_handle_page = 'f';
				map_handle_page(current_logical_page,
					current_physical_page,handle);



				break;
			}

#endif

#ifdef kkskdkddkdkdkkdk
			case 110 : /* alt F7 (display my_sub6) */
			{

				fprintf(stdprn,"\n\nTHIS is MY_SUB6\n\n");

				for(i = 0 ; i <= 
					sub_insert_ptrs[NUMBER_OF_SUBS] ;
						i++)
				{
					fprintf(stdprn,
					    "opcode = %d  arg1 = %d  arg2 = %d\n",
						my_sub6[i].op_code,
						my_sub6[i].arg1,
						my_sub6[i].arg2);
				}

				fflush(stdprn);

				break;
			}
#endif

#ifdef emdmdmdnmdndndn
			case 111 : /*alt F8 (display a non-interpreted sub)*/
			{
				printf("Enter sub's number: ");
				scanf("%d",&which_logical_page);

				printf("THIS IS SUB %d's display list\n\n",
					which_logical_page);

				for(i = 0 ; i < 50 ; i++)
					printf("  %x",sub_struct_ptr ->
						sub_display_lists[
						which_logical_page][i]);

				break;
			}
#endif

			case 75 : /*reject (left arrow)*/
			{
				my_send_ext(61);
				break;
			}

			case 77 : /*entry complete (right arrow)*/
			{
				my_send_ext(60);
				break;
			}

			case 79 : /*End key(toggle display of alt menu on/off)*/
			{
				break;

/*
    NO REASON TO TURN OFF LIGHT DATA ANYMORE (just as fast off or on)
	(Just confusing to have light data vanish if you hit the End key by
		mistake)

				if(do_ugouts_only == 't')
					break;


				if(display_alt_menu == 't')
				{
					alt_menu_off();
				}
				else
					alt_menu_on();

				break;
*/

			}


			case 80 : /*terminate (down arrow)*/
			{
				my_send_ext(62);
				break;
			}



			case 81 : /* page down (main window position toggle) */
			{
				if(do_ugouts_only == 't')
					break;


				toggle_retraction(MAIN_MENU);

				break;
			}



			case 72 : /*alternate action (up arrow)*/
			{
				my_send_ext(63);
				break;
			}

			case 30 : /*alternate action (alt A)*/
			{
				my_send_ext(63);
				break;
			}

			case 48 : /*blank/unblank (alt B)*/
			{
				my_send_ext(50);
				break;
			}

			case 46 : /*calculator (alt C)*/
			{
				my_send_ext(51);
				break;
			}

			case 32 : /*display control (alt D)*/
			{
				my_send_ext(58);
				break;
			}

			case 18 : /*edit work view (alt E)*/
			{
				my_send_ext(59);
				break;
			}

			case 33 : /*file/terminate (alt F)*/
			{
				my_send_ext(34);
				break;
			}

			case 38 : /*layer (alt L)*/
			{
				my_send_ext(57);
				break;
			}

			case 50 : /*macros (alt M)*/
			{
				my_send_ext(32);
				break;
			}

			case 120 : /*change module (alt 1)*/
			{
				my_send_ext(33);
				break;
			}

			case 121 : /*module function (alt 2)*/
			{
				my_send_ext(55);
				break;
			}

			case 122 : /*module parameters (alt 3)*/
			{
				my_send_ext(52);
				break;
			}


			case 73 : /* page up */
			{
				doing_an_alt_r = 't';
				my_send_ext(35);
				break;
			}

			case 19 : /*redisplay (alt R)*/
			{
				my_send_ext(35);
				break;
			}

			case 31 : /*system parameters (alt S)*/
			{
				my_send_ext(53);
				break;
			}

			case 17 : /*wcs control (alt W)*/
			{
				my_send_ext(54);
				break;
			}

			case 44 : /*zoom/pan/regen (alt Z)*/
			{
				my_send_ext(56);
				break;
			}

		}   /*end extended key switch*/

	}  /*end extended key case*/

/*
    }  end while keys are pressed
*/


}   /*end my_pckb()*/


