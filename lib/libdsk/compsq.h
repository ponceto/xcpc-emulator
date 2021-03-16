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

/* Squeeze (Huffman) compression/decompression. */

/******************************* compsq.h **********************************/

#define RLECODE 0x90
#define SQ_EOF  0x100	/* EOF as encoded by Huffman coding */
/* sq.txt states that according to Huffman theory the total number of nodes 
 * will not exceed 2 * number of leaf nodes. 
 */
#define MAXLEAF 0x0101	/* 256 characters + EOF */
#define MAXNODE 0x0202	/* Total size of tree */
#define MAGIC 0xFF76	/* SQ file magic */

typedef struct
{
        COMPRESS_DATA sq_super;
        char *sq_truename;

	int rle_char;	/* Last RLE character */
	int rle_run;	/* Length of current run */

/* The Huffman dictionary. sq.txt states that according to Huffman theory
 * there will never be more than 2*MAXNODE nodes needed. 
 * The format of the two shorts in the node is:
 * +ve: Branch to another node.
 * -ve: Leaf. 
 * The "count" holds the number of times this node will be traversed - 
 * eg, 15 if there are 10 instances of the left-hand leaf and 5 of the right. */
	struct
	{
		signed short parent;
		signed short left;
		signed short right;
	} huf_node [MAXNODE];
/* Character frequencies. The first MAXNODE entries are used for the 
 * tree as it is built; the last MAXLEAF for character entries that 
 * haven't yet been incorporated into the tree. */ 
	unsigned long  huf_count[MAXNODE + MAXLEAF];
	unsigned short huf_curnode;	/* Next unused node */
	unsigned short huf_nodecount;	/* Number of leaf nodes */
/* Mapping of characters to leaf nodes */
	unsigned short huf_leaves[MAXLEAF];
/* Bitstream being generated */
	unsigned char huf_bits[65];
	int huf_nbits;
/* Bitstream being written */
	unsigned char huf_out;
	int huf_nout;

	unsigned short ck_sum;		/* Checksum of source file */
	
	FILE *fp_in;
	FILE *fp_out;
} SQ_COMPRESS_DATA;


extern COMPRESS_CLASS cc_sq;


dsk_err_t sq_open(COMPRESS_DATA *self);
dsk_err_t sq_creat(COMPRESS_DATA *self);
dsk_err_t sq_commit(COMPRESS_DATA *self);
dsk_err_t sq_abort(COMPRESS_DATA *self);

