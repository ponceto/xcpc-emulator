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

/* LibDsk generalised compression support */

/* LibDsk compression works by creating an uncompressed copy of the 
 * compressed file, and passing the name of that file through to the 
 * driver.
 *
 * In fact, this generalised compress/decompress might come in useful in
 * other ways. I'll try to minimise dependencies on the rest of LibDsk.
 */

typedef struct compress_data
{
	char *cd_cfilename;	/* Filename of compressed file */
	char *cd_ufilename;	/* Filename of temporary uncompressed file */
	int cd_readonly;	/* Compressed file is read-only */
	struct compress_class *cd_class;	
} COMPRESS_DATA;


typedef struct compress_class
{
	size_t cc_selfsize;	/* Size of data associated with this class */
/* Name and description are not used for anything yet, but are present by 
 * analogy with drv_class */
	char *cc_name;		/* Compression scheme short name */
	char *cc_description;	/* Compression scheme description */

/* Functions: */

	/* Open and decompress a compressed file. Returns DSK_ERR_OK if 
	 * the file could be decompressed; DSK_ERR_NOTME if the file is
	 * not compressed; DSK_ERR_COMPRESS if the compressed file is
	 * damaged. The filenames to use are stored in "self". */
	dsk_err_t (*cc_open)(COMPRESS_DATA *self);

	/* Record that a compressed file will be created. Usually a no-op.  */
	dsk_err_t (*cc_creat)(COMPRESS_DATA *self);

	/* Close and compress the file. Returns DSK_ERR_OK if the file 
	 * could be closed, other errors if not. */
	dsk_err_t (*cc_commit)(COMPRESS_DATA *self);

	/* Close file, but don't bother re-compressing it, because 
	 * it wasn't changed. Usually a no-op. */
	dsk_err_t (*cc_abort)(COMPRESS_DATA *self);
} COMPRESS_CLASS;


/* See if a file is compressed. If the file is not compressed, (*cd) will
 * be set to NULL and DSK_ERR_NOTME will be returned. If the file *is*
 * compressed, (*cd) will be set to a new COMPRESS_DATA object.  */
dsk_err_t comp_open(COMPRESS_DATA **cd, const char *filename, const char *type);

/* Create a compressed file. If type is NULL (uncompressed) this returns 
 * dsk_err_ok with *cd = NULL */
dsk_err_t comp_creat(COMPRESS_DATA **cd, const char *filename, const char *type);

/* Close compressed file */
dsk_err_t comp_commit(COMPRESS_DATA **cd);
/* Abandon compressed file */
dsk_err_t comp_abort(COMPRESS_DATA **cd);

/* Open the compressed file to read (cd->cd_cfilename) */
dsk_err_t comp_fopen(COMPRESS_DATA *self, FILE **pfp);

/* Create a temporary file to decompress into. cd->cd_ufilename will be set 
 * to its name. */
dsk_err_t comp_mktemp(COMPRESS_DATA *cd, FILE **pfp);


dsk_err_t comp_type_enum(int index, char **compname);
const char *comp_name(COMPRESS_DATA *self);
const char *comp_desc(COMPRESS_DATA *self);


