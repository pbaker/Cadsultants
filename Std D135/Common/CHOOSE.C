#include "stdio.h"
#include "my_bios.h"



#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3



char far controls[3][50] =
{
	{26,' ','E','n','t','r','y',' ','C','o','m','p','l','e','t','e','\0'},
	{27,' ','R','e','j','e','c','t',' ',' ',' ',' ',' ',' ',' ',' ','\0'},
	{25,' ','T','e','r','m','i','n','a','t','e',' ',' ',' ',' ',' ','\0'}
};




void far control_window()
{
int i;

	make_win(45,15,64,19);
	clear_win(REVERSE_VIDEO);
	draw_box();

	
	for(i = 0 ; i < 3 ; i++)
		reverse_string(controls[i],win_y_low + 1 + i,win_x_low + 2);


	make_win(10,3,60,17);
}




int far choose_menu(header_text,text_options,num_options,the_default)
char far *header_text;
char far *text_options[];
int num_options,the_default;
{
int i,ch;


	while(2345)
	{
		make_win(10,3,60,17);
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("  %s",header_text);

		for(i = 0 ; i < num_options ; i++)
		{
			if(*(text_options[i]) != '\0')
			{
				goto_row_col(win_y_low + 3 + i,win_x_low + 1);
				if(i + 1 < 10)
					printf("    F%d %s",i + 1,
						text_options[i]);
				else
					printf("   F%d %s",i + 1,
						text_options[i]);
			}
		}

		if(the_default > 0)
		{
			goto_row_col(win_y_low + 2 + the_default,win_x_low + 7);
			putchar('>');
		}


		control_window();

		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();


		switch(ch)
		{
			case 75 :
				return(REJECT);

			case 77 :
				return(ENTRY_COMPLETE);

			case 80 :
				return(TERMINATE);

			case 0x47 :
				return(0x47);

			default :
			{
				ch -= 0x3b;

				if((ch >= 0) && (ch < num_options))
					return(ch + 1);
			}
		}
	}
}







int far information_menu(header_text,text_array,num_rows,do_read,in_a_loop)
char far *header_text;
char far *text_array[];
int num_rows;
char do_read,in_a_loop;
{
int i,ch;


	make_win(10,3,60,17);

	if(in_a_loop == 'f')
	{
		clear_win(REVERSE_VIDEO);
		draw_box();
	}

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("  %s",header_text);

	for(i = 0 ; i < num_rows ; i++)
	{
		goto_row_col(win_y_low + 3 + i,win_x_low + 1);
		printf("  %s",text_array[i]);
	}


	if(in_a_loop == 'f')
		control_window();


	if(do_read == 'f')
		return;


	while(879)
	{
		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();


		switch(ch)
		{
			case 75 :
				return(REJECT);

			case 77 :
				return(ENTRY_COMPLETE);

			case 80 :
				return(TERMINATE);
		}
	}
}




void far error_message(window_row,the_text,num_rows)
int window_row;
char far *the_text[];
int num_rows;
{
int i;


	for(i = 0 ; i < num_rows ; i++)
	{
		goto_row_col(win_y_low + window_row + i,win_x_low + 4);
		printf("%s",the_text[i]);
	}

	goto_row_col(win_y_low + window_row + i + 1,win_x_low + 4);
	printf("Press Enter To Continue");


	if(getch() == 0)
		getch();
}

