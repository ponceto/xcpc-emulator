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
#include "drvwin32.h"
#include "rpcfuncs.h"

#ifdef WIN32FLOPPY
/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass. Note that the function pointers are 
 * nearly all NULL; they are assigned dynamically by win32_open() / win32_creat(). */

DRV_CLASS dc_win32 = 
{
	sizeof(WIN32_DSK_DRIVER),
	"floppy",
	"Win32 floppy driver",

	win32_open,	/* open */
	win32_creat,	/* create new */
	NULL,		/* close */
	NULL,		/* read sector, working from physical address */
	NULL,		/* write sector, working from physical address */
	NULL,		/* format track, physical */
	NULL,		/* probe geometry */
};

/*********************************************************************************
 * WIN32 driver: general comments
 * 
 * This driver is rather big, as it incorporates three separate sets of code for
 * Win32c (95, 98, ME), Win32 proper (NT, 2000, XP), and handing off to Win16 
 * using mailslots (Win32m). There's then a vast chunk of wrapper functions for 
 * things done via ioctls.
 *
 * The function win32_open() detects the version of Windows in use, and switches
 * the device class to use win32c or win32 functions as appropriate. If it is 
 * able to open the "LibDsk16" mailslot, it will select the "win32m" functions 
 * and use them to call the 16-bit version. 
 *
 ********************************************************************************/

/* Under Win95, use the INT13 interface rather than INT25/INT26. This appears to
 * improve performance, but is less well tested. */
#define USE_INT13
/* Use GetDriveParams() / SetDriveParams(), or ignore these functions? They
 * slow the system down a lot and don't seem to be necessary */
#undef USE_GDPARAMS

#include "w95defs.h"


static char *strwin32error(int error)
{
    static char buffer[1024];

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  (LPTSTR)buffer,
                  1023, NULL);
    return buffer;
}


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

static dsk_err_t translate_win32_error(void)
{
	int error = GetLastError();

	switch (error)
        {
        	case ERROR_SUCCESS: 		return DSK_ERR_OK;
                case ERROR_INVALID_FUNCTION:	return DSK_ERR_BADPARM;
                case ERROR_FILE_NOT_FOUND:
                case ERROR_PATH_NOT_FOUND:	return DSK_ERR_NOTRDY;

                /* XXX Do some more of these, once I find out what values
                 * the NT disc subsystem actually returns */
                default:
                fprintf(stderr, "LIBDSK bug: Not translating error case: %s\n",
                		strwin32error(error));
		return DSK_ERR_NODATA;
        }


	return DSK_ERR_NODATA;
}


static BOOL LockVolume( HANDLE hDisk ) /*{{{*/
{
    DWORD ReturnedByteCount;

    return DeviceIoControl( hDisk, FSCTL_LOCK_VOLUME, NULL, 0, NULL,
                0, &ReturnedByteCount, NULL );
}
/*}}}*/
static BOOL UnlockVolume( HANDLE hDisk )  /*{{{*/
{
    DWORD ReturnedByteCount;

    return DeviceIoControl( hDisk, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL,
                0, &ReturnedByteCount, NULL );
}
/*}}}*/
static BOOL DismountVolume( HANDLE hDisk ) /*{{{*/
{
    DWORD ReturnedByteCount;

    return DeviceIoControl( hDisk, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL,
                0, &ReturnedByteCount, NULL );
}
/*}}}*/
#ifdef USE_GDPARAMS
static int GetDriveParams( HANDLE hVWin32Device, int volume, DRIVEPARAMS* pParam ) /*{{{*/
  {
  DIOC_REGISTERS reg;
  BOOL bResult;
  DWORD cb;

  reg.reg_EAX = 0x440d; /* IOCTL for block device */
  reg.reg_EBX = volume; /* one-based drive number */
  reg.reg_ECX = 0x0860; /* Get Device params */
  reg.reg_EDX = (DWORD)pParam;
  reg.reg_Flags = 1;	/* preset the carry flag */

  bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 ); 

  if ( !bResult || (reg.reg_Flags & 1) )
      return (reg.reg_EAX & 0xffff);

  return 0;
  }
/*}}}*/
static int SetDriveParams( HANDLE hVWin32Device, int volume, DRIVEPARAMS* pParam ) /*{{{*/
  {
  DIOC_REGISTERS reg;
  BOOL bResult;
  DWORD cb;

  reg.reg_EAX = 0x440d; /* IOCTL for block device */
  reg.reg_EBX = volume; /* one-based drive number */
  reg.reg_ECX = 0x0840; /* Set Device params */
  reg.reg_EDX = (DWORD)pParam;
  reg.reg_Flags = 1;	/* preset the carry flag */

  bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 ); 

  if ( !bResult || (reg.reg_Flags & 1) )
      return (reg.reg_EAX & 0xffff);

  return 0;
  }
/*}}}*/
#endif	/* def USE_GDPARAMS */

static int GetMediaID( HANDLE hVWin32Device, int volume, MID* pMid ) /*{{{*/
  {
  DIOC_REGISTERS reg;
  BOOL bResult;
  DWORD cb;

  reg.reg_EAX = 0x440d; /* IOCTL for block device */
  reg.reg_EBX = volume; /* one-based drive number */
  reg.reg_ECX = 0x0866; /* Get Media ID */
  reg.reg_EDX = (DWORD)pMid;
  reg.reg_Flags = 1;	/* preset the carry flag */

  bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

  if ( !bResult || (reg.reg_Flags & 1) ) 
      return (reg.reg_EAX & 0xffff);

  return 0;
  }
/*}}}*/
static int VolumeCheck(HANDLE hVWin32Device, int volume, WORD* flags ) /*{{{*/
{
  DIOC_REGISTERS reg;
  BOOL bResult;
  DWORD cb;

  reg.reg_EAX = 0x4409;	/* Is Drive Remote */
  reg.reg_EBX = volume;	/* one-based drive number */
  reg.reg_Flags = 1;	/* preset the carry flag */

  bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 ); 

  if ( !bResult || (reg.reg_Flags & 1) ) 
      return (reg.reg_EAX & 0xffff);

  *flags = (WORD)(reg.reg_EDX & 0xffff);
  return 0;
}
/*}}}*/
static int LockLogicalVolume(HANDLE hVWin32Device, int volume, int lock_level, int permissions) /*{{{*/
{
  DIOC_REGISTERS reg;
  BOOL bResult;
  DWORD cb;

  reg.reg_EAX = 0x440d; /* generic IOCTL */
  reg.reg_ECX = 0x084a; /* lock logical volume */
  reg.reg_EBX = volume | (lock_level << 8);
  reg.reg_EDX = permissions;
  reg.reg_Flags = 1;	/* preset the carry flag */

  bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 ); 

  if ( !bResult || (reg.reg_Flags & 1) ) 
      return (reg.reg_EAX & 0xffff);

  return 0;
}
/*}}}*/
static int UnlockLogicalVolume( HANDLE hVWin32Device, int volume ) /*{{{*/
{
  DIOC_REGISTERS reg;
  BOOL bResult;
  DWORD cb;

  reg.reg_EAX = 0x440d;
  reg.reg_ECX = 0x086a;	/* lock logical volume */
  reg.reg_EBX = volume;
  reg.reg_Flags = 1;	/* preset the carry flag */

  bResult = DeviceIoControl( hVWin32Device, VWIN32_DIOC_DOS_IOCTL,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 ); 

  if ( !bResult || (reg.reg_Flags & 1) ) return -1;
  return 0;
}
/*}}}*/


/* For a floppy drive, opening it and creating it are the same thing */
dsk_err_t win32_creat(DSK_DRIVER *self, const char *filename)
{
	return win32_open(self, filename);
}                                                                                        


dsk_err_t win32_open(DSK_DRIVER *self, const char *filename)
{
	WIN32_DSK_DRIVER *w32self;
	DWORD dwVers;
	char vname[20];
	
	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	/* Not a drive? Not interested. */
	if (strlen(filename) != 2 || filename[1] != ':') return DSK_ERR_NOTME;
	w32self->w32_hdisk = INVALID_HANDLE_VALUE;
	w32self->w32_unit   = -1;

	/* If there is a mailslot server running, use it. It is likely to be a
	 * 16-bit server, capable of more than the 32-bit API can manage. */
	if (win32m_open(self, filename) == DSK_ERR_OK)
		{
		dc_win32.dc_close  = win32m_close;
		dc_win32.dc_write  = win32m_write;
		dc_win32.dc_read   = win32m_read;
		dc_win32.dc_format = win32m_format;
		dc_win32.dc_secid  = win32m_secid;
		dc_win32.dc_status = win32m_status;
		return DSK_ERR_OK;
		}
	dwVers = GetVersion();

	if (dwVers & 0x80000000L) /* Win32s (3.1) or Win32c (Win95) */
	{
		dc_win32.dc_close  = win32c_close;
		dc_win32.dc_write  = win32c_write;
		dc_win32.dc_read   = win32c_read;
		dc_win32.dc_format = win32c_format;
		dc_win32.dc_secid  = win32c_secid;
		dc_win32.dc_status = win32c_status;
		return win32c_open(self, filename);
	}
	/* Opening in NT mode */
       	dc_win32.dc_close   = win32_close;
       	dc_win32.dc_write   = win32_write;
       	dc_win32.dc_read    = win32_read;
       	dc_win32.dc_format  = win32_format;
	dc_win32.dc_getgeom = win32_getgeom;
	dc_win32.dc_status  = win32_status;

	/* This is as near as NT lets us get to "/dev/fd0". */
	sprintf(vname, "\\\\.\\%s", filename);
	w32self->w32_hdisk   = CreateFile(vname,	/* Name */
					GENERIC_READ | GENERIC_WRITE,	/* Access mode */
					FILE_SHARE_READ|FILE_SHARE_WRITE, /*Sharing*/
					NULL,		/* Security attributes */ 
					OPEN_EXISTING,	/* See MSDN */
					0,		/* Flags & attributes */
					NULL);		/* Template file */
	if (w32self->w32_hdisk == INVALID_HANDLE_VALUE)
	{
		w32self->w32_readonly = 1;
		w32self->w32_hdisk   = CreateFile(vname,	/* Name */
						GENERIC_READ,	/* Access mode */
						FILE_SHARE_READ|FILE_SHARE_WRITE, /*Sharing*/
						NULL,		/* Security attributes */ 
						OPEN_EXISTING,	/* See MSDN */
						0,		/* Flags & attributes */
						NULL);		/* Template file */
	}
	if (w32self->w32_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_NOTME;

	if (LockVolume(w32self->w32_hdisk) == FALSE)    /* Lock drive */
	{
		return DSK_ERR_ACCESS;
	}
        return DSK_ERR_OK;
} 


/* NT close */
dsk_err_t win32_close(DSK_DRIVER *self)
{
	WIN32_DSK_DRIVER *w32self;

	if (self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	if (w32self->w32_hdisk != INVALID_HANDLE_VALUE)
	{
		DismountVolume(w32self->w32_hdisk);
		UnlockVolume  (w32self->w32_hdisk);
		CloseHandle   (w32self->w32_hdisk);
		w32self->w32_hdisk = INVALID_HANDLE_VALUE;
	}
	return DSK_ERR_OK;
}

/******************* Win32 (Windows NT/2000/XP) specific functions ****************/

dsk_err_t win32_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	WIN32_DSK_DRIVER *w32self;
        LPVOID iobuffer;
        DWORD  bytesread;
	long offset;
	int res;

	if (!buf || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;
	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */
	offset *= geom->dg_sectors;
	offset += (sector - geom->dg_secbase);
	offset *=  geom->dg_secsize;

   	/* From here on, the code is not unlike that in drvposix.c. The difference is
   	 * that instead of nice stdio or POSIX functions, we have to use horrible 
   	 * Win32 ones. */
        if (SetFilePointer(w32self->w32_hdisk, offset, NULL, FILE_BEGIN) == INVALID_FILE_SIZE)
        {
            return DSK_ERR_NOADDR;
        }
	/* We have to allocate a buffer for the data using VirtualAlloc(). malloc()
	 * isn't good enough for its lordship, oh dear me no. */
        iobuffer = VirtualAlloc(NULL, geom->dg_secsize, MEM_COMMIT, PAGE_READWRITE);
        if (!iobuffer) 
        {
            return DSK_ERR_NOMEM;
        }
        res = ReadFile(w32self->w32_hdisk, iobuffer, geom->dg_secsize, &bytesread, NULL);
        if (!res)
        {
            VirtualFree(iobuffer, 0, MEM_RELEASE);
       	    return translate_win32_error();
        }
        memcpy(buf, iobuffer, geom->dg_secsize);
        VirtualFree(iobuffer, 0, MEM_RELEASE);

        if (bytesread < geom->dg_secsize)
        {
            memset(((char *)buf) + bytesread, 0, geom->dg_secsize - bytesread);
        }
	return DSK_ERR_OK;
}


dsk_err_t win32_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	WIN32_DSK_DRIVER *w32self;
        LPVOID iobuffer;
        DWORD  byteswritten;
	long offset;
	int res;

	if (!buf || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;
	/* Convert from physical to logical sector. However, unlike the dg_* 
	 * functions, this _always_ uses "SIDES_ALT" mapping; this is the 
	 * mapping that both the Linux and NT floppy drivers use to convert 
	 * offsets back into C/H/S. */
	offset = (cylinder * geom->dg_heads) + head;	/* Drive track */
	offset *= geom->dg_sectors;
	offset += (sector - geom->dg_secbase);
	offset *=  geom->dg_secsize;

	if (w32self->w32_readonly) return DSK_ERR_RDONLY;
   	/* From here on, the code is not unlike that in drvposix.c. The difference is
   	 * that instead of nice stdio or POSIX functions, we have to use horrible 
   	 * Win32 ones. */
        if (SetFilePointer(w32self->w32_hdisk, offset, NULL, FILE_BEGIN) == INVALID_FILE_SIZE)
        {
            return DSK_ERR_NOADDR;
        }
	/* We have to allocate a buffer for the data using VirtualAlloc(). malloc()
	 * isn't good enough for its lordship, oh dear me no. */
        iobuffer = VirtualAlloc(NULL, geom->dg_secsize, MEM_COMMIT, PAGE_READWRITE);
        if (!iobuffer) 
        {
            return DSK_ERR_NOMEM;
        }
	memcpy(iobuffer, buf, geom->dg_secsize);
        res = WriteFile(w32self->w32_hdisk, iobuffer, geom->dg_secsize, &byteswritten, NULL);
        if (!res || (byteswritten < (unsigned)geom->dg_secsize))
        {
            VirtualFree(iobuffer, 0, MEM_RELEASE);
       	    return translate_win32_error();
        }
        VirtualFree(iobuffer, 0, MEM_RELEASE);
	return DSK_ERR_OK;
}



dsk_err_t win32_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	WIN32_DSK_DRIVER *w32self;
	FORMAT_PARAMETERS fp;
	DWORD dw;
	int res;

	if (!self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	if (w32self->w32_readonly) return DSK_ERR_RDONLY;
	/* Initialise fp. NT only allows certain fixed formats, and here 
	 * they are (omitting 2.88Mb and 20.8Mb formats). 
	 * IOCTL_DISK_FORMAT_TRACKS_EX will allow more. */

	if      (geom->dg_cylinders == 80 && 
	         geom->dg_heads     == 2  && 
	         geom->dg_sectors   == 15 &&
	         geom->dg_secsize   == 512)	fp.MediaType = F5_1Pt2_512;
	else if (geom->dg_cylinders == 80 && 
	         geom->dg_heads     == 2  && 
	         geom->dg_sectors   == 18 &&
	         geom->dg_secsize   == 512)	fp.MediaType = F3_1Pt44_512;
	else if (geom->dg_cylinders == 80 && 
	         geom->dg_heads     == 2  && 
	         geom->dg_sectors   == 9  &&
	         geom->dg_secsize   == 512)	fp.MediaType = F3_720_512;
	else if (geom->dg_cylinders == 40 && 
     	         geom->dg_heads     == 2  && 
     	         geom->dg_sectors   == 9  &&
	         geom->dg_secsize   == 512)	fp.MediaType = F5_360_512;
	else if (geom->dg_cylinders == 40 && 
     	         geom->dg_heads     == 2  && 
     	         geom->dg_sectors   == 8  &&
	         geom->dg_secsize   == 1024)	fp.MediaType = F5_320_1024;
	else if (geom->dg_cylinders == 40 && 
     	         geom->dg_heads     == 2  && 
     	         geom->dg_sectors   == 8  &&
	         geom->dg_secsize   == 512)	fp.MediaType = F5_320_512;
	else if (geom->dg_cylinders == 40 && 
	         geom->dg_heads     == 1  && 
	         geom->dg_sectors   == 9  &&
	         geom->dg_secsize   == 512)	fp.MediaType = F5_180_512;
	else if (geom->dg_cylinders == 40 && 
	         geom->dg_heads     == 1  && 
	         geom->dg_sectors   == 8  &&
	         geom->dg_secsize   == 512)	fp.MediaType = F5_160_512;
	else return DSK_ERR_BADFMT;

	fp.StartCylinderNumber = fp.EndCylinderNumber = cylinder;
	fp.StartHeadNumber     = fp.EndHeadNumber     = head;
	
	res = DeviceIoControl(w32self->w32_hdisk, IOCTL_DISK_FORMAT_TRACKS,
			&fp, sizeof(fp), NULL, 0, &dw, NULL);
   (void)format;
   (void)filler;
	if (res) return DSK_ERR_OK;
/* XXX Translate error properly */
	return DSK_ERR_SEEKFAIL;
}



/* Windows NT does not allow a program to set the disc geometry; it forces
 * the issue. */

dsk_err_t win32_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	WIN32_DSK_DRIVER *w32self;
        DWORD  bytesread;
	int res;
	DISK_GEOMETRY dg;	/* This is NT's DISK_GEOMETRY, not ours */
	unsigned char buf[512];
	
	if (!self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	res = DeviceIoControl(w32self->w32_hdisk, IOCTL_DISK_GET_DRIVE_GEOMETRY,
			NULL, 0, &dg, sizeof(dg), &bytesread, NULL);

	geom->dg_cylinders = (unsigned)dg.Cylinders.QuadPart;
	geom->dg_heads     = dg.TracksPerCylinder;
	geom->dg_sectors   = dg.SectorsPerTrack;
	geom->dg_secsize   = dg.BytesPerSector;
	geom->dg_sidedness = SIDES_ALT;	
	geom->dg_secbase   = 1;
	geom->dg_datarate  = (dg.SectorsPerTrack >= 12) ? RATE_HD : RATE_SD;
        switch(geom->dg_sectors)
        {
                case 8:  geom->dg_rwgap = 0x2A; geom->dg_fmtgap = 0x50; break;
                case 9:  geom->dg_rwgap = 0x2A; geom->dg_fmtgap = 0x52; break;
                case 10: geom->dg_rwgap = 0x0C; geom->dg_fmtgap = 0x17; break;
                case 15: geom->dg_rwgap = 0x1B; geom->dg_fmtgap = 0x50; break;
                case 18: geom->dg_rwgap = 0x1B; geom->dg_fmtgap = 0x50; break;
                default: geom->dg_rwgap = 0x2A; geom->dg_fmtgap = 0x52; break;
        }
        geom->dg_fm      = 0;
        geom->dg_nomulti = 0;
	
	/* Although all the other values are fixed, "sidedness" can 
	 * be set. So now we probe for formats that (i) we can read, and 
	 * (ii) have a known "sidedness". */

	if (geom->dg_secsize != 512) return DSK_ERR_OK;
		
	if (win32_read(self, geom, buf, 0, 0, 1)) return DSK_ERR_OK;
	/* Formats that use SIDES_OUTBACK are: 
	 * CP/M-86 formats that use 144FEAT 
	 * PCW formats that say so in their spec */

	/* PCW formats */
	if ((buf[0] == 0 || buf[0] == 3) && ((buf[1] & 3) == 2))
		geom->dg_sidedness = SIDES_OUTBACK;

	/* CP/M-86 formats */	
	else if (buf[511] == 0x48 || buf[511] == 0x90 || buf[511] == 0x0C) 
		geom->dg_sidedness = SIDES_OUTBACK;
	
	return DSK_ERR_OK;
}


/* XXX This code is untested! */
dsk_err_t win32_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	WIN32_DSK_DRIVER *w32self;
        DWORD  bytesread;
	int res;

	if (!result || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	res = DeviceIoControl(w32self->w32_hdisk, IOCTL_DISK_CHECK_VERIFY,
			NULL, 0, NULL, 0, &bytesread, NULL);

	if (res) *result |= DSK_ST3_READY;
	else	 *result &= ~DSK_ST3_READY;
	if (w32self->w32_readonly) *result |= DSK_ST3_RO;

	return DSK_ERR_OK;
}




/******************** Win32c (Windows 95/98/ME) specific functions ****************/


/* Open a Win32c drive. Called from win32_open() */
dsk_err_t win32c_open(DSK_DRIVER *s, const char *filename)
{
	int driveno, res;
	unsigned short drive_flags = 0;
	MID media;
	char vname[20];
	WIN32_DSK_DRIVER *self = (WIN32_DSK_DRIVER *)s;
	
	vname[0] = (char)toupper(filename[0]);
	driveno = vname[0] - 'A' + 1;   /* 1=A: 2=B: */
	self->w32_hdisk = CreateFile( "\\\\.\\vwin32",
                                       0,
                                       0,
                                       NULL,
                                       0,
                                       FILE_FLAG_DELETE_ON_CLOSE,
                                       NULL );
	/* Failed to open VWIN32 */
	if (!self->w32_hdisk) return DSK_ERR_NOTME;
	if (VolumeCheck(self->w32_hdisk, driveno, &drive_flags))
        {
                CloseHandle(self->w32_hdisk);
                return DSK_ERR_NOTME;
	} 
	res = GetMediaID( self->w32_hdisk, driveno, &media );
	if ( res )
	{
		dsk_err_t err = DSK_ERR_OK;

		/* Network drive? */
		if ( res == ERROR_INVALID_FUNCTION &&
          (drive_flags & DRIVE_IS_REMOTE )) err = DSK_ERR_NOTME;
      /* Comment this out. libdsk works fine even if you get ERROR_ACCESS_DENIED
       * trying to read the media ID.
      else if (res == ERROR_ACCESS_DENIED) err = DSK_ERR_ACCESS;
      */
                /* nb: It's perfectly legitimate for GetMediaID() to fail; most CP/M */
                /*     CP/M disks won't have a media ID. */

                if (err)
                {
                   CloseHandle(self->w32_hdisk);
                   return err;
                }
	}
	if (!res && 
		(!memcmp( media.midFileSysType, "CDROM", 5 ) ||
		 !memcmp( media.midFileSysType, "CD001", 5 ) ||
		 !memcmp( media.midFileSysType, "CDAUDIO", 5 )))
		{
			/* CD-ROM drive */
			CloseHandle(self->w32_hdisk);
			return DSK_ERR_NOTME;
		}
	/* Attempt to acquire full lock on the drive */
	if (LockLogicalVolume(self->w32_hdisk, driveno, LEVEL0_LOCK, 0))
	{
		/* Failed. Try read-only lock */
		self->w32_readonly = 1;
		if (LockLogicalVolume(self->w32_hdisk, driveno, LEVEL1_LOCK,
				LEVEL1_LOCK_MAX_PERMISSION))
		{
			/* Failed even to lock the drive read-only */
			CloseHandle(self->w32_hdisk);
			return DSK_ERR_ACCESS;
		}
	}
	self->w32_unit = driveno;
	return DSK_ERR_OK;
}


	
/* Win9x close */
dsk_err_t win32c_close(DSK_DRIVER *self)
{
	WIN32_DSK_DRIVER *w32self;

	if (self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	if (w32self->w32_hdisk != INVALID_HANDLE_VALUE)
	{
		UnlockLogicalVolume  (w32self->w32_hdisk, w32self->w32_unit);
		CloseHandle   (w32self->w32_hdisk);
		w32self->w32_hdisk = INVALID_HANDLE_VALUE;
	}
	return DSK_ERR_OK;
}





/* Set the drive geometry to match the geometry the caller expects. 
 * This is currently ifdeffed out because I don't know if Windows needs
 * it or not, and it slows performance down. */
static dsk_err_t win32c_checkgeom(WIN32_DSK_DRIVER *self, const DSK_GEOMETRY *geom)
{
	DRIVEPARAMS drvp;
	int n = 0;

	if (!memcmp(&self->w32_geom, geom, sizeof(*geom))) return DSK_ERR_OK;

	memset(&drvp, 0, sizeof(drvp));

#ifdef USE_GDPARAMS
	if (GetDriveParams( self->w32_hdisk, self->w32_unit, &drvp )) return DSK_ERR_OK;

	drvp.bytespersector  = geom->dg_secsize;
	drvp.sectorspertrack = geom->dg_sectors;
	drvp.totalsectors    = geom->dg_heads * geom->dg_cylinders * geom->dg_sectors;
        drvp.cylinders       = geom->dg_cylinders;
        drvp.heads           = geom->dg_heads;

/* Set up "reasonable" values for the other members */

	drvp.sectorspercluster = 1024 / geom->dg_secsize;
	drvp.reservedsectors   = 1;
	drvp.numberofFATs      = 2;
	drvp.sectorcount       = geom->dg_sectors;
	drvp.rootdirsize       = 64;
	drvp.mediaid           = 0xF0;
	drvp.hiddensectors     = 0;
	drvp.sectorsperfat     = 3;
	for (n = 0; n < geom->dg_sectors; n++)
	{
		drvp.sectortable[n*2]   = n + geom->dg_secbase;    /* Physical sector numbers */ 
		drvp.sectortable[n*2+1] = geom->dg_secsize;
	}
	drvp.special = 6;
/* We have not set:

    drvp.mediatype   
    drvp.devicetype
    drvp.deviceattrs  

    which should have been read correctly by GetDriveParams().
  */
	SetDriveParams( self->w32_hdisk, self->w32_unit, &drvp );
#else /* def USE_GDPARAMS */
	(void)n;
#endif /* def USE_GDPARAMS */
	return DSK_ERR_OK;
}


/* Read sector (Win95 version) */
dsk_err_t win32c_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	WIN32_DSK_DRIVER *w32self;
        DIOC_REGISTERS reg;
        BOOL bResult;
        DWORD cb;
	dsk_err_t err;
#ifndef USE_INT13
        DISKIO di;
#endif

	if (!buf || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;
	err = win32c_checkgeom(w32self, geom);
	if (err) return err;

#ifdef USE_INT13
	reg.reg_EAX      = 0x0201;  /* Read 1 sector */
        reg.reg_EBX      = (DWORD)buf;
        reg.reg_ECX      = (cylinder << 8)  | sector;
        reg.reg_EDX      = (head << 8) | (w32self->w32_unit - 1);
        reg.reg_Flags    = 1;       /* preset the carry flag */
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT13,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );
        if ( !bResult || (reg.reg_Flags & 1) )
        {
		return translate_int13_error((reg.reg_EAX >> 8) & 0xFF);
        }
#else
        reg.reg_EAX      = w32self->w32_unit - 1;  // zero-based volume number
        reg.reg_EBX      = (DWORD)&di;
        reg.reg_ECX      = 0xffff;  /* use DISKIO structure */
        reg.reg_Flags    = 1;       /* preset the carry flag */
        di.diStartSector = (sector - geom->dg_secbase) +
			   (cylinder * geom->dg_heads + head) * geom->dg_sectors;
        di.diSectors     = 1;
        di.diBuffer      = (DWORD)buf;
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT25,
             &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

        if ( !bResult || (reg.reg_Flags & 1) )
        {
		return translate_int13_error(reg.reg_EAX);
        }
#endif
        return DSK_ERR_OK;
}

/* Write sector (Win95 version) */
dsk_err_t win32c_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                             const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	WIN32_DSK_DRIVER *w32self;
        DIOC_REGISTERS reg;
        BOOL bResult;
        DWORD cb;
	dsk_err_t err;
#ifndef USE_INT13
        DISKIO di;
#endif

	if (!buf || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;
	err = win32c_checkgeom(w32self, geom); 
	if (err) return err;
	if (w32self->w32_readonly) return DSK_ERR_RDONLY;
	
#ifdef USE_INT13
        reg.reg_EAX      = 0x0301;  /* Write 1 sector */
        reg.reg_EBX      = (DWORD)buf;
        reg.reg_ECX      = (cylinder << 8)  | sector;
        reg.reg_EDX      = (head << 8) | (w32self->w32_unit - 1);
        reg.reg_Flags    = 1;       /* preset the carry flag */
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT13,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );
        if ( !bResult || (reg.reg_Flags & 1) )
        {
		return translate_int13_error((reg.reg_EAX >> 8) & 0xFF);
        }
#else
        reg.reg_EAX      = w32self->w32_unit - 1;  /* zero-based volume number */
        reg.reg_EBX      = (DWORD)&di;
        reg.reg_ECX      = 0xffff;  /* use DISKIO structure */
        reg.reg_Flags    = 1;       /* preset the carry flag */
        di.diStartSector = (sector - geom->dg_secbase) +
			   (cylinder * geom->dg_heads + head) * geom->dg_sectors;
        di.diSectors     = 1;
        di.diBuffer      = (DWORD)buf;
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT26,
             &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

        if ( !bResult || (reg.reg_Flags & 1) )
        {
		return translate_int13_error(reg.reg_EAX);
        }
#endif
        return DSK_ERR_OK;
}





dsk_err_t win32c_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	WIN32_DSK_DRIVER *w32self;
        DIOC_REGISTERS reg;
        BOOL bResult;
        DWORD cb;
	dsk_err_t err;
	unsigned char *trkbuf;
	unsigned int n;

	if (!format || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;
	err = win32c_checkgeom(w32self, geom); 
	if (err) return err;
	if (w32self->w32_readonly) return DSK_ERR_RDONLY;
	
	trkbuf = dsk_malloc(4 * geom->dg_sectors);
	if (!trkbuf) return DSK_ERR_NOMEM;

	for (n = 0; n < geom->dg_sectors; n++)
	{
		trkbuf[4*n  ] = (unsigned char)format[n].fmt_cylinder;
		trkbuf[4*n+1] = (unsigned char)format[n].fmt_head;
		trkbuf[4*n+2] = (unsigned char)format[n].fmt_sector;
		trkbuf[4*n+3] = dsk_get_psh(format[n].fmt_secsize);
	}

        reg.reg_EAX      = 0x0500 | geom->dg_sectors;  /* Format (n) sectors */
        reg.reg_EBX      = (DWORD)trkbuf;
        reg.reg_ECX      = (cylinder << 8);
        reg.reg_EDX      = (head << 8) | (w32self->w32_unit - 1);
        reg.reg_Flags    = 1;       /* preset the carry flag */
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT13,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

	dsk_free(trkbuf);
        if ( !bResult || (reg.reg_Flags & 1) )
        {
		return translate_int13_error((reg.reg_EAX >> 8) & 0xFF);
        }
        (void)filler;
        return DSK_ERR_OK;
}

dsk_err_t win32c_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
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

        err          = win32c_read(self, geom, buf, cylinder, head, psect=0x01);
        if (err) err = win32c_read(self, geom, buf, cylinder, head, psect=0x41);
        if (err) err = win32c_read(self, geom, buf, cylinder, head, psect=0xC1);

        if (result)
        {
        	result->fmt_cylinder = cylinder;
                result->fmt_head     = head;
                result->fmt_sector   = psect;
                result->fmt_secsize  = geom->dg_secsize;
        }
	return err;
}



/* XXX This code is untested! */
dsk_err_t win32c_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	WIN32_DSK_DRIVER *w32self;
        DIOC_REGISTERS reg;
        BOOL bResult;
        DWORD cb;

	if (!result || !self || !geom || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;
	if (w32self->w32_readonly) *result |= DSK_ST3_RO;

	/* See if the drive supports a changeline. If so, probe it. */
	
        reg.reg_EAX      = 0x15FF;  /* Get drive configuration */
        reg.reg_ECX      = 0xFFFF;
        reg.reg_EDX      = (w32self->w32_unit - 1);
        reg.reg_Flags    = 1;       /* preset the carry flag */
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT13,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

	switch((reg.reg_EAX >> 8) & 0xFF)
	{
		case 0: *result &= ~DSK_ST3_READY; return DSK_ERR_OK;
		case 2: break;
		default: return DSK_ERR_OK;	
	}
	/* Changeline supported. Read it. */
        reg.reg_EAX      = 0x1600;  /* Read changeline */
        reg.reg_EDX      = (w32self->w32_unit - 1);
	reg.reg_ESI      = 0;
        reg.reg_Flags    = 1;       /* preset the carry flag */
        bResult          = DeviceIoControl( w32self->w32_hdisk, VWIN32_DIOC_DOS_INT13,
              &reg, sizeof( reg ), &reg, sizeof( reg ), &cb, 0 );

	/* If we get a "not ready", then reset the "ready" bit */
	if (((reg.reg_EAX >> 8) & 0xFF) == 0x80) *result &= ~DSK_ST3_READY;
	return DSK_ERR_OK;
}


/************************** RPC Functions ***********************************/
/* These functions are used to talk to a remote instance of LibDsk over 
 * mailslots. */

/* Send a packet down a mailslot and get a response. 
 * This function is passed as a callback to the RPC client functions which
 * construct and interpret the packets. 
 * 
 * For the format of a packet, see rpcfuncs.h
 * */
dsk_err_t win32_mailslot(DSK_DRIVER *self, unsigned char *input,  int inp_len,
										   unsigned char *output, int *out_len)
	{
	WIN32_DSK_DRIVER *w32self;
	DWORD dw;
	unsigned char *buf;
	int buflen;

/* Generate the packet header: 
 *	2 bytes length of following string
 * 	0-terminated string giving mailslot to reply to */
	w32self = (WIN32_DSK_DRIVER *)self;
	buflen = 1 + strlen(w32self->w32_slotname);
	buf = dsk_malloc(inp_len + 2 + buflen);
	if (!buf) return DSK_ERR_NOMEM;

	buf[0] = (buflen >> 8) & 0xFF;
	buf[1] = (buflen & 0xFF);
	strcpy((char *)(buf+2), w32self->w32_slotname);
/* Put the packet body into the buffer */
	memcpy(buf+2+buflen, input, inp_len);

/* Send out the packet */
	if (!WriteFile(w32self->w32_hdisk, buf, buflen + 2 + inp_len, &dw, NULL)) return DSK_ERR_SYSERR;
/* Retrieve the response. */
	ReadFile (w32self->w32_hmail, output, *out_len, &dw, NULL);

	dsk_free(buf);
	*out_len = dw;
	return DSK_ERR_OK;
	}
										


dsk_err_t win32m_open(DSK_DRIVER *self, const char *filename)
	{
	int n = rand();

	// Set up the RPC transport - mailslot to server
	WIN32_DSK_DRIVER *w32self;
	w32self = (WIN32_DSK_DRIVER *)self;

/* Look for the server running on a well-known mailslot */ 
	w32self->w32_hdisk = CreateFile("\\\\.\\mailslot\\LibDsk16", 
		GENERIC_WRITE, 
		FILE_SHARE_READ,  // required to write to a mailslot 
		(LPSECURITY_ATTRIBUTES) NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
 
	if (w32self->w32_hdisk == INVALID_HANDLE_VALUE) return DSK_ERR_SYSERR;

	/* Open a mailslot with to listen on. Keep trying different numbers
	 * until we find one that isn't in use. */
	do
		{
		sprintf(w32self->w32_slotname, "\\\\.\\mailslot\\%08x", n);
		w32self->w32_hmail = CreateMailslot(w32self->w32_slotname, 16384, MAILSLOT_WAIT_FOREVER, NULL);

		if (w32self->w32_hmail == INVALID_HANDLE_VALUE)
			{
			if (GetLastError() != ERROR_FILE_EXISTS) 
				{
				CloseHandle(w32self->w32_hdisk);
				return DSK_ERR_SYSERR;
				}
			}
		} while (w32self->w32_hmail == INVALID_HANDLE_VALUE);

	// Send RPC "open" command
	return dsk_r_open(self, win32_mailslot, &w32self->w32_unit, filename, "floppy", NULL);
	}

/* Close mailslot connection */
dsk_err_t win32m_close(DSK_DRIVER *self)
	{
	WIN32_DSK_DRIVER *w32self;

	if (!self || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	dsk_r_close(self, win32_mailslot, w32self->w32_unit);
	CloseHandle(w32self->w32_hdisk);
	CloseHandle(w32self->w32_hmail);
	return DSK_ERR_OK;
	}


dsk_err_t win32m_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
	{
	WIN32_DSK_DRIVER *w32self;

	if (!self || !geom || !buf || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	return dsk_r_read(self, win32_mailslot, w32self->w32_unit, geom, buf, cylinder, head, sector);
	}


dsk_err_t win32m_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
	{
	WIN32_DSK_DRIVER *w32self;

	if (!self || !geom || !buf || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	return dsk_r_write(self, win32_mailslot, w32self->w32_unit, geom, buf, cylinder, head, sector);
	}


dsk_err_t win32m_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
	{
	WIN32_DSK_DRIVER *w32self;

	if (!self || !geom || !format || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	return dsk_r_format(self, win32_mailslot, w32self->w32_unit, geom, cylinder, head, format, filler);
	}



dsk_err_t win32m_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
	{
	WIN32_DSK_DRIVER *w32self;

	if (!self || !geom || !result || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	return dsk_r_secid(self, win32_mailslot, w32self->w32_unit, geom, cylinder, head, result);
	}



dsk_err_t win32m_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
	{
	WIN32_DSK_DRIVER *w32self;

	if (!self || !geom || !result || self->dr_class != &dc_win32) return DSK_ERR_BADPTR;
	w32self = (WIN32_DSK_DRIVER *)self;

	return dsk_r_drive_status(self, win32_mailslot, w32self->w32_unit, geom, head, result);
	}



#endif
