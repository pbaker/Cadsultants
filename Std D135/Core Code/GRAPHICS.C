#include "stdio.h"
#include "ctype.h"
#include "string.h"



#define TRUE 1
#define FALSE 0


extern void far read_lock();
extern void far restore_int9();
extern void far read_config_file();


extern char far drive_letter;
extern int far init_baud_rate;
extern char far path_error_log[];
extern int far curser_ratio;
extern char far print_list;
extern int far list_size;
extern char far rotate_mode;
extern char far time_redisplay;



FILE *fopen(),*mfile;

int far init_artist10(void);

int arry[100];
int bits_per_pixel,base_port,maxx,maxy,single_dual,board_add;
int is_eof = 9;
int far syncgreen,far sync1,far sync2;


char far ems_mode;
char far have_display_list;
char far which_high_res;




void getcolon(f_ptr)
FILE *f_ptr;
{
int ch;


	for(ch = getc(f_ptr) ; (ch != ':') && (ch != EOF) ; ch = getc(f_ptr))
		;

	if(ch == EOF)
		is_eof = EOF;
}


int intgr(num1,num2)
int num1,num2;
{
	return((num1 << 8) + num2);
};


 

void set_art_colors()
{
	usrcol(0,0,0,0);
	usrcol(1,0,0,60);
	usrcol(2,0,60,0);
	usrcol(3,0,0,60);
	usrcol(4,60,0,0);
	usrcol(5,0,60,0);
	usrcol(6,60,60,0);
	usrcol(7,0,60,60);
	usrcol(8,16,32,8);
	usrcol(9,60,0,0);
	usrcol(10,32,16,16);
	usrcol(11,60,0,60);
	usrcol(12,28,0,32);
	usrcol(13,60,60,0);
	usrcol(14,12,40,28);
	usrcol(15,60,60,60);

}





void read_art10(fp)
FILE *fp;
{
int ix,i;
int not_used1,not_used2;

char drive_ok,baud_ok;
char test_name[50];

         ix = 0;
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix],&arry[ix+1]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+2],&arry[ix+3]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+4],&arry[ix+5]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+6],&arry[ix+7]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+8],&arry[ix+9]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+10],&arry[ix+11]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+12],&arry[ix+13]);
         getcolon(fp); fscanf(fp,"%x%x%x%x%x%x",&arry[ix+14],&arry[ix+15],
			&arry[ix+16],&arry[ix+17],&arry[ix+18],&arry[ix+19]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+20],&arry[ix+21]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+22],&arry[ix+23]);
         getcolon(fp); fscanf(fp,"%x%x",&arry[ix+24],&arry[ix+25]);
         getcolon(fp); fscanf(fp,"%x%x%x%x%x%x%x%x",&arry[ix+26],&arry[ix+27],
			&arry[ix+28],&arry[ix+29],&arry[ix+30],&arry[ix+31],
			&arry[ix+32],&arry[ix+33]);
         getcolon(fp); fscanf(fp,"%x%x%x%x%x%x%x%x",&arry[ix+34],&arry[ix+35],
			&arry[ix+36],&arry[ix+37],&arry[ix+38],&arry[ix+39],
			&arry[ix+40],&arry[ix+41]);
         getcolon(fp); fscanf(fp,"%x%x%x%x%x%x%x%x",&arry[ix+42],&arry[ix+43],
			&arry[ix+44],&arry[ix+45],&arry[ix+46],&arry[ix+47],
			&arry[ix+48],&arry[ix+49]);
         getcolon(fp); fscanf(fp,"%x%x%x%x%x%x%x%x",&arry[ix+50],&arry[ix+51],
			&arry[ix+52],&arry[ix+53],&arry[ix+54],&arry[ix+55],
			&arry[ix+56],&arry[ix+57]);
         getcolon(fp); fscanf(fp,"%x",&arry[ix+58]);
         getcolon(fp); fscanf(fp,"%x",&maxx);
         getcolon(fp); fscanf(fp,"%x",&maxy);
         getcolon(fp); fscanf(fp,"%x",&base_port);
         getcolon(fp); fscanf(fp,"%x",&bits_per_pixel);
         getcolon(fp); fscanf(fp,"%x",&single_dual);

/*read the unused stuff*/
	 getcolon(fp); fscanf(fp,"%x%x",&not_used1,&not_used2);  /*aspect ratio*/
	 getcolon(fp); fscanf(fp,"%x",&syncgreen);  /*sync on green*/
	 getcolon(fp); fscanf(fp,"%x%x",&sync1,&sync2); /*external syncs*/
}






int far init_artist10()
{
	if(!(mfile = fopen("artist10.set","r")))    /* couldn't open it */
	{
		printf("Couldn't open ARTIST 10 initialization file\n");
		restore_int9();
		exit();
	}
	else
	{

		read_art10(mfile);

		fclose(mfile);

		read_lock();
		read_config_file();
	}


	initbd();

	sart10(bits_per_pixel);

	sports(base_port,0,0);

	ssync1(arry[6],arry[7],arry[8],arry[9]);

	ssync2(arry[10],arry[11],arry[12],arry[13]);

	lsync(maxx/16,maxx,maxy,intgr(arry[36],arry[37]));

	splscn(intgr(arry[14],arry[15]),intgr(arry[16],arry[17]),
		intgr(arry[18],arry[19]));
	contrg(intgr(arry[0],arry[1]),
		intgr(arry[2],arry[3]),intgr(arry[4],arry[5]));

	upscn(intgr(arry[26],arry[27]),intgr(arry[28],arry[29]),
		intgr(arry[30],arry[31]),intgr(arry[32],arry[33]));

	basscn(intgr(arry[34],arry[35]),intgr(arry[36],arry[37]),
		intgr(arry[38],arry[39]),intgr(arry[40],arry[41]));

	lowscn(intgr(arry[42],arry[43]),intgr(arry[44],arry[45]),
		intgr(arry[46],arry[47]),intgr(arry[48],arry[49]));

	winscn(intgr(arry[50],arry[51]),intgr(arry[52],arry[53]),
		intgr(arry[54],arry[55]),intgr(arry[56],arry[57]));

	hvscrn(arry[20],arry[21]);

	vwstrt(arry[22],arry[23]);

	vwwide(arry[24],arry[25]);
	
	syncrg(syncgreen,sync1,sync2);

	bitmod(bits_per_pixel,single_dual);



	igdc();

	ldpal();




	set_art_colors();  /* sets up the color registers */

	if(bits_per_pixel == 8)
		return(255);
	else
		return(15);

};  /* end init_artist10 */

