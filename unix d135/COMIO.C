
#include <stdio.h>
#include <termio.h>
#include <fcntl.h>
#include <errno.h>


extern char diag_flag;


struct termio my_termio;
int term_handle = -1;
void pw_kbd_restore();


void open_host(baud_rate)
int baud_rate;
{
	if(term_handle == -1)
	{
		term_handle = open("/dev/tty01",O_RDWR | O_NDELAY);

		if(term_handle == -1)
		{
			printf("\n\nFailed to open /dev/tty01 : Errno = %d\n\n",
				errno);
			pw_kbd_restore();
			exit(0);
		}
		else
			printf("\n\nOpened /dev/tty01 for host communications\n\n");
	}

	ioctl(term_handle,TCGETA,&my_termio);

	my_termio.c_cc[4] = 1;
	my_termio.c_iflag = IXOFF;

	switch(baud_rate)
	{
		case 6 : 
		{
			my_termio.c_cflag = B19200 | CS8 | CREAD | CLOCAL;
			break;
		}


		case 12 : 
		{
			my_termio.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
			break;
		}


		case 24 : 
		{
			my_termio.c_cflag = B4800 | CS8 | CREAD | CLOCAL;
			break;
		}

		case 48 : 
		{
			my_termio.c_cflag = B2400 | CS8 | CREAD | CLOCAL;
			break;
		}


		case 96 : 
		{
			my_termio.c_cflag = B1200 | CS8 | CREAD | CLOCAL;
			break;
		}
	}


	ioctl(term_handle,TCSETA,&my_termio);
}

void close_host()
{
	close(term_handle);
	term_handle = -1;
}

int read_host_buffer()
{
char a_buffer[3];
int check_op;

	check_op = read(term_handle,a_buffer,1);

	if(check_op == -1)
	{
		printf("\nread_host_buffer() Error : errno = %d\n",errno);
		return(-1);
	}

	if(check_op == 0)
		return(-1);
	else
	{

/*
		if(diag_flag == 't')
		{
			if(a_buffer[0] != 0x1b)
				printf("%x ",(int)a_buffer[0]);
			else
				printf("ESC  ");

			fflush(stdout);
		}
*/

		return(a_buffer[0]);
	}
}


void ugout(an_int)
int an_int;
{
unsigned char a_buffer[3];
int check_op;

	a_buffer[0] = an_int & 0xff;
	check_op = write(term_handle,a_buffer,1);

	if(check_op == -1)
		printf("\nugout() Error : errno = %d\n",errno);

}


