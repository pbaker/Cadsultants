/* dlint_test.c */


#include <stdio.h>
#include <pw_lib.h>

struct phys_mem
{
	OPCODE array1[16384],array2[16384];
};

struct phys_mem *virtual_address;


unsigned long the_size,physical_address;
unsigned int size;


main()
{
int i;


	pw_setup(10,6,0x340,NULL);

	lutinit();

	window(-2048,2048,-2048,2048);
	vwport(128,895,0,767);


	the_size = (unsigned long)pw_phys_map(&virtual_address,
		&physical_address,&size);

	printf("\n physical memory buffer size = %d\n",size);

	pw_dl_init(&(virtual_address -> array1[0]),0,NULL,0);

	prmfill(1);

	for(i = 15 ; i > 0 ; i--)
	{
		color(i);
		move2(0,0);
		circle(100 * i);
	}



	dlint(0x7000);

	pw_dl_init(NULL,0,NULL,0);


	dlrun(&(virtual_address -> array1[0]));
	pw_dl_wait();


	printf("\nreason = 0x%x\n",pw_dl_reason());


}

