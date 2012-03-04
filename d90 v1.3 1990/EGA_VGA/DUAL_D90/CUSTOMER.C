#include "stdio.h"
#include "graph.h"
#include "dos.h"



extern union REGS far my_input_regs,far my_output_regs;



char far *release_info[11] =
{
	"PC WorkWindow EGA",
	"Copyright 1989  Cadsultants, Inc",
	"ALL RIGHTS RESERVED",
	"\0",
	"Version  1.27",
	"ID #  1046",
	"\0",
	"Licensed To:",
	"  Bernie Tull",
	"  AT&T",
	"  Shreveport, LA"
};




void far customer_id()
{
int i,a_color;
long a_delay;



/* turn off cursor */
	my_input_regs.h.ah = 1;
	my_input_regs.x.cx = 0x2000;

	int86(0x10,&my_input_regs,&my_output_regs);


	_settextcolor(2);
	_settextposition(23,26);
	_outtext("Press Any Key...");


	for(a_color = 1 ; kbhit() == 0 ; a_color++)
	{
		if(a_color > 15)
			a_color = 1;


		_settextcolor(a_color);


	
		for(i = 0 ; i < 11 ; i++)
		{
			_settextposition(i + 7,26);
			_outtext(release_info[i]);
		}
	

		for(a_delay = 0L ; a_delay < 50000L ; a_delay = a_delay + 1L)
			;
	}




/* turn on cursor */
	my_input_regs.h.ah = 1;
	my_input_regs.h.ch = 6;
	my_input_regs.h.cl = 7;

	int86(0x10,&my_input_regs,&my_output_regs);
}

