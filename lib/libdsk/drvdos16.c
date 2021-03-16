/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <seasip.webmaster@gmail.com>            *
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
#include "drvdos16.h"

#ifdef DOS16FLOPPY
/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass. */
#include <dos.h>
 
DRV_CLASS dc_dos16 = 
{
	sizeof(DOS16_DSK_DRIVER),
	"floppy",
	"PC BIOS floppy driver",

	dos16_open,		/* open */
	dos16_creat,	/* create new */
	dos16_close,	/* close */
	dos16_read,		/* read sector, working from physical address */
	dos16_write,	/* write sector, working from physical address */
	dos16_format,	/* format track, physical */
	NULL,			/* probe geometry */
	dos16_secid,	/* get sector ID */
	NULL,
	dos16_status
};



/* Translate the error codes returned by INT13, INT25 or INT26 (Win32c)
 * to LIBDSK errors */
static dsk_err_t translate_int13_error(unsigned ax)
{
	switch(ax)
	{
		case 0x00: return DSK_ERR_OK;
		case 0x01: return DSK_ERR_BADPARM;
		case 0x02: return DSK_ERR_NOADDR;
		case 0x03: return DSK_ERR_RDONLY;
		case 0x04: return DSK_ERR_NODATA;
		case 0x06: return DSK_ERR_CHANGED;
		case 0x08:
		case 0x09: return DSK_ERR_OVERRUN;
		case 0x10: return DSK_ERR_DATAERR;
		case 0x20: return DSK_ERR_CTRLR;
		case 0x31: return DSK_ERR_NOTRDY;
		case 0x0C:
		case 0x40: return DSK_ERR_SEEKFAIL;
		case 0x80: return DSK_ERR_NOTRDY;
	}
	return DSK_ERR_UNKNOWN;
}



/* For a floppy drive, opening it and creating it are the same thing */
dsk_err_t dos16_creat(DSK_DRIVER *self, const char *filename)
{
	return dos16_open(self, filename);
}											


dsk_err_t dos16_open(DSK_DRIVER *self, const char *filename)
{
	DOS16_DSK_DRIVER *d16self;
	char vname[20];
	int driveno, maxdrive;
	union REGS rg;
	unsigned char far *biosdata = MK_FP(0x40, 0);

	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_dos16) return DSK_ERR_BADPTR;
	d16self = (DOS16_DSK_DRIVER *)self;
	/* Is this an Apricot PC? If so, there's no INT 13h */
	if (biosdata[1] <= 2    && 
	    biosdata[4] == 0x34 && 
	    biosdata[5] == 0x12 &&
	    biosdata[6] == 0x78 &&
	    biosdata[7] == 0x56)
	{
		return DSK_ERR_NOTME;	
	}
	/* Not a drive? Not interested. */
	if (strlen(filename) != 2 || filename[1] != ':') return DSK_ERR_NOTME;
	d16self->d16_unit  = -1;

	/* Is it one of the floppy drives? Start by getting a count of 
	 * floppies */
	int86(0x11, &rg, &rg);
	maxdrive = ((rg.x.ax >> 6) & 3);

	vname[0] = (char)toupper(filename[0]);
	driveno = vname[0] - 'A' + 1;   /* 1=A: 2=B: */

	/* If passed drive number exceeds count of floppies, fail */
	if (driveno > (1 + maxdrive))
	{
		return DSK_ERR_NOTME;
	}

	d16self->d16_unit = driveno;
	return DSK_ERR_OK;

} 


dsk_err_t dos16_close(DSK_DRIVER *self)
{
	DOS16_DSK_DRIVER *d16self;

	if (self->dr_class != &dc_dos16) return DSK_ERR_BADPTR;
	d16self = (DOS16_DSK_DRIVER *)self;

	return DSK_ERR_OK;
}   
	
/* Code to alter the diskette parameter table (pointed at by INT 0x1E). */
static int set_dpt(unsigned char *dpt)
{
	long far *vector = (long far *)MK_FP(0, 4 * 0x1E);
	unsigned char far *int1e = (unsigned char *)(*vector);
	int n;
	  
	if (int1e[0] == 0xC4 && int1e[1] == 0xC4) return 0;	   
	for (n = 0; n < 11; n++) int1e[n] = dpt[n]; 
	return 1;	
}	   
	     
	     
static int get_dpt(unsigned char *dpt)
{
	long far *vector = (long far *)MK_FP(0, 4 * 0x1E);
	unsigned char far *int1e = (unsigned char *)(*vector);
	int n;
    
   	if (int1e[0] == 0xC4 && int1e[1] == 0xC4) return 0;       
	for (n = 0; n < 11; n++) dpt[n] = int1e[n]; 
	return 1;	
}       


       
			     
static int dos16_checkgeom(DOS16_DSK_DRIVER *d16self, const DSK_GEOMETRY *geom, 
						   unsigned char *old_dpt, unsigned char filler)
{   
	unsigned char new_dpt[11];					 
	/* If get_dpt() fails, we're on Windows NT 5.x and can't change the geometry */
	if (!get_dpt(old_dpt)) return DSK_ERR_OK;				      
	get_dpt(new_dpt);
	new_dpt[3] = dsk_get_psh(geom->dg_secsize);	/* Bytes / sector */
	new_dpt[4] = geom->dg_sectors;				/* Sectors / track */
	new_dpt[5] = geom->dg_rwgap;				/* Read/write gap */
	new_dpt[6] = (new_dpt[3]) ? 0xFF : 0x80;	/* Data length */
	new_dpt[7] = geom->dg_fmtgap;				/* Format gap */
	new_dpt[8] = filler;						/* Format filler byte */
	set_dpt(new_dpt);	
	return DSK_ERR_OK;
}			     



/* Read sector  */
dsk_err_t dos16_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			     void *buf, dsk_pcyl_t cylinder,
			      dsk_phead_t head, dsk_psect_t sector)
{
	DOS16_DSK_DRIVER *d16self;
	union REGS reg;
	struct SREGS sreg;
	dsk_err_t err;
	unsigned char old_dpt[11];
	int tries;
	
	if (!buf || !self || !geom || self->dr_class != &dc_dos16) return DSK_ERR_BADPTR;
	d16self = (DOS16_DSK_DRIVER *)self;
	err = dos16_checkgeom(d16self, geom, old_dpt, 0xF6);
	if (err) return err;

	for (tries = 0; tries < 3; tries++)
	{
		reg.x.ax      = 0x0201;  /* Read 1 sector */
		reg.x.bx      = FP_OFF(buf);
		reg.h.ch      = cylinder;
		reg.h.cl      = sector;
		reg.h.dh      = head;
		reg.h.dl      = d16self->d16_unit - 1;
		reg.x.cflag   = 1;
		sreg.es = sreg.ds = FP_SEG(buf);
		int86x(0x13, &reg, &reg, &sreg);		  
		if (!reg.x.cflag) break;
	}
	set_dpt(old_dpt);
	if ( reg.x.cflag ) return translate_int13_error(reg.h.ah);
	return DSK_ERR_OK;
}

/* Write sector */
dsk_err_t dos16_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			     const void *buf, dsk_pcyl_t cylinder,
			      dsk_phead_t head, dsk_psect_t sector)
{
	DOS16_DSK_DRIVER *d16self;
	union REGS reg;
	struct SREGS sreg;
	dsk_err_t err;
	unsigned char old_dpt[11];
	int tries;

	if (!buf || !self || !geom || self->dr_class != &dc_dos16) return DSK_ERR_BADPTR;
	d16self = (DOS16_DSK_DRIVER *)self;
	if (d16self->d16_readonly) return DSK_ERR_RDONLY;
	err = dos16_checkgeom(d16self, geom, old_dpt, 0xF6);
	if (err) return err;

	for (tries = 0; tries < 3; tries++)
	{
		reg.x.ax      = 0x0301;  /* Read 1 sector */
		reg.x.bx      = FP_OFF(buf);
		reg.h.ch      = cylinder;
		reg.h.cl      = sector;
		reg.h.dh      = head;
		reg.h.dl      = d16self->d16_unit - 1;
		reg.x.cflag   = 1;
		sreg.es = sreg.ds = FP_SEG(buf);
		int86x(0x13, &reg, &reg, &sreg);
		if (!reg.x.cflag) break;
	}
	set_dpt(old_dpt);
	if ( reg.x.cflag ) return translate_int13_error(reg.h.ah);
	return DSK_ERR_OK;
}




dsk_err_t dos16_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				const DSK_FORMAT *format, unsigned char filler)
{
	DOS16_DSK_DRIVER *d16self;
	union REGS reg;
	struct SREGS sreg;
	dsk_err_t err;
	unsigned char *trkbuf;
	unsigned int n, tries;
	unsigned char old_dpt[11];

	if (!format || !self || !geom || self->dr_class != &dc_dos16) return DSK_ERR_BADPTR;
	d16self = (DOS16_DSK_DRIVER *)self;
	if (d16self->d16_readonly) return DSK_ERR_RDONLY;
	
	trkbuf = dsk_malloc(4 * geom->dg_sectors);
	if (!trkbuf) return DSK_ERR_NOMEM;

	err = dos16_checkgeom(d16self, geom, old_dpt, filler);
	if (err) { dsk_free(trkbuf); return err; }
	for (n = 0; n < geom->dg_sectors; n++)
	{
		trkbuf[4*n  ] = (unsigned char)format[n].fmt_cylinder;
		trkbuf[4*n+1] = (unsigned char)format[n].fmt_head;
		trkbuf[4*n+2] = (unsigned char)format[n].fmt_sector;
		trkbuf[4*n+3] = dsk_get_psh(format[n].fmt_secsize);
	}
	
	for (tries = 0; tries < 3; tries++)
	{
		reg.h.ah      = 0x05; 
		reg.h.al      = geom->dg_sectors;
		reg.x.bx      = FP_OFF(trkbuf);
		reg.h.ch      = cylinder;
		reg.h.cl      = 0;
		reg.h.dh      = head;
		reg.h.dl      = d16self->d16_unit - 1;
		reg.x.cflag   = 1;

		int86x(0x13, &reg, &reg, &sreg);
		if (!reg.x.cflag) break;
	}
	dsk_free(trkbuf);
	set_dpt(old_dpt);
	if ( reg.x.cflag ) return translate_int13_error(reg.h.ah);
	return DSK_ERR_OK;
}
		      
		      
dsk_err_t dos16_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				DSK_FORMAT *result)
{
	/* We don't implement a proper READ_SECTOR_ID, because Windows
	 * doesn't expose this functionality. Instead, emulate just enough
	 * to autodetect CPC-format discs (slowly) */
	dsk_err_t err;
	unsigned char *buf = dsk_malloc(geom->dg_secsize);
	unsigned char psect;

	if (geom->dg_fm) return DSK_ERR_NOADDR;

	if (!buf) return DSK_ERR_NOMEM;

	err	  = dos16_read(self, geom, buf, cylinder, head, psect=0x01);
	if (err) err = dos16_read(self, geom, buf, cylinder, head, psect=0x41);
	if (err) err = dos16_read(self, geom, buf, cylinder, head, psect=0xC1);

	if (result)
	{
		result->fmt_cylinder = cylinder;
		result->fmt_head     = head;
		result->fmt_sector   = psect;
		result->fmt_secsize  = geom->dg_secsize;
	}				    
    dsk_free(buf);
	return err;
}



/* XXX This code is untested! */
dsk_err_t dos16_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		      dsk_phead_t head, unsigned char *result)
{
	DOS16_DSK_DRIVER *d16self;
	union REGS reg;
	unsigned char old_dpt[11];

	if (!result || !self || !geom || self->dr_class != &dc_dos16) return DSK_ERR_BADPTR;
	d16self = (DOS16_DSK_DRIVER *)self;
	if (d16self->d16_readonly) *result |= DSK_ST3_RO;

	dos16_checkgeom(d16self, geom, old_dpt, 0xF6);
	/* See if the drive supports a changeline. If so, probe it. */
	reg.x.ax = 0x15FF;	/* Get drive configuration */ 
	reg.x.cx = 0xFFFF;
	reg.x.dx = d16self->d16_unit - 1;
	reg.x.cflag = 1;   
	int86(0x13, &reg, &reg);

	switch(reg.h.ah)
	{
		case 0: *result &= ~DSK_ST3_READY; set_dpt(old_dpt); return DSK_ERR_OK;
		case 2: break;
		default: set_dpt(old_dpt); return DSK_ERR_OK;	
	}      
	
	/* Changeline supported. Read it. */
	reg.x.ax = 0x1600;
	reg.x.dx = d16self->d16_unit - 1;
	reg.x.si = 0;
	reg.x.cflag = 1;
	int86(0x13, &reg, &reg);
	set_dpt(old_dpt);			       
				   
	/* If we get a "not ready", then reset the "ready" bit */
	if ((reg.h.ah & 0xFF) == 0x80) *result &= ~DSK_ST3_READY;
	return DSK_ERR_OK;
}
#endif	// def DOS16FLOPPY
