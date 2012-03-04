#include <stdio.h>
#include <pw_lib.h>
#include "mywindows.h"          /* for menu name constants ONLY*/


#define RES_1024_768 3
#define RES_1280_1024 4

#define HOST_BLUE  0x33
#define HOST_WHITE  0xff
#define HOST_CYAN  0x77 

extern char  version_str[];
extern char  which_high_res;
extern char  is_there_graphics;
extern int  global_screen_color;

extern void  color_reg();
extern void  clear_display_page();
extern void  goto_row_col();
extern void  my_size_chars();
extern void  my_set_viewport();


extern int  init_baud_rate;
extern char  serial[];
extern int  list_size;
extern int  which_page;
extern OPCODE  io_masks[];
extern int  window_shift;

extern void wait_read_keyboard();
extern int type_char_read,value_char_read;



char  *license_agreement[] = 
{
"                  SOFTWARE LICENSE AGREEMENT ",
"\0",
"CADSULTANTS, INCORPORATED (\"CADSULTANTS\") SOFTWARE LICENSE",
"\0",
"This software and the diskette on which it is contained ",
"(the \"Licensed Software\") is licensed to you, the end-user, for your own",
"use, on a single personal computer. You do not obtain title to the Licensed",
"Software or any copyrights or proprietary rights in the Licensed Software. ",
"You may not transfer, sub-license, rent, convey, modify, translate, ",
"convert to another programming language, decompile or disassemble the",
"Licensed Software for any purpose.",
"\0",
"THE LICENSED SOFTWARE IS PROVIDED \"AS-IS\". ALL WARRANTIES AND",
"REPRESENTATIONS OF ANY KIND WITH REGARD TO THE LICENSED SOFTWARE",
"ARE HEREBY DISCLAIMED, INCLUDING THE IMPLIED WARRANTIES OF",
"MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. UNDER NO",
"CIRCUMSTANCES WILL THE MANUFACTURER OR DEVELOPER OF THE LICENSED SOFTWARE",
"BE LIABLE FOR ANY CONSEQUENTIAL, INCIDENTAL, SPECIAL EXEMPLARY DAMAGES",
"EVEN IF APPRISED OF THE LIKELIHOOD OF SUCH DAMAGES OCCURING.",
"\0",
"Press  ENTER  to Continue  Or  HOME  To Exit"
};


int  num_license_lines = sizeof(license_agreement) / sizeof(char  *);



char  *release_info[] =
{
	"Version  1.34            ",
	"D135 Terminal Emulation for the IBM PC",
	"\0",
	"Copyright 1989, 1990  CADSULTANTS, INC",
	"ALL RIGHTS RESERVED",
/*	"\0",
	"\0",
	"Message Monitor :  IBM Compatable Display Adaptor",
	"Graphics        :  High Resolution Monitor",
	"Mouse           :  Microsoft",
	"Host Port       :  COM1",
	"baud rate       :                        ",
	"Serial ID #     :                        "
*/
 /* ID part MUST be the LAST row of array*/
};


int  num_release_lines = sizeof(release_info) / sizeof(char  *);




void  init_some_colors()
{
	color_reg(5,0,60,0);       /* green */
	color_reg(0,0,0,0);        /* black */
	color_reg(3,0,0,60);       /* blue */
	color_reg(7,0,60,60);      /* cyan */
	color_reg(15,60,60,60);    /* white */
}





void  display_the_license()
{
int i;

#ifdef kdskdsdskkd

	clear_display_page();

	for(i = 0 ; i < num_license_lines ; i++)
	{
		goto_row_col(i + 2,1);
		printf("%s",license_agreement[i]);
	}
#endif
}



void  display_the_floppy()
{
int i,x1,y1,x2,y2,viewport_shift;


	sprintf(release_info[0],"Version : %s",version_str);


	linpat(0);


	window(-2048,2048,-2048,2048);

	if(which_high_res == RES_1024_768)
	{
		viewport_shift = (int)(window_shift/5.3333333333);

		x1 = (int)(-2048/5.33333333333) + 512 - viewport_shift;
		x2 = (int)(2048/5.33333333333) + 512 - viewport_shift;
		y1 = (int)((-2048 + 2048)/5.333333333333);
		y2 = (int)((2048 + 2048)/5.333333333333);
	}
	else
	{
		viewport_shift = (int)(window_shift*1.25/5.3333333333);

		x1 = (int)(-2048*1.25/5.33333333333) + 640 - viewport_shift;
		x2 = (int)(2048*1.25/5.33333333333) + 640 - viewport_shift;
		y1 = (int)((-2048 + 2048)/4.0);
		y2 = (int)((2048 + 2048)/4.0);
	}

	viewport((short)x1,(short)x2,(short)y1,(short)y2);



/* floppy outline */
	color(0x33);
	move2(-1800,-1800);
	box(1800,1800);


/* floppy label */
	color(0xff);
	move2(-1600,800);
	box(1600,1700);


	color(0);

	gtstyle(0);
	gtsize(111,111);
	move2(-1500,1500);
	gtext("PC WorkWindow HYPERFORMANCE");


	gtsize(70,70);

	for(i = 0 ; i < num_release_lines ; i++)
	{
		move2(-1500,1300 - 100*i);
		gtext(release_info[i]);
	}





/* Big Hole */
	color(0);
	move2(0,0);
	prmfill(1);
	circle(500);

	color((global_screen_color << 4) | global_screen_color);
	move2(0,0);
	circle(400);

	color(0);


/* Read/Write  Slot */
	move2(0,-900);
	circle(160);

	move2(0,-1600);
	circle(160);

	move2(-160,-900);
	box(160,-1600);



/* Little  Circle  Hole */
	move2(700,-151);
	circle(70);


/* Write Lock Slot */
	color((global_screen_color << 4) | global_screen_color);
	move2(1700,900);
	box(1800,1100);
	color(0);


	gtsize(80,80);
	color(0x55);


/*
	move2(-1800,-1960);
	gtext("Press  ENTER  to Continue  Or  HOME  To Exit");
*/

	if(which_high_res == RES_1024_768)
	{
		window(0,1023,0,767);
		viewport(0,1023,0,767);
	}
	else
	{
		window(0,1279,0,1023);
		viewport(0,1279,0,1023);
	}

	gtsize(13,13);
	gtstyle(2);

	color(0);
	move2(740,697);
	boxr(14*5,18);
	move2(740,667);
	boxr(14*4,18);
	move2(740,637);
	boxr(14*3,18);

	color(HOST_WHITE);


	move2(745,700);
	gtext("ENTER  Begin/Resume");
	move2(745,670);
	gtext("HOME   Control Panel");
	move2(745,640);
	gtext("F10    EXIT TO DOS");
}




void  remove_floppy_drawing()
{
	if(which_high_res == RES_1024_768)
	{
		viewport(0,1023,0,767);
	}
	else
		viewport(0,1279,0,1023);

	if(is_there_graphics == 'f')
	{
		wrmask(0xff);
		flood((short)((global_screen_color << 4) | global_screen_color));
		wrmask(io_masks[which_page]);
	}
	else
		flood((short)((global_screen_color << 4) | global_screen_color));


#ifdef lewlwelewlel
     /* switch  graphics  "pages" */
	which_page = !which_page;
	rdmask(io_masks[which_page]);
	wrmask(io_masks[which_page]);
#endif


	my_set_viewport(-2048,-2048,2048,2048,0);

	gtstyle(2);
	prmfill(0);

	my_size_chars(list_size);
	
}



char  read_home_enter()
{
int ch;

/* read keyboard */
try_again:
	wait_read_keyboard();

	if(type_char_read == 0)
	{
		if(value_char_read == 0x47)    /* home key */
		{
			return('h');
		}
		else if(value_char_read == 0x44)
		{
			if(which_high_res == RES_1024_768)
			{
				viewport(0,1023,0,767);
			}
			else
				viewport(0,1279,0,1023);

			wrmask(0xff);
			flood(0);
			return('e');
		}
		else
		{
			putchar(7);
			goto try_again;
		}
	}
	else if((type_char_read != 13) && (type_char_read != 10))
	{
		putchar(7);
		goto try_again;
	}


	return('c');
}




char  customer_license(to_continue)
char to_continue;
{
int ch;

	display_the_license();
	display_the_floppy();

	ch = read_home_enter();


/*
	remove_floppy_drawing();
*/

/*
	clear_display_page();
*/

	return(ch);
}

