/*
 * Copyright PIXELWORKS Incorporated 1989. ALL RIGHTS RESERVED.
 * No part of this work may be reproduced or distributed in any form or by
 * any means without prior permission in writing from PIXELWORKS Incorporated.
 */

/*
 * 14-Nov-88 ddarby; created
 */

/*
 * The following defines the ifdef's to be used when compiling
 *
 *	PW_DOS - Compile for DOS	(Any version)
 *		PW_microsoft
 *		PW_lattice
 *		PW_turboc
 *		PW_highc
 *	PW_UNIX - Compile for unix	(Any version)
 *		PW_microport
 *		PW_sco286
 *		PW_sco386
 *		PW_interactive
 *	PW_OS2 - Compile for OS2	(Any version)
 *
 */

#define	OPCODE	short

#define	FALSE	0
#define	TRUE	1

#ifdef	PW_UNIX
#	define	far
#	define	huge
#	define	getch getchar
#	include	<pw.h>
#endif

/* Define test fields for rdstat command */
#define	CGA_EMUL_ON	0x0100	/* Test for single monitor */
#define	EXTRN_VIDEO	0x0200	/* Test for external video */
#define	EMULATION_FP	0x1000	/* Ultra Clipper - FP emulation */
#define	PS_2		0x2000	/* PS2 Hardware */
#define	HARDWARE_FP	0x4000	/* Ultra Clipper - FP hardware */
#define	ULTRA_CLIPPER	0x8000	/* Text for Ultra Clipper */

/* Define default name for pw_fsetup F.P. name */
#define	PWFP_DEF	((char *)0)

/* line functions */
#define	NXOR		3
#define	NOR		6
#define	OFF		7
#define	OR		9
#define	NAND		10
#define	XOR		11
#define	REPLACE		12
#define	AND		13
#define	ON		15

extern struct	pw_data
{	struct flags
	{	unsigned open : 1;
	} flags;
	struct	usr
	{	OPCODE	*ptr;
		int		size;
		OPCODE	*curbuf;
		OPCODE	*buf1;
		int		size1;
		unsigned long	phy1;
		OPCODE	*buf2;
		int		size2;
		unsigned long	phy2;
	} usr;
	struct	lcl
	{	int		size;
		OPCODE	*curbuf;
		OPCODE	*buf1;
		unsigned long	phy1;
		OPCODE	*buf2;
		unsigned long	phy2;
	} lcl;
#ifndef	PW_UNIX
	struct	ems
	{	unsigned int handle;
		unsigned int page_size;
		unsigned int page_mask;
		unsigned int page_shift;
	} ems;
#else
	struct unix_s
	{	int	FD;
		void (*int_hand)();
		struct	pw_ioctl ioctl;
		char	*v_base;		/* Base virtual address */
		char	*p_base;		/* Base physical address */
		int	b_size;			/* Byte size of physical memory */
	} unix_s;
#endif
} pw_data;
