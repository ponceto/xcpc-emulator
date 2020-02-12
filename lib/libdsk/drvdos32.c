/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2006  John Elliott <seasip.webmaster@gmail.com>       *
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
#include "drvdos32.h"

#ifdef DOS32FLOPPY
/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass. */
#include <dos.h>
#include <bios.h>
#include <sys/farptr.h>
#include <go32.h>
 
DRV_CLASS dc_dos32 = 
{
	sizeof(DOS32_DSK_DRIVER),
	"floppy",
	"DJGPP/PC BIOS floppy driver",

	dos32_open,		/* open */
	dos32_creat,	/* create new */
	dos32_close,	/* close */
	dos32_read,		/* read sector, working from physical address */
	dos32_write,	/* write sector, working from physical address */
	dos32_format,	/* format track, physical */
	NULL,			/* probe geometry */
	dos32_secid,	/* get sector ID */
	NULL,
	dos32_status
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
dsk_err_t dos32_creat(DSK_DRIVER *self, const char *filename)
{
	return dos32_open(self, filename);
}                                                                                        


dsk_err_t dos32_open(DSK_DRIVER *self, const char *filename)
{
	DOS32_DSK_DRIVER *d32self;
	char vname[20];
	int driveno, maxdrive;
	union REGS reg;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_dos32) return DSK_ERR_BADPTR;
	d32self = (DOS32_DSK_DRIVER *)self;

	/* Not a drive? Not interested. */
	if (strlen(filename) != 2 || filename[1] != ':') return DSK_ERR_NOTME;
	d32self->d32_unit  = -1;

	/* Check against count of drives */
	int86(0x11, &reg, &reg);
	maxdrive = (reg.w.ax >> 6) & 3;

	vname[0] = (char)toupper(filename[0]);
	driveno = vname[0] - 'A' + 1;   /* 1=A: 2=B: */
	if (driveno > (1 + maxdrive)) return DSK_ERR_NOTME;

	d32self->d32_unit = driveno;
	return DSK_ERR_OK;

} 


dsk_err_t dos32_close(DSK_DRIVER *self)
{
	DOS32_DSK_DRIVER *d32self;

	if (self->dr_class != &dc_dos32) return DSK_ERR_BADPTR;
	d32self = (DOS32_DSK_DRIVER *)self;

	return DSK_ERR_OK;
}   
        
/* Code to alter the diskette parameter table (pointed at by INT 0x1E). */
static int set_dpt(unsigned char *dpt)
{
	long int1e  = _farpeekw(_dos_ds, 0x1E * 4) + 16 *
		      _farpeekw(_dos_ds, 0x1E * 4 + 2);

	int n;
          
	if (_farpeekw(_dos_ds, int1e) == 0xC4C4) return 0;
	for (n = 0; n < 11; n++) _farpokeb(_dos_ds, int1e + n, dpt[n]); 
	return 1;	
}       
             
             
static int get_dpt(unsigned char *dpt)
{
	long int1e  = _farpeekw(_dos_ds, 0x1E * 4) + 16 *
		      _farpeekw(_dos_ds, 0x1E * 4 + 2);

	int n;
          
	if (_farpeekw(_dos_ds, int1e) == 0xC4C4) return 0;
	for (n = 0; n < 11; n++) dpt[n] = _farpeekb(_dos_ds, int1e + n); 
	return 1;	
}       


       
                             
static int dos32_checkgeom(DOS32_DSK_DRIVER *d32self, const DSK_GEOMETRY *geom, 
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
dsk_err_t dos32_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	DOS32_DSK_DRIVER *d32self;
	struct diskinfo_t reg;
	dsk_err_t err;
	unsigned char old_dpt[11];
   	unsigned short ax;
 
	if (!buf || !self || !geom || self->dr_class != &dc_dos32) return DSK_ERR_BADPTR;
	d32self = (DOS32_DSK_DRIVER *)self;
	err = dos32_checkgeom(d32self, geom, old_dpt, 0xF6);
	if (err) return err;

	reg.drive     = d32self->d32_unit - 1;
	reg.head      = head;
	reg.track     = cylinder;
	reg.sector    = sector;
	reg.nsectors  = 1;
	reg.buffer    = buf;
	
	ax = _bios_disk(_DISK_READ, &reg);
	set_dpt(old_dpt);
	if (ax & 0xFF00) return translate_int13_error(ax >> 8);
	return DSK_ERR_OK;
}

/* Write sector (Win95 version) */
dsk_err_t dos32_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	DOS32_DSK_DRIVER *d32self;
	struct diskinfo_t reg;
	dsk_err_t err;
	unsigned char old_dpt[11];
	unsigned short ax;

	if (!buf || !self || !geom || self->dr_class != &dc_dos32) return DSK_ERR_BADPTR;
	d32self = (DOS32_DSK_DRIVER *)self;
	if (d32self->d32_readonly) return DSK_ERR_RDONLY;
	err = dos32_checkgeom(d32self, geom, old_dpt, 0xF6);
	if (err) return err;

	reg.drive     = d32self->d32_unit - 1;
	reg.head      = head;
	reg.track     = cylinder;
	reg.sector    = sector;
	reg.nsectors  = 1;
	reg.buffer    = (void *)buf;
	
	ax = _bios_disk(_DISK_WRITE, &reg);
	set_dpt(old_dpt);
	if (ax & 0xFF00) return translate_int13_error(ax >> 8);
	return DSK_ERR_OK;
}





dsk_err_t dos32_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	DOS32_DSK_DRIVER *d32self;
	struct diskinfo_t reg;
	dsk_err_t err;
	unsigned char *trkbuf;
	unsigned int n;
	unsigned char old_dpt[11];
	unsigned short ax;

	if (!format || !self || !geom || self->dr_class != &dc_dos32) return DSK_ERR_BADPTR;
	d32self = (DOS32_DSK_DRIVER *)self;
	if (d32self->d32_readonly) return DSK_ERR_RDONLY;
	
	trkbuf = dsk_malloc(4 * geom->dg_sectors);
	if (!trkbuf) return DSK_ERR_NOMEM;

	err = dos32_checkgeom(d32self, geom, old_dpt, filler);
	if (err) { dsk_free(trkbuf); return err; }
	for (n = 0; n < geom->dg_sectors; n++)
	{
		trkbuf[4*n  ] = (unsigned char)format[n].fmt_cylinder;
		trkbuf[4*n+1] = (unsigned char)format[n].fmt_head;
		trkbuf[4*n+2] = (unsigned char)format[n].fmt_sector;
		trkbuf[4*n+3] = dsk_get_psh(format[n].fmt_secsize);
	}

	reg.drive     = d32self->d32_unit - 1;
	reg.head      = head;
	reg.track     = cylinder;
	reg.sector    = 1;
	reg.nsectors  = geom->dg_sectors;
	reg.buffer    = trkbuf;
	
	ax = _bios_disk(_DISK_FORMAT, &reg);
	dsk_free(trkbuf);
	set_dpt(old_dpt);
	if (ax & 0xFF00) return translate_int13_error(ax >> 8);
	return DSK_ERR_OK;
}
                      
                      
dsk_err_t dos32_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
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

        err          = dos32_read(self, geom, buf, cylinder, head, psect=0x01);
        if (err) err = dos32_read(self, geom, buf, cylinder, head, psect=0x41);
        if (err) err = dos32_read(self, geom, buf, cylinder, head, psect=0xC1);

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
dsk_err_t dos32_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	DOS32_DSK_DRIVER *d32self;
	union REGS reg;
	unsigned char old_dpt[11];

	if (!result || !self || !geom || self->dr_class != &dc_dos32) return DSK_ERR_BADPTR;
	d32self = (DOS32_DSK_DRIVER *)self;
	if (d32self->d32_readonly) *result |= DSK_ST3_RO;

	dos32_checkgeom(d32self, geom, old_dpt, 0xF6);
	/* See if the drive supports a changeline. If so, probe it. */
	reg.w.ax = 0x15FF;	/* Get drive configuration */ 
	reg.w.cx = 0xFFFF;
	reg.w.dx = d32self->d32_unit - 1;
	reg.w.cflag = 1;   
	int86(0x13, &reg, &reg);

	switch(reg.h.ah)
	{
		case 0: *result &= ~DSK_ST3_READY; set_dpt(old_dpt); return DSK_ERR_OK;
		case 2: break;
		default: set_dpt(old_dpt); return DSK_ERR_OK;	
	}      
	
	/* Changeline supported. Read it. */
	reg.w.ax = 0x3200;
	reg.w.dx = d32self->d32_unit - 1;
	reg.w.si = 0;
	reg.w.cflag = 1;
	int86(0x13, &reg, &reg);
	set_dpt(old_dpt);                               
                                   
	/* If we get a "not ready", then reset the "ready" bit */
	if ((reg.h.ah & 0xFF) == 0x80) *result &= ~DSK_ST3_READY;
	return DSK_ERR_OK;
}

            


#endif	// def DOS32FLOPPY
