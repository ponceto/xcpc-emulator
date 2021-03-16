/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001, 2008  John Elliott <seasip.webmaster@gmail.com>      *
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

#include "compi.h"
#include "compdskf.h"

/* This driver provides limited support for compressed DSK files generated 
 * by IBM's SaveDskF utility, intended to be unpacked with their LoadDskF.
 *   
 * What it should eventually do is convert a compressed DSKF (magic 5AAA) to 
 * an uncompressed one (magic 59AA) and back.
 *
 * The compression system used appears to be based on LZW, but it reuses
 * dictionary entries when the dictionary fills up.
 *
 * TO DO:
 * - Fix the decompressor. Currently it's based on the LZW algorithm on 
 *   Wikipedia, and only works if the LZW dictionary doesn't overflow.
 *
 * - Write a matching compressor.
 */
COMPRESS_CLASS cc_dskf =
{
        sizeof(DSKF_COMPRESS_DATA),
        "dskf",
        "IBM LoadDskF (LZW compression)",
        cdskf_open,        /* open */
        cdskf_creat,       /* create new */
        cdskf_commit,      /* commit */
        cdskf_abort        /* abort */
};

static dsk_err_t dskf_decomp(DSKF_COMPRESS_DATA *self);

dsk_err_t cdskf_open(COMPRESS_DATA *s)
{
        FILE *fp;
        dsk_err_t err;
	DSKF_COMPRESS_DATA *self;

        /* Sanity check: Is this meant for our driver? */
        if (s->cd_class != &cc_dskf) return DSK_ERR_BADPTR;

	self = (DSKF_COMPRESS_DATA *)s;

        /* Open the file to decompress */
        err = comp_fopen(s, &fp);
        if (err) return DSK_ERR_NOTME;

	/* Read the DSKF header */
	if (fread(self->dskf_header, 1, sizeof(self->dskf_header), fp) <
			sizeof(self->dskf_header))
	{
		fclose(fp);
		return DSK_ERR_NOTME;
	}
	/* Check for compressed DSKF magic */
	if (self->dskf_header[1] != 0x5A || self->dskf_header[0] != 0xAA)
	{
		fclose(fp);
		return DSK_ERR_NOTME;
	}

	/* Open uncompressed output file */  
	err = comp_mktemp(s, &self->dskf_fpout);
	if (err) 
	{ 
		fclose(fp);
		return err;
	}
	self->dskf_fpin = fp;
	err = dskf_decomp(self);

	fclose(self->dskf_fpout);
	fclose(self->dskf_fpin);

	if (err) remove(s->cd_ufilename);
	return err; 
}


dsk_err_t cdskf_creat(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}

dsk_err_t cdskf_commit(COMPRESS_DATA *self)
{
	return DSK_ERR_NOTIMPL;
#if 0
	FILE *fp;
        dsk_err_t err;
	int c;
	gzFile gzfp;

        /* Sanity check: Is this meant for our driver? */
        if (self->cd_class != &cc_gz) return DSK_ERR_BADPTR;

        /* Open the file to compress */
	fp = fopen(self->cd_ufilename, "rb");
	if (!fp) return DSK_ERR_SYSERR;

	gzfp = gzopen(self->cd_cfilename, "wb");
	if (!gzfp) { fclose(fp); return DSK_ERR_SYSERR; }

	err = DSK_ERR_OK;
	while ((c = fgetc(fp)) != EOF)
	{
		if (gzputc(gzfp, c) == -1)
		{
			err = DSK_ERR_SYSERR;
			break;
		}
	}
	gzclose(gzfp);
	fclose(fp);
	return err;
#endif
}


dsk_err_t cdskf_abort(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}


/* Initialise the LZW decompression dictionary. Entries 1-257 map to 
 * themselves, others to blank. */
static void dskf_dict_init(DSKF_COMPRESS_DATA *self)
{
	int n;

	for (n = 0; n < MAXDICT; n++)
	{
		self->dskf_dict[n].first = 0;
		self->dskf_dict[n].second = 0;
	}
	for (n = 1; n <= 256; n++)
	{
		self->dskf_dict[n].first = n;
	}
}

/* Get the first word of a dictionary string */
static lzw_word dskf_dict_firstword(DSKF_COMPRESS_DATA *self, DICTBLOCK *db)
{
	do
	{
		if (db->first >= MAXDICT)
		{
			fprintf(stderr, "Internal error or corrupt LZW file!\n");
			return 0;
		}
		if (db->first < 257) return db->first;

		db = &self->dskf_dict[db->first];
	}
	while (1);
}


/* Output a dictionary entry. If we just walk the string of entries the
 * results will come out in reverse order; so push them onto a stack, and
 * output them as they come off. */
static dsk_err_t dskf_output_entry(DSKF_COMPRESS_DATA *self, DICTBLOCK *db)
{
	DICTBLOCK *cur;
	int stack_cur = 0;

	/* Blank entry */
	if (!db->first) return DSK_ERR_OK;

	do
	{
		/* If entry has a second character, output it */
		cur = db;
		if (cur->second)
		{
			self->dskf_stack[stack_cur++] = cur->second -1;
		}
		/* If entry has a first character which is literal, output 
		 * it */
		if (cur->first <= 257 && cur->first > 0)
		{
			self->dskf_stack[stack_cur++] = cur->first -1;
		}
		/* If entry has a first character which is a token, proceed
		 * to it */
		db = &self->dskf_dict[cur->first];
	}
	while (cur->first > 257);

	/* Now dump out the contents of the stack to the output file */
	while (stack_cur > 0)
	{
		if (fputc(self->dskf_stack[--stack_cur], 
					self->dskf_fpout) == EOF) 
			return DSK_ERR_SYSERR;
	} 
	return DSK_ERR_OK;
}


/* Read the next word from the LZW-compressed stream. Treat errors as EOF. */
static lzw_word dskf_getword(DSKF_COMPRESS_DATA *self)
{
	self->dskf_toggle = !self->dskf_toggle;

	if (!self->dskf_toggle)
	{
		int ch = fgetc(self->dskf_fpin);
		if (ch == EOF) return 0;
		return (self->dskf_pending << 8) | (ch & 0xFF);
	}
	else
	{
		int hi, lo;
	       	hi = fgetc(self->dskf_fpin);
		lo = fgetc(self->dskf_fpin);
		if (hi == EOF || lo == EOF) return 0;
		self->dskf_pending = (lo & 0x0F);
		return (hi << 4) | ((lo >> 4) & 0x0F);
	}
}



/* Decompress a compressed DSKF file */
static dsk_err_t dskf_decomp(DSKF_COMPRESS_DATA *self)
{
	int offset;
	lzw_word w, k, e0;
	dsk_err_t err;
	DICTBLOCK entry;
	int dict_size;

	/* OK, we have a 5AAA-format disc image. Find the start of the data. */
	offset = self->dskf_header[38] + 256 * self->dskf_header[39];
	self->dskf_header[1] = 0x59;	/* Set format to uncompressed */

	/* Write the modified header to the expanded file */
	if (fwrite(self->dskf_header, 1, sizeof(self->dskf_header), 
				self->dskf_fpout) < sizeof(self->dskf_header))
	{
		return DSK_ERR_SYSERR;
	}
	/* Copy the comment (if any) */
	while (offset > sizeof(self->dskf_header))
	{
		int c = fgetc(self->dskf_fpin);

		if (c == EOF) return DSK_ERR_SYSERR;
		if (fputc(c, self->dskf_fpout) == EOF) return DSK_ERR_SYSERR;
		--offset;
	}

	/* Start LZW decompression. Based on the pseudocode at Wikipedia:
	 * http://en.wikipedia.org/wiki/LZW */

	dskf_dict_init(self);
	dict_size = 257;
	self->dskf_toggle = 0;
	/* Read the first character */
	k = dskf_getword(self);
	if (k == 0) return DSK_ERR_OK;	/* EOF */
	/* Emit the first character */
	entry.first = k;
	entry.second = 0;
	err = dskf_output_entry(self, &entry);
	if (err) return err;
	w = k;
	/* Start full LZW decompression */
	while ( (k=dskf_getword(self)) )
	{
		/* See if the token is present in the dictionary */
		if (self->dskf_dict[k].first)
		{
			entry = self->dskf_dict[k];
		}
		else if (k == dict_size) /* The special case where a token
	       				  * is being used and added at the 
					  * same time */
		{
			entry.first = w;
			entry.second = dskf_dict_firstword(self, 
					&self->dskf_dict[w]);
		}
		else	return DSK_ERR_COMPRESS; /* Out of range */
		/* Output the entry */
		err = dskf_output_entry(self, &entry);
		if (err) return err;
		if (dict_size < MAXDICT)
		{
		/* Add w + entry[0] to the dictionary */
			e0 = dskf_dict_firstword(self, &entry);
			self->dskf_dict[dict_size].first = w;
			self->dskf_dict[dict_size].second = e0;
			++dict_size;
		}
		else
		{
/* XXX If the dictionary is full, replace the least recently used entry. */
		}
		w = k;
	}
	return DSK_ERR_OK;
}


