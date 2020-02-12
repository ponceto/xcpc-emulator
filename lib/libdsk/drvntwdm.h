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

#ifdef NTWDMFLOPPY

/* Declarations for the NT WDM floppy driver */

typedef struct
{
        DSK_DRIVER   nt_super;
	DSK_GEOMETRY nt_geom;
        HANDLE	     nt_hdisk;
	int          nt_readonly;
	int	     nt_forcehead;
	int          nt_doublestep;
	dsk_pcyl_t   nt_cylinder;
} NTWDM_DSK_DRIVER;

dsk_err_t ntwdm_open(DSK_DRIVER *self, const char *filename);
dsk_err_t ntwdm_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t ntwdm_close(DSK_DRIVER *self);
dsk_err_t ntwdm_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t ntwdm_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t ntwdm_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t ntwdm_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t ntwdm_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t ntwdm_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted);
dsk_err_t ntwdm_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted);
dsk_err_t ntwdm_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              dsk_phead_t head, unsigned char *result);
dsk_err_t ntwdm_tread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t ntwdm_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected);

/* List driver-specific options */
dsk_err_t ntwdm_option_enum(DSK_DRIVER *self, int idx, char **optname);

/* Set a driver-specific option */
dsk_err_t ntwdm_option_set(DSK_DRIVER *self, const char *optname, int value);
/* Get a driver-specific option */
dsk_err_t ntwdm_option_get(DSK_DRIVER *self, const char *optname, int *value);


/****************************************************************************/

/* build-time driver version to check at run-time for compatability - high 16-bits must match */
#define FDRAWCMD_VERSION		0x0100000f

#define IOCTL_FD_BASE                   FILE_DEVICE_UNKNOWN

#define IOCTL_FDRAWCMD_GET_VERSION	CTL_CODE(IOCTL_FD_BASE, 0x888, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)

#define IOCTL_FDCMD_READ_TRACK		CTL_CODE(IOCTL_FD_BASE, 0x802, METHOD_OUT_DIRECT, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_SENSE_DRIVE_STATUS	CTL_CODE(IOCTL_FD_BASE, 0x804, METHOD_BUFFERED,   FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_WRITE_DATA		CTL_CODE(IOCTL_FD_BASE, 0x805, METHOD_IN_DIRECT,  FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_READ_DATA		CTL_CODE(IOCTL_FD_BASE, 0x806, METHOD_OUT_DIRECT, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_WRITE_DELETED_DATA	CTL_CODE(IOCTL_FD_BASE, 0x809, METHOD_IN_DIRECT,  FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_READ_ID		CTL_CODE(IOCTL_FD_BASE, 0x80a, METHOD_BUFFERED,   FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_READ_DELETED_DATA	CTL_CODE(IOCTL_FD_BASE, 0x80c, METHOD_OUT_DIRECT, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_FORMAT_TRACK        CTL_CODE(IOCTL_FD_BASE, 0x80d, METHOD_BUFFERED,   FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FDCMD_SEEK                CTL_CODE(IOCTL_FD_BASE, 0x80f, METHOD_BUFFERED,   FILE_READ_DATA|FILE_WRITE_DATA)

#define IOCTL_FD_GET_RESULT             CTL_CODE(IOCTL_FD_BASE, 0x901, METHOD_BUFFERED,   FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_FD_SET_DATA_RATE          CTL_CODE(IOCTL_FD_BASE, 0x904, METHOD_BUFFERED,   FILE_READ_DATA|FILE_WRITE_DATA)

#define FD_OPTION_MULTI_TRACK		0x80
#define FD_OPTION_MFM			0x40
#define FD_OPTION_SKIP			0x20

typedef struct tagFD_ID_HEADER
{
	UCHAR cyl;
	UCHAR head;
	UCHAR sector;
	UCHAR size;
}
FD_ID_HEADER, *PFD_ID_HEADER;

typedef struct tagFD_CMD_RESULT
{
	UCHAR st0, st1, st2;
	UCHAR cyl, head, sector, size;
}
FD_CMD_RESULT, *PFD_CMD_RESULT;

typedef struct tagFD_READ_WRITE_PARAMS
{
	UCHAR flags;
	UCHAR phead;
	UCHAR cyl, head, sector, size;
	UCHAR eot, gap, datalen;
}
FD_READ_WRITE_PARAMS, *PFD_READ_WRITE_PARAMS;

typedef struct tagFD_FORMAT_PARAMS
{
	UCHAR flags;
	UCHAR phead;
	UCHAR size, sectors, gap, fill;

/*	FD_ID_HEADER Header[0]; This upsets Borland C++ 5.0 */
}
FD_FORMAT_PARAMS, *PFD_FORMAT_PARAMS;

typedef struct tagFD_READ_ID_PARAMS
{
	UCHAR flags;
	UCHAR head;
}
FD_READ_ID_PARAMS, *PFD_READ_ID_PARAMS;

typedef struct tagFD_SENSE_PARAMS
{
	UCHAR head;
}
FD_SENSE_PARAMS, *PFD_SENSE_PARAMS;

typedef struct tagFD_SENSE_RESULT
{
	UCHAR st3;
}
FD_SENSE_RESULT, *PFD_SENSE_RESULT;

#endif
