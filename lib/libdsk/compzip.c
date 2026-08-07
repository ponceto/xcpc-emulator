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

#ifdef HAVE_LIBZIP
#include <zip.h>
#include "compzip.h"

COMPRESS_CLASS cc_zip =
{
        sizeof(COMPRESS_DATA),
        "zip",
        "Zip archive (deflate compression)",
        dskzip_open,    /* open */
        dskzip_creat,   /* create new */
        dskzip_commit,  /* commit */
        dskzip_abort    /* abort */
};


/* Pick the alphabetically-first regular member whose name ends with ".dsk"
 * (case-insensitive). Returns the entry index, or -1 if none found.
 */
static zip_int64_t dskzip_select_member(zip_t *archive)
{
	zip_int64_t entry_count;
	zip_int64_t entry_index;
	zip_int64_t selected_index = -1;
	const char *selected_name  = NULL;

	entry_count = zip_get_num_entries(archive, 0);
	for (entry_index = 0; entry_index < entry_count; entry_index++)
	{
		const char *name = zip_get_name(archive, (zip_uint64_t)entry_index, 0);
		size_t name_len;
		if (!name) continue;
		name_len = strlen(name);
		/* skip directories (zip uses trailing '/') */
		if (name_len == 0 || name[name_len - 1] == '/') continue;
		/* require ".dsk" suffix (case-insensitive) */
		if (name_len < 4) continue;
		if (strcasecmp(&name[name_len - 4], ".dsk") != 0) continue;
		/* keep the alphabetically-first match */
		if (selected_name == NULL || strcasecmp(name, selected_name) < 0) {
			selected_index = entry_index;
			selected_name  = name;
		}
	}
	return selected_index;
}


dsk_err_t dskzip_open(COMPRESS_DATA *self)
{
        FILE *fp, *fpout = NULL;
        dsk_err_t err;
	zip_t *archive = NULL;
	zip_file_t *entry = NULL;
	zip_int64_t selected_index;
	unsigned char zin[4];
	char buffer[4096];
	zip_int64_t bytes_read;

        /* Sanity check: Is this meant for our driver? */
        if (self->cd_class != &cc_zip) return DSK_ERR_BADPTR;

        /* Open the file to decompress */
        err = comp_fopen(self, &fp);
        if (err) return DSK_ERR_NOTME;

	/* Check for zip local-file-header magic: "PK\003\004" */
	if (fread(zin, 1, 4, fp) < 4
	 || zin[0] != 'P' || zin[1] != 'K'
	 || zin[2] != 003 || zin[3] != 004)
		err = DSK_ERR_NOTME;
	fclose(fp);
	if (err) return err;

	/* Open the zip archive */
	archive = zip_open(self->cd_cfilename, ZIP_RDONLY, NULL);
	if (!archive) return DSK_ERR_NOTME;

	/* Find a .dsk member; alphabetically-first if several */
	selected_index = dskzip_select_member(archive);
	if (selected_index < 0)
	{
		zip_close(archive);
		return DSK_ERR_NOTME;
	}

	/* Open the selected entry */
	entry = zip_fopen_index(archive, (zip_uint64_t)selected_index, 0);
	if (!entry)
	{
		zip_close(archive);
		return DSK_ERR_COMPRESS;
	}

	/* Open uncompressed output file */
	err = comp_mktemp(self, &fpout);
	if (err) { zip_fclose(entry); zip_close(archive); return err; }

	/* Stream entry contents into the temp file */
	while ((bytes_read = zip_fread(entry, buffer, sizeof(buffer))) > 0)
	{
		if (fwrite(buffer, 1, (size_t)bytes_read, fpout) != (size_t)bytes_read)
		{
			err = DSK_ERR_COMPRESS;
			break;
		}
	}
	if (bytes_read < 0) err = DSK_ERR_COMPRESS;

	fclose(fpout);
	zip_fclose(entry);
	zip_close(archive);

	if (err) remove(self->cd_ufilename);
/* libzip is treated as a read-only archive container here. Writing modified
 * disks back into a zip would require repacking the whole archive on close,
 * which is intentionally not supported. */
	self->cd_readonly = 1;
	return err;
}


dsk_err_t dskzip_creat(COMPRESS_DATA *self)
{
	return DSK_ERR_NOTIMPL;
}


/* Zip write-back is not supported */
dsk_err_t dskzip_commit(COMPRESS_DATA *self)
{
	return DSK_ERR_NOTIMPL;
}


dsk_err_t dskzip_abort(COMPRESS_DATA *self)
{
	return DSK_ERR_OK;
}

#endif //def HAVE_LIBZIP

