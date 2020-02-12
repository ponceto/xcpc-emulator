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

#ifdef HAVE_LIBZ
#include <zlib.h>
#include "compgz.h"

COMPRESS_CLASS cc_gz =
{
        sizeof(COMPRESS_DATA),
        "gz",
        "Gzip (deflate compression)",
        gz_open,        /* open */
        gz_creat,       /* create new */
        gz_commit,      /* commit */
        gz_abort        /* abort */
};



dsk_err_t gz_open(COMPRESS_DATA *self)
{
        FILE *fp, *fpout = NULL;
        dsk_err_t err;
	int c;
	gzFile gzfp;
	unsigned char uzin[2];

        /* Sanity check: Is this meant for our driver? */
        if (self->cd_class != &cc_gz) return DSK_ERR_BADPTR;

        /* Open the file to decompress */
        err = comp_fopen(self, &fp);
        if (err) return DSK_ERR_NOTME;

	/* Check for gzip magic */
	if (fread(uzin, 1, 2, fp) < 2 || uzin[0] != 037 || uzin[1] != 0213) 
		err = DSK_ERR_NOTME;
	fclose(fp);	
	if (err) return err;

	gzfp = gzopen(self->cd_cfilename, "rb");
	if (!gzfp) return DSK_ERR_NOTME;

	/* Open uncompressed output file */  
	err = comp_mktemp(self, &fpout);
	if (err) { gzclose(gzfp); return err; }

	while ((c = gzgetc(gzfp)) != EOF)
	{
		if (fputc(c, fpout) == -1)
		{
			err = DSK_ERR_NOTME;
			break;
		}
	}
	fclose(fpout);
	gzclose(gzfp);

	if (err) remove(self->cd_ufilename);
	return err; 
}


dsk_err_t gz_creat(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}

dsk_err_t gz_commit(COMPRESS_DATA *self)
{
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
}


dsk_err_t gz_abort(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}

#endif //def HAVE_LIBZ

