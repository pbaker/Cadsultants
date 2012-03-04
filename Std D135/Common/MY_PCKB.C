
#include "stdio.h"
#include "dos.h"



extern void far ugout(int);
extern int far keyboard_status(void);
extern void far menu(void);




#define APPLICATIONS_MODE 0
#define NUMERIC_MODE 1

extern char far curser_key_mode;
extern keypad_key_mode;


extern union REGS far my_input_regs;
extern union REGS far my_output_regs;
extern int far type_char_read;
extern int far value_char_read;



void far read_vt100_keyboard()
{
	my_input_regs.h.ah = 0;

	int86(0x16,&my_input_regs,&my_output_regs);

	type_char_read = my_output_regs.h.al;
	value_char_read = my_output_regs.h.ah;
}




void far keypad_application_mode(variable_char)
int variable_char;
{
	ugout(0x1b);
	ugout('O');
	ugout(variable_char);
}




void far output_curser_key(ch2,ch3)
char ch2,ch3;
{
	ugout(0x1b);
	ugout(ch2);
	ugout(ch3);
}






void far keypad_numbers()
{
	switch(value_char_read)
	{
		case 82 :  /*Ins '0' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('0');
			else
				keypad_application_mode('p');

			break;
		}


		case 79 :  /*End '1' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('1');
			else
				keypad_application_mode('q');

			break;
		}

		case 80 :  /*down arror '2' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('2');
			else
				keypad_application_mode('r');

			break;
		}

		case 81 :  /*PgDn '3' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('3');
			else
				keypad_application_mode('s');


			break;
		}

		case 75 :  /*left arrow '4' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('4');
			else
				keypad_application_mode('t');

			break;
		}

		case 76 :  /* '5' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('5');
			else
				keypad_application_mode('u');

			break;
		}

		case 77 :  /*right arrow '6' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('6');
			else
				keypad_application_mode('v');

			break;
		}

		case 71 : /*home '7' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('7');
			else
				keypad_application_mode('w');

			break;
		}

		case 72 : /*up arrow '8' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('8');
			else
				keypad_application_mode('x');

			break;
		}

		case 73 : /*PgUp '9' key*/
		{
			if(keypad_key_mode == NUMERIC_MODE)
				ugout('9');
			else
				keypad_application_mode('y');

			break;
		}
	}
}




void far vt100_pckb()
{
	if(keyboard_status() == -1)   /*no key was pressed*/
		return;

	read_vt100_keyboard();

	if(type_char_read != 0)   /*normal key*/
	{
		if(type_char_read == 8)
			ugout(127);
		else if((type_char_read >= '0') && (type_char_read <= '9'))
		{
			if((value_char_read >= 2) && (value_char_read <= 11))
				ugout(type_char_read);
			else
				keypad_numbers();
		}
		else if(type_char_read == '.')
		{
			if(value_char_read == 83)  /*del key*/
			{
				if(keypad_key_mode == NUMERIC_MODE)
					ugout('.');
				else
					keypad_application_mode('n');
			}
			else
				ugout('.');

		}
		else
			ugout(type_char_read);

	}
	else          /*extended code key  (not on keypad)*/
	{
		switch(value_char_read)
		{
			case 119 : /* Control home */
			{
				menu();
				break;
			}

			case 72 :  /* up arrow */
			{
				if(curser_key_mode == APPLICATIONS_MODE)
					output_curser_key('[','A');
				else
					output_curser_key('O','A');

				break;
			}

			case 75 :  /* left arrow*/
			{
				if(curser_key_mode == APPLICATIONS_MODE)
					output_curser_key('[','D');
				else
					output_curser_key('O','D');

				break;
			}

			case 77 :  /* right arrow*/
			{
				if(curser_key_mode == APPLICATIONS_MODE)
					output_curser_key('[','C');
				else
					output_curser_key('O','C');

				break;
			}

			case 80 :  /* down arrow */
			{
				if(curser_key_mode == APPLICATIONS_MODE)
					output_curser_key('[','B');
				else
					output_curser_key('O','B');

				break;
			}



		/*  PF 1 - 4 Keys are the same in Numeric & Application Mode */

			case 0x3b :  /* F1 */
			{
				keypad_application_mode('P');
				break;
			}

			case 0x3c :  /* F2 */
			{
				keypad_application_mode('Q');
				break;
			}

			case 0x3d :  /* F3 */
			{
				keypad_application_mode('R');
				break;
			}

			case 0x3e :  /* F4 */
			{
				keypad_application_mode('S');
				break;
			}

		}   /* end Not on keypad switch */

	}     /* End else Not on keypad */

}

