/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2002, 2014  John Elliott <seasip.webmaster@gmail.com>      *
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

/* LZHUF (as used in Teledisk). Decompression only, no compression. */
/* Based on tdlzhuf.c by Will Kranz, released under GPLv2. Permission to 
 * relicense this source file under LGPLv2 was granted by email.
 *
 * Rewritten to hold decoder state in the TLZH_COMPRESS_DATA structure 
 * rather than static variables. */

/******************************* compsq.h **********************************/

/* Buffer size */
#define TLZH_BUFSZ 512
/* LZSS Parameters */
#define TLZH_N         4096    /* Size of string buffer */
#define TLZH_F           60    /* Size of look-ahead buffer */
#define TLZH_THRESHOLD    2
#define TLZH_NIL     TLZH_N    /* End of tree's node  */
/* Huffman coding parameters */

#define TLZH_NCHAR      (256 - TLZH_THRESHOLD + TLZH_F)
                /* character code (= 0..TLZH_NCHAR-1) */
#define TLZH_T         (TLZH_NCHAR * 2 - 1)    /* Size of table */
#define TLZH_R         (TLZH_T - 1)            /* root position */
#define TLZH_MAX_FREQ    0x8000
                    /* update when cumulative frequency */
                    /* reaches to this value */



typedef struct
{
        COMPRESS_DATA tlzh_super;

	FILE *fp_in;
	FILE *fp_out;

	unsigned short r, bufcnt,bufndx,bufpos,  // string buffer
         // the following to allow block reads from input in next_word()
        ibufcnt,ibufndx; // input buffer counters
	unsigned char  inbuf[TLZH_BUFSZ];    // input buffer
	unsigned char  obuf[TLZH_BUFSZ];	// output buffer
	
	unsigned char text_buf[TLZH_N + TLZH_F - 1];
	short    match_position, match_length;
/* Tree structure, held as 3 arrays: children on left, children on right, 
   parents */
	short    lson[TLZH_N + 1], rson[TLZH_N + 257], dad[TLZH_N + 1];
/* Huffman data */
	unsigned short freq[TLZH_T + 1];    /* cumulative freq table */

/*
 * pointing parent nodes.
 * area [T..(T + TLZH_NCHAR - 1)] are pointers for leaves
 */
	short prnt[TLZH_T + TLZH_NCHAR];

/* pointing children nodes (son[], son[] + 1)*/
	short son[TLZH_T];

	unsigned short getbuf /*= 0*/;
	unsigned char getlen /*= 0*/;



} TLZH_COMPRESS_DATA;


extern COMPRESS_CLASS cc_tlzh;


dsk_err_t tlzh_open(COMPRESS_DATA *self);
dsk_err_t tlzh_creat(COMPRESS_DATA *self);
dsk_err_t tlzh_commit(COMPRESS_DATA *self);
dsk_err_t tlzh_abort(COMPRESS_DATA *self);

