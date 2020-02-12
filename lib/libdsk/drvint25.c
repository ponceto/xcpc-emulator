/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2006  John Elliott <seasip.webmaster@gmail.com>            *
 *                                                                         *
 *    This library is free software; you can redistribute it and/or        *
 *    modify it under the terms of the GNU Library General Public          *
 *    License as published by the Free Software Foundation; either         *
 *    version 2 of the License, or (at your option) any later version.     *
 *                                                                         *
 *    This library is distributed in the hope that it will be useful,      *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *    Library General Public License for more details.                     *
 *                                                                         *
 *    You should have received a copy of the GNU Library General Public    *
 *    License along with this library; if not, write to the Free           *
 *    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,      *
 *    MA 02111-1307, USA                                                   *
 *                                                                         *
 ***************************************************************************/


#include "drvi.h"
#include "drvint25.h"

/* This driver accesses DOS drives/partitions using INT 25h. It's mainly 
 * intended for use on Apricot PCs, which don't provide INT 13h. I could 
 * have written a driver that used the Apricot BIOS functions (INT 0FCh)
 * but, unlike INT 13h, these don't have any advantage over INT 25h/26h.
 */

#if (defined(DOS16FLOPPY) || defined(DOS32FLOPPY))
#include <dos.h>

#ifdef DOS32FLOPPY
#include <sys/farptr.h>
#include <dpmi.h>
#include <go32.h>
#endif

DRV_CLASS dc_dosint25 =
{
	sizeof(INT25_DSK_DRIVER),
	"int25",
	"DOS INT 25/26 driver",

	int25_open,	/* open */
	int25_creat,	/* create new */
	int25_close,	/* close */
	int25_read,	/* read sector, working from physical address */
	int25_write,	/* write sector, working from physical address */
	int25_format,	/* format track, physical */
	int25_getgeom,	/* probe geometry */
	NULL,		/* get sector ID */
	NULL,
	NULL		/* get drive status */
};




/* Translate the error codes returned by INT 25/26 to LIBDSK errors */
static dsk_err_t translate_error(unsigned ax)
{
	if (ax == 0) return DSK_ERR_OK;
	switch(ax >> 8)
	{
		case 0x80: return DSK_ERR_NOTRDY;
		case 0x40: return DSK_ERR_SEEKFAIL;
		case 0x20: return DSK_ERR_CTRLR;
		case 0x10: return DSK_ERR_DATAERR;
		case 0x08: return DSK_ERR_OVERRUN;
		case 0x04: return DSK_ERR_NODATA;
		case 0x03: return DSK_ERR_RDONLY;
		case 0x02: return DSK_ERR_NOADDR;
		case 0x01: return DSK_ERR_BADPARM;
	}
	fprintf(stderr, "Unknown INT25/26 error 0x%04x\n", ax);
	return DSK_ERR_UNKNOWN;
}

/* For a floppy drive, opening it and creating it are the same thing */
dsk_err_t int25_creat(DSK_DRIVER *self, const char *filename)
{
	return int25_open(self, filename);
}											


dsk_err_t int25_open(DSK_DRIVER *self, const char *filename)
{
	INT25_DSK_DRIVER *int25self;
	char vname[20];
	int driveno;                      
	dsk_err_t err;
	union REGS rg;

	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_dosint25) return DSK_ERR_BADPTR;
	int25self = (INT25_DSK_DRIVER *)self;

	/* Not a drive? Not interested. */
	if (strlen(filename) != 2 || filename[1] != ':') return DSK_ERR_NOTME;
	int25self->int25_unit  = -1;

	vname[0] = (char)toupper(filename[0]);
	driveno = vname[0] - 'A';   /* 0=A: 1=B: */

	int25self->int25_unit = driveno;
	/* Get DOS version */
	rg.h.ah = 0x30;
	intdos(&rg, &rg);
	int25self->int25_dosver = rg.h.al * 256 + rg.h.ah;
	int25self->int25_oem    = rg.h.bh;

	err = int25_getgeom(self, &int25self->int25_geom);
	/* If we can't get the geometry (eg a network drive) bail out */
	if (err == DSK_ERR_NOTIMPL) err = DSK_ERR_NOTME;
	if (!err)
	{
		/* If DOS 7.10 or greater try to acquire a lock on the drive.
		 * This does not appear to make the slightest difference; 
		 * perhaps if I got as far as writing, it would. */
		if (int25self->int25_dosver >= 0x70A && 
		    int25self->int25_oem != 0)
		{
			rg.x.ax = 0x440D;
			rg.x.bx = 1 + int25self->int25_unit;
			rg.x.cx = 0x084A;
			rg.x.dx = 0;
			intdos(&rg, &rg);
			if (rg.x.cflag)
				return DSK_ERR_ACCESS;
			int25self->int25_lock = 1;
		}
	}
	return err;

} 


dsk_err_t int25_close(DSK_DRIVER *self)
{
	INT25_DSK_DRIVER *int25self;

	if (self->dr_class != &dc_dosint25) return DSK_ERR_BADPTR;
	int25self = (INT25_DSK_DRIVER *)self;
	/* If we had a lock on the disc, release it. */
	if (int25self->int25_lock)
	{
		union REGS rg;

		rg.x.ax = 0x440D;
		rg.x.bx = 1 + int25self->int25_unit;
		rg.x.cx = 0x086A;
		rg.x.dx = 0;
		intdos(&rg, &rg);
		if (rg.x.cflag)
			return DSK_ERR_NOTME;
		int25self->int25_lock = 0;
	}

	return DSK_ERR_OK;
}   


/* This code behaved oddly when I tested it on Windows ME. On a FAT16 
 * drive, it was fine when using the "under 32Mb" form of the INT 25/26 
 * calls. But both the 'packet' calls (INT 25/CX=FFFF and INT 21/AX=7305)
 * fail with error 0x701F. I don't know what this error is supposed to 
 * mean; it probably implies that the registers haven't been set up
 * correctly, but I can't see exactly where the problem lies.
 */
#ifdef DOS32FLOPPY
#define PACKED __attribute__((packed))
typedef struct
{
	unsigned long sector;
	unsigned short count;
	void *buf;
} INT25_PACKET;

dsk_err_t int25_rdwr(int rdwr, INT25_PACKET *pkt)
{
	long addr;
	__dpmi_regs regs;
	int try_bigdrive = 1;

/* INT 25 and INT 26 leave a word on the stack, which makes it awkward to
 * call them directly using int86() or __dpmi_int(). So call this trampoline
 * which handles the stack weirdness. */
	unsigned char code16[] = 
	{
		0xCC, //0x55, 		/* push bp */
		0x8B, 0xEC,	/* mov bp,sp */
		0xCD, 0x25,	/* INT 25 */
		0x8B, 0xE5,	/* mov sp,bp */
		0x90, //0x5D,		/* pop bp */
		0xCB		/* retf */
	};
	code16[4] = (rdwr & 0xFF);	/* Interrupt to call */
	dosmemput(code16, sizeof(code16), __tb + 16);

	memset(&regs, 0, sizeof(regs));

/*	{
		unsigned char buf[1024];
		dosmemget(__tb, 1024, buf);
		fprintf(stderr, "Packet: %04lx %02x %p\n",
			pkt->sector, pkt->count, pkt->buf);
		dosmemput(buf, 1024, __tb);
	} */

	if (pkt->sector < 65536L)
	{
		regs.x.ax = (rdwr >> 8);
		regs.x.cx = pkt->count;
		regs.x.dx = pkt->sector;
		regs.x.bx = (__tb & 0x0F);
		regs.x.ds = (__tb >> 4) + 2;
		regs.x.ip = (__tb & 0x0F);
		regs.x.cs = (__tb >> 4) + 1;
		_go32_dpmi_simulate_fcall(&regs);

		if (!(regs.x.flags & 1))
			return DSK_ERR_OK;
/*	{
		unsigned char buf[1024];
		dosmemget(__tb, 1024, buf);
		fprintf(stderr, "smalldrive: error 0x%04x\n", regs.x.ax);
		dosmemput(buf, 1024, __tb);
	} */
/* Returns 0x207 on a >32Mb drive, 0x0001 on a FAT32 drive */
		if (regs.x.ax != 0x207 && regs.x.ax != 0x0001 && 
				regs.x.ax != 0x701F)
			return translate_error(regs.x.ax);
	}
	/* OK. Try the bigdrive version of the call */
	addr = __tb + 32;

	_farpokew(_dos_ds, __tb,   pkt->sector);
	_farpokew(_dos_ds, __tb+2, pkt->sector >> 16);
	_farpokew(_dos_ds, __tb+4, pkt->count);
	_farpokew(_dos_ds, __tb+6, addr & 0x0F);
	_farpokew(_dos_ds, __tb+8, addr >> 4);
/*
		{
			unsigned char buf[1024];
			int n;
			dosmemget(__tb, 1024, buf);
			for (n = 0; n < 32; n++)
			{
				printf("%02x ", buf[n]);
			}
			printf("\n");
			dosmemput(buf, 1024, __tb);
		} */
	if (try_bigdrive)
	{

		regs.x.ax = (rdwr >> 8);
		regs.x.cx = 0xFFFF;
		regs.x.bx = (__tb & 0x0F);
		regs.x.ds = (__tb >> 4);
		regs.x.ip = (__tb & 0x0F);
		regs.x.cs = (__tb >> 4) + 1;
		_go32_dpmi_simulate_fcall(&regs);

		if (!(regs.x.flags & 1))
			return DSK_ERR_OK;
/*	{
		unsigned char buf[1024];
		dosmemget(__tb, 1024, buf);
		fprintf(stderr, "bigdrive: error 0x%04x\n", regs.x.ax);
		dosmemput(buf, 1024, __tb);
	} */
/* Returns 0x0001 on a FAT32 drive, 0x701F on a FAT16 drive under WinME */
		if (regs.x.ax != 0x207 && regs.x.ax != 0x0001 &&
		    regs.x.ax != 0x701F)
			return translate_error(regs.x.ax);
	}
/*	{
		unsigned char buf[1024];
		dosmemget(__tb, 1024, buf);
		fprintf(stderr, "Error 207. Trying fat32\n");
		dosmemput(buf, 1024, __tb);
	} */

	/* That failed. Try the FAT32 version */
	regs.x.ax = 0x7305;
	regs.x.bx = __tb & 0x0F;
	regs.x.cx = 0xFFFF;
	regs.x.dx = (rdwr >> 8) + 1;	/* Drive */
	regs.x.ds = (__tb >> 4);
	regs.x.ip = (__tb & 0x0F);
	regs.x.cs = (__tb >> 4) + 1;
	code16[4] = 0x21;		/* Interrupt to call */
	dosmemput(code16, sizeof(code16), __tb + 16);
	_go32_dpmi_simulate_fcall(&regs);

	if (!(regs.x.flags & 1))
		return DSK_ERR_OK;
	return translate_error(regs.x.ax);
}

#else

typedef struct
{
	unsigned long sector;
	unsigned short count;
	void far *buf;	
} INT25_PACKET;

dsk_err_t int25_rdwr(int rdwr, INT25_PACKET *pkt);
#endif






dsk_err_t int25_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			     void *buf, dsk_pcyl_t cylinder,
			      dsk_phead_t head, dsk_psect_t sector)
{
	INT25_DSK_DRIVER *int25self;         
	INT25_PACKET pkt;
	dsk_err_t error;
	short unit;
	
	if (!buf || !self || !geom || self->dr_class != &dc_dosint25) 
		return DSK_ERR_BADPTR;
	int25self = (INT25_DSK_DRIVER *)self;
/*
	fprintf(stderr, "int25_read: cyl=%d head=%d sector=%d\n",
			cylinder, head, sector);
*/
	pkt.sector = ((((long)cylinder * geom->dg_heads) + (long)head) * 
			geom->dg_sectors) + sector - geom->dg_secbase;
	pkt.count = 1;
	pkt.buf = buf;
	unit = ((int25self->int25_unit) << 8) | 0x25;
#ifdef DOS16FLOPPY
	error = translate_error(int25_rdwr(unit, &pkt));

	return error;
#else
	error = int25_rdwr(unit, &pkt);
	dosmemget(__tb + 32, geom->dg_secsize, buf);
	return error;
#endif
}

dsk_err_t int25_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			     const void *buf, dsk_pcyl_t cylinder,
			      dsk_phead_t head, dsk_psect_t sector)
{
	INT25_DSK_DRIVER *int25self;         
	INT25_PACKET pkt;
	dsk_err_t error;
	short unit;
	
	if (!buf || !self || !geom || self->dr_class != &dc_dosint25) 
		return DSK_ERR_BADPTR;
	int25self = (INT25_DSK_DRIVER *)self;

	pkt.sector = ((((long)cylinder * geom->dg_heads) + (long)head) * 
			geom->dg_sectors) + sector - geom->dg_secbase;
	pkt.count = 1;
	pkt.buf = (void *)buf;
	unit = ((int25self->int25_unit) << 8) | 0x26;
#ifdef DOS16FLOPPY
	error = translate_error(int25_rdwr(unit, &pkt));

	return error;
#else
	dosmemput(buf, geom->dg_secsize, __tb + 32);
	error = int25_rdwr(unit, &pkt);
	return error;
#endif

}

dsk_err_t int25_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				const DSK_FORMAT *format, unsigned char filler)
{
	/* XXX Implement this, presumably using ioctls */
	return DSK_ERR_NOTIMPL;
}

dsk_err_t int25_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	INT25_DSK_DRIVER *int25self;         
#ifdef DOS16FLOPPY
	union REGS rg;
	struct SREGS sg;
#else
	__dpmi_regs reg;
#endif
	unsigned char bpb[92];
	long sectot, factor;
	int cflag, ax;

	/* XXX Support FAT32 drives here */
	if (!self || !geom || self->dr_class != &dc_dosint25) 
		return DSK_ERR_BADPTR;
	int25self = (INT25_DSK_DRIVER *)self;

	memset(bpb, 0, sizeof(bpb));

	if (int25self->int25_dosver < 0x314)	
					/* DOS pre 3.20 doesn't have this 
					 * ioctl. Fall back to our default
					 * geometry probe.*/
	{
		return dsk_defgetgeom(self, geom);	
	}

#ifdef DOS16FLOPPY
	rg.x.ax = 0x440D;
	rg.h.bl = int25self->int25_unit + 1;
	rg.x.cx = 0x0860;	/* Get device parameters */
	rg.x.dx = rg.x.di = FP_OFF(bpb);
	sg.ds   = rg.x.si = FP_SEG(bpb);
	intdosx(&rg, &rg, &sg);
/*	fprintf(stderr, "rg.x.cflag=0x%04x rg.x.ax=0x%04x\n", 
			rg.x.cflag, rg.x.ax); */
	cflag = rg.x.cflag;
	ax = rg.x.ax;
#else
	reg.x.ax = 0x440D;
	reg.h.bl = int25self->int25_unit + 1;
	reg.x.cx = 0x0860;	/* Get device parameters */
	reg.x.dx = reg.x.di = (__tb & 0x0F);
	reg.x.ds = reg.x.si = (__tb >> 4);
	dosmemput(bpb, sizeof(bpb), __tb);
	__dpmi_int(0x21, &reg);
	dosmemget(__tb, sizeof(bpb), bpb);
	cflag = reg.x.flags & 1;
	ax = reg.x.ax;
#endif
	if (cflag) switch(ax)
	{
		case 1: return DSK_ERR_NOTIMPL;	/* Invalid function */
		case 5: return DSK_ERR_ACCESS;	/* Access denied */
		case 6:	return DSK_ERR_ECHECK;	/* Invalid handle */
		default: return DSK_ERR_UNKNOWN;
	}
	sectot = bpb[15] + 256 * bpb[16];
	if (sectot == 0) sectot = *((long *)&bpb[28]);
	
	geom->dg_sidedness = SIDES_ALT; 
	geom->dg_heads     = bpb[22] + 256 * bpb[23];
	geom->dg_sectors   = bpb[20] + 256 * bpb[21];
	geom->dg_secbase   = 1;
	geom->dg_secsize   = bpb[7] + 256 * bpb[8];
	factor = geom->dg_heads * geom->dg_sectors;
	if (factor)
		geom->dg_cylinders = (sectot /*+ factor - 1*/) / factor;
	else	geom->dg_cylinders = 0;
	geom->dg_datarate  = RATE_SD;
	geom->dg_rwgap     = 42;
	geom->dg_fmtgap    = 82;
	geom->dg_fm        = 0;
	geom->dg_nomulti   = 0;
	geom->dg_noskip    = 0;
	return DSK_ERR_OK;
}




#endif	// def DOS16FLOPPY
