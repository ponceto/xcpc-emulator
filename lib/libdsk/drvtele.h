/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2,2007  John Elliott <seasip.webmaster@gmail.com>     *
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

/* Declarations for the Teledisk driver */

/* This structure (for the file header) is based on 
 * <http://www.fpns.net/willy/wteledsk.htm>
 */

typedef unsigned char tele_byte;
typedef unsigned short tele_word;

typedef struct
{
	char magic[3];
	tele_byte volume_seq;	/* 0 for first file in sequence, 1 for 2nd... */
	tele_byte volume_id;	/* All files must have the same volume ID */
	tele_byte ver;		/* Version of Teledisk used to create file */
	tele_byte datarate;	/* 0=250Kbps (360k/720k) 1=300Kbps (1.2M)
				   2=500Kbps (1.4M). Bit 7 set for FM. */
	tele_byte drivetype;	/* 1=360k 2=1.2M 3=720k 4=1.4M */
	tele_byte doubletrack;	/* 0=media matched drive
				   1=48tpi media, 96tpi drive
				   2=96tpi media, 48tpi drive
				  
				   Bit 7 set if comment present. */
	tele_byte dosmode;	/* Only allocated sectors backed up? */
	tele_byte sides;	/* Number of heads */
	tele_word crc;	
} TELE_HEADER;

typedef struct
{
	int year, mon, day;
	int hour, min, sec;
	char text[1];
} TELE_COMMENT;

typedef struct
{
	tele_byte sectors;
	tele_byte cylinder;
	tele_byte head;
	tele_byte crc;
} TELE_TRKHEAD;

typedef struct
{
	tele_byte cylinder_id;	/* On-disk sector ID */
	tele_byte head_id;
	tele_byte sector_id;
	size_t sector_len;
	tele_byte syndrome;	/* Various errors:
       				 * bit 0: Sector appeared more than once
				 * bit 1: CRC error
				 * bit 2: Deleted data
				 * bit 4: Unallocated sector not saved
				 * bit 5: Sector ID but no data
				 * bit 6: Data but no sector ID	*/
	tele_byte header_crc;
/* Strictly speaking the last 2 members are the start of the sector data 
 * rather than the end of the sector header. But there's not a lot of 
 * difference - you just have to be sure not to load them if 
 * (syndrome & 0x30). */
	tele_word compressed_len;
	tele_byte encoding;
} TELE_SECHEAD;

typedef struct
{
	DSK_DRIVER	tele_super;
	TELE_HEADER	tele_head;	
	FILE		*tele_fp;
	TELE_COMMENT	*tele_comment;
	/* Fake sector for READ ID command */
	unsigned int	tele_sector;
	/* Addresses of track headers */
	long		tele_track_addr[100][2];
	TELE_TRKHEAD	tele_trackhead;
	TELE_SECHEAD	tele_sechead;
} TELE_DSK_DRIVER;


dsk_err_t tele_open(DSK_DRIVER *self, const char *filename);
dsk_err_t tele_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t tele_close(DSK_DRIVER *self);
dsk_err_t tele_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t tele_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t tele_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t tele_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t tele_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head);
dsk_err_t tele_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int *deleted);
dsk_err_t tele_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted);
dsk_err_t tele_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                            dsk_pcyl_t cylinder, dsk_phead_t head,
                            dsk_psect_t *count, DSK_FORMAT **result);
dsk_err_t tele_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                  dsk_phead_t head, unsigned char *result);

dsk_err_t tele_option_enum(DSK_DRIVER *self, int idx, char **optname);

dsk_err_t tele_option_set(DSK_DRIVER *self, const char *optname, int value);
dsk_err_t tele_option_get(DSK_DRIVER *self, const char *optname, int *value);

dsk_err_t tele_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *dg);

