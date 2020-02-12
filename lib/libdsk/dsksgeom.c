/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2, 2005  John Elliott <seasip.webmaster@gmail.com>    *
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

#include "drvi.h"

/* Standard disc geometries. These are used 
 * (i)  when logging in a disc, if the superblock is recognised
 * (ii) when formatting */

typedef struct dsk_namedgeom
{
    dsk_cchar_t name;
    DSK_GEOMETRY dg;
    dsk_cchar_t desc;
    struct dsk_namedgeom *next;
} DSK_NAMEDGEOM;

/* These must match the order of the entries in dsk_format_t in libdsk.h */

static DSK_NAMEDGEOM stdg[] = 
{
        /*    sidedness cyl hd sec  psn  sz   rate    rwgap  fmtgap  fm  nomulti*/
{"pcw180",  { SIDES_ALT,     40, 1, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "PCW / IBM 180k" }, /* 180k */
{"cpcsys",  { SIDES_ALT,     40, 1, 9, 0x41, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "CPC System" }, /* CPC system */
{"cpcdata", { SIDES_ALT,     40, 1, 9, 0xC1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "CPC Data" }, /* CPC data */
{"pcw720",  { SIDES_ALT,     80, 2, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "PCW / IBM 720k" }, /* 720k */
{"pcw1440", { SIDES_ALT,     80, 2,18,    1, 512, RATE_HD, 0x1B, 0x54,   0,  0 }, "PcW16 / IBM 1440k "}, /* 1.4M */
{"ibm160",  { SIDES_ALT,     40, 1, 8,    1, 512, RATE_SD, 0x2A, 0x50,   0,  0 }, "IBM 160k (CP/M-86 / DOSPLUS)" }, /* 160k */
{"ibm320",  { SIDES_ALT,     40, 2, 8,    1, 512, RATE_SD, 0x2A, 0x50,   0,  0 }, "IBM 320k (CP/M-86 / DOSPLUS)" }, /* 320k */
{"ibm360",  { SIDES_ALT,     40, 2, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "IBM 360k (CP/M-86 / DOSPLUS)" }, /* 360k */
{"ibm720",  { SIDES_OUTBACK, 80, 2, 9,    1, 512, RATE_SD, 0x2A, 0x52,   0,  0 }, "IBM 720k (144FEAT)", }, /* 720k 144FEAT */
{"ibm1200", { SIDES_OUTBACK, 80, 2,15,    1, 512, RATE_HD, 0x1B, 0x54,   0,  0 }, "IBM 1.2M (144FEAT)", }, /* 1.2M 144FEAT */
{"ibm1440", { SIDES_OUTBACK, 80, 2,18,    1, 512, RATE_HD, 0x1B, 0x54,   0,  0 }, "IBM 1.4M (144FEAT)", }, /* 1.4M 144FEAT */
{"acorn160",    { SIDES_OUTOUT,  40, 1,16,    0, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "Acorn 160k" }, /* Acorn 160k */
{"acorn320",    { SIDES_OUTOUT,  80, 1,16,    0, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "Acorn 320k" }, /* Acorn 320k */
{"acorn640",    { SIDES_OUTOUT,  80, 2,16,    0, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "Acorn 640k" }, /* Acorn 640k */
{"acorn800",    { SIDES_ALT,     80, 2, 5,    0,1024, RATE_SD, 0x04, 0x05,   0,  0 }, "Acorn 800k" }, /* Acorn 800k */
{"acorn1600",   { SIDES_ALT,     80, 2,10,    0,1024, RATE_HD, 0x04, 0x05,   0,  0 }, "Acorn 1600k" }, /* Acorn 1600k */
{"pcw800",  { SIDES_ALT,     80, 2,10,    1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "PCW 800k" }, /* 800k */
{"pcw200",  { SIDES_ALT,     40, 1,10,    1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "PCW 200k" }, /* 200k */
{"bbc100",  { SIDES_ALT,     40, 1,10,    0, 256, RATE_SD, 0x2A, 0x50,   1,  0 }, "BBC 100k" }, /* 100k */
{"bbc200",  { SIDES_ALT,     80, 1,10,    0, 256, RATE_SD, 0x2A, 0x50,   1,  0 }, "BBC 200k" }, /* 200k */
{"mbee400", { SIDES_ALT,     40, 1,10,    0, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "Microbee 400k" }, /* 400k */
{"mgt800",  { SIDES_OUTOUT,  80, 2,10,    1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "MGT 800k" }, /* MGT 800k */
{"trdos640",{ SIDES_ALT,     80, 2,16,    1, 256, RATE_SD, 0x12, 0x60,   0,  0 }, "TR-DOS 640k" }, /* TR-DOS 640k */
{"ampro200",{ SIDES_ALT,     40, 1,10,   1, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "Ampro 40 track single-sided" }, /* Ampro 200k (22DISK AMP1) */
{"ampro400d",{ SIDES_ALT,     40, 2,10,   17, 512, RATE_SD, 0x0C, 0x17,   0,  0 }, "Ampro 40 track double-sided" }, /* Ampro 400k (22DISK AMP2) */
{"ampro400s",{ SIDES_ALT,     80, 1, 5,   1,1024, RATE_SD, 0x04, 0x05,   0,  0 }, "Ampro 80 track single-sided" }, /* Ampro 400k (22DISK AMP3) */
{"ampro800",{ SIDES_ALT,     80, 2, 5,   17,1024, RATE_SD, 0x04, 0x05,   0,  0 }, "Ampro 80 track double-sided" }, /* Ampro 800k (22DISK AMP4) */
/* Geometries below this line don't appear in dsk_format_t and can be accessed
 * only by name. */

{"myz80",   { SIDES_ALT,     64, 1,128,   0,1024, RATE_ED, 0x2A, 0x52,   0,  0 }, "MYZ80 8Mb" }, /* MYZ80 8Mb */

/* re this comment ('This was commented out in libdsk-1.1.3, but I can't 
 * remember why. Bring it back.') -- I think I now remember why. The 
 * numeric format IDs were out of sync with the actual definitions, and I
 * think this may be part of the reason. Move it down to the bottom, beyond
 * the last numeric ID. */
{"pcpm320", { SIDES_OUTBACK, 40, 2, 8,    1, 512, RATE_SD, 0x2A, 0x50,   0,  0 }, "IBM 320k (CP/M-86 / DOSPLUS)" }, /* 320k */
};


#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#include <io.h>
#include <objidl.h>

static void dg_shell_folder(int csidl, char *buf)
{
    LPMALLOC pMalloc;
    LPITEMIDLIST pidl;
    char *cwd;
        char result[PATH_MAX]; 

    cwd = getcwd(result, PATH_MAX);
    if (cwd == NULL) strcpy(result, ".");
    strcpy(buf, result);

    /* Get the shell's allocator */
    if (!SUCCEEDED(SHGetMalloc(&pMalloc))) return;
    /* Get the PIDL for the folder in question */
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, csidl, &pidl)))
    {
        if (SHGetPathFromIDList(pidl, result))
            strcpy(buf, result);
        (pMalloc->lpVtbl->Free)(pMalloc, pidl);
    }
    /* Release the shell's allocator */
    (pMalloc->lpVtbl->Release)(pMalloc);
}

const char *dg_homedir(void)
{
    HKEY hk;
    DWORD dws;
    static char buf[PATH_MAX];
    char *t;
    long l;

    l = RegOpenKeyEx(HKEY_CURRENT_USER, 
        "Software\\jce@seasip\\LibDsk", 0, KEY_READ, &hk);
    if (l == ERROR_SUCCESS)
    {
        dws = PATH_MAX;
        l = RegQueryValueEx(hk, "HomeDir", NULL, NULL, 
                (BYTE *)buf, &dws); 
        RegCloseKey(hk);
        if (l == ERROR_SUCCESS) return buf;
    }
    dg_shell_folder(CSIDL_PERSONAL, buf);
    
    while ((t = strchr(buf, '\\'))) *t = '/';

    /* Ensure path ends with a slash */
    if (buf[strlen(buf)-1] != '/') strcat(buf, "/");

    return buf;
}
#elif defined(HAVE_PWD_H) && defined(HAVE_UNISTD_H)

#include <unistd.h>
#include <pwd.h>

const char *dg_homedir(void)
{
    char *s;
    static char buf[PATH_MAX];
    struct passwd *pw;
    int uid = getuid();

    s = getenv("HOME");
    if (s)
    {
        strcpy(buf, s);
        /* Ensure path ends with a slash */

        if (buf[strlen(buf)-1] != '/') strcat(buf, "/");
        return buf;
    }
    else
    {
        setpwent();
        while ((pw = getpwent()))
        {
            if (pw->pw_uid == uid)
            {
                strcpy(buf, pw->pw_dir);
                if ( buf[strlen(buf)-1] != '/') strcat(buf, "/");
                endpwent();
                return buf;
            }
        }
        endpwent();
    }
    return NULL;
}
#else

const char *dg_homedir(void)
{
    static char buf[PATH_MAX];
    char *s = getenv("HOME");

    if (!s) return NULL;
    strcpy(buf, s);
    /* Ensure path ends with a slash */
    if (s && buf[strlen(buf)-1] != '/') strcat(buf, "/");

    return buf;
}
#endif



#ifdef HAVE_WINDOWS_H

const char *dg_sharedir(void)
{
    static char buf[PATH_MAX + 8];
    char *t;
#ifdef _WIN32   /* Win16 doesn't have HKLM */
    long l;
    HKEY hk;
    DWORD dws = sizeof(buf)-1;

    l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        "Software\\jce@seasip\\LibDsk", 0, KEY_READ, &hk);
    if (l == ERROR_SUCCESS)
    {                                             
        dws = PATH_MAX;
        l = RegQueryValueEx(hk, "ShareDir", NULL, NULL, 
                (BYTE *)buf, &dws); 
        RegCloseKey(hk);
        if (l == ERROR_SUCCESS) return buf;
    }
#endif
    GetModuleFileName((HINSTANCE)NULL, buf, PATH_MAX);
    while ((t = strchr(buf, '\\'))) *t = '/';
    for (t = buf + strlen(buf); t >= buf; t--)
    {
        if (t[0] == ':' || t[0] == '/')
        {
            t[1] = 0;
            break;
        }
    }
    strcat(buf, "share/");
    return buf;
}             

#else /* def HAVE_WINDOWS_H */

const char *dg_sharedir()
{
    static char buf[PATH_MAX];
    char *s = getenv("LIBDSK");

#ifdef AUTOSHARE
    if (SHAREDIR)
    {
        return SHAREDIR "/";
    }
#endif

    if (!s) return NULL;
    strcpy(buf, s);
    /* Ensure path ends with a slash */
    if (s && buf[strlen(buf)-1] != '/') strcat(buf, "/");
    return buf;
}

#endif /* def HAVE_WINDOWS_H */

static DSK_NAMEDGEOM *customgeom = NULL;


dsk_err_t dg_parse(FILE *fp, DSK_GEOMETRY *dg, char *description)
{
    char linebuf[160];
    dsk_err_t err;

    /* Init geometry to default values */
    memcpy(dg, &stdg[0].dg, sizeof(*dg));

    while (fgets(linebuf, sizeof(linebuf), fp))
    {
        /* Start of next section */
        if (linebuf[0] == '[') return DSK_ERR_OK;
        err = dg_parseline(linebuf, dg, description);
        if (err) return err;
    }
    return DSK_ERR_OK;
}

dsk_err_t dg_parseline(char *linebuf, DSK_GEOMETRY *dg, char *description)
{
    char *s;
    char *value;
/* Drop comments and newlines */

    s = strchr(linebuf, ';'); if (s) *s = 0;
    s = strchr(linebuf, '#'); if (s) *s = 0;
    s = strchr(linebuf, '\n'); if (s) *s = 0;

    value = strchr(linebuf, '=');
    if (!value) return DSK_ERR_OK;
    *value = 0;
    ++value;

    /* Chop variable name at first space */
    s = strchr(linebuf, ' '); 
    if (s) *s = 0;
    /* Skip leading spaces in the value */
    while (*value == ' ') ++value;

/* Make the variable name case-insensitive */
    for (s = linebuf; s[0]; s++)
    {
        *s = tolower(*s);
    }
    if (!strcmp(linebuf, "description"))
    {
        if (description) 
            strcpy(description, value);
    }
    if (!strcmp(linebuf, "sidedness") || !strcmp(linebuf, "sides"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "alt"))     
            dg->dg_sidedness = SIDES_ALT;
        if (!strcmp(value, "outback")) 
            dg->dg_sidedness = SIDES_OUTBACK;
        if (!strcmp(value, "outout"))  
            dg->dg_sidedness = SIDES_OUTOUT;
        if (!strcmp(value, "extsurface"))	/* [1.3.7] */
            dg->dg_sidedness = SIDES_EXTSURFACE;
    }
    if (!strcmp(linebuf, "cylinders") && atoi(value))
        dg->dg_cylinders = atoi(value);
    if (!strcmp(linebuf, "heads") && atoi(value))
        dg->dg_heads     = atoi(value);
    if (!strcmp(linebuf, "sectors") && atoi(value))
        dg->dg_sectors   = atoi(value);
    if (!strcmp(linebuf, "secbase"))
        dg->dg_secbase   = atoi(value);
    if (!strcmp(linebuf, "secsize") && atoi(value))
        dg->dg_secsize   = atoi(value);
    if (!strcmp(linebuf, "datarate"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "hd")) dg->dg_datarate = RATE_HD;
        if (!strcmp(value, "dd")) dg->dg_datarate = RATE_DD;
        if (!strcmp(value, "sd")) dg->dg_datarate = RATE_SD;
        if (!strcmp(value, "ed")) dg->dg_datarate = RATE_ED;
    }
    if (!strcmp(linebuf, "rwgap") && atoi(value))
        dg->dg_rwgap  = atoi(value);
    if (!strcmp(linebuf, "fmtgap") && atoi(value))
        dg->dg_fmtgap  = atoi(value);
    if (!strcmp(linebuf, "fm"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) 
	    dg->dg_fm = (dg->dg_fm & RECMODE_FLAGMASK) | RECMODE_FM;
        if (!strcmp(value, "n")) 
	    dg->dg_fm = (dg->dg_fm & RECMODE_FLAGMASK) | RECMODE_MFM;
    }
    /* [1.4.1] Allow 'recmode=mfm' / 'recmode=fm' as a synonym for 'fm=y / fm=n'
     *         Obviously this syntax would allow additional recording modes in
     *         future. */
    if (!strcmp(linebuf, "recmode"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "fm")) 
	    dg->dg_fm = (dg->dg_fm & RECMODE_FLAGMASK) | RECMODE_FM;
        if (!strcmp(value, "mfm")) 
	    dg->dg_fm = (dg->dg_fm & RECMODE_FLAGMASK) | RECMODE_MFM;
    }
    /* [1.4.1] 'Complement' flag */
    if (!strcmp(linebuf, "complement"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) 
	    dg->dg_fm |= RECMODE_COMPLEMENT;
        if (!strcmp(value, "n")) 
	    dg->dg_fm &= ~RECMODE_COMPLEMENT;
    }
    if (!strcmp(linebuf, "multitrack"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) dg->dg_nomulti = 0;
        if (!strcmp(value, "n")) dg->dg_nomulti = 1;
    }
    if (!strcmp(linebuf, "skipdeleted"))
    {
        for (s = value; s[0]; s++) *s = tolower(*s);
        if (!strcmp(value, "y")) dg->dg_noskip = 0;
        if (!strcmp(value, "n")) dg->dg_noskip = 1;
    }
    return DSK_ERR_OK;
}



dsk_err_t dg_store(FILE *fp, DSK_GEOMETRY *dg, char *description)
{
    if (description) fprintf(fp, "description=%s\n", description);
    switch(dg->dg_sidedness)
    {
        case SIDES_ALT:        fprintf(fp, "sides=alt\n");     break;
        case SIDES_OUTOUT:     fprintf(fp, "sides=outback\n"); break;
        case SIDES_OUTBACK:    fprintf(fp, "sides=outout\n");  break;
        case SIDES_EXTSURFACE: fprintf(fp, "sides=extsurface\n");  break;
    }
    fprintf(fp, "cylinders=%d\n", dg->dg_cylinders);
    fprintf(fp, "heads=%d\n",     dg->dg_heads);
    fprintf(fp, "sectors=%d\n",   dg->dg_sectors);
    fprintf(fp, "secbase=%d\n",   dg->dg_secbase);
    fprintf(fp, "secsize=%ld\n",  (long)dg->dg_secsize);
    switch(dg->dg_datarate)
    {
        case RATE_HD: fprintf(fp, "datarate=HD\n"); break;
        case RATE_DD: fprintf(fp, "datarate=DD\n"); break;
        case RATE_SD: fprintf(fp, "datarate=SD\n"); break;
        case RATE_ED: fprintf(fp, "datarate=ED\n"); break;
    }
    fprintf(fp, "rwgap=%d\n", dg->dg_rwgap);
    fprintf(fp, "fmtgap=%d\n", dg->dg_fmtgap);
  
    switch (dg->dg_fm & RECMODE_MASK)
    {
        case RECMODE_MFM: fprintf(fp, "recmode=MFM\n"); break;
        case RECMODE_FM:  fprintf(fp, "recmode=FM\n"); break;
    }
    fprintf(fp, "complement=%c\n", (dg->dg_fm & RECMODE_COMPLEMENT) ? 'Y' : 'N');
    fprintf(fp, "multitrack=%c\n", dg->dg_nomulti ? 'N' : 'Y');
    fprintf(fp, "skipdeleted=%c\n", dg->dg_noskip ? 'N' : 'Y');
    return DSK_ERR_OK;
}


static dsk_err_t dg_parse_file(FILE *fp)
{
    DSK_NAMEDGEOM ng, *pg;
    char linebuf[160];
    char formname[160];
    char formdesc[160];
    char *s;
    long pos;
    dsk_err_t err;

    while (fgets(linebuf, sizeof(linebuf), fp))
    {
        formdesc[0] = 0;
/* Drop comments and newlines */
        s = strchr(linebuf, ';'); if (s) *s = 0;
        s = strchr(linebuf, '#'); if (s) *s = 0;
        s = strchr(linebuf, '\n'); if (s) *s = 0;

        if (linebuf[0] != '[') continue;
/* Format names cannot start with "-", so any section beginning "[-" is 
 * going to be something other than a format. */
	if (linebuf[1] == '-') continue;
        strcpy(formname, linebuf+1);
        s = strchr(formname, ']');
        if (s) *s = 0;
        pos = ftell(fp);
        err = dg_parse(fp, &ng.dg, formdesc);
        if (err) return err;
        fseek(fp, pos, SEEK_SET);   

        pg = dsk_malloc(sizeof(ng) + 2 + 
                strlen(formdesc) + strlen(formname));
        if (!pg) return DSK_ERR_NOMEM;
        memcpy(pg, &ng, sizeof(ng));

        pg->name = ((char *)pg) + sizeof(ng);
        pg->desc = ((char *)pg) + sizeof(ng) + 1 + strlen(formname);
        strcpy((char *)pg->name, formname);
        strcpy((char *)pg->desc, formdesc);
        pg->next = customgeom;
        customgeom = pg;
    }
    return DSK_ERR_OK;
}

dsk_err_t dg_custom_init(void)
{
    const char *path;
    char buf[2 * PATH_MAX];
    FILE *fp;
    dsk_err_t err;

    static int custom_inited = 0;

/* Assert that the format names and numeric IDs are in sync. */

#ifdef HAVE_ASSERT_H
    assert(!strcmp(stdg[FMT_180K].name, "pcw180"));
    assert(!strcmp(stdg[FMT_320K].name, "ibm320"));
    assert(!strcmp(stdg[FMT_720F].name, "ibm720"));
    assert(!strcmp(stdg[FMT_1440F].name, "ibm1440"));
    assert(!strcmp(stdg[FMT_ACORN160].name, "acorn160"));
    assert(!strcmp(stdg[FMT_AMPRO800].name, "ampro800"));
#endif
    if (custom_inited < 1)
    {
        path = dg_sharedir();
        if (path)
        {
            sprintf(buf, "%s%s", path, "libdskrc");
            fp = fopen(buf, "r");
            if (fp)
            {
                err = dg_parse_file(fp);
                fclose(fp);
                if (err) return err;
            }
        }
        custom_inited = 1;
    }
    if (custom_inited < 2)
    {
        path = dg_homedir();
        if (path)
        {
            sprintf(buf, "%s%s", path, ".libdskrc");
            fp = fopen(buf, "r");
            if (fp)
            {
                err = dg_parse_file(fp);
                fclose(fp);
                if (err) return err;
            }       
        }
        custom_inited = 2;
    }
    return DSK_ERR_OK;
}



/* Initialise a DSK_GEOMETRY with a standard format */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_stdformat(DSK_GEOMETRY *self, dsk_format_t formatid,
            dsk_cchar_t *fname, dsk_cchar_t *fdesc)
{
    int idx = (formatid - FMT_180K);

    dg_custom_init();

/* If index is out of range in the standard set, search the custom set */
    if ((unsigned)idx >= (sizeof(stdg)/sizeof(stdg[0]))  ) 
    {
        DSK_NAMEDGEOM *cg = customgeom;
        idx -= (sizeof(stdg) / sizeof(stdg[0]));

        while (idx)
        {
            if (!cg) return DSK_ERR_BADFMT;
            cg = cg->next;
            --idx;
        }
        if (!cg) return DSK_ERR_BADFMT;

        if (self) memcpy(self, &cg->dg, sizeof(*self));
        if (fname) *fname = cg->name;
        if (fdesc) *fdesc = cg->desc;
        return DSK_ERR_OK;
    }
/* Search the standard set */
    if (self) memcpy(self, &stdg[idx].dg, sizeof(*self));
    if (fname) *fname = stdg[idx].name;
    if (fdesc) *fdesc = stdg[idx].desc;
    return DSK_ERR_OK;
}



