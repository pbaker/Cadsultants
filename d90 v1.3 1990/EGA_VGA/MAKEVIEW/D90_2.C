#include "display.h"
#include "math.h"
#include "stdlib.h"
#include "generics.h"

#define TRUE 1
#define FALSE 0
#define ctrl_Z 26
#define ctrl_E 5
#define escape 27
#define set 1
#define clear 0
#define cr 13



/*externs for display file stuff*/
extern struct a_word far an_element;
extern struct a_word far read_element;
extern char far write_display_error;
extern char far quit_redisplay;
extern void far insert_word(void);
extern void far get_word();
extern void far redisplay();


extern void far d90_device();
extern void far set_color();
extern void far display_indicator();
extern void far select_erase();
extern void far set_video_lookup();
extern void far ugout();

extern char far filter_type;
extern char far controller_type;
extern char far display_flag;
extern int far my_x;
extern int far my_y;
extern int far current_color;
extern int far line_style;


char rgb_config[] = {
				0,0,0,		/*0*/
				0,0,15,		/*1*/
				0,15,0,		/*2*/
				0,15,15,	/*3*/
				15,0,0,		/*4*/
				15,0,15,	/*5*/
				15,15,0,	/*6*/
				15,15,15,	/*7*/
				4,8,2,		/*8*/
				15,6,6,		/*9*/
				8,4,4,		/*10*/
				15,8,0,		/*11*/
				7,0,8,		/*12*/
				9,0,0,		/*13*/
				3,10,7,		/*14*/
				6,6,6		/*15*/
			 };


int syscolor_index = 7;


char genisco_buffer[60] = {0,0,0,0,0,0,0,0,0,0,
			   0,0,0,0,0,0,0,0,0,0,
			   0,0,0,0,0,0,0,0,0,0,
			   0,0,0,0,0,0,0,0,0,0,
			   0,0,0,0,0,0,0,0,0,0,
			   0,0,0,0,0,0,0,0,0,0
			  };

char lescape_flag = FALSE,function_flag = FALSE;
int buff_start = 0,buff_cnt = 0,buff_end = 0;
char far video_index = 0,far red_gain = 0,far green_gain = 0,far blue_gain = 0;
int rgb = 0,i,si;



void far megatek_device(command)
{
	command &= 0xff;

	if(lescape_flag != set)
	{
		if(command == escape)
			lescape_flag = set;
		else
			d90_device(command);
	}
	else
	{
		if(command == 'x')
		{
			select_erase(command);
			lescape_flag = clear;
		}
		else if(command == 'y')
		{
			select_erase(command);
			lescape_flag = clear;
		}
		else if(function_flag == FALSE)
		{
			if(command == 'G')
			{
				function_flag = TRUE;
			}
			else
			{
				lescape_flag = clear;
				function_flag = clear;
				d90_device(escape);
				d90_device(command);
			}
		}
		else
		{   /*genisco extension*/
			if(command != cr)
			{
				genisco_buffer[buff_end] = (char)command;
				buff_end++;
				buff_cnt++;
			}
			else   /*read genisco buffer*/
			{
				genisco_buffer[buff_end] = cr;
				lescape_flag = clear;
				function_flag = clear;

				switch(genisco_buffer[0])
				{
					case 'O' :
					{
						display_indicator(my_x,my_y);
						buff_start = 0;
						buff_cnt = 0;
						buff_end = 0;
						break;
					}

					case 'C' :
					{
						set_color(atoi(genisco_buffer + 1));

						#ifdef HAVE_DISP_FILE
							an_element.op_code = 3;
							an_element.arg1 =
								atoi(genisco_buffer + 1);

							insert_word();
						#endif

						buff_start = 0;
						buff_cnt = 0;
						buff_end = 0;
						break;
					}

					case 'V' :
					{
						if(genisco_buffer[2] == ',')
						{    /*single_index*/
							si = 1;
							video_index = 
								genisco_buffer[1]
									- 48;
							goto get_rgb;
						}
						else   /*double_index*/
						{
							si = 2;
							video_index =
								genisco_buffer[2]
									- 38;
					get_rgb:
							rgb = atoi(genisco_buffer 
								+ si + 2);

							red_gain = rgb & 0x0f;
							green_gain = ((rgb & 0xf0)
								>> 4) & 0x0f;
							blue_gain = (rgb & 0x0f00)
								>> 8;

							set_video_lookup();

							rgb_config[3*video_index]
								= red_gain;

							rgb_config[3*video_index
								+ 1] = green_gain;

							rgb_config[3*video_index
								+ 2] = blue_gain;



						}


/*						printf("\n\nV_function: Genisco_buffer = ");
						for(i = 0 ; genisco_buffer[i]
							!= cr ; i++)
							printf("%c",genisco_buffer[i]);

						printf("\n\n");
*/

						buff_start = 0;
						buff_cnt = 0;
						buff_end = 0;

						break;
					}


					case 'E' : /*set attributes*/
					{
						syscolor_index = 
							atoi(genisco_buffer + 3);
						
/*						red_gain = rgb_config[3*
							syscolor_index];
						green_gain = rgb_config[3*
							syscolor_index + 1];
						blue_gain = rgb_config[3*
							syscolor_index + 2];

						set_video_lookup();
*/
						buff_start = 0;
						buff_cnt = 0;
						buff_end = 0;
						break;
					}

					default :
					{
						buff_start = 0;
						buff_cnt = 0;
						buff_end = 0;
					}
				
				}

			}
		}
	}
	

}



