
#include <stdio.h>


extern int pw_kbd_setraw();
extern void pw_kbd_restore();
extern int kbhit();
extern char get_ch();

char raw_buffer[256];
int type_char_read,value_char_read;


void function_key()
{
	if((raw_buffer[2] >= 'P') && (raw_buffer[2] <= 'Y')) 
		value_char_read = raw_buffer[2] - 'P' + 0x3b;
	else
		value_char_read = 0;

	type_char_read = 0;
}



void alt_function_key()
{
	if((raw_buffer[2] == 0x1b) && (raw_buffer[3] == 'O'))
	{
		if((raw_buffer[4] >= 'P') && (raw_buffer[4] <= 'Y'))
			value_char_read = raw_buffer[4] - 'P' + 104;
		else
			value_char_read = 0;
	}
	else
		 value_char_read = 0;

	type_char_read = 0;
}



void alt_normal_key()
{
	type_char_read = 0;

	if((raw_buffer[2] >= '1') && (raw_buffer[2] <= '9'))
		value_char_read = raw_buffer[2] - '1' + 120;
	else if(raw_buffer[2] == '0')
		value_char_read = 129;
	else
		switch(raw_buffer[2])
		{
			case 'q' :
			case 'Q' :
			{
				value_char_read = 16;
				break;
			}

			case 'w' :
			case 'W' :
			{
				value_char_read = 17;
				break;
			}

			case 'e' :
			case 'E' :
			{
				value_char_read = 18;
				break;
			}

			case 'r' :
			case 'R' :
			{
				value_char_read = 19;
				break;
			}

			case 't' :
			case 'T' :
			{
				value_char_read = 20;
				break;
			}

			case 'y' :
			case 'Y' :
			{
				value_char_read = 21;
				break;
			}

			case 'u' :
			case 'U' :
			{
				value_char_read = 22;
				break;
			}

			case 'i' :
			case 'I' :
			{
				value_char_read = 23;
				break;
			}

			case 'o' :
			case 'O' :
			{
				value_char_read = 24;
				break;
			}

			case 'p' :
			case 'P' :
			{	
				value_char_read = 25;
				break;
			}



			case 'a' :
			case 'A' :
			{
				value_char_read = 30;
				break;
			}

			case 's' :
			case 'S' :
			{
				value_char_read = 31;
				break;
			}

			case 'd' :
			case 'D' :
			{
				value_char_read = 32;
				break;
			}

			case 'f' :
			case 'F' :
			{
				value_char_read = 33;
				break;
			}

			case 'g' :
			case 'G' :
			{
				value_char_read = 34;
				break;
			}

			case 'h' :
			case 'H' :
			{
				value_char_read = 35;
				break;
			}

			case 'j' :
			case 'J' :
			{
				value_char_read = 36;
				break;
			}

			case 'k' :
			case 'K' :
			{
				value_char_read = 37;
				break;
			}

			case 'l' :
			case 'L' :
			{
				value_char_read = 38;
				break;
			}


			case 'z' :
			case 'Z' :
			{
				value_char_read = 44;
				break;
			}

			case 'x' :
			case 'X' :
			{
				value_char_read = 45;
				break;
			}

			case 'c' :
			case 'C' :
			{
				value_char_read = 46;
				break;
			}

			case 'v' :
			case 'V' :
			{
				value_char_read = 47;
				break;
			}

			case 'b' :
			case 'B' :
			{
				value_char_read = 48;
				break;
			}

			case 'n' :
			case 'N' :
			{
				value_char_read = 49;
				break;
			}

			case 'm' :
			case 'M' :
			{
				value_char_read = 50;
				break;
			}

			default :
				value_char_read = 0;
		}

}


void other_extended_key()
{
	type_char_read = 0;

	switch(raw_buffer[2])
	{
		case 'D' :  /* reject arrow */
		{
			value_char_read = 75;
			break;
		}

		case 'C' :  /* entry complete arrow */
		{
			value_char_read = 77;
			break;
		}

		case 'B' :  /* terminate arrow */
		{
			value_char_read = 80;
			break;
		}

		case 'A' :  /* alt. action arrow */
		{
			value_char_read = 72;
			break;
		}

		case 'H' :  /* home key */
		{
			value_char_read = 71;
			break;
		}

		case 0x40 :  /* At Sign in hex (Ins key) */
		{
			value_char_read = 82;
			break;
		}

		case 'Y' :  /*  End key */
		{
			value_char_read = 79;
			break;
		}

		case 'V' :  /* Page Up key */
		{
			value_char_read = 73;
			break;
		}

		case 'U' :  /* Page Down key */
		{
			value_char_read = 81;
			break;
		}

		default :
			value_char_read = 0;
	}
}


void read_keyboard()
{
int i;

	if(kbhit() == 0)
	{
		type_char_read = value_char_read = 0;
		return; 
	}

	raw_buffer[0] = get_ch();
	for(i = 1 ; kbhit() != 0 ; i++)
		raw_buffer[i] = get_ch();

	raw_buffer[i] = '\0';


	if(raw_buffer[0] == 0x1b)
	{
		if(raw_buffer[1] == '\0')
		{
			type_char_read = 0x1b;
			value_char_read = 0;
			return;
		}
		else
		{
			switch(raw_buffer[1])
			{
				case 'O' :  
				{
					function_key();
					break;
				}

				case 'a' :
				{
					alt_function_key();
					break;
				}

				case 'N' :
				{
					alt_normal_key();
					break;	
				}

				case '[' :
				{
					other_extended_key();
					break;	
				}

				default :
				{
					value_char_read = type_char_read = 0;
					break;
				}
			}
		}
	}
	else   /* normal non-extended char */
	{
		type_char_read = raw_buffer[0];
		value_char_read = 0;
		return;	
	}

}



main()
{
	if(pw_kbd_setraw() == 0)
	{
		printf("\nError setting keyboard to raw mode\n");
		exit(0);
	}

	type_char_read = 0;

	while((type_char_read != 'q') && (type_char_read != 'Q') &&
		(type_char_read != 's') && (type_char_read != 'S'))
	{
		read_keyboard();

		if((type_char_read != 0) || (value_char_read != 0))
		{
			printf("type = %d = 0x%x , value = %d = 0x%x\n",
				type_char_read,type_char_read,value_char_read,
				value_char_read);
			fflush(stdout);
		}
	}


	pw_kbd_restore();
}



