/*
      LIBDSK: General floppy and diskimage access library                 
      Copyright (C) 2002, 2014  John Elliott <seasip.webmaster@gmail.com>    
 
      LZH decompression code based on tdlzhuf.c / tdcrc.c 
      (both from wteledsk by Will Kranz, GPLv2. Permission to 
      relicense these source files under LGPLv2 was granted by
      email).
      All functions made static. All static global variables moved 
      into the TLZH_COMPRESS_DATA structure.

      tdlzhuf.c  module to perform lzss-huffman decompression
      as is used in teledisk.exe
      Conditionally compiled main to convert *.td0 advanced 
      compression file back normal compression file.
      derived from lzhuf.c 1.0 per below 

      This library is free software; you can redistribute it and/or         
      modify it under the terms of the GNU Library General Public           
      License as published by the Free Software Foundation; either          
      version 2 of the License, or (at your option) any later version.      
                                                                            
      This library is distributed in the hope that it will be useful,       
      but WITHOUT ANY WARRANTY; without even the implied warranty of        
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     
      Library General Public License for more details.                      
                                                                            
      You should have received a copy of the GNU Library General Public     
      License along with this library; if not, write to the Free            
      Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,       
      MA 02111-1307, USA                                                    

      11/10/02 this was working with struct tdlzhuf * passed to
      both Decode() and init_Decode() as first arg.  Save this as
      tdlzhuf1.c and then make this structure locally static and try
      to switch to unsigned shorts where it matters so works in linux.

Started with program below: 
 * LZHUF.C English version 1.0
 * Based on Japanese version 29-NOV-1988
 * LZSS coded by Haruhiko OKUMURA
 * Adaptive Huffman Coding coded by Haruyasu YOSHIZAKI
 * Edited and translated to English by Kenji RIKITAKE

In summary changes by WTK:
  wrote a new conditionally compiled main() 
  remove Encode() modules and arrays
  make remaing arrays and variables static to hide from external modules
  add struct tdlzhuf to provide state retension between calls to Decode()
  change from fgetc(FILE *) to read(int fp) so read
     a block at a time into an input buffer.  Now the
     Decode() routine can be called instead of read()
     by a user, ie from wteledisk.c
  change size of data elements for Linux, 
     int -> short
     unsigned int -> unsigned short

*/
#include "compi.h"
#include "comptlzh.h"

unsigned short teledisk_crc(unsigned char *b, unsigned short len);

#if 0 /* These functions are not called; presumably they would be used for 
         LZHUF compression were LibDsk to implement that */

/* Initializing tree */
static void InitTree(TLZH_COMPRESS_DATA *self)  
{
    int  i;

    for (i = N + 1; i <= N + 256; i++)
        self->rson[i] = NIL;            /* root */
    for (i = 0; i < N; i++)
        self->dad[i] = NIL;            /* node */
}

/* Inserting node to the tree */
static void InsertNode(TLZH_COMPRESS_DATA *self, int r)  
{
    int  i, p, cmp;
    unsigned char  *key;
    unsigned c;

    cmp = 1;
    key = &self->text_buf[r];
    p = N + 1 + key[0];
    self->rson[r] = self->lson[r] = NIL;
    self->match_length = 0;
    for ( ; ; ) {
        if (cmp >= 0) {
            if (self->rson[p] != NIL)
                p = self->rson[p];
            else {
                self->rson[p] = r;
                self->dad[r] = p;
                return;
            }
        } else {
            if (self->lson[p] != NIL)
                p = self->lson[p];
            else {
                self->lson[p] = r;
                self->dad[r] = p;
                return;
            }
        }
        for (i = 1; i < F; i++)
            if ((cmp = key[i] - self->text_buf[p + i]) != 0)
                break;
        if (i > THRESHOLD) {
            if (i > self->match_length) {
                self->match_position = ((r - p) & (N - 1)) - 1;
                if ((self->match_length = i) >= F)
                    break;
            }
            if (i == self->match_length) {
                if ((c = ((r - p) & (N - 1)) - 1) < self->match_position) {
                    self->match_position = c;
                }
            }
        }
    }
    self->dad[r] = self->dad[p];
    self->lson[r] = self->lson[p];
    self->rson[r] = self->rson[p];
    self->dad[self->lson[p]] = r;
    self->dad[self->rson[p]] = r;
    if (self->rson[self->dad[p]] == p)
        self->rson[self->dad[p]] = r;
    else
        self->lson[self->dad[p]] = r;
    self->dad[p] = NIL;  /* remove p */
}

static void DeleteNode(TLZH_COMPRESS_DATA *self, int p)  /* Deleting node from the tree */
{
    int  q;

    if (self->dad[p] == NIL)
        return;            /* unregistered */
    if (self->rson[p] == NIL)
        q = self->lson[p];
    else
    if (self->lson[p] == NIL)
        q = self->rson[p];
    else {
        q = self->lson[p];
        if (self->rson[q] != NIL) {
            do {
                q = self->rson[q];
            } while (self->rson[q] != NIL);
            self->rson[self->dad[q]] = self->lson[q];
            self->dad[self->lson[q]] = self->dad[q];
            self->lson[q] = self->lson[p];
            self->dad[self->lson[p]] = q;
        }
        self->rson[q] = self->rson[p];
        self->dad[self->rson[p]] = q;
    }
    self->dad[q] = self->dad[p];
    if (self->rson[self->dad[p]] == p)
        self->rson[self->dad[p]] = q;
    else
        self->lson[self->dad[p]] = q;
    self->dad[p] = NIL;
}
#endif

/*
 * Tables for encoding/decoding upper 6 bits of
 * sliding dictionary pointer
 */

/* decoder table */
static const unsigned char d_code[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
    0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
    0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
    0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
    0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
    0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
    0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

static const unsigned char d_len[256] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};


/* this is old code duplicated in GetBit() and GetByte()
    while (getlen <= 8) {
        if ((i = getc(infile)) < 0) i = 0;
        getbuf |= i << (8 - getlen);
        getlen += 8;
    }

replace this with next_word() routine that handles block read
*/
static int next_word(TLZH_COMPRESS_DATA *self)
{
    if(self->ibufndx >= self->ibufcnt)
    {
        self->ibufndx = 0;
        self->ibufcnt = fread(self->inbuf,1, TLZH_BUFSZ, self->fp_in);
        if(self->ibufcnt <= 0)
            return(-1);
    }
    while (self->getlen <= 8) { // typically reads a word at a time
        self->getbuf |= self->inbuf[self->ibufndx++] << (8 - self->getlen);
        self->getlen += 8;
    }
    return(0);
}


static int GetBit(TLZH_COMPRESS_DATA *self)    /* get one bit */
{
    short i;
    if(next_word(self) < 0)
        return(-1);
    i = self->getbuf;
    self->getbuf <<= 1;
    self->getlen--;
        if(i < 0)
        return(1);
    else
        return(0);
}

static int GetByte(TLZH_COMPRESS_DATA *self)    /* get a byte */
{
    unsigned short i;
        if(next_word(self) != 0)
        return(-1);
    i = self->getbuf;
    self->getbuf <<= 8;
    self->getlen -= 8;
        i = i >> 8;
    return((int) i);
}



/* initialize freq tree */

static void StartHuff(TLZH_COMPRESS_DATA *self)
{
    int i, j;

    for (i = 0; i < TLZH_NCHAR; i++) {
        self->freq[i] = 1;
        self->son[i] = i + TLZH_T;
        self->prnt[i + TLZH_T] = i;
    }
    i = 0; j = TLZH_NCHAR;
    while (j <= TLZH_R) {
        self->freq[j] = self->freq[i] + self->freq[i + 1];
        self->son[j] = i;
        self->prnt[i] = self->prnt[i + 1] = j;
        i += 2; j++;
    }
    self->freq[TLZH_T] = 0xffff;
    self->prnt[TLZH_R] = 0;
}


/* reconstruct freq tree */

static void reconst(TLZH_COMPRESS_DATA *self)
{
    short i, j, k;
    unsigned short f, l;

    /* halven cumulative freq for leaf nodes */
    j = 0;
    for (i = 0; i < TLZH_T; i++) {
        if (self->son[i] >= TLZH_T) {
            self->freq[j] = (self->freq[i] + 1) / 2;
            self->son[j] = self->son[i];
            j++;
        }
    }
    /* make a tree : first, connect children nodes */
    for (i = 0, j = TLZH_NCHAR; j < TLZH_T; i += 2, j++) {
        k = i + 1;
        f = self->freq[j] = self->freq[i] + self->freq[k];
        for (k = j - 1; f < self->freq[k]; k--);
        k++;
        l = (j - k) * 2;
        
        /* movmem() is Turbo-C dependent
           rewritten to memmove() by Kenji */
        
        /* movmem(&freq[k], &freq[k + 1], l); */
        (void)memmove(&self->freq[k + 1], &self->freq[k], l);
        self->freq[k] = f;
        /* movmem(&son[k], &son[k + 1], l); */
        (void)memmove(&self->son[k + 1], &self->son[k], l);
        self->son[k] = i;
    }
    /* connect parent nodes */
    for (i = 0; i < TLZH_T; i++) {
        if ((k = self->son[i]) >= TLZH_T) {
            self->prnt[k] = i;
        } else {
            self->prnt[k] = self->prnt[k + 1] = i;
        }
    }
}


/* update freq tree */

static void update(TLZH_COMPRESS_DATA *self, int c)
{
    int i, j, k, l;

    if (self->freq[TLZH_R] == TLZH_MAX_FREQ) {
        reconst(self);
    }
    c = self->prnt[c + TLZH_T];
    do {
        k = ++self->freq[c];

        /* swap nodes to keep the tree freq-ordered */
        if (k > self->freq[l = c + 1]) {
            while (k > self->freq[++l]);
            l--;
            self->freq[c] = self->freq[l];
            self->freq[l] = k;

            i = self->son[c];
            self->prnt[i] = l;
            if (i < TLZH_T) self->prnt[i + 1] = l;

            j = self->son[l];
            self->son[l] = i;

            self->prnt[j] = c;
            if (j < TLZH_T) self->prnt[j + 1] = c;
            self->son[c] = j;

            c = l;
        }
    } while ((c = self->prnt[c]) != 0);    /* do it until reaching the root */
}


static short DecodeChar(TLZH_COMPRESS_DATA *self)
{
    int ret;
    unsigned short c;

    c = self->son[TLZH_R];

    /*
     * start searching tree from the root to leaves.
     * choose node #(son[]) if input bit == 0
     * else choose #(son[]+1) (input bit == 1)
     */
    while (c < TLZH_T) {
                if((ret = GetBit(self)) < 0)
            return(-1);
        c += (unsigned) ret;
        c = self->son[c];
    }
    c -= TLZH_T;
    update(self, c);
    return c;
}

static short DecodePosition(TLZH_COMPRESS_DATA *self)
{
    short bit;
    unsigned short i, j, c;

    /* decode upper 6 bits from given table */
        if((bit=GetByte(self)) < 0)
        return(-1);
    i = (unsigned short) bit;
    c = (unsigned short)d_code[i] << 6;
    j = d_len[i];

    /* input lower 6 bits directly */
    j -= 2;
    while (j--) {
                if((bit = GetBit(self)) < 0)
                     return(-1);
        i = (i << 1) + bit;
    }
    return(c | (i & 0x3f));
}

/* DeCompression 

split out initialization code to init_Decode()

*/

static void init_Decode(TLZH_COMPRESS_DATA *self)
{
	int i;

	self->ibufcnt= self->ibufndx = 0; // input buffer is empty
	self->bufcnt = 0;
	StartHuff(self);
	for (i = 0; i < TLZH_N - TLZH_F; i++)
	{
		self->text_buf[i] = ' ';
	}
	self->r = TLZH_N - TLZH_F;
}


static int Decode(TLZH_COMPRESS_DATA *self, unsigned char *buf, int len)
  /* Decoding/Uncompressing */
{
    short c,pos;
    int  count;  // was an unsigned long, seems unnecessary
    for (count = 0; count < len; ) {
            if(self->bufcnt == 0) {
                if((c = DecodeChar(self)) < 0)
                    return(count); // fatal error
                if (c < 256) {
                    *(buf++) = (unsigned char)c;
                    self->text_buf[self->r++] = (unsigned char)c;
                    self->r &= (TLZH_N - 1);
                    count++;                
                } 
                else {
                    if((pos = DecodePosition(self)) < 0)
                           return(count); // fatal error
                    self->bufpos = (self->r - pos - 1) & (TLZH_N - 1);
                    self->bufcnt = c - 255 + TLZH_THRESHOLD;
                    self->bufndx = 0;
                 }
            }
            else { // still chars from last string
                while( self->bufndx < self->bufcnt && count < len ) {
                    c = self->text_buf[(self->bufpos + self->bufndx) & (TLZH_N - 1)];
                    *(buf++) = (unsigned char)c;
                    self->bufndx++;
                    self->text_buf[self->r++] = (unsigned char)c;
                    self->r &= (TLZH_N - 1);
                    count++;
                }
                // reset bufcnt after copy string from text_buf[]
                if(self->bufndx >= self->bufcnt) 
                    self->bufndx = self->bufcnt = 0;
        }
    }
    return(count); // count == len, success
}


/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

COMPRESS_CLASS cc_tlzh = 
{
        sizeof(TLZH_COMPRESS_DATA),
        "tdlzh",
        "TeleDisk advanced compression",
        tlzh_open,        /* open */
        tlzh_creat,       /* create new */
        tlzh_commit,      /* commit */
        tlzh_abort        /* abort */
};



static dsk_err_t uncompress(TLZH_COMPRESS_DATA *self)
{
	unsigned char head[12];
	int rd;
	long off=0;
	unsigned short crc=0;

	/* Load the header */
	if (fread(head, 1, sizeof(head), self->fp_in) != sizeof(head))
	{
		return DSK_ERR_SYSERR;
	}
	off = sizeof(head);
	init_Decode(self);

	/* Change the 'compressed' signature to 'uncompressed' */
	head[0] = 'T';
	head[1] = 'D'; 

	/* And update the CRC */
	crc = teledisk_crc(head,10);
	head[10] = crc & 0xff;
	head[11] = (crc >> 8) & 0xff;

	/* Write out the new header */
	if(fwrite(head, 1, sizeof(head), self->fp_out) != sizeof(head))
	{
		return DSK_ERR_SYSERR;
	}
	/* Expand the compressed contents */
	do
	{
		if((rd = Decode(self, self->obuf,TLZH_BUFSZ)) > 0)
		{
			if (fwrite(self->obuf, 1, rd, self->fp_out) < (unsigned)rd) 
				return DSK_ERR_SYSERR;
		}
		off += rd;
	}  while(rd == TLZH_BUFSZ);

/* This is a decompress-only driver, so set the read-only flag in the 
 * compression structure */
	self->tlzh_super.cd_readonly = 1;
	return DSK_ERR_OK;
}


/* Expand a compressed ('td') file to uncompressed ('TD') */
dsk_err_t tlzh_open(COMPRESS_DATA *self)
{
	TLZH_COMPRESS_DATA *tlzh_self;
	dsk_err_t err;
	unsigned char magic[12];

	/* Sanity check: Is this meant for our driver? */
	if (self->cd_class != &cc_tlzh) return DSK_ERR_BADPTR;
	tlzh_self = (TLZH_COMPRESS_DATA *)self;
	tlzh_self->fp_in = NULL;
	tlzh_self->fp_out = NULL; 

		/* Open the file to decompress */
		err = comp_fopen(self, &tlzh_self->fp_in);
		if (err) return DSK_ERR_NOTME;

	/* Check for magic number: "td\0" */
	/* Check header CRC */
	if (fread(magic, 1, 12, tlzh_self->fp_in) < 12 ||
		memcmp(magic, "td", 3) ||
	        magic[10] + 256 * magic[11] != teledisk_crc(magic, 10))
	{
		fclose(tlzh_self->fp_in);
		return DSK_ERR_NOTME;
	}

	/* OK. This looks like an advanced Teledisk file. Decompress it to a 
	 * normal Teledisk file. */
	rewind(tlzh_self->fp_in);
	err = comp_mktemp(self, &tlzh_self->fp_out);
	if (!err) err = uncompress(tlzh_self);

	if (tlzh_self->fp_out) fclose(tlzh_self->fp_out);
	fclose(tlzh_self->fp_in);

	return err;
}	

dsk_err_t tlzh_creat(COMPRESS_DATA *self)
{
	return DSK_ERR_RDONLY;
}

dsk_err_t tlzh_commit(COMPRESS_DATA *self)
{
	return DSK_ERR_RDONLY;
}

dsk_err_t tlzh_abort(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}

/* Wholesale inclusion of tdcrc.c */

/*  tdcrc.c - a crc routine to mimic tdcheck.exe

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    http://www.gnu.org/licenses/gpl.txt


    10/19/02 got it working!
    Contains conditionals to build a test program
    or just use as a module.
    Define DOMAIN if *.exe is desired
    if not defined get a linkable crc module   
*/

static unsigned char  table[] = {
0x00,0xa0,0xe1,0x41,0x63,0xc3,0x82,0x22,0xc7,0x67,0x26,0x86,0xa4,0x04,0x45,0xe5,
0x2f,0x8f,0xce,0x6e,0x4c,0xec,0xad,0x0d,0xe8,0x48,0x09,0xa9,0x8b,0x2b,0x6a,0xca,
0x5e,0xfe,0xbf,0x1f,0x3d,0x9d,0xdc,0x7c,0x99,0x39,0x78,0xd8,0xfa,0x5a,0x1b,0xbb,  
0x71,0xd1,0x90,0x30,0x12,0xb2,0xf3,0x53,0xb6,0x16,0x57,0xf7,0xd5,0x75,0x34,0x94,  
0xbc,0x1c,0x5d,0xfd,0xdf,0x7f,0x3e,0x9e,0x7b,0xdb,0x9a,0x3a,0x18,0xb8,0xf9,0x59,  
0x93,0x33,0x72,0xd2,0xf0,0x50,0x11,0xb1,0x54,0xf4,0xb5,0x15,0x37,0x97,0xd6,0x76, 
0xe2,0x42,0x03,0xa3,0x81,0x21,0x60,0xc0,0x25,0x85,0xc4,0x64,0x46,0xe6,0xa7,0x07,  
0xcd,0x6d,0x2c,0x8c,0xae,0x0e,0x4f,0xef,0x0a,0xaa,0xeb,0x4b,0x69,0xc9,0x88,0x28,  

0xd8,0x78,0x39,0x99,0xbb,0x1b,0x5a,0xfa,0x1f,0xbf,0xfe,0x5e,0x7c,0xdc,0x9d,0x3d,
0xf7,0x57,0x16,0xb6,0x94,0x34,0x75,0xd5,0x30,0x90,0xd1,0x71,0x53,0xf3,0xb2,0x12,
0x86,0x26,0x67,0xc7,0xe5,0x45,0x04,0xa4,0x41,0xe1,0xa0,0x00,0x22,0x82,0xc3,0x63,
0xa9,0x09,0x48,0xe8,0xca,0x6a,0x2b,0x8b,0x6e,0xce,0x8f,0x2f,0x0d,0xad,0xec,0x4c,
0x64,0xc4,0x85,0x25,0x07,0xa7,0xe6,0x46,0xa3,0x03,0x42,0xe2,0xc0,0x60,0x21,0x81,
0x4b,0xeb,0xaa,0x0a,0x28,0x88,0xc9,0x69,0x8c,0x2c,0x6d,0xcd,0xef,0x4f,0x0e,0xae,
0x3a,0x9a,0xdb,0x7b,0x59,0xf9,0xb8,0x18,0xfd,0x5d,0x1c,0xbc,0x9e,0x3e,0x7f,0xdf,
0x15,0xb5,0xf4,0x54,0x76,0xd6,0x97,0x37,0xd2,0x72,0x33,0x93,0xb1,0x11,0x50,0xf0,

0x00,0x97,0xb9,0x2e,0xe5,0x72,0x5c,0xcb,0xca,0x5d,0x73,0xe4,0x2f,0xb8,0x96,0x01,
0x03,0x94,0xba,0x2d,0xe6,0x71,0x5f,0xc8,0xc9,0x5e,0x70,0xe7,0x2c,0xbb,0x95,0x02,
0x06,0x91,0xbf,0x28,0xe3,0x74,0x5a,0xcd,0xcc,0x5b,0x75,0xe2,0x29,0xbe,0x90,0x07,
0x05,0x92,0xbc,0x2b,0xe0,0x77,0x59,0xce,0xcf,0x58,0x76,0xe1,0x2a,0xbd,0x93,0x04,
0x0c,0x9b,0xb5,0x22,0xe9,0x7e,0x50,0xc7,0xc6,0x51,0x7f,0xe8,0x23,0xb4,0x9a,0x0d,
0x0f,0x98,0xb6,0x21,0xea,0x7d,0x53,0xc4,0xc5,0x52,0x7c,0xeb,0x20,0xb7,0x99,0x0e,
0x0a,0x9d,0xb3,0x24,0xef,0x78,0x56,0xc1,0xc0,0x57,0x79,0xee,0x25,0xb2,0x9c,0x0b,
0x09,0x9e,0xb0,0x27,0xec,0x7b,0x55,0xc2,0xc3,0x54,0x7a,0xed,0x26,0xb1,0x9f,0x08,

0x8f,0x18,0x36,0xa1,0x6a,0xfd,0xd3,0x44,0x45,0xd2,0xfc,0x6b,0xa0,0x37,0x19,0x8e,
0x8c,0x1b,0x35,0xa2,0x69,0xfe,0xd0,0x47,0x46,0xd1,0xff,0x68,0xa3,0x34,0x1a,0x8d,
0x89,0x1e,0x30,0xa7,0x6c,0xfb,0xd5,0x42,0x43,0xd4,0xfa,0x6d,0xa6,0x31,0x1f,0x88,
0x8a,0x1d,0x33,0xa4,0x6f,0xf8,0xd6,0x41,0x40,0xd7,0xf9,0x6e,0xa5,0x32,0x1c,0x8b,
0x83,0x14,0x3a,0xad,0x66,0xf1,0xdf,0x48,0x49,0xde,0xf0,0x67,0xac,0x3b,0x15,0x82,
0x80,0x17,0x39,0xae,0x65,0xf2,0xdc,0x4b,0x4a,0xdd,0xf3,0x64,0xaf,0x38,0x16,0x81,
0x85,0x12,0x3c,0xab,0x60,0xf7,0xd9,0x4e,0x4f,0xd8,0xf6,0x61,0xaa,0x3d,0x13,0x84,
0x86,0x11,0x3f,0xa8,0x63,0xf4,0xda,0x4d,0x4c,0xdb,0xf5,0x62,0xa9,0x3e,0x10,0x87
};


// basically same logic as below, but doesn't initialize crc so can append
void updt_crc(unsigned short *crc,unsigned char *b, unsigned short len)
{
   unsigned char tmp;
   while(len-- != 0)
   {
      tmp = *b ^ (*crc >> 8);
      *crc = ((table[tmp] ^ (*crc & 0xff)) << 8) + table[tmp+0x100];
      b++;
   }

}

// let unsigned char *b point to start of buffer
// this initializes crc, then does updt_crc logic
unsigned short teledisk_crc(unsigned char *b, unsigned short len)
{
   unsigned char tmp;
   unsigned short i,crc = 0;
   for(i=0;i<len;i++,b++)
   {
      tmp = *b ^ (crc >> 8);
      crc = ((table[tmp] ^ (crc & 0xff)) << 8) + table[tmp+0x100];
   }
   return(crc);
}

#if 0
#ifndef MSDOS
#define O_BINARY 0      // not defined for Linux gcc
#define strnicmp strncasecmp
#endif

#define BLKSZ 256

main(int argc,char *argv[])
{  int rd,fp=EOF;
   unsigned char buf[BLKSZ];
   unsigned short crc=0;
   long start = -1,end = -1,off = 0,bytes = 0;
   if(argc < 2)
       printf("\nusage: tdcrc <filename> [-s#] [-e#]");
   else if((fp = open(argv[1],O_RDONLY|O_BINARY)) == EOF)
       printf("\nfailed to open %s",argv[1]);
   else 
   {
       for(rd=2;rd < argc;rd++)
       {
           if(strnicmp(argv[rd],"-s",2) == 0 &&
              sscanf(argv[rd]+2,"%lx",&start) == 1)
           {
              printf("\nstart offset 0x%lx",start);
              if(lseek(fp,start,SEEK_SET) != start)
              {
                   printf("  -- seek failed!");
                   start = -1;
              }
              else
                   off = start;
           }
           if(strnicmp(argv[rd],"-e",2) == 0 &&
              sscanf(argv[rd]+2,"%lx",&end) == 1)
              printf("\nend offset 0x%lx",end);
       }
       while(1)
       {
          rd = read(fp,buf,BLKSZ);
          if(end > 0 && off + rd > end)
              rd = end - off;
          if(rd <= 0)
              break;
          updt_crc(&crc,buf,rd);
          bytes += rd;
          off += rd;
       }
   }
   if(fp != EOF)
   {
       printf("\nfile crc = 0x%x  = %u",crc,crc);
       printf("\nread %d bytes",bytes);
       close(fp);
   }
   putchar('\n');
   
}

#endif


