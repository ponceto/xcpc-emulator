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

#include "compi.h"

#ifdef HAVE_LIBBZ2
#include <bzlib.h>
#include "compbz2.h"

COMPRESS_CLASS cc_bz2 =
{
        sizeof(COMPRESS_DATA),
        "bz2",
        "Bzip2 (Burrows-Wheeler compression)",
        bz2_open,        /* open */
        bz2_creat,       /* create new */
        bz2_commit,      /* commit */
        bz2_abort        /* abort */
};



dsk_err_t bz2_open(COMPRESS_DATA *self)
{
        FILE *fp, *fpout = NULL;
        dsk_err_t err;
	BZFILE *bz2fp;
	unsigned char bzin[3];

        /* Sanity check: Is this meant for our driver? */
        if (self->cd_class != &cc_bz2) return DSK_ERR_BADPTR;

        /* Open the file to decompress */
        err = comp_fopen(self, &fp);
        if (err) return DSK_ERR_NOTME;

	/* Check for bz2ip magic */
	if (fread(bzin, 1, 3, fp) < 2 || memcmp(bzin, "BZh", 3))
		err = DSK_ERR_NOTME;
	fclose(fp);	
	if (err) return err;

	bz2fp = BZ2_bzopen(self->cd_cfilename, "rb");
	if (!bz2fp) return DSK_ERR_NOTME;

	/* Open uncompressed output file */  
	err = comp_mktemp(self, &fpout);
	if (err) { BZ2_bzclose(bz2fp); return err; }

	while (BZ2_bzread(bz2fp, bzin, 1))
	{
		if (fputc(bzin[0], fpout) == -1)
		{
			err = DSK_ERR_NOTME;
			break;
		}
	}
	fclose(fpout);
	BZ2_bzclose(bz2fp);

	if (err) remove(self->cd_ufilename);
/* libbzip2 doesn't support stdio-style compression yet. So force read-only
 * access */
	self->cd_readonly = 1;
	return err; 
}


dsk_err_t bz2_creat(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}


/* BZIP2 doesn't yet have write ability */
dsk_err_t bz2_commit(COMPRESS_DATA *self)
{
	return DSK_ERR_NOTIMPL;

#if 0
        FILE *fp;
        dsk_err_t err;
	int c;
	unsigned char ch;
	BZFILE *bz2fp;

        /* Sanity check: Is this meant for our driver? */
        if (self->cd_class != &cc_bz2) return DSK_ERR_BADPTR;

        /* Open the file to compress */
	fp = fopen(self->cd_ufilename, "rb");
	if (!fp) return DSK_ERR_SYSERR;

	bz2fp = BZ2_bzopen(self->cd_cfilename, "wb");
	if (!bz2fp) { fclose(fp); return DSK_ERR_SYSERR; }

	err = DSK_ERR_OK;
	while ((c = fgetc(fp)) != EOF)
	{
		ch = c;	
		if (BZ2_write(bz2fp, &ch, 1) != 1)
		{
			err = DSK_ERR_SYSERR;
			break;
		}
	}
	BZ2_bzclose(bz2fp);
	fclose(fp);
	return err;
#endif
}


dsk_err_t bz2_abort(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}

#endif //def HAVE_LIBBZ2

