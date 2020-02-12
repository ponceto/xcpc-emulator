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

typedef struct dsk_option
{
	struct dsk_option *do_next;
	int do_value;
	char do_name[1];
} DSK_OPTION;

/* Moved here from libdsk.h; there's no need for it to be public */
typedef struct dsk_driver
{
        struct drv_class     *dr_class;
        struct compress_data *dr_compress;      /* Compressed? */
	struct remote_data   *dr_remote;	/* Remote, if any */
	struct dsk_option    *dr_options;	/* Optional properties, if any */
	char *dr_comment;	/* Comment, if any */
/*        int dr_forcehead;     Force drive to use head 0 or head 1
 *        			Moved to Linux floppy driver; it's the only one
 *        			that supports it. */
	int dr_dirty;		/* Has this device been written to? 
				 * Set to 1 by writes and formats */
	unsigned dr_retry_count; /* Number of times to retry if error */	
} DSK_DRIVER;



/* Functions a driver must provide. If you are implementing a driver, 
 * create instances of these functions and have pointers to them (see
 * eg. drvposix.c) */

typedef struct drv_class
{
	/* Variables */
	size_t	dc_selfsize;	/* Size of the DSK_DRIVER subclass to be
				 * malloced and zeroed before we enter 
				 * dc_open */
	char   *dc_drvname;	/* Short driver name, as used by eg. the 
				 * -itype and -otype arguments in DSKTRANS. */
	char   *dc_description;	/* Human-readable description of driver */

	/* Functions: */

	/* Open an existing disc image. Return DSK_ERR_OK if successful.
	 * Return DSK_ERR_NOTME if the image cannot be handled by this driver.
	 * Return other errors only if you want to stop other drivers having 
	 * a go. */ 
	dsk_err_t (*dc_open )(DSK_DRIVER *self, const char *filename);
	/* Create a new disc image. Return DSK_ERR_OK if successful, any
	 * other errors if failed. For floppy drives, does the same as
	 * dc_open */
	dsk_err_t (*dc_creat)(DSK_DRIVER *self, const char *filename);
	/* Close disc image / drive */
	dsk_err_t (*dc_close)(DSK_DRIVER *self);

	/* Read a physical sector */
	dsk_err_t (*dc_read)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head, dsk_psect_t sector);
	/* Write a physical sector */
	dsk_err_t (*dc_write)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      const void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head, dsk_psect_t sector);

	/* Format a track */
	dsk_err_t (*dc_format)(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
	/* Get geometry. Only provide this function if your disc image
	 * does not allow a program to set its own disc geometry; if it
	 * /does/, then use the DSK_GEOMETRY argument passed to the other
	 * functions (drvlinux.c is a good example here). 
	 * The two drivers which do this are the Windows NT one, because
	 * Windows NT uses an arbitrary geometry which can't be overridden;
	 * and the MYZ80 one, which has a single fixed geometry.
	 * Return DSK_ERR_NOTME to fall back to the default geometry
	 * probe; other values indicate immediate success or failure. */
	dsk_err_t (*dc_getgeom)(DSK_DRIVER *self, DSK_GEOMETRY *geom);

	/* Read the ID of a random sector on a certain track/head, and 
	 * put it in "result". This function is primarily used to test for
	 * discs in CPC format (which have oddly-numbered physical sectors); 
	 * drivers such as NT and POSIX can't implement it, and so don't. */
	dsk_err_t (*dc_secid)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
	/* Seek to a track */
	dsk_err_t (*dc_xseek)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_pcyl_t cylinder, dsk_phead_t head);
	/* Get drive status */
	dsk_err_t (*dc_status)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				dsk_phead_t head, unsigned char *result);
	/* Read a sector whose ID doesn't match its location on disc */
	dsk_err_t (*dc_xread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, 
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len,
			      int *deleted);
	/* Write a sector whose ID doesn't match its location on disc */
	dsk_err_t (*dc_xwrite)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      const void *buf,
			      dsk_pcyl_t cylinder, dsk_phead_t head, 
			      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			      dsk_psect_t sector, size_t sector_len,
			      int deleted);
	/* Read a track (8272 READ TRACK command) */
	dsk_err_t (*dc_tread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head);
	/* Read a track: Version where the sector ID doesn't necessarily
	 * match */
	dsk_err_t (*dc_xtread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			      void *buf, dsk_pcyl_t cylinder, 
			      dsk_phead_t head, dsk_pcyl_t cyl_expected,
			      dsk_phead_t head_expected);

	/* List driver-specific options */
	dsk_err_t (*dc_option_enum)(DSK_DRIVER *self, int idx, char **optname);

	/* Set a driver-specific option */
	dsk_err_t (*dc_option_set)(DSK_DRIVER *self, const char *optname, int value);
	/* Get a driver-specific option */
	dsk_err_t (*dc_option_get)(DSK_DRIVER *self, const char *optname, int *value);
	/* Read headers for an entire track at once */
	dsk_err_t (*dc_trackids)(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                dsk_psect_t *count, DSK_FORMAT **result);

	/* Read raw track, including sector headers */
	dsk_err_t (*dc_rtread)(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			void *buf, dsk_pcyl_t cylinder,  dsk_phead_t head, 
			int reserved, size_t *bufsize);
} DRV_CLASS;

