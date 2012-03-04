#include "stdio.h"
#include "dos.h"
#include "my_utils.h"



char far *zeke[5] =
{
	"Matilda Functions",
	"Zeke Options",
	"Zelda's Stories",
	"Frank Zappa wierd songs",
	"EXIT TO DOS"
};


int far selection;
char far *mono_ptr;
char far test_string[222];



main()
{
	FP_SEG(mono_ptr) = 0xb000;
	FP_OFF(mono_ptr) = 0;


	selection = choose_menu("Pick An Option",zeke,5,0);

	sprintf(test_string,"YOU Selected %d",selection);
	mono_string(test_string,22,12,0x15);
}
