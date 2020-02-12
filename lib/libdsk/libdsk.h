/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2015 John Elliott <seasip.webmaster@gmail.com>    *
 *                                                                         *
 *    Modifications to add dsk_dirty()                                     *
 *    (c) 2005 Philip Kendall <pak21-spectrum@srcf.ucam.org>               *
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

#ifndef LIBDSK_H
#define LIBDSK_H 1
/*
 * LIBDSK: Portable library for floppy / disk image access
 *
 */

/* The Magic Incantations for exporting the functions from DLLs under 
 * Windows. */
#ifdef WIN16
# define LDPUBLIC16 __export __far __pascal
# define LDPUBLIC32
#else   /* def WIN16 */
# ifdef WIN32
#  ifdef NOTWINDLL
#   define LDPUBLIC16
#   define LDPUBLIC32
#  else /* def NOTDLL */
#   define LDPUBLIC16 __stdcall
#   ifdef LIBDSK_EXPORTS
#    define LDPUBLIC32 __declspec(dllexport)
#   else  /* def LIBDSK_EXPORTS */
#    define LDPUBLIC32 __declspec(dllimport)
#   endif /* def LIBDSK_EXPORTS */ 
#  endif  /* def NOTDLL */
# else    /* def WIN32 */
#  define LDPUBLIC32 
#  define LDPUBLIC16
# endif /* def WIN32 */
#endif  /* def WIN16 */

#ifdef __cplusplus
extern "C" {
#endif

#define LIBDSK_VERSION "1.4.2"

/************************* TYPES ********************************/

typedef          int  dsk_err_t;	/* Error number */
typedef unsigned long dsk_lsect_t;	/* Logical sector */
typedef unsigned int  dsk_ltrack_t;	/* Logical track */
typedef unsigned int  dsk_psect_t;	/* Physical sector */
typedef unsigned int  dsk_pcyl_t;	/* Physical cylinder */
typedef unsigned int  dsk_phead_t;	/* Physical head */
typedef unsigned char dsk_gap_t;	/* Gap length */
typedef const char *  dsk_cchar_t;	/* Const char * */
#define DSK_ERR_OK	 (0)	/* No error */
#define DSK_ERR_BADPTR	 (-1)	/* Bad pointer */
#define DSK_ERR_DIVZERO  (-2)	/* Division by zero */
#define DSK_ERR_BADPARM  (-3)	/* Bad parameter */
#define DSK_ERR_NODRVR   (-4)	/* Driver not found */
#define DSK_ERR_NOTME    (-5)	/* Driver rejects disc */
#define DSK_ERR_SYSERR   (-6)	/* System error, use errno */
#define DSK_ERR_NOMEM  	 (-7)	/* Null return from malloc */
#define DSK_ERR_NOTIMPL  (-8)	/* Function not implemented */
#define DSK_ERR_MISMATCH (-9)	/* Check sectors: No match*/
#define DSK_ERR_NOTRDY   (-10)	/* Drive not ready */
#define DSK_ERR_RDONLY	 (-11)	/* Read-only disc */
#define DSK_ERR_SEEKFAIL (-12)	/* Seek fail */
#define DSK_ERR_DATAERR  (-13)	/* CRC data error */
#define DSK_ERR_NODATA   (-14)	/* No data */
#define DSK_ERR_NOADDR	 (-15)	/* Missing address mark */
#define DSK_ERR_BADFMT   (-16)	/* Bad format */
#define DSK_ERR_CHANGED  (-19)	/* Disc changed */
#define DSK_ERR_ECHECK	 (-20)	/* Equipment check */
#define DSK_ERR_OVERRUN  (-21)	/* Overrun */
#define DSK_ERR_ACCESS   (-22)	/* Access denied */
#define DSK_ERR_CTRLR 	 (-23)	/* Failed controller */
#define DSK_ERR_COMPRESS (-24)	/* Compressed file is corrupt */
#define DSK_ERR_RPC	 (-25)  /* Error encoding / decoding RPC */
#define DSK_ERR_BADOPT	 (-26)  /* Requested optional feature not present */
#define DSK_ERR_BADVAL	 (-27)  /* Bad value for requested option */
#define DSK_ERR_ABORT    (-28)  /* User abort requested */
#define DSK_ERR_TIMEOUT  (-29)  /* Communications timed out */
#define DSK_ERR_UNKRPC   (-30)  /* RPC server does not recognise function */
#define DSK_ERR_BADMEDIA (-31)	/* Unsuitable media for drive */
#define DSK_ERR_UNKNOWN  (-99)	/* Unknown error */

/* Is this error a transient error, that may be cleared by a retry? */
/* 1.1.3: Get this the right way round; they're negative numbers! */
#define DSK_TRANSIENT_ERROR(e) ((e) > DSK_ERR_COMPRESS && (e) <= DSK_ERR_NOTRDY)

/* Disc sidedness (logical/physical mapping). Use SIDES_ALT for single-sided floppies. */
typedef enum 
{	
	SIDES_ALT, 	/* Track n is cylinder (n/heads) head (n%heads) */
	SIDES_OUTBACK, 	/* Tracks go (head 0) 0,1,2,3,...37,38,39, then
			             (head 1) 39,38,37,...,2,1,0 */
	SIDES_OUTOUT,	/* Tracks go (head 0) 0,1,2,3,...37,38,39, then
			             (head 1) 0,1,2,3,...37,38,39 */
	SIDES_EXTSURFACE/* As SIDES_ALT, but sectors on head 1 identify
			 * as head 0, with numbers in sequence 
			 * eg: Head 0 has sectors 1-9, head 1 has 10-18 */
} dsk_sides_t;

typedef enum
{
	RATE_HD,	/* Data rate for 1.4Mb 3.5"  in 3.5"  drive */
	RATE_DD,	/* Data rate for 360k  5.25" in 1.2Mb drive */
	RATE_SD,	/* Data rate for 720k  3.5"  in 3.5"  drive */
	RATE_ED		/* Data rate for 2.8Mb 3.5"  in 3.5"  drive */
} dsk_rate_t;

typedef enum
{
	FMT_180K,	/* 9 sectors, 1 side */
	FMT_CPCSYS,	/* CPC system 180K */
	FMT_CPCDATA,	/* CPC data 180K */
	FMT_720K,	/* 9 sectors, 80 tracks, 2 sides */
	FMT_1440K,	/* 1.4M */
	FMT_160K,	/* 8 sectors, 1 side */
	FMT_320K,	/* 8 sectors, 2 sides */
	FMT_360K,	/* 9 sectors, 2 sides */
	FMT_720F,	/* 9 sectors, 2 sides out-and-back */
	FMT_1200F,	/* 15 sectors, 2 sides out-and-back */
	FMT_1440F,	/* 18 sectors, 2 sides out-and-back */
	FMT_ACORN160,	/* 16 sectors,  256 bytes/sector, 1 side */
	FMT_ACORN320,	/* 16 sectors,  256 bytes/sector, 1 side */
	FMT_ACORN640,	/* 16 sectors,  256 bytes/sector, 2 sides */
	FMT_ACORN800,	/*  5 sectors, 1024 bytes/sector, 2 sides */
	FMT_ACORN1600,	/* 10 sectors, 1024 bytes/sector, 2 sides */
	FMT_800K,	/* 10 sectors, 80 tracks, 2 sides */
	FMT_200K,	/* 10 sectors, 40 tracks, 1 side */
	FMT_BBC100,	/* 10 sectors, 40 tracks, 1 side, FM */
	FMT_BBC200,	/* 10 sectors, 80 tracks, 1 side, FM */
	FMT_MBEE400,	/* 10 sectors, 80 tracks, 1 side */
	FMT_MGT800,     /* 10 sectors, 80 tracks, 2 sides out and out */
	FMT_TRDOS640,	/* 16 sectors,  256 bytes/sector, 2 sides */
	FMT_AMPRO200,	/* 10 sectors, 512 bytes/sector, 1 side */
	FMT_AMPRO400D,	/* 10 sectors, 512 bytes/sector, 2 sides */
	FMT_AMPRO400S,	/* 5 sectors, 1024 bytes/sector, 1 side */
	FMT_AMPRO800,	/* 5 sectors, 1024 bytes/sector, 2 sides */
	FMT_UNKNOWN = -1
} dsk_format_t;

typedef enum
{
	/* Low byte of dg_fm: Recording mode */
	RECMODE_MASK     = 0x00FF,
	RECMODE_MFM      = 0x0000,	
	RECMODE_FM       = 0x0001,
	/* High byte of dg_fm: Other data recording flags */
	RECMODE_FLAGMASK = 0xFF00,
	RECMODE_COMPLEMENT = 0x0100	
	
} dsk_recmode_t;

/* DSK_GEOMETRY holds information used to convert physical to/from logical
 * sectors and to access the disc */
typedef struct
{
	dsk_sides_t	dg_sidedness;	/* How to handle multisided discs? */
	dsk_pcyl_t  	dg_cylinders;	/* Number of cylinders */
	dsk_phead_t	dg_heads;	/* Number of heads */
	dsk_psect_t	dg_sectors;	/* Sectors per track */
	dsk_psect_t	dg_secbase;	/* First physical sector number */
	size_t		dg_secsize;	/* Sector size in bytes */
	dsk_rate_t	dg_datarate;	/* Data rate */
	dsk_gap_t	dg_rwgap;	/* Read/write gap length */
	dsk_gap_t	dg_fmtgap;	/* Format gap length */
	int		dg_fm;		/* Really a dsk_recmode_t, kept as
					 * int for backward compatibility  */
	int		dg_nomulti;	/* Disable multitrack? */
	int		dg_noskip;	/* Set to 0 to skip deleted data */
} DSK_GEOMETRY;


/* Used when formatting a sector or reading its ID */
typedef struct
{
	dsk_pcyl_t	fmt_cylinder;
	dsk_phead_t	fmt_head;
	dsk_psect_t	fmt_sector;	
	size_t		fmt_secsize;
} DSK_FORMAT;

/* Callbacks from LibDsk to program */

typedef void (*DSK_REPORTFUNC)(const char *message);
typedef void (*DSK_REPORTEND)(void);

/***************************** GLOBAL FUNCTIONS ******************************/

/* Convert physical C/H/S to logical sector */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_ps2ls(const DSK_GEOMETRY *self,  
	/* in */	dsk_pcyl_t cyl, dsk_phead_t head, dsk_psect_t sec,
	/* out */	dsk_lsect_t *logical);

/* Convert logical sector to physical C/H/S */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_ls2ps(const DSK_GEOMETRY *self, 
	/* in */	dsk_lsect_t logical, 
	/* out */	dsk_pcyl_t *cyl, dsk_phead_t *head, dsk_psect_t *sec);

/* Convert physical cylinder/head to logical track */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_pt2lt(const DSK_GEOMETRY *self,  
	/* in */	dsk_pcyl_t cyl, dsk_phead_t head,
	/* out */	dsk_ltrack_t *logical);

/* Convert logical track to physical cylinder/head */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_lt2pt(const DSK_GEOMETRY *self, 
	/* in */	dsk_ltrack_t logical, 
	/* out */	dsk_pcyl_t *cyl, dsk_phead_t *head);
/* Expand error message */
LDPUBLIC32 char * LDPUBLIC16 dsk_strerror(/* in */ dsk_err_t err);

/* Initialise a DSK_GEOMETRY with one of the standard formats. 
 * If name / desc are not null, these are populated with the format's
 * short name and a brief description. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_stdformat(DSK_GEOMETRY *self, dsk_format_t formatid,
			dsk_cchar_t *name, dsk_cchar_t *desc);

/* Convert sector size to a physical sector shift as used by the controller.
 * To go the other way, size = 128 << psh  */
LDPUBLIC32 unsigned char LDPUBLIC16 dsk_get_psh(size_t sector_size);

/* Register callbacks for LibDsk functions to display information on the
 * screen. */
LDPUBLIC32 void LDPUBLIC16 dsk_reportfunc_set(DSK_REPORTFUNC report, 
					      DSK_REPORTEND  repend);
/* Retrieve the values of the callbacks */
LDPUBLIC32 void LDPUBLIC16 dsk_reportfunc_get(DSK_REPORTFUNC *report, 
					      DSK_REPORTEND  *repend);

/* Calls to these functions (or no-op, as appropriate) */
/* Report a work-in-progress message - LibDsk does this when starting a long 
 * operation such as decompression */
LDPUBLIC32 void LDPUBLIC16 dsk_report(const char *s);
/* Remove a work-in-progress message, if appropriate */
LDPUBLIC32 void LDPUBLIC16 dsk_report_end(void);

/*****************************************************************************/

/* Open a DSK file, POSIX image, drive or whatever. 
 * "type" is:
 *   NULL    : Autodetect file type from name or magic number
 *   "dsk"   : CPCEMU DSK or EDSK format 
 *   "raw"   : Raw dd if=foo of=bar file
 *   "floppy": Host system's floppy drive 
 *   "myz80" : MYZ80 image file
 *   "cfi"   : RLE-encoded raw file. Not done as a transparent compression
 *            driver, because it has no magic number.
 * "compress" is: 
 *  NULL     : Autodetect compression system.
 *  "sq"     : Huffman (Squeezed) 
 *  "gz"     : Gzip 
 * Will allocate a DSK_DRIVER object.
 */

typedef struct dsk_driver *DSK_PDRIVER;

LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_open(DSK_PDRIVER *self, const char *filename, 
			const char *type,
			const char *compress);

/* As for "open", but creates a new image file. On a floppy drive 
 * this will act exactly as "open"; for an image this will attempt to 
 * create the file new or truncate it.
 * Note that "type" cannot be NULL. "compress" can be (to create an 
 * uncompresed file). */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_creat(DSK_PDRIVER *self, const char *filename, 
			const char *type,
			const char *compress);

/* Close a DSK file. Frees the pointer and sets it to null. */

LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_close(DSK_PDRIVER *self);

/* Returns whether the disk has been modified since it was opened */

LDPUBLIC32 int              LDPUBLIC16 dsk_dirty(DSK_PDRIVER self);

/* Get drive status (Ready, Read-Only etc.). The actual status is 
 * based on the FDC's ST3 register. The following bits should be available: */

#define DSK_ST3_FAULT	0x80	/* Drive fault */
#define DSK_ST3_RO	0x40	/* Read only */
#define DSK_ST3_READY	0x20	/* Drive ready */
#define DSK_ST3_TRACK0	0x10	/* Head is over track 0 (not all drivers) */
#define DSK_ST3_DSDRIVE	0x08	/* Drive is double-sided */
#define DSK_ST3_HEAD1	0x04	/* Current head is head 1, not head 0 */

LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_drive_status(DSK_PDRIVER self, const DSK_GEOMETRY *geom, 
				dsk_phead_t head, unsigned char *status);

/* Read a sector. There are three alternative versions:
 *  One that uses physical sectors
 *  One that uses logical sectors
 *  One that uses physical sectors *which can have numbers not matching
 *  their positions on disc* - this functionality is only exposed by 
 *  drivers which can manipulate the FDC directly. */ 
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_pread(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_lread(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_lsect_t sector);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_xread(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              void *buf, 
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len,
			      int *deleted);
/* Write a sector. There are three alternative versions:
 *  One that uses physical sectors
 *  One that uses logical sectors
 *  One that uses physical sectors *which can have numbers not matching
 *  their positions on disc* - this functionality is only exposed by 
 *  drivers which can manipulate the FDC directly. */ 
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_pwrite(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_lwrite(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_lsect_t sector);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_xwrite(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              const void *buf, 
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len,
			      int deleted);

/* Verify sector against memory buffer. There are three alternative versions:
 *  One that uses physical sectors
 *  One that uses logical sectors
 *  One that uses physical sectors *which can have numbers not matching
 *  their positions on disc* - this functionality is only exposed by 
 *  drivers which can manipulate the FDC directly. */ 
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_pcheck(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_lcheck(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_lsect_t sector);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_xcheck(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
                              const void *buf, 
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len);
/* Format a track.
 * Note that the geometry in these functions is not const; the CPCEMU driver
 * will increase the number of tracks/heads as the disc image file outgrows
 * the geometry.
 */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_pformat(DSK_PDRIVER self, DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				const DSK_FORMAT *format, unsigned char filler);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_lformat(DSK_PDRIVER self, DSK_GEOMETRY *geom,
				dsk_ltrack_t track, const DSK_FORMAT *format, 
				unsigned char filler);
/* Read a track.
 */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_xtread(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
				void *buf, 
			    dsk_pcyl_t cylinder,     dsk_phead_t head,
				dsk_pcyl_t cyl_expected, dsk_phead_t head_expected);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_ptread(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
				void *buf, dsk_pcyl_t cylinder, 
				dsk_phead_t head);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_ltread(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
				void *buf, dsk_ltrack_t track);
/* Auto-format: generates the sector headers from "geom" */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_apform(DSK_PDRIVER self, DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				unsigned char filler);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_alform(DSK_PDRIVER self, DSK_GEOMETRY *geom,
				dsk_ltrack_t track, unsigned char filler);

/* Probe the geometry of a disc. This will use the boot sector and any
 * information the driver can give. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_getgeom(DSK_PDRIVER self, DSK_GEOMETRY *geom);
/* Convert various types of boot sector to DSK_GEOMETRY 
 * Return DSK_ERR_OK if successful, else DSK_ERR_BADFMT */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_dosgeom(DSK_GEOMETRY *self, const unsigned char *bootsect);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_pcwgeom(DSK_GEOMETRY *self, const unsigned char *bootsect);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_cpm86geom(DSK_GEOMETRY *self, const unsigned char *bootsect);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_aprigeom(DSK_GEOMETRY *self, const unsigned char *bootsect);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dg_opusgeom(DSK_GEOMETRY *self, const unsigned char *bootsect);

/* Read a random sector header from current track */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_psecid(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				DSK_FORMAT *result);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_lsecid(DSK_PDRIVER self, const DSK_GEOMETRY *geom,
				dsk_ltrack_t track, DSK_FORMAT *result);

/* Read all sector headers from current track in the order they appear. 
 * Not implemented yet; this is for future expansion. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_ptrackids(DSK_PDRIVER self, 
				const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head,
				dsk_psect_t *count, DSK_FORMAT **results);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_ltrackids(DSK_PDRIVER self, 
				const DSK_GEOMETRY *geom,
				dsk_ltrack_t track, 
				dsk_psect_t *count, DSK_FORMAT **result);

/* Read a track as it appears on the disk, including sector headers.
 * Not implemented yet; this is for future expansion. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_rtread(DSK_PDRIVER self, 
		const DSK_GEOMETRY *geom, void *buf, 
	        dsk_pcyl_t cylinder,     dsk_phead_t head, int reserved);

/* Seek to a cylinder */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_lseek(DSK_PDRIVER self, const DSK_GEOMETRY *geom, 
				dsk_ltrack_t track);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_pseek(DSK_PDRIVER self, const DSK_GEOMETRY *geom, 
				dsk_pcyl_t cylinder, dsk_phead_t head);

/* If "index" is in range, returns the n'th driver name in (*drvname).
 * Else sets (*drvname) to null. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_type_enum(int idx, char **drvname);

/* If "index" is in range, returns the human-readable description of the 
 * n'th driver name in (*drvdesc).  Else sets (*drvdesc) to null. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_typedesc_enum(int idx, char **drvdesc);

/* If "index" is in range, returns the n'th compressor name in (*compname).
 * Else sets (*drvname) to null. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_comp_enum(int idx, char **compname);

/* Force a drive to use head 0 or head 1 only for single-sided discs
 * Pass 0 or 1, or -1 to unset it. 
 * Deprecated: Use dsk_{set,get}_option(self, "HEAD", n) instead */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_set_forcehead(DSK_PDRIVER self, int force);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_get_forcehead(DSK_PDRIVER self, int *force);

/* Set a named option to an integer value. Returns DSK_ERR_BADOPT if the 
 * driver doesn't support the option; DSK_ERR_BADVAL if the value is out
 * of range */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_set_option(DSK_PDRIVER self, const char *name, int value);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_get_option(DSK_PDRIVER self, const char *name, int *value);
/* If "index" is in range, returns the n'th option name in (*optname).
 * Else sets (*optname) to null. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_option_enum(DSK_PDRIVER self, int idx, char **optname);

/* Get or set the comment for a disc image file. Not supported by all 
 * file formats. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_set_comment(DSK_PDRIVER self, const char *comment);
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_get_comment(DSK_PDRIVER self, char **comment);

/* Set / get the retry count. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_set_retry(DSK_PDRIVER self, unsigned int count);
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_get_retry(DSK_PDRIVER self, unsigned int *count);

/* Get the driver name and description */
LDPUBLIC32 const char * LDPUBLIC16 dsk_drvname(DSK_PDRIVER self);
LDPUBLIC32 const char * LDPUBLIC16 dsk_drvdesc(DSK_PDRIVER self);

/* Get the compression system name and description */
LDPUBLIC32 const char * LDPUBLIC16 dsk_compname(DSK_PDRIVER self);
LDPUBLIC32 const char * LDPUBLIC16 dsk_compdesc(DSK_PDRIVER self);

/* Decode and act on RPC packets sent by another LIBDSK.
 * This function is used to implement the 16-bit server app. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_rpc_server(unsigned char *input,
	int inp_len, unsigned char *output, int *out_len,
	int *ref_count);

/* Map a DSK_PDRIVER to an integer handle. Used for RPC and JNI. 
 * The null pointer is always mapped to zero. 
 * dsk_map_dtoi will add the pointer to the map if it isn't found. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_dtoi(DSK_PDRIVER ptr, unsigned int *n);
/* Given an integer handle, retrieve the corresponding DSK_PDRIVER. 
 * If the handle is not found, returns *ptr = NULL. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_itod(unsigned int n, DSK_PDRIVER *ptr);
/* Remove an integer <--> DSK_DRIVER mapping. If it was the last one, free
 *  * all the memory used by the mapping */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_delete(unsigned int index);



/* Define this to print on the console a trace of all mallocs */
#undef TRACE_MALLOCS 
#ifdef TRACE_MALLOCS
void *dsk_malloc(size_t size);
void  dsk_free(void *ptr);
#else
#define dsk_malloc malloc
#define dsk_free free
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBDSK_H */
