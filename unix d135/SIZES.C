#include <stdio.h>

int *int_ptr;


void ssss(ch)
char ch;
{
	printf("\nsizeof char parameter = %d\n",sizeof(ch));
}


main()
{
	printf("\nsizeof int = %d\n",sizeof(int));
	printf("sizeof char = %d\n",sizeof(char));
	printf("sizeof short = %d\n",sizeof(short));
	printf("sizeof long = %d\n",sizeof(long));
	printf("sizeof pointer = %d\n",sizeof(int_ptr));
	printf("sizeof float = %d\n",sizeof(float));
	printf("sizeof double = %d\n",sizeof(double));

	ssss('a');
}


