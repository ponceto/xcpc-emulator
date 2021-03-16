/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2005,2011  John Elliott <seasip.webmaster@gmail.com>       *
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

/* Declarations for the Reverse-CP/M driver */

/* Sectors that are written but don't correspond to files have to be
 * saved in memory. These include the CP/M directory, and areas of the 
 * disc that aren't allocated to files (but a future write to the 
 * directory could change that!) 
 *
 * The sectors are buffered in a singly-linked list.
 */ 

typedef struct rcpmfs_buffer
{
	struct rcpmfs_buffer *rcb_next;
	size_t		      rcb_size;
	dsk_lsect_t           rcb_lsect;	
	unsigned char         rcb_data[1];
} RCPMFS_BUFFER;

typedef struct
{
        DSK_DRIVER rc_super;
	DSK_GEOMETRY rc_geom;

	char rc_dir[PATH_MAX];
	char *rc_namemap;

	RCPMFS_BUFFER *rc_bufhead;

/* CP/M filesystem description */
	unsigned rc_blocksize;
	unsigned rc_dirblocks;	
	unsigned rc_totalblocks;
	unsigned rc_systracks;

/* Number of directory entries added */
	unsigned rc_dirent;

/* Filesystem options */
	signed rc_fsversion;

/* Temporary sector buffer */
	unsigned char *rc_sectorbuf;

/* Directory label byte, controlling such things as time stamps */
	unsigned char rc_dirlabel;
/* Last sector ID returned */
	int rc_secid;
} RCPMFS_DSK_DRIVER;

#define FSVERSION_CPM2 2
#define FSVERSION_CPM3 3
#define FSVERSION_ISX  (-2)


dsk_err_t rcpmfs_open(DSK_DRIVER *self, const char *filename);
dsk_err_t rcpmfs_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t rcpmfs_close(DSK_DRIVER *self);
dsk_err_t rcpmfs_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);
dsk_err_t rcpmfs_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t rcpmfs_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t rcpmfs_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t rcpmfs_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t rcpmfs_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_phead_t head, unsigned char *result);
dsk_err_t rcpmfs_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		         dsk_pcyl_t cyl, dsk_phead_t head, DSK_FORMAT *result);
dsk_err_t rcpmfs_option_enum(DSK_DRIVER *self, int idx, char **optname);
dsk_err_t rcpmfs_option_set(DSK_DRIVER *self, const char *optname, int value);
dsk_err_t rcpmfs_option_get(DSK_DRIVER *self, const char *optname, int *value);
