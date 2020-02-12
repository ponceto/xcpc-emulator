/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <seasip.webmaster@gmail.com>            *
 *    Copyright (C) 2005  Simon Owen <simon.owen@simcoupe.org>             *
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

/* Floppy access using fdrawcmd.sys (http://simonowen.com/fdrawcmd/) */

#include "drvi.h"
#include "drvntwdm.h"

#ifdef NTWDMFLOPPY

static DWORD dwRet;

/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_ntwdm = 
{
	sizeof(NTWDM_DSK_DRIVER),
	"ntwdm",
	"NT WDM floppy driver",
	&ntwdm_open,	/* open */
	&ntwdm_creat,	/* create new */
	&ntwdm_close,	/* close */
	&ntwdm_read,	/* read sector, working from physical address */
	&ntwdm_write,	/* write sector, working from physical address */
	&ntwdm_format,	/* format track, physical */
	NULL,		/* Get (fixed) geometry */
	&ntwdm_secid,	/* Get sector ID, physical */
	&ntwdm_xseek,	/* seek to track */
	NULL,/*&ntwdm_status,*/	/* get drive status [disabled] */
	&ntwdm_xread,	/* read sector extended */
	&ntwdm_xwrite,	/* write sector extended */
	&ntwdm_tread,	/* read track */
	NULL,		/* ntwdm_xtread */
	&ntwdm_option_enum,	/* List options */
	&ntwdm_option_set,	/* Set option */
	&ntwdm_option_get	/* Get option */
};

/* Translate an FDC765 error to LIBDSK */
static dsk_err_t xlt_error(DWORD dwErr)
{
    switch (dwErr)
    {
        case ERROR_NOT_READY:                return DSK_ERR_NOTRDY;
        case ERROR_CRC:                      return DSK_ERR_DATAERR;
        case ERROR_WRITE_PROTECT:            return DSK_ERR_RDONLY;
        case ERROR_FLOPPY_ID_MARK_NOT_FOUND: return DSK_ERR_NOADDR;
        case ERROR_FLOPPY_WRONG_CYLINDER:    return DSK_ERR_SEEKFAIL;
        case ERROR_IO_DEVICE:                return DSK_ERR_OVERRUN;
    }
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
	NTWDM_DSK_DRIVER *ntself = (NTWDM_DSK_DRIVER *)self;
	/* Is the driver being forced to one or other side of the disc? */
	if (ntself->nt_forcehead >= 0) head = ntself->nt_forcehead;

	return head ? 1 : 0;
}




static dsk_err_t check_geom(NTWDM_DSK_DRIVER *self, const DSK_GEOMETRY *dg)
{
	UCHAR rate;

	if (dg->dg_cylinders    == self->nt_geom.dg_cylinders &&
	    dg->dg_sectors      == self->nt_geom.dg_sectors   &&
	    dg->dg_heads        == self->nt_geom.dg_heads     &&
	    dg->dg_datarate     == self->nt_geom.dg_datarate) return DSK_ERR_OK;

	rate = get_rate(dg);
	if (!DeviceIoControl(self->nt_hdisk, IOCTL_FD_SET_DATA_RATE, &rate, sizeof(rate),
		NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());

	memcpy(&self->nt_geom, dg, sizeof(*dg));
	return DSK_ERR_OK;
}


dsk_err_t ntwdm_open(DSK_DRIVER *self, const char *filename)
{
	NTWDM_DSK_DRIVER *ntself;
	HANDLE hdriver;
	DWORD dwVersion;
	BOOL ret;
	char vname[20];
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	ntself->nt_hdisk = INVALID_HANDLE_VALUE;
	ntself->nt_forcehead = -1;
	ntself->nt_doublestep = 0;
	ntself->nt_readonly = 0;

/* Accept Win32 drive letter requests, before the standard NT driver sees them? */
#if 1
	if (strlen(filename) == 2 && filename[1] == ':')
	{
		sprintf(vname, "\\\\.\\fdraw%u", tolower(filename[0])-'a');
		filename = vname;
	}
	else
#endif
	{
		/* Check for a native fdrawcmd.sys device */
		if (strlen(filename) < 10) return DSK_ERR_NOTME;
		if (memcmp(filename, "\\\\.\\fdraw", 9)) return DSK_ERR_NOTME;
		if (filename[9] < '0' || filename[9] > '9') return DSK_ERR_NOTME;
	}

	hdriver = CreateFile("\\\\.\\fdrawcmd", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hdriver == INVALID_HANDLE_VALUE) return DSK_ERR_NOTME;

	ret = DeviceIoControl(hdriver, IOCTL_FDRAWCMD_GET_VERSION, NULL, 0,
                       &dwVersion, sizeof(dwVersion), &dwRet, NULL);
	if (!ret || (dwVersion ^ FDRAWCMD_VERSION) & 0xffff0000)
	{
		CloseHandle(hdriver);
		return DSK_ERR_NOTME;
	}

	if (!DeviceIoControl(hdriver, IOCTL_FDRAWCMD_GET_VERSION, NULL, 0,
			&dwVersion, sizeof(dwVersion), &dwRet, NULL)) return DSK_ERR_NOTME;
	if ((dwVersion ^ FDRAWCMD_VERSION) & 0xffff0000) return DSK_ERR_NOTME;

	ntself->nt_cylinder = ~0;	

	ntself->nt_hdisk = CreateFile(filename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) 
		return GetLastError() == ERROR_ACCESS_DENIED ? DSK_ERR_SYSERR : DSK_ERR_NOTME;

	return DSK_ERR_OK;
}


/* For a floppy drive, opening it and creating it are the same thing */
dsk_err_t ntwdm_creat(DSK_DRIVER *self, const char *filename)
{
	return ntwdm_open(self, filename);
}


dsk_err_t ntwdm_close(DSK_DRIVER *self)
{
	NTWDM_DSK_DRIVER *ntself;

	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;

	if (ntself->nt_hdisk != INVALID_HANDLE_VALUE) 
	{
		if (!CloseHandle(ntself->nt_hdisk)) return DSK_ERR_SYSERR;
		ntself->nt_hdisk = INVALID_HANDLE_VALUE;
	}
	return DSK_ERR_OK;
}

dsk_err_t ntwdm_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return ntwdm_xread(self, geom, buf, cylinder, head, cylinder, 
			dg_x_head(geom, head), 
			dg_x_sector(geom, head, sector), 
			geom->dg_secsize, 0);
}

dsk_err_t ntwdm_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted)
{
	NTWDM_DSK_DRIVER *ntself;
	FD_READ_WRITE_PARAMS rwp;
	LPVOID iobuffer;
	dsk_err_t err;
	DWORD dwIoCode;

	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	err = check_geom(ntself, geom);
	if (err) return err;

	rwp.flags = 0;
	if (!geom->dg_noskip)  rwp.flags |= FD_OPTION_SKIP;	      /* Skip deleted data */
	if (!geom->dg_fm)      rwp.flags |= FD_OPTION_MFM;	      /* MFM recording mode */
	if (!geom->dg_nomulti) rwp.flags |= FD_OPTION_MULTI_TRACK;    /* Enable multitrack */

	rwp.phead = encode_head(self, head);
	rwp.cyl = cyl_expected;
	rwp.head = head_expected;
	rwp.sector = sector;
	rwp.size = dsk_get_psh(sector_size);	/* [JCE] v1.1.5 */
	rwp.eot = sector+1;
	rwp.gap = geom->dg_rwgap;
	rwp.datalen = (sector_size < 255) ? sector_size : 0xFF;

	if (cylinder != ntself->nt_cylinder)
	{
		UCHAR cyl = ntself->nt_doublestep ? cylinder*2 : cylinder;
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SEEK, &cyl, sizeof(cyl),
				NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());
		ntself->nt_cylinder = cylinder;
	}
/* v1.1.11: Use passed sector size, not geometry sector size */
        iobuffer = VirtualAlloc(NULL, sector_size, MEM_COMMIT, PAGE_READWRITE);
        if (!iobuffer) return DSK_ERR_NOMEM;

	dwIoCode = (deleted && (*deleted)) ? IOCTL_FDCMD_READ_DELETED_DATA : IOCTL_FDCMD_READ_DATA;
	if (!DeviceIoControl(ntself->nt_hdisk, dwIoCode, &rwp, sizeof(rwp), iobuffer,
			sector_size, &dwRet, NULL)) err = xlt_error(GetLastError());

	memcpy(buf, iobuffer, geom->dg_secsize);
	VirtualFree(iobuffer, 0, MEM_RELEASE);

	return err;
}


dsk_err_t ntwdm_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return ntwdm_xwrite(self, geom, buf, cylinder, head, cylinder, 
			dg_x_head(geom, head), 
			dg_x_sector(geom, head, sector), 
			geom->dg_secsize, 0);
}


dsk_err_t ntwdm_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted)
{
	NTWDM_DSK_DRIVER *ntself;
	FD_READ_WRITE_PARAMS rwp;
	LPVOID iobuffer;
	dsk_err_t err;
	DWORD dwIoCode;

	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	err = check_geom(ntself, geom);
	if (err) return err;

	rwp.flags = 0;
	if (!geom->dg_noskip)  rwp.flags |= FD_OPTION_SKIP;	      /* Skip deleted data */
	if (!geom->dg_fm)      rwp.flags |= FD_OPTION_MFM;	      /* MFM recording mode */
	if (!geom->dg_nomulti) rwp.flags |= FD_OPTION_MULTI_TRACK;    /* Enable multitrack */

	rwp.phead = encode_head(self, head);
	rwp.cyl = cyl_expected;
	rwp.head = head_expected;
	rwp.sector = sector;
	rwp.size = dsk_get_psh(sector_size);	/* [JCE] v1.1.5 */
	rwp.eot = sector+1;
	rwp.gap = geom->dg_rwgap;
	rwp.datalen = (sector_size < 255) ? sector_size : 0xFF;

	if (cylinder != ntself->nt_cylinder)
	{
		UCHAR cyl = ntself->nt_doublestep ? cylinder*2 : cylinder;
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SEEK, &cyl, sizeof(cyl),
				NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());
		ntself->nt_cylinder = cylinder;
	}

/* v1.1.11: Use passed sector size, not geometry sector size */
        iobuffer = VirtualAlloc(NULL, sector_size, MEM_COMMIT, PAGE_READWRITE);
        if (!iobuffer) return DSK_ERR_NOMEM;
	memcpy(iobuffer, buf, sector_size);

	dwIoCode = deleted ? IOCTL_FDCMD_WRITE_DELETED_DATA : IOCTL_FDCMD_WRITE_DATA;
	if (!DeviceIoControl(ntself->nt_hdisk, dwIoCode, &rwp, sizeof(rwp), iobuffer,
			sector_size, &dwRet, NULL)) err = xlt_error(GetLastError());

	VirtualFree(iobuffer, 0, MEM_RELEASE);

	return err;
}




dsk_err_t ntwdm_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	NTWDM_DSK_DRIVER *ntself;
	unsigned u, fsize;
	dsk_err_t err;
	FD_FORMAT_PARAMS *pfp;
	FD_ID_HEADER *ph;

	if (!self || !geom || !format) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	err = check_geom(ntself, geom);
	if (err) return err;

	fsize = sizeof(FD_FORMAT_PARAMS) + geom->dg_sectors * sizeof(FD_ID_HEADER);
	pfp = (FD_FORMAT_PARAMS *)dsk_malloc(fsize);
	if (!pfp) return DSK_ERR_NOMEM;
	pfp->flags   = geom->dg_fm ? 0 : FD_OPTION_MFM;
	pfp->phead   = encode_head(self, head);
	pfp->size    = dsk_get_psh(geom->dg_secsize);
	pfp->sectors = geom->dg_sectors;
	pfp->gap     = geom->dg_fmtgap;
	pfp->fill    = filler;

	ph = (FD_ID_HEADER *)(pfp+1);
	for (u = 0; u < geom->dg_sectors; u++)
	{
		ph[u].cyl    = format[u].fmt_cylinder;
		ph[u].head   = format[u].fmt_head;
		ph[u].sector = format[u].fmt_sector;
		ph[u].size   = dsk_get_psh(format[u].fmt_secsize);
	}

	if (cylinder != ntself->nt_cylinder)
	{
		UCHAR cyl = ntself->nt_doublestep ? cylinder*2 : cylinder;
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SEEK, &cyl, sizeof(cyl),
				NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());
		ntself->nt_cylinder = cylinder;
	}

	if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_FORMAT_TRACK, pfp, fsize,
			NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());

	return DSK_ERR_OK;
}







dsk_err_t ntwdm_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	NTWDM_DSK_DRIVER *ntself;
	FD_READ_ID_PARAMS rip;
	FD_CMD_RESULT res;
	dsk_err_t err;

	if (!self || !geom || !result) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	err = check_geom(ntself, geom);
	if (err) return err;

	if (cylinder != ntself->nt_cylinder)
	{
		UCHAR cyl = ntself->nt_doublestep ? cylinder*2 : cylinder;
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SEEK, &cyl, sizeof(cyl),
				NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());
		ntself->nt_cylinder = cylinder;
	}

	rip.flags = geom->dg_fm ? 0 : FD_OPTION_MFM;
	rip.head  = encode_head(self, head);
	if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_READ_ID, &rip, sizeof(rip),
			&res, sizeof(res), &dwRet, NULL)) return xlt_error(GetLastError());

	result->fmt_cylinder = res.cyl;
	result->fmt_head     = res.head;
	result->fmt_sector   = res.sector;
	result->fmt_secsize  = 128 << res.size;	

	return DSK_ERR_OK;
}



dsk_err_t ntwdm_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	NTWDM_DSK_DRIVER *ntself;
	UCHAR pcyl;

	if (!self || !geom) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	pcyl = ntself->nt_doublestep ? cylinder * 2 : cylinder;
	if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SEEK, &pcyl, sizeof(pcyl),
			NULL, 0, &dwRet, NULL)) return xlt_error(GetLastError());

	ntself->nt_cylinder = cylinder;
	return DSK_ERR_OK;
}

dsk_err_t ntwdm_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                       dsk_phead_t head, unsigned char *result)
{
	NTWDM_DSK_DRIVER *ntself;
	FD_SENSE_PARAMS sp;
	FD_SENSE_RESULT res;

	if (!self || !geom || !result) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	sp.head = encode_head(self, head);
	if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SENSE_DRIVE_STATUS, &sp, sizeof(sp),
			&res, sizeof(res), &dwRet, NULL)) return xlt_error(GetLastError());

	*result = res.st3;

	return DSK_ERR_OK;
}

/* Read a track (FDC765 READ TRACK command) */
dsk_err_t ntwdm_tread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head)
{
	return ntwdm_xtread(self,geom,buf,cylinder,head,cylinder,head);
}

/* Read a track (FDC765 READ TRACK command) */
dsk_err_t ntwdm_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected)
{
	NTWDM_DSK_DRIVER *ntself;
	FD_READ_WRITE_PARAMS rwp;
	LPVOID iobuffer;
	DWORD tracksize;
	dsk_err_t err;

	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;
	if (ntself->nt_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTRDY;

	err = check_geom(ntself, geom);
	if (err) return err;

	rwp.flags = 0;
	if (!geom->dg_fm)      rwp.flags |= FD_OPTION_MFM;	      /* MFM recording mode */
	if (!geom->dg_nomulti) rwp.flags |= FD_OPTION_MULTI_TRACK;    /* Enable multitrack */

	rwp.phead = encode_head(self, head);
	rwp.cyl = cyl_expected;
	rwp.head = head_expected;
	rwp.sector = geom->dg_secbase;
	rwp.size = dsk_get_psh(geom->dg_secsize);
	rwp.eot = geom->dg_secbase + geom->dg_sectors;
	rwp.gap = geom->dg_rwgap;
	rwp.datalen = 0xFF;

	tracksize = rwp.size * geom->dg_sectors;
        iobuffer = VirtualAlloc(NULL, tracksize, MEM_COMMIT, PAGE_READWRITE);
        if (!iobuffer) return DSK_ERR_NOMEM;

	if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_READ_TRACK, &rwp, sizeof(rwp),
			iobuffer, geom->dg_secsize, &dwRet, NULL)) err = xlt_error(GetLastError());

	memcpy(buf, iobuffer, geom->dg_secsize);
	VirtualFree(iobuffer, 0, MEM_RELEASE);

	return err;
}

/* List driver-specific options */
dsk_err_t ntwdm_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	if (!self) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;

	switch(idx)
	{
		case 0: if (optname) *optname = "HEAD"; return DSK_ERR_OK;
		case 1: if (optname) *optname = "DOUBLESTEP"; return DSK_ERR_OK;
		case 2: if (optname) *optname = "ST0"; return DSK_ERR_OK;
		case 3: if (optname) *optname = "ST1"; return DSK_ERR_OK;
		case 4: if (optname) *optname = "ST2"; return DSK_ERR_OK;
	}
	return DSK_ERR_BADOPT;	

}

/* Set a driver-specific option */
dsk_err_t ntwdm_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	NTWDM_DSK_DRIVER *ntself;
	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;

	if (!strcmp(optname, "HEAD")) switch(value)
	{
		case 0: case 1: case -1: 
			ntself->nt_forcehead = value;	
			return DSK_ERR_OK;
		default: return DSK_ERR_BADVAL;
	}
	if (!strcmp(optname, "DOUBLESTEP")) switch(value)
	{
		case 0: case 1: 
			ntself->nt_doublestep = value;	
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
dsk_err_t ntwdm_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	NTWDM_DSK_DRIVER *ntself;
	FD_CMD_RESULT res;

	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_ntwdm) return DSK_ERR_BADPTR;
	ntself = (NTWDM_DSK_DRIVER *)self;

	if (!strcmp(optname, "HEAD")) 
	{
		if (value) *value = ntself->nt_forcehead;
	}
	else if (!strcmp(optname, "DOUBLESTEP")) 
	{
		if (value) *value = ntself->nt_doublestep;
	}
	else if (!strcmp(optname, "ST0"))
	{
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FD_GET_RESULT, NULL, 0, 
			&res, sizeof(res), &dwRet, NULL)) return DSK_ERR_SYSERR;
		if (value) *value = res.st0;
	}
	else if (!strcmp(optname, "ST1"))
	{
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FD_GET_RESULT, NULL, 0, 
			&res, sizeof(res), &dwRet, NULL)) return DSK_ERR_SYSERR;
		if (value) *value = res.st1;
	}
	else if (!strcmp(optname, "ST2"))
	{
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FD_GET_RESULT, NULL, 0, 
			&res, sizeof(res), &dwRet, NULL)) return DSK_ERR_SYSERR;
		if (value) *value = res.st2;
	}
	else if (!strcmp(optname, "ST3"))
	{
		FD_SENSE_PARAMS sp;
		FD_SENSE_RESULT res;
		sp.head = encode_head(self, 0);
		if (!DeviceIoControl(ntself->nt_hdisk, IOCTL_FDCMD_SENSE_DRIVE_STATUS,
			NULL, 0, &res, sizeof(res), &dwRet, NULL)) return DSK_ERR_SYSERR;

			if (value) *value = res.st3;
	}
	else return DSK_ERR_BADOPT;
	return DSK_ERR_OK;
}
	


#endif

