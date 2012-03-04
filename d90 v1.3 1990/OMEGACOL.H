/*
 * METHEUS OMEGA/PC,1004,1008 and 1104 COL include file:
 *
 *	This file contains the macros and opcode definition part of the COL.
 *	The remaining part is in the omegacol.c file.
 *	Information about the use of these macros is in the:
 *
 *		     "Programmer's Toolkit "
 *	 " For the 1000-Series Color Graphics Display Processor"
 *
 *	Copyright 1986, 1987 Metheus Corporation (all rights reserved)
 *	5510 N.E. Elam Young Parkway, Hillsboro OR 97123 1-(503) 640-8000
 *
 *	RCS: $Header: omegacol.h,v 1.2 87/09/08  $
 */

/*
 * Structure Definations
 */

struct bit_planes	/* bit plane informations	*/
{
	short planes,	/* number of bit planes 			*/
	      ulx,	/* coordinates of addressable memory		*/
	      uly,
	      lrx,
	      lry,
	      dwidth,	/* height of displayable pixel memory		*/
	      dheight,	/* width of displayable pixel memory		*/
	      bred,	/* # bits in red, green and blue dacs		*/
	      bgreen,
	      bblue,
	      zoom_max, /* max. zoom amount (0 ==> no zoom)		*/
	      zoom_inc, /* zoom increment (0 ==> 2X zoom)		*/
	      panx,	/* distance between pans (0 ==> no pan) 	*/
	      pany;
};

struct device_info	   /* device information	   */
{
	long   name;	      /* Metheus Device Driver comment string	    */
	short  model;		/* hold device model number, eg. 1104, 1008   */
	short  hmajor, hminor;	/* hardware release number		      */
	long   hdate;		/* date of hardware release		      */
	short  smajor, sminor;	/* software release number		      */
	long   sdate;		/* date of software release (ie. microcode)   */
	struct bit_planes
	       base,		/* description of base bit planes	      */
	       overlay; 	/* description of overlay bit planes	      */
};

struct driver_info	   /* driver information	   */
{
	short  dmajor, dminor;	/* driver release number	 */
	long   ddate;		/* date of driver release	 */
	short  ioctlmax;	/* max # for IOCTL command	 */
	short  errno;		/* current driver specific error */
};

struct fontformat {
	unsigned char	format,        /* font format 0-fixed 1-variable */
			flags,	       /* reserved all 0		 */
			width,	       /* font width  max 16		 */
			height;        /* font height max 85		 */
	unsigned int	char0,	       /* char starting position	 */
			nchar;	       /* number of characters in font	 */
	unsigned char	*glyphwidths;  /* pointer to glyph width table	 */
	unsigned int	size;	       /* size of font data in bytes	 */
	unsigned char	*data;	       /* pointer to font data		 */
};

/*  */

#define OMEGA_NAME	"$MAGIK" /* name of device               */
#define COL_REL 	4.4	 /* col release version x.x	 */

#define COL_ERROR	(int)-1 /* return ERROR code		*/
#define COL_OK		(int)0	/* return OK code		*/

/*
 * ioctl function request numbers
 * (serviced by omega and driver via DOS int 21h)
 */
#define OMEGA_IOCTL_GETDRIVER	0
#define OMEGA_IOCTL_GETDEVICE	1
#define OMEGA_IOCTL_GETPTRS	2
#define OMEGA_IOCTL_RDR 	3
#define OMEGA_IOCTL_WRR 	4
#define OMEGA_IOCTL_DFONT	5
#define OMEGA_IOCTL_MODE	6
#define OMEGA_IOCTL_SIGRD	7

/*
 * open() error codes
 */
#define MNOMAGIK	1
#define MTIMEOUT	2
#define MOPENERR	3

/*
 * OMEGA OPCODES
 */
#define AFILL1		(0x68)
#define AFILL2		(0x69)
#define ARC		(0x62)
#define BDCONF		(0x88)
#define BLANK		(0x4b)
#define CARC		(0x7a)
#define CCIRCLE 	(0x82)
#define CGACTL		(0x83)
#define CHAR		(0x6b)
#define CHARE		(0xa2)
#define CLEAR		(0x60)
#define CLIPEN		(0x7e)
#define COMPDR		(0x72)
#define CMAP		(0x51)
#define CRECT		(0x7d)
#define CRTWR		(0x46)
#define CSPACE		(0x48)
#define CURS		(0x71)
#define CURSXY		(0x7f)
#define DRAW		(0x61)
#define DRAWTA		(0x78)
#define DRAWTL		(0x77)
#define DRAWTS		(0x76)
#define EEWR		(0x86)
#define EERD		(0x87)
#define ELLARC		(0x8f)
#define ELLIPSE 	(0x8d)
#define FCIRCLE 	(0x7b)
#define FELLIPSE	(0x8e)
#define FFILL		(0x65)
#define FONTDEL 	(0x9f)
#define FONTLD		(0x9e)
#define FONTSEL 	(0xa1)
#define FONTSZ		(0x49)
#define DRAWM		(0x30)
#define INI		(0x5e)
#define INQ		(0x41)
#define INVERTY 	(0x90)
#define LDPAT		(0x40)
#define LDFONT		(0x3e)
#define MOVP1		(0x52)
#define MOVP2		(0x53)
#define PATERN		(0x50)
#define PIXBLT		(0x70)
#define POLYC		(0x44)
#define POLYD		(0x7c)
#define POLYF		(0x67)
#define POLYL		(0xa3)
#define POLYM		(0x45)
#define POLYO		(0x66)
#define POLYS		(0x56)
#define POLYV		(0x57)
#define PPAN		(0x5b)
#define PUTP1P2 	(0x8b)
#define RASTOP		(0x43)
#define RASTTRAN	(0x84)
#define RDCMAP		(0xa0)
#define RDMSK		(0x4c)
#define RECT1		(0x63)
#define RECT2		(0x64)
#define RESTORE 	(0x37)
#define RLFILL		(0x6a)
#define RMOVP1		(0x54)
#define RMOVP1S 	(0x33)
#define RMOVP2		(0x55)
#define RMOVP2S 	(0x34)
#define RPIXEL		(0x6c)
#define RRT		(0x8a)
#define SAVE		(0x36)
#define SCANLR		(0xa4)
#define SETBCOL 	(0x9d)
#define SETCOL		(0x4e)
#define SETCSZ		(0x58)
#define SETORN		(0x59)
#define SETROC		(0x42)
#define SETPZOOM	(0x97)
#define SIGRD		(0x5c)
#define SYNCH		(0x5f)
#define SZCURS		(0x47)
#define WPIXEL		(0x6d)
#define WRMSK		(0x4f)
#define WRR		(0x6f)
#define XDRAW		(0x73)
#define XFLD		(0x80)
#define XFORMEN 	(0x81)
#define XFORMSET	(0x8c)
#define ZOOM		(0x5a)

/*
 * OMEGA interface MACROS:
 * these fast text macros replace COL function calls
 * omega_put?() calls send OPCODES and parameters directly to driver
 * (see COL.ASM)
 */
#define afill1()		  {omega_puto(AFILL1);}
#define afill2(filluntil)	  {omega_putm(AFILL2|0x200,filluntil);}
#define arc(x)			  {omega_putm(ARC|0x200,x);}
#define blank(mode)		  {omega_putm(BLANK|0x200,mode);}
#define ccircle(cx,cy,radius)	  {omega_putm(CCIRCLE|0x0f00,cx,cy,radius);}
#define cgacntl1(x,y,ts,ls,rs,bs) {omega_putm(CGACTL|0xfe00,1,x,y,ts,ls,rs,bs);}
#define cgacntl2(enable)	  {omega_putm(CGACTL|0x0400, 2, enable);}
#define chars1(n,str)		  {omega_puto(CHAR); \
				   omega_write(str,n); omega_putc(0x1b);}
#define clear() 		  {omega_puto(CLEAR);}
#define clipen(opt)		  {omega_putm(CLIPEN|0x0200,opt);}
#define cmap(a,r,g,b)		  {omega_putm(CMAP|0x1000, a, r, g, b);}
#define compdr()		  {omega_puto(COMPDR);}
#define crect(x0,y0,x1,y1)	  {omega_putm(CRECT|0x1f00,x0,y0,x1,y1);}
#define crtwr(reg,value)	  {omega_putm(CRTWR|0x400,reg,value);}
#define cspace(dx,dy)		  {omega_putm(CSPACE|0x700,dx,dy);}
#define curs()			  {omega_puto(CURS);}
#define cursxy(x,y)		  {omega_putm(CURSXY|0x700,x,y);}
#define draw()			  {omega_puto(DRAW);}
#define drawta(x,y)		  {omega_putm(DRAWTA|0x700,x,y);}
#define drawtl(dx,dy)		  {omega_putm(DRAWTL|0x700,dx,dy);}
#define drawm(n,vlist)		  {omega_puto(DRAWM);\
				   omega_write((char*)vlist,n<<2);\
				   omega_putw(0x8000);}
#define drawts(dx,dy)		  {omega_putm(DRAWTS|0x400,dx,dy);}
#define ellarc(a,b,sx,sy,ex,ey)   {omega_putm(ELLARC|0x7f00,a,b,sx,sy,ex,ey);}
#define ellipse(a,b)		  {omega_putm(ELLIPSE|0x700,a,b);}
#define fcircle(cx,cy,radius)	  {omega_putm(FCIRCLE|0xf00,cx,cy,radius);}
#define fellipse(a,b)		  {omega_putm(FELLIPSE|0x700,a,b);}
#define ffill() 		  {omega_puto(FFILL);}
#define fini()			  ;
#define fontdel(index)		  {omega_putm(FONTDEL|0x200,index);}
#define fontsel(index)		  {omega_putm(FONTSEL|0x200,index);}
#define ini()			  {omega_puto(INI);}
#define inverty(opt)		  {omega_putm(INVERTY|0x200,opt);}
#define movp1(x,y)		  {omega_putm(MOVP1|0x700,x,y);}
#define movp2(x,y)		  {omega_putm(MOVP2|0x700,x,y);}
#define nop()			  {omega_puto(0);}
#define patern(x)		  {omega_putm(PATERN|0x200,x);}
#define pixblt(dx,dy,dir)	  {omega_putm(PIXBLT|0xb00,dx,dy,dir);}
#define polyc() 		  {omega_puto(POLYC);}
#define polyd1(n,x)		  {omega_putm(POLYD|0x300,n); \
				   omega_write((char *)x,(n*4));}
#define polyf() 		  {omega_puto(POLYF);}
#define polyl(flags,n,vlist)	  {omega_putm(POLYL|0x0600,flags,n);\
				   omega_write((char*)vlist,n<<2);}
#define polym(x,y)		  {omega_putm(POLYM|0x700,x,y);}
#define polyo() 		  {omega_puto(POLYO);}
#define polys() 		  {omega_puto(POLYS);}
#define polyv(x,y)		  {omega_putm(POLYV|0x700,x,y);}
#define ppan()			  {omega_puto(PPAN);}
#define rastop(func,alu)	  {omega_putm(RASTOP|0x400,func,alu);}
#define rasttran(ctl,width,height){omega_putm(RASTTRAN|0xE00,ctl,width,height);}
#define rdmsk(mask)		  {omega_putm(RDMSK|0x200,mask);}
#define rect1() 		  {omega_puto(RECT1);}
#define rect2() 		  {omega_puto(RECT2);}
#define restore()		  {omega_puto(RESTORE);}
#define rlfill(dx)		  {omega_putm(RLFILL|0x300,dx);}
#define rmovp1(x,y)		  {omega_putm(RMOVP1|0x700,x,y);}
#define rmovp1s(dx,dy)		  {omega_putm(RMOVP1S|0x400,dx,dy);}
#define rmovp2(x,y)		  {omega_putm(RMOVP2|0x700,x,y);}
#define rmovp2s(dx,dy)		  {omega_putm(RMOVP2S|0x400,dx,dy);}
#define save()			  {omega_puto(SAVE);}
#define scnalr(flag,color)	  {omega_putm(SCANLR|0x400,flag,color);}
#define setbcol(c)		  {omega_putm(SETBCOL|0x200,c);}
#define setcol(c)		  {omega_putm(SETCOL|0x200,c);}
#define setcsz(sx,sy)		  {omega_putm(SETCSZ|0x400,sx,sy);}
#define setorn(orn)		  {omega_putm(SETORN|0x200,orn);}
#define setpzoom(fac)		  {omega_putm(SETPZOOM|0x200,fac);}
#define synch(n)		  {omega_putm(SYNCH|0x200,n);}
#define szcurs(hx,hy)		  {omega_putm(SZCURS|0x700,hx,hy);}
#define wpixel()		  {omega_puto(WPIXEL);}
#define wrmsk(mask)		  {omega_putm(WRMSK|0x200,mask);}
#define wrr1(n,pixel)		  {omega_puto(WRR);omega_write(pixel,n);}
#define xdraw() 		  {omega_puto(XDRAW);}
#define xfld(trans)		  {omega_puto(XFLD);\
				   omega_write((char*)trans,sizeof(long)*6);}
#define xformen(opt)		  {omega_putm(XFORMEN|0x200,opt);}
#define xformset(opt)		  {omega_putm(XFORMSET|0x200,opt);}
#define zoom(fac)		  {omega_putm(ZOOM|0x200,fac);}

