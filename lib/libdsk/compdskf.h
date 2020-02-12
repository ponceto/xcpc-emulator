/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2008  John Elliott <seasip.webmaster@gmail.com>       *
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

typedef unsigned char lzw_byte;
typedef unsigned short lzw_word;

/* The LZW token system used by DSKF compression is:
 * 	0x000		- EOF
 * 	0x001-0x100	- Literal bytes 0-0xFF ('ch')
 * 	0x101-0xFFF	- Compression tokens   ('tk')
 */

/* In the dictionary, all strings are stored as <first,second> pairs.
 *
 * 	<0, 0> 	=> empty entry
 * 	<ch,0>	=> one character
 * 	<ch,ch>	=> two characters
 * 	<tk,ch>	=> three or more characters -- expand tk recursively.
 */

/* A dictionary entry */
typedef struct dictblock
{
	lzw_word first;	
	lzw_word second;	
} DICTBLOCK;

#define MAXDICT 0x1000	/* 12-bit, so dictionary size is 2^12 entries */


/* The decompression engine */
typedef struct
{
	COMPRESS_DATA dskf_super;
	FILE *dskf_fpin;		/* Input file */
	FILE *dskf_fpout;		/* Output file */
	lzw_byte dskf_header[40];	/* The DSKF header */

	/* Decoder variables. For use when converting a 3-byte sequence to
	 * 2 words. */

	short dskf_toggle;	/* eg: decoding AB CD EF, toggle is 0 if the 
				 * next word should be ABC and 1 if the next 
				 * word should be DEF. */
	lzw_word dskf_pending; 	/* If toggle = 1, pending = high nibble of
				 * next word to return. In the above example,
				 * it would be 0x0D. */

	/* Decompression variables. */

	DICTBLOCK dskf_dict[MAXDICT]; /* The LZW dictionary */
	lzw_byte dskf_stack[MAXDICT]; /* Buffer used to expand tokens */


} DSKF_COMPRESS_DATA;

extern COMPRESS_CLASS cc_dskf;


dsk_err_t cdskf_open(COMPRESS_DATA *self);
dsk_err_t cdskf_creat(COMPRESS_DATA *self);
dsk_err_t cdskf_commit(COMPRESS_DATA *self);
dsk_err_t cdskf_abort(COMPRESS_DATA *self);


