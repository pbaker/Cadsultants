#include "stdio.h"
#include "ctype.h"


extern int far init_baud_rate;
extern void far clear_display_page();
extern void far goto_row_col(int,int);


FILE *fopen(),*mfile;

int far init_artist10(void);
int far is_eof = 4;



void getcolon(f_ptr)
FILE *f_ptr;
{
int ch;


	for(ch = getc(f_ptr) ; (ch != ':') && (ch != EOF) ; ch = getc(f_ptr))
		;

	if(ch == EOF)
		is_eof = EOF;
}




void read_art10(fp)
FILE *fp;
{
		if(fp == NULL)
			goto baud_again;


		getcolon(fp);
		if(is_eof == EOF)
			goto baud_again;

		fscanf(fp,"%d",&init_baud_rate);

		switch(init_baud_rate)
		{
			case 6 :
				return;

			case 12 :
				return;

			case 24 :
				return;

			case 48 :
				return;

			case 96 :
				return;
		}
		

		
baud_again:
		printf("Enter baud rate:\n");
		printf("   0) 1200\n");
		printf("   1) 2400\n");
		printf("   2) 4800\n");
		printf("   3) 9600\n");
		printf("   4) 19200\n\n");
		printf("  ? ");

		scanf("%d",&init_baud_rate);


		if(init_baud_rate == 0)
			init_baud_rate = 96;
		else if(init_baud_rate == 1)
			init_baud_rate = 48;
		else if(init_baud_rate == 2)
			init_baud_rate = 24;
		else if(init_baud_rate == 3)
			init_baud_rate = 12;
		else if(init_baud_rate == 4)
			init_baud_rate = 6;
		else
		{
			printf("Invalid baud rate indicator\n");
			goto baud_again;
		}
		
}






int far init_artist10()
{

	if(!(mfile = fopen("d90.set","r")))    /* couldn't open it */
	{
		printf("Couldn't open D90.SET baud rate initialization file\n");

		read_art10(mfile);
	}
	else
	{
		read_art10(mfile);
		fclose(mfile);
	}



	return(255);

};  /* end init_artist10 */


