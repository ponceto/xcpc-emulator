/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2002  John Elliott <seasip.webmaster@gmail.com>            *
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

/* Squeeze (Huffman) compression/decompression. This is entirely new code,
 * created by John Elliott to avoid possible licensing problems with the 
 * Richard Greenlaw/Theo Pozzy code used in earlier releases. 
 *
 * In brief: Richard Greenlaw's original code had no licence with it at all. 
 *          However, a file dated 1983 on the CP/M CDROM suggests that he 
 *          intended it as not-for-profit rather than true PD.
 *
 *           Theo Pozzy's UNIX port of SQ (1985) implies that the code he used
 *           is truly PD. But his belief that SQ was public domain may not
 *           be correct in the light of the 1983 letter above.
 * 
 * My thanks to Russell Marks for pointing this out.
 *
 * Note that my code is less sophisticated than Richard Greenlaw's original. 
 * It doesn't, for example, try to deal with pathologically unbalanced 
 * encoding trees (where a source byte encodes to more than 16 bits). 
 *
 */

#include "compi.h"
#include "compsq.h"


/******************************* compsq.c **********************************/
typedef dsk_err_t (*RLEFUNC)(SQ_COMPRESS_DATA *self, int ch);



/************************ RLE COMPRESSION FUNCTIONS ************************/
static void rle_reset(SQ_COMPRESS_DATA *self)
{
	self->rle_char = -1;
	self->rle_run = 0;
}




static dsk_err_t rle_flush(SQ_COMPRESS_DATA *self, RLEFUNC writer)
{
	dsk_err_t err = DSK_ERR_OK;
/* We encode sequences of >3 bytes using RLE. However, the RLE trigger byte
 * itself (0x90) is encoded as 2 bytes, and we can't encode sequences of it; it 
 * would encode to (eg) 20 90 90 xx, and then decompress as (20 90 90) xx 
 * when it was compressed as 20 (90 90 xx). 
 */
	if ( self->rle_run > 3 && self->rle_char != RLECODE)
	{
		err           = (*writer)(self, self->rle_char);
		if (!err) err = (*writer)(self, RLECODE);
		if (!err) err = (*writer)(self, self->rle_run);		
	}
	else while (self->rle_run)
	{
		if (!err) err = (*writer)(self, self->rle_char);
		if (self->rle_char == RLECODE && !err) err = (*writer)(self, 0x00);	
		--self->rle_run;
	}
	self->rle_run = 0;
	return err;
}



/* Generate an RLE stream from a file */
static dsk_err_t rle_stream(SQ_COMPRESS_DATA *self, RLEFUNC writer)
{
	int c;
	dsk_err_t err;

	while (  (c = fgetc(self->fp_in)) != EOF )	
	{
		self->ck_sum += (unsigned char)c;
/* If this character is the same as the last, increase the count. */ 
		if (c == self->rle_char)
		{
			++self->rle_run;
			if (self->rle_run == 0xFF) /* Max length of run */
			{	/* flush & start again */
				err = rle_flush(self, writer);
				if (err) return err;
				self->rle_char = -1;
				self->rle_run  = 0;				
			}
		}
		else
		{
/* If not, write out what we've got and start again. */
			err = rle_flush(self, writer);
			if (err) return err;
			self->rle_char = c;
			self->rle_run = 1;
		}
	}
	if (ferror(self->fp_in)) return DSK_ERR_SYSERR;
/* Write out what we've got; then EOF. */
	err = rle_flush(self, writer);
	if (err) return err;
	self->rle_char = SQ_EOF;
	self->rle_run = 1;
	err = rle_flush(self, writer);
	if (err) return err;
	rewind(self->fp_in);
	return DSK_ERR_OK;
}


/*
static dsk_err_t rle_print(SQ_COMPRESS_DATA *self, int ch)
{
	if (ch == '\r' || ch == '\n' || (ch > 0x1F && ch < 0x7F))
		fputc(ch, self->fp_out);
	else fprintf(self->fp_out, "<%02x>", ch);
	return DSK_ERR_OK;
} */
/******************* RLE COMPRESSION FUNCTIONS: END ************************/

static dsk_err_t huf_counter(SQ_COMPRESS_DATA *self, int ch)
{
	if (ch >= 0 && ch <= SQ_EOF) ++self->huf_count[ch + MAXNODE];
	else return DSK_ERR_COMPRESS;

	return DSK_ERR_OK;
}

/* Find the index of the smallest nonzero character count in huf_count.
 */
static int huf_smallest(SQ_COMPRESS_DATA *self, int skip)
{
	unsigned long mv = ~0UL;
	int n, mi = -1;

	for (n = 0; n < MAXLEAF + MAXNODE; n++) 
	{
		if (n == skip) continue;
		if (self->huf_count[n] < mv && self->huf_count[n] > 0) 
		{ 
			mv = self->huf_count[n]; 
			mi = n;
		}
	}
	return mi;
}


/* Add a node. */
static void huf_addnode(SQ_COMPRESS_DATA *self, int n1, int n2)
{
	short e_n1, e_n2;

	/* printf("%04X:%04X\r\n", n1, n2); */

	if (n1 < MAXNODE) e_n1 = n1; else e_n1 = MAXNODE - 1 - n1;
	if (n2 < MAXNODE) e_n2 = n2; else e_n2 = MAXNODE - 1 - n2;

	/* If parent not set, parent is MAXNODE */
	self->huf_node[self->huf_curnode].parent = MAXNODE;
	self->huf_node[self->huf_curnode].left = e_n1;
	self->huf_node[self->huf_curnode].right = e_n2;

/* If nodes hold leaves, record them. If they hold other nodes, set parents. */
	if (n1 >= MAXNODE) self->huf_leaves[n1 - MAXNODE] = self->huf_curnode;
	else		   self->huf_node[n1].parent      = self->huf_curnode;
	if (n2 >= MAXNODE) self->huf_leaves[n2 - MAXNODE] = self->huf_curnode;
	else		   self->huf_node[n2].parent      = self->huf_curnode;

}



/* Generate the Huffman dictionary for a file */
static dsk_err_t analyze(SQ_COMPRESS_DATA *self)
{
	int n, n1, n2;
	dsk_err_t err;

	/* Initialise counters */
	self->ck_sum = 0;
	for (n = 0; n < MAXNODE + MAXLEAF; n++) self->huf_count[n] = 0;
	rle_reset(self);
	err = rle_stream(self, huf_counter);
	if (err) return err;

	/* Start allocating nodes from the end. That way, the root node,
	 * the last to be populated, will have the lowest number. */
	self->huf_curnode = MAXNODE - 1;
	self->huf_nodecount = 0;
	for (n = 0; n < MAXNODE + MAXLEAF; n++) if (self->huf_count[n]) 
		++self->huf_nodecount;

	/* Special case if there's only one node (empty file) */
	if (self->huf_nodecount == 1)
	{
		n1 = huf_smallest(self, -1);
		huf_addnode(self, n1, n1);
		self->huf_nodecount = 0;
		--self->huf_curnode;
	}

	/* huf_nodecount = number of leaves */
	while (self->huf_nodecount > 1)
	{
		n1 = huf_smallest(self, -1);
		n2 = huf_smallest(self, n1);
		/* We have n1 and n2: Two nodes to be combined */
		huf_addnode(self, n1, n2);
		
		self->huf_count[self->huf_curnode] = 
			self->huf_count[n1] + self->huf_count[n2];	
		self->huf_count[n1] = 0;
		self->huf_count[n2] = 0;
		--self->huf_curnode;
		--self->huf_nodecount;
	}
	/* self->huf_curnode is now the node before the root node */
	return DSK_ERR_OK;
}

static dsk_err_t writec(SQ_COMPRESS_DATA *self, unsigned char c)
{
	if (fputc(c, self->fp_out) == EOF) return DSK_ERR_SYSERR;

	return DSK_ERR_OK;
}


static dsk_err_t writes(SQ_COMPRESS_DATA *self, unsigned short s)
{
	dsk_err_t e;

	e = writec(self, (unsigned char)(s & 0xFF));        if (e) return e;
	e = writec(self, (unsigned char)((s >> 8) & 0xFF)); if (e) return e;
	return DSK_ERR_OK;
}

static unsigned char st_masks[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

static dsk_err_t flipbits(SQ_COMPRESS_DATA *self)
{
	int n, bit;

	for (n = self->huf_nbits - 1; n >= 0; n--)
	{
		bit = (self->huf_bits[n / 8] & st_masks[n % 8]);

		if (bit) self->huf_out |= st_masks[self->huf_nout];
		++self->huf_nout;
		if (self->huf_nout == 8)
		{
			if (fputc(self->huf_out, self->fp_out) == EOF) 
				return DSK_ERR_SYSERR;
			self->huf_nout = 0;
			self->huf_out = 0;
		}
	}	
	return DSK_ERR_OK;
}


/* Add a bit to the output bitstream */
static void pushbit(SQ_COMPRESS_DATA *self, int bit)
{
	unsigned offs = self->huf_nbits / 8;
	unsigned char mask = st_masks[self->huf_nbits % 8];

	if (offs >= sizeof(self->huf_bits)) return;

	if (bit) self->huf_bits[offs] |= mask;
	else	 self->huf_bits[offs] &= ~mask;
	++self->huf_nbits;
}


static dsk_err_t huf_encode(SQ_COMPRESS_DATA *self, int ch)
{
	unsigned short nodepos;
	signed short child;

	if (ch < 0 || ch > SQ_EOF) return DSK_ERR_COMPRESS;
	/* Now find where this character ended up in the tree */
	nodepos = self->huf_leaves[ch];
	self->huf_nbits = 0;	

	child = (-1 - ch);

	do
	{
		if      (self->huf_node[nodepos].left  == child) pushbit(self, 0);
		else if (self->huf_node[nodepos].right == child) pushbit(self, 1);
		else return DSK_ERR_COMPRESS;

		child = nodepos;
		nodepos = self->huf_node[nodepos].parent;
	}
	while (nodepos < MAXNODE);
	/* self->huf_bits holds the output bitstream (in reverse order). 
	 * Now write it out to disc */
	return flipbits(self);	
}



static dsk_err_t compress(SQ_COMPRESS_DATA *self)
{
	char *s = self->sq_truename;
	dsk_err_t e;
	unsigned short dictbase;
	unsigned short dictlen;
	unsigned short dn;

	e = writes(self, MAGIC); if (e) return e;     /* Magic */
	e = writes(self, self->ck_sum);
				 if (e) return e;     /* Source file checksum */	
	do		/* Filename */
	{
		e = writec(self, *s); if (e) return e;
	} while ( *(s++) );	

	dictbase = self->huf_curnode + 1;
	/* Dictionary length = MAXNODE - dictbase */
	dictlen = MAXNODE - dictbase;
	e = writes(self, dictlen); if (e) return e;
	for (dn = dictbase; dn < MAXNODE; dn++)
	{
		short l = self->huf_node[dn].left;
		short r = self->huf_node[dn].right;

		if (l >= 0) l -= dictbase;
		if (r >= 0) r -= dictbase;

		e = writes(self, l);
		e = writes(self, r);
	}	
	/* Dictionary written. Start spitting bytes. */ 
	self->huf_nout = 0;
	self->huf_out  = 0;	
	rle_reset(self);
	e = rle_stream(self, huf_encode); if (e) return e;

	/* Write any pending bits */
	if (self->huf_nout)
	{
		if (fputc(self->huf_out, self->fp_out) == EOF) 
				return DSK_ERR_SYSERR;
	}
	fseek(self->fp_out, 2, SEEK_SET);
	
	return DSK_ERR_OK;
}

/* DEBUG CODE ONLY
static dsk_err_t putter(SQ_COMPRESS_DATA *self, int ch)
{
	if (fputc(ch, self->fp_out) == EOF) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
} */

static dsk_err_t squeeze(SQ_COMPRESS_DATA *self)
{
	dsk_err_t err;

	err = analyze(self);
	if (err) return err;
	err = compress(self);	

	return DSK_ERR_OK;
}


static dsk_err_t readc(SQ_COMPRESS_DATA *self, unsigned char *c)
{
	int i;

	i = fgetc(self->fp_in); 
	if (i == EOF) return DSK_ERR_SYSERR;
	*c = i;
	return DSK_ERR_OK;
}


static dsk_err_t readu(SQ_COMPRESS_DATA *self, unsigned short *u)
{
	unsigned char c1, c2;
	dsk_err_t err;
	
	err = readc(self, &c1); if (err) return err;
	err = readc(self, &c2); if (err) return err;

	*u = ((c2 << 8) | c1);
	return DSK_ERR_OK;
}



static dsk_err_t reads(SQ_COMPRESS_DATA *self, signed short *s)
{
	unsigned char c1, c2;
	dsk_err_t err;
	
	err = readc(self, &c1); if (err) return err;
	err = readc(self, &c2); if (err) return err;

	*s = ((c2 << 8) | c1);
	return DSK_ERR_OK;
}

static dsk_err_t ckputc(SQ_COMPRESS_DATA *self, unsigned char b)
{
	if (fputc(b, self->fp_out) == EOF) return DSK_ERR_SYSERR;
	self->ck_sum += b;
	return DSK_ERR_OK;
}

static dsk_err_t wrbyte(SQ_COMPRESS_DATA *self, unsigned char b)
{
	int n;
	dsk_err_t e;

	if (self->rle_run == 1)
	{
		/* Last byte was 0x90. The one before that is self->rle_char */
		if (b == 0) 		
		{
			/* xx 90 00 becomes xx 90 */
			if (self->rle_char != -1)
			{
				e = ckputc(self, (unsigned char)(self->rle_char)); if (e) return e;
			}
			e = ckputc(self, RLECODE); if (e) return e;
		}
		else
		{
			if (self->rle_char == -1) return DSK_ERR_SYSERR;
			for (n = 0; n < b; n++)
			{
				e = ckputc(self, (unsigned char)(self->rle_char)); if (e) return e;
			}
		}
		self->rle_run = 0;
		self->rle_char = -1;
		return DSK_ERR_OK;
	}

	if (b == RLECODE)
	{
		/* Retain rle_char */
		self->rle_run = 1;
		return DSK_ERR_OK;
	}
	/* No special characters */
	if (self->rle_char != -1)
	{
		e = ckputc(self, (unsigned char)(self->rle_char)); if (e) return e;
	}
	self->rle_char = b;
	
	return DSK_ERR_OK;
}



static dsk_err_t unsqueeze(SQ_COMPRESS_DATA *self)
{
	dsk_err_t err;
	unsigned short magic, checksum;
	unsigned char c;
	unsigned short dictlen;
	unsigned short nd;
	unsigned short node;
	signed short code;
	
	self->ck_sum = 0;
	err = readu(self, &magic); 
	if (err) return err;
	if (magic != MAGIC) return DSK_ERR_COMPRESS;
	err = readu(self, &checksum);
	/* Filename (skip) */
	do
	{	
		err = readc(self, &c);
		if (err) return err;
	} while (c);
	/* Dictionary length */
	err = readu(self, &dictlen); 
	if (err) return err;
	if (dictlen > MAXNODE) return DSK_ERR_COMPRESS;
	/* Dictionary */
	for (nd = 0; nd < dictlen; nd++)
	{
		err = reads(self, &self->huf_node[nd].left); 
		if (err) return err;
		err = reads(self, &self->huf_node[nd].right); 
		if (err) return err;
	}
	/* Now start decoding data */
	self->huf_out = 0;
	self->huf_nout = 8;
	node = 0;
	rle_reset(self);
/* if dictlen == 0, the file is empty */
	if (dictlen) while (1)
	{
		if (self->huf_nout == 8)
		{
			self->huf_nout = 0;
			err = readc(self, &self->huf_out);
			if (err) return err;	
		}
		code = self->huf_out & st_masks[self->huf_nout];
		++self->huf_nout;
		if (code) code = self->huf_node[node].right;
		else	  code = self->huf_node[node].left;

		if (code < 0)
		{
			if (-1 - code == SQ_EOF) break;	/* Reached EOF */
			err = wrbyte(self, (unsigned char)(-1 - code));
			if (err) return err;
			node = 0;
		}
		else node = code;
	}
	if (self->rle_char != -1)
	{
		err = ckputc(self, (unsigned char)(self->rle_char)); if (err) return err;

	}
	if (checksum != self->ck_sum) return DSK_ERR_COMPRESS;
	return DSK_ERR_OK;
}


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

COMPRESS_CLASS cc_sq = 
{
	sizeof(SQ_COMPRESS_DATA),
	"sq",
	"Squeeze (Huffman coding)",
	sq_open,	/* open */
	sq_creat,	/* create new */
	sq_commit,	/* commit */
	sq_abort	/* abort */
};



dsk_err_t sq_open(COMPRESS_DATA *self)
{
	SQ_COMPRESS_DATA *sq_self;
	dsk_err_t err;
	unsigned short magic;

	
	/* Sanity check: Is this meant for our driver? */
	if (self->cd_class != &cc_sq) return DSK_ERR_BADPTR;
	sq_self = (SQ_COMPRESS_DATA *)self;
	sq_self->sq_truename = NULL;
	sq_self->fp_in = NULL;
	sq_self->fp_out = NULL; 

	/* Open the file to decompress */
	err = comp_fopen(self, &sq_self->fp_in);
	if (err) return DSK_ERR_NOTME;

	/* Check for SQ signature */
	err = readu(sq_self, &magic);
	if (err) 	/* v1.1.11 Don't leak file handles */
	{
		fclose(sq_self->fp_in);
		return err;
	}

	if (magic != MAGIC)
	{
		fclose(sq_self->fp_in);
		return DSK_ERR_NOTME;
	}
	/* OK. This is a Squeezed file. Decompress it. */
	rewind(sq_self->fp_in);
	err = comp_mktemp(self, &sq_self->fp_out);

	if (!err) err = unsqueeze(sq_self);
	fclose(sq_self->fp_in);
	if (sq_self->fp_out) fclose(sq_self->fp_out);
	if (err && sq_self->sq_truename) free(sq_self->sq_truename);
	return err;	
}


dsk_err_t sq_commit(COMPRESS_DATA *self)
{
	SQ_COMPRESS_DATA *sq_self;
	dsk_err_t err = DSK_ERR_OK;
	
	if (self->cd_class != &cc_sq) return DSK_ERR_BADPTR;
	sq_self = (SQ_COMPRESS_DATA *)self;

	sq_self->fp_in = NULL;
	sq_self->fp_out = NULL;
        if (self->cd_cfilename && self->cd_ufilename)
        {
		sq_self->fp_in  = fopen(self->cd_ufilename, "rb");
		sq_self->fp_out = fopen(self->cd_cfilename, "wb");
		if (!sq_self->fp_in || !sq_self->fp_out) err = DSK_ERR_SYSERR;
		else err = squeeze(sq_self);
	}
	if (sq_self->fp_in) fclose(sq_self->fp_in);
	if (sq_self->fp_out) fclose(sq_self->fp_out);
	if (sq_self->sq_truename) free(sq_self->sq_truename);
	sq_self->sq_truename = NULL;

	return err;
}


/* abort: Free truename */
dsk_err_t sq_abort(COMPRESS_DATA *self)
{
	SQ_COMPRESS_DATA *sq_self;
	if (self->cd_class != &cc_sq) return DSK_ERR_BADPTR;
	sq_self = (SQ_COMPRESS_DATA *)self;

	if (sq_self->sq_truename) free(sq_self->sq_truename);
	sq_self->sq_truename = NULL;
	return DSK_ERR_OK;
}

/* Create: Set up truename */
dsk_err_t sq_creat(COMPRESS_DATA *self)
{
	char *ss;
	SQ_COMPRESS_DATA *sq_self;
	if (self->cd_class != &cc_sq) return DSK_ERR_BADPTR;
	sq_self = (SQ_COMPRESS_DATA *)self;

	if (sq_self->sq_truename) free(sq_self->sq_truename);
	sq_self->sq_truename = NULL;

/* Try to guess the true name of the file from the compressed name. This 
 * can only be done for certain well-known name manglings. */
        sq_self->sq_truename = malloc(1 + strlen(self->cd_cfilename));
        if (!sq_self->sq_truename) return DSK_ERR_NOMEM;

	/* UNIX SQ squeezes files by appending ".SQ" */
        strcpy(sq_self->sq_truename, self->cd_cfilename);
	ss = strstr(sq_self->sq_truename, ".SQ");
	if (ss) *ss = 0;

	/* Convert .DQK back to .DSK */
	ss = strstr(sq_self->sq_truename, ".DQK");
	if (ss) memcpy(ss, ".DSK", 4);

	/* Convert .DQK back to .DSK */
	ss = strstr(sq_self->sq_truename, ".dqk");
	if (ss) memcpy(ss, ".dsk", 4);

	return DSK_ERR_OK;
}



