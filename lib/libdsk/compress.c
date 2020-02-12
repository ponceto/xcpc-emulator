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

#include "drvi.h"   /* For LINUXFLOPPY and WIN32FLOPPY */
#include "compi.h"
#include "comp.h"
/* LibDsk generalised compression support */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#define TMPDIR "/tmp"

static COMPRESS_CLASS *classes[] =
{

#include "compress.inc"

    NULL
};


static dsk_err_t comp_construct(COMPRESS_DATA *cd, const char *filename)
{
    cd->cd_cfilename = dsk_malloc(1 + strlen(filename));    
    if (!cd->cd_cfilename) return DSK_ERR_NOMEM;
    strcpy(cd->cd_cfilename, filename);
    cd->cd_ufilename = NULL;
    cd->cd_readonly = 0;
    return DSK_ERR_OK;
}

void comp_free(COMPRESS_DATA *cd)
{
    if (!cd) return;
    if (cd->cd_cfilename) free(cd->cd_cfilename);
    if (cd->cd_ufilename) free(cd->cd_ufilename);
    free(cd);
}


/* See if a file can be decompressed */
static dsk_err_t comp_iopen(COMPRESS_DATA **cd, const char *filename, int nc)
{
        COMPRESS_CLASS *cc = classes[nc];
        dsk_err_t err;

        if (!cc) return DSK_ERR_BADPTR;

        (*cd) = dsk_malloc(cc->cc_selfsize);
        if (!*cd) return DSK_ERR_NOMEM;
	memset((*cd), 0, cc->cc_selfsize);
        err = comp_construct(*cd, filename);
	(*cd)->cd_class = cc;
    if (err == DSK_ERR_OK) 
    {
        char *s = dsk_malloc(strlen(cc->cc_description) + 50);
        if (s) 
        {
            sprintf(s, "Checking compression: %s...", cc->cc_description);
            dsk_report(s);
            dsk_free(s);
        }
        else    dsk_report("Checking compression...");
        err = (cc->cc_open)(*cd);
        dsk_report_end();
    }
        if (err == DSK_ERR_OK) return err;
        comp_free (*cd);
        *cd = NULL;
        return err;
}

/* See if a file can be decompressed */
static dsk_err_t comp_icreat(COMPRESS_DATA **cd, const char *filename, int nc)
{
        COMPRESS_CLASS *cc = classes[nc];
        dsk_err_t err;
	FILE *fp = NULL;

        if (!cc) return DSK_ERR_BADPTR;

        (*cd) = dsk_malloc(cc->cc_selfsize);
        if (!*cd) return DSK_ERR_NOMEM;
	memset((*cd), 0, cc->cc_selfsize);
        err = comp_construct(*cd, filename);
	(*cd)->cd_class = cc;
	if (err == DSK_ERR_OK) err = (cc->cc_creat)(*cd);
/* Stake out our claim to the temporary file. */
	if (err == DSK_ERR_OK) err = comp_mktemp(*cd, &fp);
	if (fp) fclose(fp);
	if (err == DSK_ERR_OK) return err;
    
        comp_free (*cd);
        *cd = NULL;
        return err;
}



dsk_err_t comp_open(COMPRESS_DATA **cd, const char *filename, const char *type)
{
    int nc;
    dsk_err_t e;
    struct stat st;

    if (!cd || !filename) return DSK_ERR_BADPTR;
    *cd = NULL;

/* Do not attempt to decompress directories */  
#ifdef __PACIFIC__
    /* Pacific doesn't declare the first parameter to stat() as const */
    if (stat((char *)filename, &st)) return DSK_ERR_NOTME;
#else
    if (stat(filename, &st)) return DSK_ERR_NOTME;
#endif
    if (S_ISDIR(st.st_mode)) return DSK_ERR_NOTME;

#ifdef LINUXFLOPPY 
/* Do not try to decompress a floppy drive. The reason for this is: If the 
 * floppy has a format Linux can't detect, repeatedly failing to open /dev/fd0
 * as we check for each compression type causes long delays */
        if (S_ISBLK(st.st_mode) && (major(st.st_rdev) == 2)) return DSK_ERR_NOTME;
#endif
/* Same shortcut for Windows / DOS */
#ifdef ANYFLOPPY
        if (strlen(filename) == 2 && filename[1] == ':') return DSK_ERR_NOTME;
#endif
    
    if (type)
    {
        for (nc = 0; classes[nc]; nc++)
        {
            if (!strcmp(type, classes[nc]->cc_name))
                return comp_iopen(cd, filename, nc);
        }
        return DSK_ERR_NODRVR;
    }
    for (nc = 0; classes[nc]; nc++)
    {
        e = comp_iopen(cd, filename, nc);
        if (e != DSK_ERR_NOTME) return e;
    }   
    return DSK_ERR_NOTME;
}

dsk_err_t comp_creat(COMPRESS_DATA **cd, const char *filename, const char *type)
{
    int nc;

    if (!type)  /* Uncompressed */
    {
        *cd = NULL;
        return DSK_ERR_OK;
    }

    if (!cd || !filename) return DSK_ERR_BADPTR;
    
    for (nc = 0; classes[nc]; nc++)
    {
        if (!strcmp(type, classes[nc]->cc_name))
            return comp_icreat(cd, filename, nc);
    }
    return DSK_ERR_NODRVR;
}



dsk_err_t comp_commit(COMPRESS_DATA **self)
{
    dsk_err_t e;

    if (!self || (!(*self)) || (!(*self)->cd_class))    return DSK_ERR_BADPTR;

    dsk_report("Compressing...");
    e = ((*self)->cd_class->cc_commit)(*self);
    dsk_report_end();

    if ((*self)->cd_ufilename) remove((*self)->cd_ufilename);
    comp_free (*self);
    *self = NULL;
    return e;
}

dsk_err_t comp_abort(COMPRESS_DATA **self)
{
    dsk_err_t e;

    if (!self || (!(*self)) || (!(*self)->cd_class))    return DSK_ERR_BADPTR;

    e = ((*self)->cd_class->cc_abort)(*self);

    if ((*self)->cd_ufilename) remove((*self)->cd_ufilename);
    comp_free (*self);
    *self = NULL;
    return e;
}


/* If "index" is in range, returns the n'th driver name in (*drvname).
 * Else sets (*drvname) to null. */
dsk_err_t comp_type_enum(int index, char **compname)
{
    int nc;

    if (!compname) return DSK_ERR_BADPTR;

    for (nc = 0; classes[nc]; nc++)
    {
        if (index == nc)
        {
            *compname = classes[nc]->cc_name;
            return DSK_ERR_OK;
        }
    }
    *compname = NULL;
    return DSK_ERR_NODRVR;
}


const char *comp_name(COMPRESS_DATA *self)
{
    if (!self || !self->cd_class || !self->cd_class->cc_name)
        return "(null)";
    return self->cd_class->cc_name;
}


const char *comp_desc(COMPRESS_DATA *self)
{
    if (!self || !self->cd_class || !self->cd_class->cc_description)
        return "(null)";
    return self->cd_class->cc_description;
}



dsk_err_t comp_fopen(COMPRESS_DATA *self, FILE **fp)
{
        *fp = fopen(self->cd_cfilename, "r+b");
        if (!(*fp))
        {
                self->cd_readonly = 1;
                (*fp) = fopen(self->cd_cfilename, "rb");
        }
        if (!(*fp)) return DSK_ERR_SYSERR;
    return DSK_ERR_OK;
}
    
    
dsk_err_t comp_mktemp(COMPRESS_DATA *self, FILE **fp)
{
    char *tdir;
    int fd;
    char tmpdir[PATH_MAX];

    self->cd_ufilename = dsk_malloc(PATH_MAX);

/* Win32: Create temp file using GetTempFileName() */
#ifdef HAVE_GETTEMPFILENAME
        GetTempPath(PATH_MAX, tmpdir);
        if (!GetTempFileName(tmpdir, "dsk", 0, self->cd_ufilename))
    {
        dsk_free(self->cd_ufilename);
        self->cd_ufilename = NULL;
        return DSK_ERR_SYSERR;
    }
        *fp = fopen(self->cd_ufilename, "wb");
        (void)fd;
        (void)tdir;
#else /* def HAVE_GETTEMPFILENAME */

/* Modern Unixes: Use mkstemp() */
#ifdef HAVE_MKSTEMP
    tdir = getenv("TMPDIR");
    if (tdir) sprintf(tmpdir, "%s/libdskdXXXXXXXX", tdir);
    else      sprintf(tmpdir, TMPDIR "/libdskXXXXXXXX");

    fd = mkstemp(tmpdir);
    *fp = NULL;
    if (fd != -1) fp[0] = fdopen(fd, "wb");
    strcpy(self->cd_ufilename, tmpdir);
#else   /* def HAVE_MKSTEMP */
/* Old Unixes, old xenix clones such as DOS */ 
    tdir = getenv("TMP");
    if (!tdir) tdir = getenv("TEMP");
    if (tdir) sprintf(tmpdir, "%s/LDXXXXXX", tdir);
    else      sprintf(tmpdir, "./LDXXXXXX");

    strcpy(self->cd_ufilename, mktemp(tmpdir));
    fp[0] = fopen(self->cd_ufilename, "wb");
    (void)fd;
#endif /* HAVE_MKSTEMP */                  
#endif /* HAVE_GETTEMPFILENAME */
    if (!*fp)   
    {
        dsk_free(self->cd_ufilename);
        self->cd_ufilename = NULL;
        return DSK_ERR_SYSERR;
    }
    return DSK_ERR_OK;
}



