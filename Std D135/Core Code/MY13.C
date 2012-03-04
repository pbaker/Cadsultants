#include "my_globs.h"
#include "memory.h"



#define VT100_MODE  0
#define MESS_MON  1



extern void far vt100_pckb(void);
extern int far vt100_mess_mon_flag;



extern int far megatek_seek(int,long);

extern void far my_ugout();
extern int far keyboard_status();
extern void far megatek_device();
extern void far clear_page();
extern void far alt_menu_on();
extern void far alt_menu_off();
extern void far my_set_viewport(int,int,int,int,int);
extern char far display_alt_menu;

extern void far clear_my_listing(void);


char far doing_an_alt_r = 'f';
char far doing_an_alt_u = 'f';


/*contains "C" versions of Paul's stuff*/
/*nov 8, 1988 changed my_pckb()'s normal charactor code to send the char
to the host only if doing_redisplay == 'f'*/


void far my_send_ext(ch)
int ch;
{

	my_ugout(31);
	my_ugout(97);
	my_ugout(ch);
}



void far read_keyboard()
{
	my_input_regs.h.ah = 0;

	int86(0x16,&my_input_regs,&my_output_regs);

	type_char_read = my_output_regs.h.al;
	value_char_read = my_output_regs.h.ah;
}



int far sub_seg_number;



void far my_pckb()
{
int save_which_cseg;
int zeke;
int row,col;



	if(vt100_mess_mon_flag == VT100_MODE)
	{
		vt100_pckb();
		return;
	}


	if(keyboard_status() == -1)   /*no key was pressed*/
		return;

	read_keyboard();

	if(type_char_read != 0)   /*normal key*/
	{
	#ifdef HAVE_DIAGS
		if(type_char_read == 0x01)
			megatek_device(1);
		else if(type_char_read == 0x02)
			megatek_device(2);
		else
	#endif
		if(doing_redisplay == 'f')
			my_ugout(type_char_read);
		else if((type_char_read == 's') || (type_char_read == 'S'))
				quit_redis = 't';

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
				doing_an_alt_u = 't';
				megatek_device(4);
				doing_an_alt_u = 'f';
				break;
			}


			case 71 : /*home*/
			{
				megatek_device(5);
				break;
			}

			case 82 : /*Ins key  (clear graphics monitor)*/
			{
				clear_my_listing();
				my_set_viewport(-2048,-2048,2048,2048,2);
				clear_page(2 /*both pages*/);
				my_set_viewport(headers_segs[which_cseg].
						save_x_lower_clip,
					headers_segs[which_cseg].
						save_y_lower_clip,
					headers_segs[which_cseg].
						save_x_upper_clip,
					headers_segs[which_cseg].
						save_y_upper_clip,
					which_page);

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



			case 108 : /*alt F5  (show a segment start) */
			{
				save_which_cseg = which_cseg;

				printf("Enter segment #   ");
				scanf("%d",&sub_seg_number);
				fprintf(stdprn,
				    "\n\nTHIS IS THE start of segment %d\n\n",
					 sub_seg_number);

				megatek_seek(sub_seg_number,0L);


				for(zeke = 0 ; zeke < 32 ; zeke++)
				{
					memcpy(&an_element,
					   current_megatek_pointer +
						zeke*size_an_element,
							size_an_element);

					fprintf(stdprn,
					    "opcode = %3d  arg1 = %5d  arg2 = %5d  arg3 = %5d  arg4 = %5d\n",
						an_element.op_code,
						an_element.arg1,
						an_element.arg2,
						an_element.arg3,
						an_element.arg4);
				}


				fflush(stdprn);
				which_cseg = save_which_cseg;

				break;
			}



			case 109 : /*alt F6  (print megatek_locate) */
			{
				fprintf(stdprn,"\n\nMegatek_locate :\n\n");

				for(row = 0 ; row < 21 ; row++)
				{
					for(col = 0 ; col < 12 ; col++)
						fprintf(stdprn,"%5d",
							megatek_locate[row]
								[col] );

					fprintf(stdprn,"\n");
				}

				fprintf(stdprn,"\n\n");
				fflush(stdprn);


				break;
			}



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
				if(display_alt_menu == 't')
				{
					alt_menu_off();
				}
				else
					alt_menu_on();

				break;
			}

			case 80 : /*terminate (down arrow)*/
			{
				my_send_ext(62);
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


			case 73 :   /* page up (PgUp  key) */
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
		}
	}

}




