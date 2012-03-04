/*
 * Copyright PIXELWORKS Incorporated 1989. ALL RIGHTS RESERVED.
 * No part of this work may be reproduced or distributed in any form or by
 * any means without prior permission in writing from PIXELWORKS Incorporated.
 */

/*
 * 14-Nov-88 ddarby; added modifier bits & PW_COMP
 * 20-Oct-88 nrockwel; added SETTRN?D
 */

/*
 *	Command Word Format
 * |d15------------------d8|d7------------------d0|
 * |   command modifier    |    command opcode    |
 *
 *  d8 Floating Point  0 - 16-bit integer coordinates
 *                     1 - 32-bit floating point coordinates
 *
 *  d9 Reserved
 *
 * d10 Reserved
 *
 * d11 Extents         0 - No extent information
 *                     1 - Extent information with this command (2d Only)
 *
 * d12 Multiple        0 - Single command
 *                     1 - Multiple command
 *
 * d13 Indirect        0 - Parameter block is within command block
 *                     1 - Parameter block is indirectly referenced
 *
 * d14 Transform       0 - Transform coordinates
 *                     1 - No transform required for coordinates (Screen coord)
 *
 * d15 IRQ             0 - No interrupt on completion of command
 *                     1 - Interrupt with reason code 2 on completion
 */

#define	FLPNT		0x0100
#define	EXTENTS		0x0800
#define	MULTIPLE	0x1000
#define	INDIRECT	0x2000
#define	NOTRANSFORM	0x4000
#define	IRQ			0x8000

#define	NOOP		0x00
#define	COMMENT		0x04	/* n, w1, w2,...,wn */

#define	WAIT		0x05	/* count */
#define	COLOR		0x06	/* color */
#define	FLOOD		0x07	/* color */

#define	POINT2		0x08	/* x,y */
#define	POINT3		0x09	/* x,y,z */

#define	GMARK	   	0x0C	/* markid */
#define	GMSIZE		0x0D	/* size */
#define	GMANGLE		0x0E	/* angle */

#define	MOVE2		0x10	/* x,y */
#define	MOVE2R		0x11	/* dx,dy */
#define	MOVE3		0x12	/* x,y,z */
#define	MOVE3R		0x13	/* dx,dy,dz */

#define	CIRCLE		0x18	/* radius */
#define	ELLIPSE		0x19	/* xradius,yradius */
#define	ARC			0x1C	/* radius,sdeg,edeg */
#define	SECTOR		0x1D	/* radius,sdeg,edeg */
#define	EARC		0x1E	/* xradius,yradius,sdeg,edeg */
#define	ESECTOR		0x1F	/* xradius,yradius,sdeg,edeg */

#define	CURVE2		0x20	/* x1,y1,x2,y2,x3,y3 */
#define	CURVE2R		0x21	/* x1,y1,x2,y2,x3,y3 */
#define	CURVE3		0x22	/* x1,y1,z1,x2,y2,z2,x3,y3,z3 */
#define	CURVE3R		0x23	/* x1,y1,z1,x2,y2,z2,x3,y3,z3 */

#define	DRAW2		0x28	/* x,y */
#define	DRAW2R		0x29	/* dx,dy */
#define	DRAW3		0x2A	/* x,y,z */
#define	DRAW3R		0x2B	/* dx,dy,dz */

#define	POLYL2		0x2C	/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	POLYL2R		0x2D	/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	POLYL3		0x2E	/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	POLYL3R		0x2F	/* cnt,x1,y1,x2,y2,...,xn,yn */

#define	POLYG2		0x30	/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	POLYG2R		0x31	/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	POLYG3		0x32	/* cnt,x1,y1,z1,x2,y2,z2,...,xn,yn,zn */
#define	POLYG3R		0x33	/* cnt,x1,y1,z1,x2,y2,z2,...,xn,yn,zn */

#define	BOX			0x34	/* x,y */
#define	BOXR		0x35	/* x,y */

#define	RESET		0x41	/* software reset */
#define	RDVER		0x42	/* read microcode version number */
#define	BOARDIN		0x43	/* Do a board in */
#define	RDSTAT		0x44	/* read hardware status register */

#define	VECT2		0x48	/* x1,y1,x2,y2 */
#define	VECT2R		0x49	/* x1,y1,dx,dy */
#define	VECT3		0x4A	/* x1,y1,z1,x2,y2,z2 */
#define	VECT3R		0x4B	/* x1,y1,z1,dx,dy,dz */

#define	RDDRWFLG	0x53
#define	GCTINFO		0x54	/* Get cursor text info */

#define	GIPMASK		0x55	/* read number of pixel planes */

#define	BLKMASK		0x5A	/* mask */
#define	RDMASK		0x5B	/* mask */
#define	SLPAT32		0x5C	/* patternl,patternh */
#define	SETLSTYLE	0x5D	/* style,on-count,off-count,...,0 */

#define	GPIXSIZE	0x5E	/* read pixel memory size */
#define	GVPXSIZE	0x5F	/* read visual pixel memory size */
 
#define	SETVIDEO	0x65	/* video-bits */

#define	ARCSTEP		0x69	/* inc */
#define	CURVSTEP	0x6B	/* inc */

#define	SDLADDR		0x73	/* Set display list address */
#define	RDLADDR		0x74	/* Read display list address */

#define	DLINT		0x75	/* display list interrupt */
#define	SLAVE		0x76	/* Slave mode on */
#define	UNSLAVE		0x77	/* Slave mode off */
#define	DLGO		0x78	/* continue with display list */
#define	DLEND		0x79	/* end of display list */
#define	DLRUN		0x7A	/* addrl,addrh */
#define	CALL		0x7B	/* addrl,addrh */
#define	RET			0x7C	/* */
#define	BRANCH		0x7D
#define	BRANCHID	0x7E	/* Conditional branch on pick id */

#define	TUNDEFIN	0x7F	/* undefine text font */
#define	GTEXT		0x80	/* text string, null */
#define	GTSIZE		0x81	/* xsize, ysize */
#define	GTANGLE		0x82	/* angle */
#define	GTPROP		0x83	/* flag */
#define	TDEFIN		0x84	/* define text font */
#define	GTJUST		0x85	/* h, v */
#define	GTSTYLE		0x86	/* high quality,italic,draft,IBM,user def */
#define	GTSPACE		0x87	/* x spacing */
#define	GCHRWID		0x89	/* Get character width */
#define	SCTADDR		0x8A	/* caddr */

#define	CTEXT		0x8B	/* color/char */
#define	RCTEXT		0x8C	/* addrl,addrh */
#define	RCTADDR		0x8D	/* addrl,addrh */
#define	SCTBCOL		0x8E	/* color */

/* New commands 03-18-87 */
#define	MDIDEN3		0x90	/* set 3d modeling matrix to identity */
#define	MDORG3		0x91	/* set 3d modeling origin */
#define	MDSCAL3		0x92	/* scale 3d model */
#define	MDROTX3		0x93	/* rotate 3d model about x-axis */
#define	MDROTY3		0x94	/* rotate 3d model about y-axis */
#define	MDROTZ3		0x95	/* rotate 3d model about z-axis */
#define	MDTRAN3		0x96	/* translate 3d model */
#define	MDMATX3		0x97	/* set complete 3d modeling matrix */
#define	MDIDEN2		0x98	/* set 2d modeling matrix to identity */
#define	MDORG2		0x99	/* set 2d modeling origin */
#define	MDSCAL2		0x9A	/* scale 2d model */
#define	MDROT2		0x9B	/* rotate 2d model */
#define	MDTRAN2		0x9C	/* translate 2d model */
#define	MDMATX2		0x9D	/* set complete 2d modeling matrix */
#define	VWIDEN3		0xA0	/* set 3d viewing matrix to identity */
#define	VWRPT3		0xA1	/* set 3d viewing reference point */
#define	VWROTX3		0xA3	/* rotate 3d view about x */
#define	VWROTY3		0xA4	/* rotate 3d view about y */
#define	VWROTZ3		0xA5	/* rotate 3d view about z */
#define	VWMATX2		0xA6	/* set complete 2d viewing matrix */
#define	VWMATX3		0xA7	/* set complete 3d viewing matrix */
         
/* added 12-02-87 skfincher */
#define	MAP2D3D		0xAC	/* */
#define	CONVERT		0xAD	/* */
#define	VWPARA		0xAE	/* set parallel projection parameters */
#define	RECONCAT	0xAF	/* recat matrices */
#define	VWDIST		0xB1	/* set perspective projection distance */

#define	VWPORT		0xB2	/* set viewport in screen space */
#define	WINDOW		0xB3	/* set 2d window in virtual space */
#define	PRECAT		0xB4	/* set pre-cat flag */

#define	PUSHATTR	0xBE	/* push drawing attributes */
#define	POPATTR		0xBF	/* pop drawing attributes */
#define	PUSHMD2		0xC0	/* push 2d modeling matrix */
#define	POPMD2		0xC1	/* pop 2d modeling matrix */
#define	PUSHVW2		0xC2	/* push 2d viewing matrix */
#define	POPVW2		0xC3	/* pop 2d viewing matrix */
#define	PUSHFM2		0xC4	/* push 2d final transformation */
#define	POPFM2		0xC5	/* pop 2d final transformation */
#define	PUSHTM		0xC6	/* push text modeling transformation */
#define	POPTM		0xC7	/* pop text modeling transformation */
#define	PUSHMM		0xC8	/* push marker modeling transformation */
#define	POPMM		0xC9	/* pop marker modeling transformation */
#define	PUSHMD3		0xCA	/* push 3d modeling matrix */
#define	POPMD3		0xCB	/* pop 3d modeling matrix */
#define	PUSHVW3		0xCC	/* push 3d viewing matrix */
#define	POPVW3		0xCD	/* pop 3d viewing matrix */
#define	PUSHFM3		0xCE	/* push 3d final transformation */
#define	POPFM3		0xCF	/* pop 3d final transformation */

#define	DISPLAY		0xD0	/* cga on/off */

#define	SETCID		0xD2	/* set conditional pic id */
#define	PICKEN		0xD3	/* set/clear the pick flag */
#define	PICKWIND	0xD4	/* define new pick window */
#define	SETPID		0xD5	/* set pick id */
#define	RDPID		0xD6	/* read pick id */
		
#define	BYTESWAP	0xD7	/* blt byteswap flag */
#define	IMAGER		0xD8	/* y, xmin, xmax, addrl, addrh */
#define	IMAGEW		0xD9	/* y, xmin, xmax, addrl, addrh */

#define	BLTMASK		0xDA	/* mask */
#define	BLTPCM		0xDB	/* addrl,addrh,width,xoffset,yoffset */
#define	BLTRWIND	0xDC	/* dx,dy */
#define	BLTPC		0xDD	/* addrl,addrh */
#define	PCBLT		0xDE	/* addrl,addrh */
#define	BLT			0xDF	/* x,y */

#define	PUSHWORD	0xE0	/* push word */
#define	POPWORD		0xE1	/* pop word */
#define	PUSHLUT		0xE2	/* push entire color lookup table */
#define	POPLUT		0xE3	/* pop entire lookup table */
#define	PUSHCOLOR	0xE4	/* push color lookup table entry */
#define	POPCOLOR	0xE5	/* pop color lookup table entry */
         
#define	LUT8		0xE6	/* index,red,green,blue */
#define	SETFPAT		0xE7	/* set primitive fill pattern */
#define	WRMASK		0xE8	/* mask */
#define	PRMFILL		0xE9	/* set primitive fill flag */
#define	LINPAT		0xEA	/* style */
#define	LINFUN		0xEB	/* function */
#define	LUTINIT		0xEC	/* */
#define	LUT4		0xEE	/* index,red,green,blue */
#define	RDPOS2		0xF0	/* xaddrl,xaddrh,yaddrl,yaddrh */
#define	RDPOS3		0xF1	/* xaddrl,xaddrh,yaddrl,yaddrh,zaddrl,zaddrh */
#define	RDPIX2		0xF4	/* addrl,addrh */
#define	RDPIX3		0xF5	/* addrl,addrh */

#define	RDMATMD2	0xF8	/* addrl,addrh */
#define	RDMATVW2	0xF9	/* addrl,addrh */
#define	RDMATMD3	0xFA	/* addrl,addrh */
#define	RDMATVW3	0xFB	/* addrl,addrh */
#define	RDLUT		0xFC	/* raddrl,raddrh,gaddrl,gaddrh,baddrl,baddrh */

#define	RDMEMSIZ	0xFF	/* read graphics memory size */

/* *** Floating point opcode definitions. ...added 9-26-88 ck *** */

#define	FPOINT2		(FLPNT|POINT2) 
#define	FPOINT3		(FLPNT|POINT3)

#define	FMOVE2		(FLPNT|MOVE2)		/* x,y */
#define	FMOVE2R		(FLPNT|MOVE2R)		/* dx,dy */
#define	FMOVE3		(FLPNT|MOVE3)		/* x,y,z */
#define	FMOVE3R		(FLPNT|MOVE3R)		/* dx,dy,dz */

#define	FDRAW2		(FLPNT|DRAW2)		/* x,y */
#define	FDRAW2R		(FLPNT|DRAW2R)		/* dx,dy */
#define	FDRAW3		(FLPNT|DRAW3)		/* x,y,z */
#define	FDRAW3R		(FLPNT|DRAW3R)		/* dx,dy,dz */

#define	FPOLYL2		(FLPNT|POLYL2)		/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	FPOLYL2R	(FLPNT|POLYL2R)		/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	FPOLYL3		(FLPNT|POLYL3)		/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	FPOLYL3R	(FLPNT|POLYL3R)		/* cnt,x1,y1,x2,y2,...,xn,yn */

#define	FPOLYG2		(FLPNT|POLYG2)		/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	FPOLYG2R	(FLPNT|POLYG2R)		/* cnt,x1,y1,x2,y2,...,xn,yn */
#define	FPOLYG3		(FLPNT|POLYG3)		/* cnt,x1,y1,z1,...,xn,yn,zn */
#define	FPOLYG3R	(FLPNT|POLYG3R)		/* cnt,x1,y1,z1,...,xn,yn,zn */

#define	FVECT2		(FLPNT|VECT2)		/* Draw 2d vector */
#define	FVECT2R		(FLPNT|VECT2R)		/* Draw relative 2d vector */
#define	FVECT3		(FLPNT|VECT3)		/* Draw 3d vector */
#define	FVECT3R		(FLPNT|VECT3R)		/* Draw relative 3d vector */

#define	FMDIDEN3	(FLPNT|MDIDEN3)		/* set 3d modeling matrix to identity */
#define	FMDMATX3	(FLPNT|MDMATX3)		/* set complete 3d modeling matrix */
#define	FMDIDEN2	(FLPNT|MDIDEN2)		/* set 2d modeling matrix to identity */
#define	FMDMATX2	(FLPNT|MDMATX2)		/* set complete 2d modeling matrix */
#define	FVWIDEN3	(FLPNT|VWIDEN3)		/* set 3d viewing matrix to identity */
#define	FVWRPT3		(FLPNT|VWRPT3)		/* set 3d viewing reference point */
#define	FVWMATX2	(FLPNT|VWMATX2)		/* set complete 2d viewing matrix */
#define	FVWMATX3	(FLPNT|VWMATX3)		/* set complete 3d viewing matrix */

#define	FVWDISTH	0x1A8				/* set 3d hither clipping distance */
#define	FVWDISTY	0x1A9				/* set 3d yon clipping distance */
#define	FVWCLIPH	0x1AA				/* enable/disable hither clipping */
#define	FVWCLIPY	0x1AB				/* enable/disable yon clipping */

#define	FRECONCAT	(FLPNT|RECONCAT)	/* recat matrices */

#define	FVWDIST 	(FLPNT|VWDIST)		/* set perspective projection distance */

#define	FVWPORT		(FLPNT|VWPORT)		/* set viewport in screen space */
#define	FWINDOW		(FLPNT|WINDOW)		/* set 2d window in virtual space */

#define	FSETTRN2D   0x01BC				/* addrl,addrh */
#define	FSETTRN3D   0x01BD				/* addrl,addrh */

#define	FPUSHMD2	(FLPNT|PUSHMD2)		/* push 2d modeling matrix */
#define	FPOPMD2		(FLPNT|POPMD2)		/* pop 2d modeling matrix */
#define	FPUSHVW2	(FLPNT|PUSHVW2)		/* push 2d viewing matrix */
#define	FPOPVW2		(FLPNT|POPVW2)		/* pop 2d viewing matrix */
#define	FPUSHFM2	(FLPNT|PUSHFM2)		/* push 2d final transform */
#define	FPOPFM2		(FLPNT|POPFM2)		/* pop 2d final transform */
#define	FPUSHMD3	(FLPNT|PUSHMD3)		/* push 3d modeling matrix */
#define	FPOPMD3		(FLPNT|POPMD3)		/* pop 3d modeling matrix */
#define	FPUSHVW3	(FLPNT|PUSHVW3)		/* push 3d viewing matrix */
#define	FPOPVW3		(FLPNT|POPVW3)		/* pop 3d viewing matrix */
#define	FPUSHFM3	(FLPNT|PUSHFM3)		/* push 3d final transform */
#define	FPOPFM3		(FLPNT|POPFM3)		/* pop 3d final transform */

#define	FPRMFILL	(FLPNT|PRMFILL)		/* set primitive fill flag */

#define	FRDPOS2		(FLPNT|RDPOS2)		/* x,y */
#define	FRDPOS3		(FLPNT|RDPOS3)		/* x,y,z */

#define	FRDMATMD2	(FLPNT|RDMATMD2)	/* read 2d modeling matrix */
#define	FRDMATVW2	(FLPNT|RDMATVW2)	/* read 2d viewing matrix */
#define	FRDMATMD3	(FLPNT|RDMATMD3)	/* read 3d modeling matrix */
#define	FRDMATVW3	(FLPNT|RDMATVW3)	/* read 3d viewing matrix */

#ifdef	PW_COMP
/* Unsupported/RESERVED OPCODES */

#define	SETROT		0x60	/* Use window,vwport */
#define	SETXFORM	0x61	/* Use window/vwport */
#define	SETCLIP		0x62	/* Use vwport */
#define	SET3DM		0x63	/* 3xx,3yx,3zx,3ox,3xy,3yy,3zy,3oy */
#define	TROT		0x68	/* Use GTANGLE */
#define	TXFRM		0x6E	/* Use GTSIZE */

#define	POINT		(POINT2|NOTRANSFORM)
#define	SCRMV		(MOVE2|NOTRANSFROM)
#define	SCRDW		(DRAW2|NOTRANSFORM)
#define	RDPOS		(RDPOS2|NOTRANSFORM)
#define	RDPIX		(RDPIX2|NOTRANSFORM)
#define	TSTYLE		GTSTYLE
#define	CHRCMD		GTEXT

/* Old opdcode definitions for portability */

#define	MOVE		MOVE2
#define	MOVER		MOVE2R
#define	MOVER3		MOVE3R	
#define	CURVE		CURVE2
#define	CURVER		CURVE2R
#define	CURVER3		CURVE3R
#define	DRAW		DRAW2
#define	DRAWR		DRAW2R
#define	DRAWR3		DRAW3R
#define	POLYLINE	POLYL2
#define	POLYL		POLYL2
#define	POLYLR		POLYL2R	
#define	POLYLR3		POLYL3R
#define	POLYGON		POLYG2 
#define	POLYG		POLYG2
#define	POLYGR		POLYG2R
#define	POLYGR3		POLYG3R
#define	FRECT		BOX
#define	FRECTR		BOXR
#define	RSTAT		RDSTAT
#define	PLANES		GIPMASK
#define	GPIXSIZ		GPIXSIZE
#define	GVISSIZ		GVPXSIZE
#define	SETVID		SETVIDEO
#define	SARCINC		ARCSTEP
#define	SCURINC		CURVSTEP
#define	SLAVEON		SLAVE
#define	SLAVEOFF	UNSLAVE
#define	CENT		CALL
#define	ERET		RET
#define	PARPROJ		VWPARA
#define	RECAT		RECONCAT
#define	VIEWPORT	VWPORT
#define	SETPRE		PRECAT
#define	SETCGA		DISPLAY
#define	BSWAP		BYTESWAP
#define	BLTRWIN		BLTRWIND
#define	PRMFIL		PRMFILL
#define	LUT			LUT4
#define	RDV2POS		RDPOS2
#define	RDV3POS		RDPOS3
#define	RD2MD		RDMATMD2
#define	RD2VW		RDMATVW2
#define	RD3MD		RDMATMD3
#define	RD3VW		RDMATVW3
#endif
