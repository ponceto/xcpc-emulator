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

/* DSK access using the Linux floppy driver. This might come in handy
 * for you if you are implementing direct access to a uPD765A on another
 * platform. */

#include "drvi.h"
#include "drvlinux.h"

#ifdef LINUXFLOPPY

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_UNISTD_H 
#include <unistd.h>
#endif



/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_linux = 
{
	sizeof(LINUX_DSK_DRIVER),
	"floppy",
	"Linux floppy driver",
	&linux_open,	/* open */
	&linux_creat,	/* create new */
	&linux_close,	/* close */
	&linux_read,	/* read sector, working from physical address */
	&linux_write,	/* write sector, working from physical address */
	&linux_format,	/* format track, physical */
	NULL,		/* Get (fixed) geometry */
	&linux_secid,	/* Get sector ID, physical */
	&linux_xseek,	/* seek to track */
	NULL,/*&linux_status,*/	/* get drive status [disabled] */
	&linux_xread,	/* read sector extended */
	&linux_xwrite,	/* write sector extended */
	&linux_tread,	/* read track */
	NULL,		/* linux_xtread */
	&linux_option_enum,	/* List options */
	&linux_option_set,	/* Set option */
	&linux_option_get	/* Get option */
};

/* Initialise a raw FDC command */
static void init_raw_cmd(struct floppy_raw_cmd *raw_cmd)
{
	raw_cmd->flags = 0;
	raw_cmd->track = 0;
	raw_cmd->data  = NULL;
	raw_cmd->kernel_data = NULL;
	raw_cmd->next  = NULL;
	raw_cmd->length = 0;
	raw_cmd->phys_length = 0;
	raw_cmd->buffer_length = 0;
	raw_cmd->cmd_count = 0;
	raw_cmd->reply_count = 0;
	raw_cmd->resultcode = 0;	
}

/* Translate an FDC765 error to LIBDSK */
static dsk_err_t xlt_error(unsigned char *st)
{

	if (st[0] & ST0_NR)  return DSK_ERR_NOTRDY;
	if (st[0] & ST0_ECE) return DSK_ERR_ECHECK;
	if (st[1] & ST1_OR)  return DSK_ERR_OVERRUN;
	if (st[1] & ST1_CRC) return DSK_ERR_DATAERR;
	if (st[1] & ST1_ND)  return DSK_ERR_NODATA;
	if (st[1] & ST1_WP)  return DSK_ERR_RDONLY;
	if (st[1] & ST1_MAM) return DSK_ERR_NOADDR;
	if (st[2] & ST2_CRC) return DSK_ERR_DATAERR;
	if (st[2] & ST2_WC)  return DSK_ERR_SEEKFAIL;
	if (st[2] & ST2_CM)  return DSK_ERR_NODATA;
	if (st[2] & ST2_BC)  return DSK_ERR_SEEKFAIL;
	if (st[2] & ST2_MAM) return DSK_ERR_NOADDR;
	return DSK_ERR_UNKNOWN;
}	


/* Convert a LIBDSK data rate to Linux kernel data rate */
static int get_rate(const DSK_GEOMETRY *dg)
{	
	switch(dg->dg_datarate)
	{
		case RATE_HD: return 0;
		case RATE_DD: return 1;
		case RATE_SD: return 2;
		case RATE_ED: return 3;
	}
	return 2;
}

/* Convert a head number to uPD765A format. Note that on non-PC hardware,
 * you'd then encode the unit number in the bottom 2 bits. */
static dsk_phead_t encode_head(DSK_DRIVER *self, dsk_phead_t head)
{
	LINUX_DSK_DRIVER *lxself = (LINUX_DSK_DRIVER *)self;
	/* Is the driver being forced to one or other side of the disc? */
	if (lxself->lx_forcehead >= 0) head = lxself->lx_forcehead;

	return head ? 4 : 0;
}




static dsk_err_t check_geom(LINUX_DSK_DRIVER *self, const DSK_GEOMETRY *dg)
{
	struct floppy_struct str;

	if (dg->dg_cylinders    == self->lx_geom.dg_cylinders &&
	    dg->dg_sectors      == self->lx_geom.dg_sectors   &&
	    dg->dg_heads        == self->lx_geom.dg_heads) return DSK_ERR_OK;

	str.sect    = dg->dg_sectors;
	str.head    = dg->dg_heads;
	str.track   = dg->dg_cylinders;
	str.stretch = 0;	/* LibDsk does its own double-stepping */
	str.gap     = dg->dg_rwgap;
	str.rate    = get_rate(dg);
	str.spec1   = 0xDF;	/* XXX Is it always this? */
	str.fmt_gap = dg->dg_fmtgap;

	if (ioctl(self->lx_fd, FDSETPRM, &str)) return DSK_ERR_SYSERR;
/* 1.1.10: Make the next command do a seek, in case the FDSETPRM
 * has moved the head. */
	self->lx_cylinder = ~0;
	memcpy(&self->lx_geom, dg, sizeof(*dg));
	return DSK_ERR_OK;
}


dsk_err_t linux_open(DSK_DRIVER *self, const char *filename)
{
	LINUX_DSK_DRIVER *lxself;
	struct stat st;
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	lxself->lx_fd = -1;
	lxself->lx_forcehead = -1;
	lxself->lx_doublestep = 0;
/* 
 * We are only interested in the file if it's a block device, major 2 
 */
	if (stat(filename, &st)) return DSK_ERR_NOTME;
	if (!S_ISBLK(st.st_mode)) return DSK_ERR_NOTME;	
	if (major(st.st_rdev) != 2) return DSK_ERR_NOTME;

	lxself->lx_cylinder = ~0;	
/* [v0.8.3] fdutils uses O_NONBLOCK when opening drives in fdrawcmd. This 
 * seems to allow FM-encoded discs and other discs which the Linux kernel
 * can't probe to be accepted, and it omits the long delay detecting CPC-
 * format discs. */
	lxself->lx_fd = open(filename, O_NONBLOCK | O_RDWR);
	if (lxself->lx_fd < 0)
	{
		lxself->lx_readonly = 1;
		lxself->lx_fd = open(filename, O_NONBLOCK | O_RDONLY);
	}
	if (lxself->lx_fd < 0) return DSK_ERR_SYSERR;
/* [v1.3.1] This appears to be necessary, otherwise an aborted dskscan will
 * not necessarily seek back to cylinder 0 */
	ioctl(lxself->lx_fd, FDCLRPRM);

	return DSK_ERR_OK;
}


/* For a floppy drive, opening it and creating it are the same thing, 
 * unless we wanted to mess around with mknod() here */
dsk_err_t linux_creat(DSK_DRIVER *self, const char *filename)
{
	return linux_open(self, filename);
}


dsk_err_t linux_close(DSK_DRIVER *self)
{
	LINUX_DSK_DRIVER *lxself;

	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;

	if (lxself->lx_fd > 0) 
	{
		if (close(lxself->lx_fd) == -1) return DSK_ERR_SYSERR;
		lxself->lx_fd = -1;
	}
	return DSK_ERR_OK;	
}
/*
static unsigned char fakebuf[512] = 
{
	0x03, 0x81, 0x50, 0x0a, 0x02, 0x01, 0x05, 0x02, 0x0c, 0x17, 
	0xe5, 0xe5, 0xe5, 0xe5, 0xe5, 0xe5
};
*/

dsk_err_t linux_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return linux_xread(self, geom, buf, cylinder, head, cylinder, 
			dg_x_head(geom, head), 
			dg_x_sector(geom, head, sector), geom->dg_secsize, 0);
}

dsk_err_t linux_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;
	dsk_err_t err;
	unsigned char mask = 0xFF;

	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	err = check_geom(lxself, geom);
	if (err) return err;

	if (geom->dg_noskip)  mask &= ~0x20;	/* Don't skip deleted data */
	if (geom->dg_fm & RECMODE_MASK) mask &= ~0x40;	/* FM recording mode */
	if (geom->dg_nomulti) mask &= ~0x80;	/* Disable multitrack */

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_READ | FD_RAW_INTR;
	if (cylinder != lxself->lx_cylinder) raw_cmd.flags |= FD_RAW_NEED_SEEK;
	raw_cmd.track = cylinder;
	if (lxself->lx_doublestep) raw_cmd.track *= 2;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.length= sector_size;
	raw_cmd.data  = buf;

	if (deleted && (*deleted))
			raw_cmd.cmd[raw_cmd.cmd_count++] = 0xEC    & mask;
	else		raw_cmd.cmd[raw_cmd.cmd_count++] = FD_READ & mask;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);
	raw_cmd.cmd[raw_cmd.cmd_count++] = cyl_expected;
	raw_cmd.cmd[raw_cmd.cmd_count++] = head_expected;
	raw_cmd.cmd[raw_cmd.cmd_count++] = sector;
	raw_cmd.cmd[raw_cmd.cmd_count++] = dsk_get_psh(sector_size);
	raw_cmd.cmd[raw_cmd.cmd_count++] = sector;
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_rwgap;
	raw_cmd.cmd[raw_cmd.cmd_count++] = (sector_size < 255) ? sector_size : 0xFF; 

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	memcpy(lxself->lx_status, raw_cmd.reply, 4);
	if (raw_cmd.reply[0] & 0x40) return xlt_error(raw_cmd.reply);

	if (deleted) deleted[0] = (raw_cmd.reply[2] & 0x40) ? 1 : 0;
	lxself->lx_cylinder = cylinder;
	return DSK_ERR_OK;
}


dsk_err_t linux_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return linux_xwrite(self, geom, buf, cylinder, head, cylinder, 
			dg_x_head(geom, head),
			dg_x_sector(geom, head, sector), 
			geom->dg_secsize, 0);
}


dsk_err_t linux_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;
	dsk_err_t err;
	unsigned char mask = 0xFF;

	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	err = check_geom(lxself, geom);
	if (err) return err;

	if (geom->dg_noskip)  mask &= ~0x20;	/* Don't skip deleted data */
	if (geom->dg_fm & RECMODE_MASK)      mask &= ~0x40;
	if (geom->dg_nomulti) mask &= ~0x80;

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_WRITE | FD_RAW_INTR;
	if (cylinder != lxself->lx_cylinder) raw_cmd.flags |= FD_RAW_NEED_SEEK;
	raw_cmd.track = cylinder;
	if (lxself->lx_doublestep) raw_cmd.track *= 2;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.length= sector_size;
	raw_cmd.data  = (void *)buf;
		
	if (deleted)	raw_cmd.cmd[raw_cmd.cmd_count++] = 0xE9     & mask;
	else		raw_cmd.cmd[raw_cmd.cmd_count++] = FD_WRITE & mask;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);
	raw_cmd.cmd[raw_cmd.cmd_count++] = cyl_expected;
	raw_cmd.cmd[raw_cmd.cmd_count++] = head_expected;
	raw_cmd.cmd[raw_cmd.cmd_count++] = sector;
	raw_cmd.cmd[raw_cmd.cmd_count++] = dsk_get_psh(sector_size);
	raw_cmd.cmd[raw_cmd.cmd_count++] = sector;
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_rwgap;
	raw_cmd.cmd[raw_cmd.cmd_count++] = (sector_size < 255) ? sector_size : 0xFF; 

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	memcpy(lxself->lx_status, raw_cmd.reply, 4);
	if (raw_cmd.reply[0] & 0x40) return xlt_error(raw_cmd.reply);
	lxself->lx_cylinder = cylinder;
	return DSK_ERR_OK;
}




dsk_err_t linux_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;
	int n;
	dsk_err_t err;
	unsigned char *buf;
	unsigned char mask = 0xFF;

	if (!self || !geom || !format) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	err = check_geom(lxself, geom);
	if (err) return err;
	if (geom->dg_fm & RECMODE_MASK)      mask &= ~0x40;
	if (geom->dg_nomulti) mask &= ~0x80;

	buf = dsk_malloc(geom->dg_sectors * 4);
	if (!buf) return DSK_ERR_NOMEM;
	for (n = 0; n < geom->dg_sectors; n++)
	{
		buf[n*4]   = format[n].fmt_cylinder;
		buf[n*4+1] = format[n].fmt_head;
		buf[n*4+2] = format[n].fmt_sector;
		buf[n*4+3] = dsk_get_psh(format[n].fmt_secsize);
	}

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_WRITE | FD_RAW_INTR;
	if (cylinder != lxself->lx_cylinder) raw_cmd.flags |= FD_RAW_NEED_SEEK;
	raw_cmd.track = cylinder;
	if (lxself->lx_doublestep) raw_cmd.track *= 2;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.length= 4 * geom->dg_sectors;
	raw_cmd.data  = buf;

	raw_cmd.cmd[raw_cmd.cmd_count++] = FD_FORMAT & mask;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);
	raw_cmd.cmd[raw_cmd.cmd_count++] = dsk_get_psh(geom->dg_secsize);
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_sectors;
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_fmtgap;
	raw_cmd.cmd[raw_cmd.cmd_count++] = filler;

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	memcpy(lxself->lx_status, raw_cmd.reply, 4);
	if (raw_cmd.reply[0] & 0x40) return xlt_error(raw_cmd.reply);
	lxself->lx_cylinder = cylinder;
	return DSK_ERR_OK;
}







dsk_err_t linux_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;
	unsigned char mask = 0xFF;
	dsk_err_t err;

	if (!self || !geom || !result) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	if (geom->dg_fm & RECMODE_MASK)      mask &= ~0x40;
	if (geom->dg_nomulti) mask &= ~0x80;

/* [v0.8.3] It was necessary to add this check correctly to detect 100k 
 * FM-recorded discs in a 5.25" drive */
	err = check_geom(lxself, geom);
	if (err) return err;

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_INTR;
	if (cylinder != lxself->lx_cylinder)  raw_cmd.flags |= FD_RAW_NEED_SEEK;
	raw_cmd.track = cylinder;
	if (lxself->lx_doublestep) raw_cmd.track *= 2;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.cmd[raw_cmd.cmd_count++] = FD_READID & mask;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	memcpy(lxself->lx_status, raw_cmd.reply, 4);
	if (raw_cmd.reply[0] & 0x40) return xlt_error(raw_cmd.reply);
	result->fmt_cylinder = raw_cmd.reply[3];	
	result->fmt_head     = raw_cmd.reply[4];	
	result->fmt_sector   = raw_cmd.reply[5];	
	result->fmt_secsize  = 128 << raw_cmd.reply[6];	
	lxself->lx_cylinder = cylinder;
	return DSK_ERR_OK;
}



dsk_err_t linux_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;

	if (!self || !geom) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_INTR;
	raw_cmd.flags |= FD_RAW_NEED_SEEK;
	raw_cmd.track = cylinder;
	if (lxself->lx_doublestep) raw_cmd.track *= 2;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.cmd[raw_cmd.cmd_count++] = FD_SEEK;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);
	raw_cmd.cmd[raw_cmd.cmd_count++] = cylinder;

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	memcpy(lxself->lx_status, raw_cmd.reply, 4);
	if (raw_cmd.reply[0] & 0x40) return xlt_error(raw_cmd.reply);
	lxself->lx_cylinder = cylinder;
	return DSK_ERR_OK;
}

dsk_err_t linux_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                       dsk_phead_t head, unsigned char *result)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;

	if (!self || !geom || !result) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_INTR;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.cmd[raw_cmd.cmd_count++] = FD_GETSTATUS;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	*result = raw_cmd.reply[0];
	return DSK_ERR_OK;
}

/* Read a track (FDC765 READ TRACK command) */
dsk_err_t linux_tread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head)
{
	return linux_xtread(self,geom,buf,cylinder,head,cylinder,head);
}

/* Read a track (FDC765 READ TRACK command) */
dsk_err_t linux_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected)
{
	struct floppy_raw_cmd raw_cmd;
	LINUX_DSK_DRIVER *lxself;
	dsk_err_t err;
	unsigned char mask = 0xFF;

	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;
	if (lxself->lx_fd < 0) return DSK_ERR_NOTRDY;

	err = check_geom(lxself, geom);
	if (err) return err;

	if (geom->dg_fm & RECMODE_MASK)      mask &= ~0x40;
	if (geom->dg_nomulti) mask &= ~0x80;

	init_raw_cmd(&raw_cmd);
	raw_cmd.flags = FD_RAW_READ | FD_RAW_INTR;
	if (cylinder != lxself->lx_cylinder) raw_cmd.flags |= FD_RAW_NEED_SEEK;
	raw_cmd.track = cylinder;
	if (lxself->lx_doublestep) raw_cmd.track *= 2;
	raw_cmd.rate  = get_rate(geom);
	raw_cmd.length= geom->dg_secsize * geom->dg_sectors;
	raw_cmd.data  = buf;
	
	/* fdreg.h doesn't define the Read Track command, but here it is */	
	raw_cmd.cmd[raw_cmd.cmd_count++] = 0x62 & mask;
	raw_cmd.cmd[raw_cmd.cmd_count++] = encode_head(self, head);
	raw_cmd.cmd[raw_cmd.cmd_count++] = cyl_expected;
	raw_cmd.cmd[raw_cmd.cmd_count++] = head_expected;
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_secbase;
	raw_cmd.cmd[raw_cmd.cmd_count++] = dsk_get_psh(geom->dg_secsize);
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_secbase + geom->dg_sectors;
	raw_cmd.cmd[raw_cmd.cmd_count++] = geom->dg_rwgap;
	raw_cmd.cmd[raw_cmd.cmd_count++] = 0xFF;

	if (ioctl(lxself->lx_fd, FDRAWCMD, &raw_cmd) < 0) return DSK_ERR_SYSERR;

	memcpy(lxself->lx_status, raw_cmd.reply, 4);
	if (raw_cmd.reply[0] & 0x40) return xlt_error(raw_cmd.reply);
	lxself->lx_cylinder = cylinder;
	return DSK_ERR_OK;
}

/* List driver-specific options */
dsk_err_t linux_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	if (!self) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;

	switch(idx)
	{
		case 0: if (optname) *optname = "HEAD"; return DSK_ERR_OK;
		case 1: if (optname) *optname = "DOUBLESTEP"; return DSK_ERR_OK;
		case 2: if (optname) *optname = "ST0"; return DSK_ERR_OK;
		case 3: if (optname) *optname = "ST1"; return DSK_ERR_OK;
		case 4: if (optname) *optname = "ST2"; return DSK_ERR_OK;
		case 5: if (optname) *optname = "ST3"; return DSK_ERR_OK;
	}
	return DSK_ERR_BADOPT;	

}

/* Set a driver-specific option */
dsk_err_t linux_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	LINUX_DSK_DRIVER *lxself;
	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;

	if (!strcmp(optname, "HEAD")) switch(value)
	{
		case 0: case 1: case -1: 
			lxself->lx_forcehead = value;	
			return DSK_ERR_OK;
		default: return DSK_ERR_BADVAL;
	}
	if (!strcmp(optname, "DOUBLESTEP")) switch(value)
	{
		case 0: case 1: 
			lxself->lx_doublestep = value;	
			return DSK_ERR_OK;
		default: return DSK_ERR_BADVAL;
	}
/* ST0 - ST3 are read-only options. No values can be set for them. */
	if ((!strcmp(optname, "ST0")) ||
	    (!strcmp(optname, "ST1")) ||
	    (!strcmp(optname, "ST2")) ||
	    (!strcmp(optname, "ST3"))) return DSK_ERR_BADVAL;
	return DSK_ERR_BADOPT;
}
	
/* Get a driver-specific option */
dsk_err_t linux_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	LINUX_DSK_DRIVER *lxself;
	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_linux) return DSK_ERR_BADPTR;
	lxself = (LINUX_DSK_DRIVER *)self;

	if (!strcmp(optname, "HEAD")) 
	{
		if (value) *value = lxself->lx_forcehead;
		return DSK_ERR_OK;
	}
	if (!strcmp(optname, "DOUBLESTEP")) 
	{
		if (value) *value = lxself->lx_doublestep;
		return DSK_ERR_OK;
	}
	if (!strcmp(optname, "ST0"))
	{
		if (value) *value = lxself->lx_status[0];
		return DSK_ERR_OK;
	}
	if (!strcmp(optname, "ST1"))
	{
		if (value) *value = lxself->lx_status[1];
		return DSK_ERR_OK;
	}
	if (!strcmp(optname, "ST2"))
	{
		if (value) *value = lxself->lx_status[2];
		return DSK_ERR_OK;
	}
	if (!strcmp(optname, "ST3"))
	{
		if (value) *value = lxself->lx_status[3];
		return DSK_ERR_OK;
	}
	return DSK_ERR_BADOPT;
}
	


#endif


