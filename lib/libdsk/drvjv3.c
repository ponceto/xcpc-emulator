/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2,2015  John Elliott <seasip.webmaster@gmail.com>     *
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


/* Declarations for the JV3 driver */

/* JV3 spec at <http://www.tim-mann.org/trs80/dskspec.html> */

#include "drvi.h"
#include "drvjv3.h"


DRV_CLASS dc_jv3 = 
{
	sizeof(JV3_DSK_DRIVER),
	"jv3",
	"JV3 file driver",
	jv3_open,
	jv3_creat,
	jv3_close,
	jv3_read,
	jv3_write,	
	jv3_format,
	jv3_getgeom,
	jv3_secid,
	jv3_xseek,
	jv3_status,
	jv3_xread,
	jv3_xwrite,	
	NULL,			/* jv3_tread */
	NULL,			/* jv3_xtread */
	NULL,			/* jv3_option_enum */
	NULL,			/* jv3_option_set */
	NULL,			/* jv3_option_get */
	jv3_trackids,
	NULL			/* jv3_rtread */
};

/* #define MONITOR(x) printf x     */

#define MONITOR(x) 


#define DC_CHECK(s) \
	JV3_DSK_DRIVER *self; \
	if (s->dr_class != &dc_jv3) return DSK_ERR_BADPTR; \
	self = (JV3_DSK_DRIVER *)s;

typedef struct 
{
	/* Currently-loaded header block */
	unsigned char cur_header[JV3_HEADER_LEN];

	/* Its location within the disk file */
	long header_offset;

	/* Did the callback change anything? */
	int touched;

	/* Stop processing? */
	int stop;

	/* Offset of the current sector */
	long data_offset;

	/* Pointer to the encoded header of the current sector */
	unsigned char *sector_head;

	/* Location of the current sector, and its flags */
	dsk_pcyl_t  cyl;
	dsk_phead_t head;
	dsk_psect_t sector;
	unsigned char flags;
	/* Size of the current sector */
	size_t secsize;

	/* Is the current sector an empty slot? */
	int isfree;
} JV3_ENUM_STATE;


/* If the header in 'st' has been touched, write the changes out */
dsk_err_t flush_header(JV3_DSK_DRIVER *self, JV3_ENUM_STATE *st)
{
	long pos;

	if (st->touched)
	{
		/* Get current file position */
		pos = ftell(self->jv3_fp);
		if (pos < 0) return DSK_ERR_SYSERR;

		/* Seek to the header */
		if (fseek(self->jv3_fp, st->header_offset, SEEK_SET) < 0)
		{
			return DSK_ERR_SYSERR;
		}
		/* Write the header */
		if (fwrite(st->cur_header, 1, JV3_HEADER_LEN, self->jv3_fp) 
							< JV3_HEADER_LEN)
		{
			return DSK_ERR_SYSERR;
		}
		/* Reset the 'touched' flag */
		st->touched = 0;
		/* And seek back to where we were */
		if (fseek(self->jv3_fp, pos, SEEK_SET) < 0)
		{
			return DSK_ERR_SYSERR;
		}
		/* Update cached header if appropriate */
		if (st->header_offset == 0)
			memcpy(self->jv3_header, st->cur_header, JV3_HEADER_LEN);
	}
	return DSK_ERR_OK;
}

/* Decode the 2-bit size code as a sector size. Annoyingly the encoding
 * is different for used and free sectors */
static size_t decode_size(int isfree, unsigned char size)
{
	switch (size & JV3_SIZE)
	{
		case 0: return (isfree) ?  512 :  256; 
		case 1: return (isfree) ? 1024 :  128; 	
		case 2: return (isfree) ?  128 : 1024; 	
		case 3: return (isfree) ?  256 :  512; 	
	}
	/* Can't happen */
	return 256;
}


/* Encode the sector size as a 2-bit size code. Annoyingly the encoding
 * is different for used and free sectors */
unsigned char encode_size(int isfree, size_t size)
{
	switch (size)
	{
		case  128: return (isfree) ? 2 : 1;
		default:
		case  256: return (isfree) ? 3 : 0;
		case  512: return (isfree) ? 0 : 3;
		case 1024: return (isfree) ? 1 : 2;
	}
}





typedef dsk_err_t (*JV3_CALLBACK)(JV3_DSK_DRIVER *self, 
				JV3_ENUM_STATE *state, void *param);

/* Because of the awkward chunked nature of JV3 (it consists of one or more 
 * blocks, each containing 0-JV3_HEADER_COUNT sectors, and you need to parse
 * each block completely to find the next) this driver uses a callback 
 * mechanism. jv3_enum_sectors() handles all the grubby business of 
 * enumerating each sector in turn 
 *
 * The function should return DSK_ERR_OK to continue, other values to abort 
 * with error. 
 * 
 * To stop the enumeration set state->stop to nonzero.
 */
dsk_err_t jv3_enum_sectors(JV3_DSK_DRIVER *self, unsigned char append,
		JV3_CALLBACK cbk, void *param)
{
	JV3_ENUM_STATE state;

	dsk_err_t err;
	int n;

	memset(&state, 0, sizeof(state));

	memcpy(state.cur_header, self->jv3_header, JV3_HEADER_LEN);
	state.header_offset = 0;
	state.data_offset   = JV3_HEADER_LEN; 

	while (state.data_offset < self->jv3_len)
	{
		for (n = 0; n < JV3_HEADER_COUNT; n++)
		{
			state.sector_head = state.cur_header + 3 * n;
			state.cyl    = state.sector_head[0];
			state.sector = state.sector_head[1];
			state.flags  = state.sector_head[2];
			state.head   = (state.flags & JV3_SIDE) ? 1 : 0;
			state.isfree = (state.cyl == JV3_FREE 
						&& state.sector == JV3_FREE);
			state.secsize = decode_size(state.isfree, state.flags);	

			err = (*cbk)(self, &state, param);

			if (state.stop)
			{
				dsk_err_t e2 = flush_header(self, &state);
				return (err == DSK_ERR_OK) ? e2 : err;
			}

			state.data_offset += state.secsize;
/* If we have reached EOF and 'stop' is not set and 'append' is nonzero,
 * append a blank sector of the appropriate type */

			if (state.data_offset >= self->jv3_len)
			{
				if (append && !state.stop &&
					n < (JV3_HEADER_COUNT - 1))
				{
/* sector_head points at the last valid sector. If there is space for
 * another sector in the current header, insert it */
					state.sector_head += 3;
					state.sector_head[0] = JV3_FREE;
					state.sector_head[1] = JV3_FREE;
					state.sector_head[2] = append;
					state.flags  = append;
					state.isfree = 1;
					state.secsize = decode_size(1, append);	
				
					err = (*cbk)(self, &state, param);
					if (err)
					{
						flush_header(self, &state);
						return err;
					}
					append = 0;
				}
				break;
			}
		}
		/* Have searched the header to no avail. Try the next
		 * header. */
		if (fseek(self->jv3_fp, state.data_offset, SEEK_SET) < 0)
		{
			flush_header(self, &state);
			return DSK_ERR_SYSERR;
		}
		if (fread(state.cur_header, 1, JV3_HEADER_LEN, self->jv3_fp) 
			< JV3_HEADER_LEN) 
		{
			flush_header(self, &state);
			return DSK_ERR_OK;
		}
	}
	err = flush_header(self, &state);
	if (err) return err;

	/* OK, we need to append a sector, but the existing header is 
	 * entirely full. Create a new header and create that. */
	if (append && !state.stop)
	{
		memset(state.cur_header, JV3_FREE, JV3_HEADER_LEN);
		/* If the last block in the file is a header block, 
		 * replace it. Otherwise, append a new header block. */
		if (state.data_offset > state.header_offset + JV3_HEADER_LEN)
		{
			state.header_offset = self->jv3_len;
			state.data_offset   = self->jv3_len + JV3_HEADER_LEN;
		}
		state.sector_head = state.cur_header;	
		state.sector_head[0] = JV3_FREE;
		state.sector_head[1] = JV3_FREE;
		state.sector_head[2] = append;
		state.flags  = append;
		state.isfree = 1;
		state.secsize = decode_size(1, append);	
				
		err = (*cbk)(self, &state, param);
		if (!err)
		{
			return flush_header(self, &state);
		}
		else
		{
			flush_header(self, &state);
			return err;
		}
	}
        return DSK_ERR_OK;
}





/*
dsk_err_t dump_callback(JV3_DSK_DRIVER *self, JV3_ENUM_STATE *state,
		void *param)
{
	printf("cyl=%02x head=%02x sec=%02x size=%4d offset=%6ld free=%d flags=%02x\n",
		state->cyl, state->head, state->sector, 
		(int)state->secsize, state->data_offset, state->isfree, 
		state->flags);
	return DSK_ERR_OK;
}*/


dsk_err_t jv3_open(DSK_DRIVER *s, const char *filename)
{
	DC_CHECK(s);

	self->jv3_fp = fopen(filename, "rb");
	if (!self->jv3_fp) return DSK_ERR_NOTME;

	/* Get file size */ 
	if (fseek(self->jv3_fp, 0, SEEK_END) < 0        ||
	    ((self->jv3_len = ftell(self->jv3_fp)) < 0) || 
	    fseek(self->jv3_fp, 0, SEEK_SET) < 0)
	{
		return DSK_ERR_NOTME;
	}  

	if (fread(self->jv3_header, 1, sizeof(self->jv3_header), self->jv3_fp) 
		< (int)sizeof(self->jv3_header))
	{
		fclose(self->jv3_fp);
		return DSK_ERR_NOTME;
	}
/* Diagnostic dump of what we just loaded
	jv3_enum_sectors(self, 0, dump_callback, NULL);
*/
	/* OK, the header loaded. There's no magic number or other 
	 * metadata we can check, so just assume it worked */
	return DSK_ERR_OK;
}

dsk_err_t jv3_creat(DSK_DRIVER *s, const char *filename)
{
	DC_CHECK(s);

	self->jv3_fp = fopen(filename, "wb");
	if (!self->jv3_fp) return DSK_ERR_SYSERR;

	memset(self->jv3_header, JV3_FREE, sizeof(self->jv3_header));
	self->jv3_len = sizeof(self->jv3_header);

	if (fwrite(self->jv3_header, 1, sizeof(self->jv3_header), self->jv3_fp)
		< (int)sizeof(self->jv3_header))
	{
		return DSK_ERR_SYSERR;
	}
	return DSK_ERR_OK;
}



dsk_err_t jv3_close(DSK_DRIVER *s)
{
	DC_CHECK(s);

	if (fclose(self->jv3_fp)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}



dsk_err_t jv3_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return jv3_xwrite (self, geom, buf, cylinder, head, 
				cylinder, head, sector, geom->dg_secsize, 0);
}


typedef struct
{
	int found;
	dsk_pcyl_t cylinder;
	dsk_phead_t head;
	dsk_psect_t sector;
	int deleted;
	int fm;
	size_t sector_size;	
	void *buf;
	dsk_err_t result;

} READSEC_DATA;




dsk_err_t droptrack_callback(JV3_DSK_DRIVER *self, JV3_ENUM_STATE *state,
		void *param)
{
	READSEC_DATA *rsd = (READSEC_DATA *)param;

	if (state->cyl    == rsd->cylinder &&
	    state->head   == rsd->head     &&
	    state->isfree == 0)
	{
		state->sector_head[0] = JV3_FREE;
		state->sector_head[1] = JV3_FREE;
		state->sector_head[2] = JV3_FREEF;
		state->sector_head[2] |= encode_size(1, state->secsize);
		state->touched = 1;
	}
	return DSK_ERR_OK;
}


dsk_err_t dropsector_callback(JV3_DSK_DRIVER *self, JV3_ENUM_STATE *state,
		void *param)
{
	READSEC_DATA *rsd = (READSEC_DATA *)param;

	if (state->cyl    == rsd->cylinder &&
	    state->head   == rsd->head     &&
	    state->sector == rsd->sector   &&
	    state->isfree == 0)
	{
		state->sector_head[0] = JV3_FREE;
		state->sector_head[1] = JV3_FREE;
		state->sector_head[2] = JV3_FREEF;
		state->sector_head[2] |= encode_size(1, state->secsize);
		state->touched = 1;
	}
	return DSK_ERR_OK;
}

dsk_err_t format_sector_callback(JV3_DSK_DRIVER *self,
				 JV3_ENUM_STATE *state, void *param)
{
	READSEC_DATA *rsd = (READSEC_DATA *)param;
	unsigned n;
	long pos;

	if (!state->isfree || state->secsize != rsd->sector_size)
	{
		return DSK_ERR_OK;
	}
	/* We've got a suitable blank space */
	state->touched = 1;
	state->sector_head[0] = rsd->cylinder;
	state->sector_head[1] = rsd->sector;
	state->sector_head[2] = encode_size(0, rsd->sector_size) & JV3_SIZE;
	if (!rsd->fm)  state->sector_head[2] |= JV3_DENSITY;
	if (rsd->head) state->sector_head[2] |= JV3_SIDE;

	/* Write fillers */
	if (fseek(self->jv3_fp, state->data_offset, SEEK_SET) < 0)
		return DSK_ERR_SYSERR;

	for (n = 0; n < state->secsize; n++)
	{
		if (fputc(rsd->deleted, self->jv3_fp) == EOF)
			return DSK_ERR_SYSERR;
	}
	pos = ftell(self->jv3_fp);
	/* If we have extended the file, record the fact. */
	if (pos > self->jv3_len)
	{
		self->jv3_len = pos;
	}
	state->stop = 1;	/* New sector written. */

	return DSK_ERR_OK;
}


dsk_err_t jv3_format(DSK_DRIVER *s, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	READSEC_DATA rsd;
	dsk_psect_t sec;
	dsk_err_t err;
	unsigned char blanksize;
	
	DC_CHECK(s);

	if (self->jv3_header[JV3_HEADER_LEN - 1] == 0) return DSK_ERR_RDONLY;

	rsd.found = 0;	
	rsd.cylinder = cylinder;
	rsd.head = head;

	/* Delete any existing sectors */
	err = jv3_enum_sectors(self, 0, droptrack_callback, &rsd);

	if (err) return err;
	
	/* And write a blank set */
	for (sec = 0; sec < geom->dg_sectors; sec++)
	{
		rsd.found       = 0;
		rsd.cylinder    = cylinder;
		rsd.head        = head;
		rsd.sector      = format[sec].fmt_sector;
		rsd.fm          = (geom->dg_fm & RECMODE_MASK) == RECMODE_FM;
		rsd.sector_size = format[sec].fmt_secsize;
		rsd.deleted     = filler;
		blanksize       = JV3_FREEF | encode_size(1, rsd.sector_size);
		err = jv3_enum_sectors(self, blanksize, format_sector_callback, &rsd);
		if (err) return err;
	}
	return DSK_ERR_OK;
}



dsk_err_t jv3_secid(DSK_DRIVER *s, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	DSK_FORMAT *buf = NULL;
	dsk_psect_t count = 0;
	dsk_err_t err;
	int offset;

	DC_CHECK(s)

	/* Gather all the sector IDs for a track together, and then
	 * pick one */
	err = jv3_trackids(s, geom, cylinder, head, &count, &buf);

	if (count == 0 && !err)
	{
		self->jv3_sector = 0;
		err = DSK_ERR_NOADDR;
	}
	if (!err)
	{
		offset = self->jv3_sector % count;

		*result = buf[offset];

		++self->jv3_sector;
	}
	if (buf)
	{
		dsk_free(buf);
	}
	return err;
}


dsk_err_t jv3_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	DSK_FORMAT dummy;

	dsk_err_t err = jv3_secid(self, geom, cylinder, head, &dummy);

	if (err == DSK_ERR_NOADDR) return DSK_ERR_SEEKFAIL;

	return err;
}


dsk_err_t jv3_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	return jv3_xread(self, geom, buf, cylinder, head, cylinder, 
		dg_x_head(geom, head),
		dg_x_sector(geom, head, sector), geom->dg_secsize, NULL);
}




static dsk_err_t xread_callback(JV3_DSK_DRIVER *self, 
				JV3_ENUM_STATE *state, void *param)
{
	READSEC_DATA *rsd = (READSEC_DATA *)param;

	size_t sec_size = state->secsize;

	int fm      = (state->flags & JV3_DENSITY) ? 0 : 1;
	int deleted = (state->flags & JV3_DAM)     ? 1 : 0;

	if (state->isfree                  ||
	    state->cyl    != rsd->cylinder ||
 	    state->head   != rsd->head     ||
	    state->sector != rsd->sector   ||
	    fm            != rsd->fm       ||
            deleted       != rsd->deleted) return DSK_ERR_OK;	/* No match */

	if (fseek(self->jv3_fp, state->data_offset, SEEK_SET) < 0) 
		return DSK_ERR_SYSERR;
	if (sec_size > rsd->sector_size) sec_size = rsd->sector_size;

	sec_size = fread(rsd->buf, 1, sec_size, self->jv3_fp);
	for (; sec_size < rsd->sector_size; sec_size++)
	{
		((unsigned char *)(rsd->buf))[sec_size] = 0xE5;
	}
	rsd->result = (state->flags & JV3_ERROR) ? DSK_ERR_DATAERR : DSK_ERR_OK;
	rsd->deleted = deleted;
	state->stop = 1;
	return DSK_ERR_OK;
}


/* Attempt to write to an existing sector */
static dsk_err_t xwrite_callback1(JV3_DSK_DRIVER *self, 
				JV3_ENUM_STATE *state, void *param)
{
	READSEC_DATA *rsd = (READSEC_DATA *)param;

	size_t sec_size = state->secsize;

	int fm      = (state->flags & JV3_DENSITY) ? 0 : 1;
	int deleted = (state->flags & JV3_DAM)     ? 1 : 0;

	if (state->isfree                  ||
	    state->cyl    != rsd->cylinder ||
 	    state->head   != rsd->head     ||
	    state->sector != rsd->sector   ||
	    fm            != rsd->fm       ||
            deleted       != rsd->deleted) return DSK_ERR_OK;	/* No match */

	if (fseek(self->jv3_fp, state->data_offset, SEEK_SET) < 0) 
		return DSK_ERR_SYSERR;
	if (sec_size > rsd->sector_size) sec_size = rsd->sector_size;

	if (fwrite(rsd->buf, 1, sec_size, self->jv3_fp) < sec_size)
		return DSK_ERR_SYSERR;

	if (rsd->deleted)
	{
		state->sector_head[2] &= ~JV3_DAM;
		state->sector_head[2] |= 0x20;
	}
	else
	{
		state->sector_head[2] &= ~JV3_DAM;
	}
	if (state->sector_head[2] != state->flags) 
		state->touched = 1;
	state->stop = 1;
	rsd->found = 1;
	return DSK_ERR_OK;
}





dsk_err_t jv3_xread(DSK_DRIVER *s, const DSK_GEOMETRY *geom, void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, 
			      dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, 
			      int *deleted)
{
	dsk_err_t err = DSK_ERR_OK;
	READSEC_DATA rsd;
	
	DC_CHECK(s);


	rsd.cylinder = cylinder;
	rsd.head     = head;
	rsd.sector   = sector;
	if (deleted && *deleted) rsd.deleted = 1;
	else			 rsd.deleted = 0;
	rsd.fm       = (geom->dg_fm & RECMODE_MASK) == RECMODE_FM;
	rsd.sector_size = sector_size;
	rsd.buf         = buf;
	rsd.result      = DSK_ERR_NOADDR;

	err = jv3_enum_sectors(self, 0, xread_callback, &rsd);

	if (err) return err;
	if (rsd.result == DSK_ERR_NOADDR) 
	{
		self->jv3_sector = 0;
	}
	if (deleted) *deleted = rsd.deleted;
	return rsd.result;
}

dsk_err_t jv3_xwrite(DSK_DRIVER *s, const DSK_GEOMETRY *geom, const void *buf,
                              dsk_pcyl_t cylinder, dsk_phead_t head,
                              dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
                              dsk_psect_t sector, size_t sector_size, int deleted)
{
	dsk_err_t err = DSK_ERR_OK;
	READSEC_DATA rsd;

	DC_CHECK(s);

	if (self->jv3_header[JV3_HEADER_LEN - 1] == 0) return DSK_ERR_RDONLY;

	rsd.cylinder = cylinder;
	rsd.head     = head;
	rsd.sector   = sector;
	rsd.deleted  = deleted;
	rsd.fm       = (geom->dg_fm & RECMODE_MASK) == RECMODE_FM;
	rsd.sector_size = sector_size;
	rsd.buf         = (void *)buf;
	rsd.result      = DSK_ERR_NOADDR;
	rsd.found       = 0;

	err = jv3_enum_sectors(self, 0, xwrite_callback1, &rsd);

	if (err) return err;

	if (!rsd.found)
	{
/* If the sector has not been found, should we create it? Probably not; only
 * jv3_format should create sectors if they aren't there. */
		self->jv3_sector = 0;
		return DSK_ERR_NOADDR;
	}
	return DSK_ERR_OK;
}

typedef struct
{
	int fm;
        dsk_pcyl_t cylinder;
	dsk_phead_t head;
	unsigned count;
	DSK_FORMAT *result;
} TRACKIDS_PARAM;


static dsk_err_t trackids_callback(JV3_DSK_DRIVER *self, 
				JV3_ENUM_STATE *state, void *p)
{
	TRACKIDS_PARAM *param = (TRACKIDS_PARAM *)p;

	int fm          = (state->flags & JV3_DENSITY) ? 0 : 1;

	if (state->cyl    == param->cylinder &&
	    state->head   == param->head &&
	    fm            == param->fm &&
	    state->isfree == 0)
	{
		if (param->result)
		{
			param->result[param->count].fmt_cylinder = state->cyl;
			param->result[param->count].fmt_head     = state->head;
			param->result[param->count].fmt_sector   = state->sector;
			param->result[param->count].fmt_secsize  = state->secsize;

		}
		++param->count;
	}
	return DSK_ERR_OK;
}



dsk_err_t jv3_trackids(DSK_DRIVER *s, const DSK_GEOMETRY *geom,
                            dsk_pcyl_t cylinder, dsk_phead_t head,
                            dsk_psect_t *count, DSK_FORMAT **result)
{
	TRACKIDS_PARAM param;
	dsk_err_t err;

	DC_CHECK(s);

	param.fm   = (geom->dg_fm & RECMODE_MASK) == RECMODE_FM;
	param.cylinder = cylinder;
	param.head     = head;
	param.count = 0;
	param.result = NULL;

	/* We call this function twice: Once to count the number of 
	 * sectors with the given cylinder and head; then allocate the
	 * results array; then call again to populate the array. */
	err = jv3_enum_sectors(self, 0, trackids_callback, &param);
	if (err) return err;

	if (!param.count)
	{
		*count = 0;
		*result = NULL;
		return DSK_ERR_OK;
	}
	param.result = dsk_malloc(param.count * sizeof(DSK_FORMAT));
	if (!param.result) return DSK_ERR_NOMEM;

	param.count = 0;
	err = jv3_enum_sectors(self, 0, trackids_callback, &param);
	if (err) return err;

	*count = param.count;
	*result = param.result;

	return DSK_ERR_OK;
}

dsk_err_t jv3_status(DSK_DRIVER *s, const DSK_GEOMETRY *geom,
                  dsk_phead_t head, unsigned char *result)
{
	DC_CHECK(s);

	if (!self->jv3_fp) 
		*result &= ~DSK_ST3_READY;

	if (self->jv3_header[JV3_HEADER_LEN - 1] == 0) 
		*result |= DSK_ST3_RO;
	
	return DSK_ERR_OK;
}


typedef struct
{
	DSK_GEOMETRY testgeom;
        dsk_psect_t minsec0, maxsec0, minsec1, maxsec1;
} GEOM_RESULT;

static dsk_err_t geom_callback(JV3_DSK_DRIVER *self, 
				JV3_ENUM_STATE *state, void *param)
{
	GEOM_RESULT *gr = (GEOM_RESULT *)param;

	/* Ignore free sectors */
	if (state->isfree) return DSK_ERR_OK;

	gr->testgeom.dg_secsize = state->secsize;
	if (state->cyl >= gr->testgeom.dg_cylinders) 
		gr->testgeom.dg_cylinders = state->cyl + 1;
	if (state->head >= gr->testgeom.dg_heads)
		gr->testgeom.dg_heads = state->head + 1;
	gr->testgeom.dg_datarate = RATE_SD;
	gr->testgeom.dg_fm = (state->flags & JV3_DENSITY) ? RECMODE_MFM : RECMODE_FM;
	if (state->head == 1)
	{
		if (state->sector < gr->minsec1) gr->minsec1 = state->sector;
		if (state->sector > gr->maxsec1) gr->maxsec1 = state->sector;			
	}
	else
	{
		if (state->sector < gr->minsec0) gr->minsec0 = state->sector;
		if (state->sector > gr->maxsec0) gr->maxsec0 = state->sector;			
	}
	return DSK_ERR_OK;
}


dsk_err_t jv3_getgeom(DSK_DRIVER *s, DSK_GEOMETRY *geom)
{
	dsk_err_t err;
	GEOM_RESULT result;

	DC_CHECK(s);

	/* First try the normal geometry probe */
	err = dsk_defgetgeom(s, geom);

	if (!err) return err;

        dg_stdformat(&result.testgeom, FMT_180K, NULL, NULL);
        result.testgeom.dg_cylinders = 0;
        result.testgeom.dg_sectors = 0;
        result.testgeom.dg_heads = 0;

        result.minsec0 = result.minsec1 = 256;
        result.maxsec0 = result.maxsec1 = 0;

	err = jv3_enum_sectors(self, 0, geom_callback, &result);
	
	if (err) return err;	
	
        result.testgeom.dg_secbase = result.minsec0;
	result.testgeom.dg_sectors = (result.maxsec0 - result.minsec0) + 1;

	if (result.testgeom.dg_cylinders == 0 ||
            result.testgeom.dg_sectors   == 0) return DSK_ERR_BADFMT;

        memcpy(geom, &result.testgeom, sizeof(*geom));
        return DSK_ERR_OK;

}


