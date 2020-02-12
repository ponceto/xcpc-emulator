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
#include "drvwin16.h"

#ifdef WIN16FLOPPY
/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass. */
#include <dos.h>
 
DRV_CLASS dc_win16 = 
{
	sizeof(WIN16_DSK_DRIVER),
	"floppy",
	"Win16 floppy driver",

	win16_open,		/* open */
	win16_creat,	/* create new */
	win16_close,	/* close */
	win16_read,		/* read sector, working from physical address */
	win16_write,	/* write sector, working from physical address */
	win16_format,	/* format track, physical */
	NULL,			/* probe geometry */
	win16_secid,	/* get sector ID */
	NULL,
	win16_status
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



/* The following wrapper functions are translated from the
 * Win32c I/O functions; they may not be necessary under Win16 */ 

#ifdef USE_GDPARAMS
static int GetDriveParams( int volume, DRIVEPARAMS* pParam ) /*{{{*/
  { 
  union REGS reg;          
  struct SREGS sreg;

  reg.x.ax = 0x440d; /* IOCTL for block device */
  reg.x.bx = volume; /* one-based drive number */
  reg.x.cx = 0x0860; /* Get Device params */
  reg.x.dx = FP_OFF(pParam);
  sreg.ds  = FP_SEG(pParam);
  reg.x.cflag = 1;  /* preset the carry flag */
  
  int86x(0x13, &reg, &reg, &sreg);

  if (reg.x.cflag) return reg.x.ax;
  return 0;
  }       
  
  
  
/*}}}*/
static int SetDriveParams( int volume, DRIVEPARAMS* pParam ) /*{{{*/
  {
  union REGS reg;          
  struct SREGS sreg;

  reg.x.ax = 0x440d; /* IOCTL for block device */
  reg.x.bx = volume; /* one-based drive number */
  reg.x.cx = 0x0840; /* Set Device params */
  reg.x.dx = FP_OFF(pParam);
  sreg.ds  = FP_SEG(pParam);
  reg.x.cflag = 1;  /* preset the carry flag */
  
  int86x(0x13, &reg, &reg, &sreg);

  if (reg.x.cflag) return reg.x.ax;
  return 0;
  }
/*}}}*/
#endif	/* def USE_GDPARAMS */


#include "w16defs.h"


static int GetMediaID( int volume, MID* pMid ) /*{{{*/
  {
  union REGS reg;          
  struct SREGS sreg;

  reg.x.ax = 0x440d; /* IOCTL for block device */
  reg.x.bx = volume; /* one-based drive number */
  reg.x.cx = 0x0866; /* Get media ID */
  reg.x.dx = FP_OFF(pMid);
  sreg.ds  = FP_SEG(pMid);
  reg.x.cflag = 1;  /* preset the carry flag */
  
  int86x(0x13, &reg, &reg, &sreg);

  if (reg.x.cflag) return reg.x.ax;
  return 0;
  }
  
  
  
/*}}}*/
static int VolumeCheck(int volume, WORD* flags ) /*{{{*/
{
  union REGS reg;          

  reg.x.ax = 0x4409; /* Is Drive Remote */
  reg.x.bx = volume; /* one-based drive number */
  reg.x.cflag = 1;  /* preset the carry flag */

  int86(0x13, &reg, &reg);

  if (reg.x.cflag) return reg.x.ax;
  *flags = reg.x.dx;
  return 0;
}


/*}}}*/
static int LockLogicalVolume( int volume, int lock_level, int permissions) /*{{{*/
{
  union REGS reg;          

  reg.x.ax = 0x440D; /* Is Drive Remote */
  reg.h.bh = lock_level;
  reg.h.bl = volume; /* one-based drive number */
  reg.x.cx = 0x084A;
  reg.x.dx = permissions;
  reg.x.cflag = 1;  /* preset the carry flag */

  int86(0x13, &reg, &reg);

  if (reg.x.cflag) return reg.x.ax;

  return 0;
}


/*}}}*/
static int UnlockLogicalVolume( int volume ) /*{{{*/
{
  union REGS reg;          

  reg.x.ax = 0x440D; /* Is Drive Remote */
  reg.x.bx = volume; /* one-based drive number */
  reg.x.cx = 0x086A;
  reg.x.cflag = 1;  /* preset the carry flag */

  int86(0x13, &reg, &reg);

  if (reg.x.cflag) return reg.x.ax;

  return 0;
}
/*}}}*/


/* For a floppy drive, opening it and creating it are the same thing */
dsk_err_t win16_creat(DSK_DRIVER *self, const char *filename)
{
	return win16_open(self, filename);
}                                                                                        


dsk_err_t win16_open(DSK_DRIVER *self, const char *filename)
{
	WIN16_DSK_DRIVER *w16self;
	char vname[20];
	int driveno;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_win16) return DSK_ERR_BADPTR;
	w16self = (WIN16_DSK_DRIVER *)self;

	/* Not a drive? Not interested. */
	if (strlen(filename) != 2 || filename[1] != ':') return DSK_ERR_NOTME;
	w16self->w16_unit  = -1;

	vname[0] = (char)toupper(filename[0]);
	driveno = vname[0] - 'A' + 1;   /* 1=A: 2=B: */

	w16self->w16_unit = driveno;
	return DSK_ERR_OK;

} 


dsk_err_t win16_close(DSK_DRIVER *self)
{
	WIN16_DSK_DRIVER *w16self;

	if (self->dr_class != &dc_win16) return DSK_ERR_BADPTR;
	w16self = (WIN16_DSK_DRIVER *)self;
/*	UnlockLogicalVolume(w16self->w16_unit); */

	return DSK_ERR_OK;
}   
        
/* Code to alter the diskette parameter table (pointed at by INT 0x1E). 
 * This is a real-mode DOS interrupt, but seems to be supported in all
 * Windows versions except NT 5.x (2000 and XP). Under 2000/XP, INT 
 * 0x1E points at an NTVDM trap; attempts to write to it cause a 
 * GPF. Fortunately we can tell if this is the case; the first 2
 * bytes of the trap code are 0xC4, which wouldn't be sensible values
 * for the head load/unload time.   
 *
 * Also, of course, setting up your own geometry under NT has no effect
 * because for 16-bit apps the FDC is emulated in software. 
 *
 */
static int set_dpt(unsigned char *dpt)
{
	unsigned char __far *int1e = (unsigned char *)(_dos_getvect(0x1e));                                   
	int n;
          
   	if (int1e[0] == 0xC4 && int1e[1] == 0xC4) return 0;       
	for (n = 0; n < 11; n++) int1e[n] = dpt[n]; 
	return 1;	
}       
             
             
static int get_dpt(unsigned char *dpt)
{
	unsigned char __far *int1e = (unsigned char *)(_dos_getvect(0x1e));                                   
	int n;
    
   	if (int1e[0] == 0xC4 && int1e[1] == 0xC4) return 0;       
	for (n = 0; n < 11; n++) dpt[n] = int1e[n]; 
	return 1;	
}       


       
                             
static int win16_checkgeom(WIN16_DSK_DRIVER *w16self, const DSK_GEOMETRY *geom, 
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
dsk_err_t win16_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	WIN16_DSK_DRIVER *w16self;
	union REGS reg;
	struct SREGS sreg;
	dsk_err_t err;
	unsigned char old_dpt[11];
	int tries;
	
	if (!buf || !self || !geom || self->dr_class != &dc_win16) return DSK_ERR_BADPTR;
	w16self = (WIN16_DSK_DRIVER *)self;
	err = win16_checkgeom(w16self, geom, old_dpt, 0xF6);
	if (err) return err;

	for (tries = 0; tries < 3; tries++)	
	{
		reg.x.ax      = 0x0201;  /* Read 1 sector */
		reg.x.bx      = FP_OFF(buf);
		reg.h.ch      = cylinder;
		reg.h.cl      = sector;
		reg.h.dh      = head;
		reg.h.dl      = w16self->w16_unit - 1;
		reg.x.cflag   = 1;
		sreg.es = sreg.ds = FP_SEG(buf);
		int86x(0x13, &reg, &reg, &sreg);                  
		if (!reg.x.cflag) break;
	}
	set_dpt(old_dpt);
	if ( reg.x.cflag ) return translate_int13_error(reg.h.ah);
	return DSK_ERR_OK;
}

/* Write sector (Win95 version) */
dsk_err_t win16_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
	                         const void *buf, dsk_pcyl_t cylinder,
	                          dsk_phead_t head, dsk_psect_t sector)
{
	WIN16_DSK_DRIVER *w16self;
	union REGS reg;
	struct SREGS sreg;
	dsk_err_t err;
	unsigned char old_dpt[11];
	int tries;

	if (!buf || !self || !geom || self->dr_class != &dc_win16) return DSK_ERR_BADPTR;
	w16self = (WIN16_DSK_DRIVER *)self;
	if (w16self->w16_readonly) return DSK_ERR_RDONLY;
	err = win16_checkgeom(w16self, geom, old_dpt, 0xF6);
	if (err) return err;

	for (tries = 0; tries < 3; tries++)	
	{
		reg.x.ax      = 0x0301;  /* Read 1 sector */
		reg.x.bx      = FP_OFF(buf);
		reg.h.ch      = cylinder;
		reg.h.cl      = sector;
		reg.h.dh      = head;
		reg.h.dl      = w16self->w16_unit - 1;
		reg.x.cflag   = 1;
		sreg.es = sreg.ds = FP_SEG(buf);
		int86x(0x13, &reg, &reg, &sreg);
		if (!reg.x.cflag) break;
	}
	set_dpt(old_dpt);
	if ( reg.x.cflag ) return translate_int13_error(reg.h.ah);
	return DSK_ERR_OK;
}




dsk_err_t win16_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
	                            dsk_pcyl_t cylinder, dsk_phead_t head,
	                            const DSK_FORMAT *format, unsigned char filler)
{
	WIN16_DSK_DRIVER *w16self;
	union REGS reg;
	struct SREGS sreg;
	dsk_err_t err;
	unsigned char *trkbuf;
	unsigned int n, tries;
	unsigned char old_dpt[11];

	if (!format || !self || !geom || self->dr_class != &dc_win16) return DSK_ERR_BADPTR;
	w16self = (WIN16_DSK_DRIVER *)self;
	if (w16self->w16_readonly) return DSK_ERR_RDONLY;
	
	trkbuf = dsk_malloc(4 * geom->dg_sectors);
	if (!trkbuf) return DSK_ERR_NOMEM;

	err = win16_checkgeom(w16self, geom, old_dpt, filler);
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
		reg.h.dl      = w16self->w16_unit - 1;
		reg.x.cflag   = 1;

		int86x(0x13, &reg, &reg, &sreg);
		if (!reg.x.cflag) break;
	}
	dsk_free(trkbuf);
	set_dpt(old_dpt);
	if ( reg.x.cflag ) return translate_int13_error(reg.h.ah);
	return DSK_ERR_OK;
}
                      
                      
dsk_err_t win16_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
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

        err          = win16_read(self, geom, buf, cylinder, head, psect=0x01);
        if (err) err = win16_read(self, geom, buf, cylinder, head, psect=0x41);
        if (err) err = win16_read(self, geom, buf, cylinder, head, psect=0xC1);

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
dsk_err_t win16_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	WIN16_DSK_DRIVER *w16self;
	union REGS reg;
	unsigned char old_dpt[11];

	if (!result || !self || !geom || self->dr_class != &dc_win16) return DSK_ERR_BADPTR;
	w16self = (WIN16_DSK_DRIVER *)self;
	if (w16self->w16_readonly) *result |= DSK_ST3_RO;

	win16_checkgeom(w16self, geom, old_dpt, 0xF6);
	/* See if the drive supports a changeline. If so, probe it. */
	reg.x.ax = 0x15FF;	/* Get drive configuration */ 
	reg.x.cx = 0xFFFF;
	reg.x.dx = w16self->w16_unit - 1;
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
	reg.x.dx = w16self->w16_unit - 1;
	reg.x.si = 0;
	reg.x.cflag = 1;
	int86(0x13, &reg, &reg);
	set_dpt(old_dpt);                               
                                   
	/* If we get a "not ready", then reset the "ready" bit */
	if ((reg.h.ah & 0xFF) == 0x80) *result &= ~DSK_ST3_READY;
	return DSK_ERR_OK;
}

            

    
#endif	// def WIN16FLOPPY
