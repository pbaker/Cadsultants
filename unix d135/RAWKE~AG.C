#include <termio.h>
#include <fcntl.h>
#include <signal.h>


struct termio keybd_state;
unsigned int keybd_flags;
unsigned int char_pending = 0;
char char_value;
unsigned int raw_enabled;

void pw_kbd_restore();

void pw_trap_c()
{
	pw_kbd_restore();
	exit(1);   
}

int pw_kbd_setraw()
{
	struct termio tbuf;
	int flags;

	if(ioctl(0,TCGETA,&keybd_state) == -1)
	{
		printf("\n Error in keyboard init : 1\n");
		return(0);
	}

	tbuf = keybd_state;
	tbuf.c_lflag &= ~(ICANON | ECHO);
	tbuf.c_cc[4] = 100;  /*min*/
	tbuf.c_cc[5] = 1;  /*time*/

	if(ioctl(0,TCSETAF,&tbuf) == -1)
	{
		printf("\nError in keyboard init : 2\n");
		return(0);
	}

	if((keybd_flags = fcntl(0,F_GETFL,0)) == -1)
	{
		printf("\n Error in keyboard init : 3\n");
		return(0);
	}

	flags = keybd_flags | O_NDELAY;

	if(fcntl(0,F_SETFL,flags) == -1)
	{
		printf("\n Error in keyboard init : 4\n");
		return(0);
	}

	raw_enabled = 1;
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	read(0,&char_value,10);
	return(1);
}


void pw_kbd_restore()
{
	/*
	flush input and output queues, and reset to old 'state'
	*/

	raw_enabled = 0;
	signal(SIGINT,SIG_DFL);
	signal(SIGQUIT,SIG_DFL);

	if(ioctl(0,TCFLSH,2) == -1)
		printf("\nError reseting keyboard : 1\n");

	if(ioctl(0,TCSETAF,&keybd_state) == -1)
		printf("\nError reseting keyboard : 2\n");

	if(fcntl(0,F_SETFL,&keybd_flags) == -1)
		printf("\nError reseting keyboard\n");

}


int kbhit()
{
	if(raw_enabled)
	{
		char_pending = read(0,&char_value,1);
		return(char_pending);
	}

	return(0);
}


char get_ch()
{
char ch;

	if(char_pending)
	{
		char_pending = 0;
		return(char_value);
	}

	if(read(0,&ch,1) == 0)
		return(0);

	return(ch);
}


