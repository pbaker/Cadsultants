#include "stdio.h"

main()
{
int ch = 0;

	while(ch != 's')
	{
		ch = getch();
		printf("%x , ",ch);
		if(ch == 0)
		{
			ch = getch();
			printf("%x   ",ch);
		}
	}
}