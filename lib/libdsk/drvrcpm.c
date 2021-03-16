/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2005,2008,2010-11  John Elliott <seasip.webmaster@gmail.com> *
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


/* This driver is probably the weirdest I've written so far. It simulates
 * a CP/M disc using a collection of separate files; the idea being to
 * make a host directory appear as a CP/M filesystem. 
 */ 
 
#include <stdio.h>
#include "libdsk.h"
#include "drvi.h"
#include "drvrcpm.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_UTIME_H
#include <utime.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_SYS_UTIME_H
#include <sys/utime.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_DIR_H
#include <dir.h>
#endif

#ifdef HAVE_RCPMFS

#define CONFIGFILE ".libdsk.ini"
#define BOOTFILE   ".libdsk.boot"

static const int DIR_EX = 12;	/* Extent counter (low bits) */
static const int DIR_S1 = 13;	/* Last record byte count */
static const int DIR_S2 = 14;	/* Extent counter (high bits) */
static const int DIR_RC = 15;	/* Record counter */

/* Filename map entry holds real name. We allow 13 bytes for filename.type, 
   plus 4 bytes for the user number. User numbers are done by prepending 
   the number and then two dots; so 10:filename.typ would become 
   10..filename.typ  */
#define NAMEMAP_ENTRYSIZE (17)

/* Use this as directory separator char */
#define SEPARATOR '/'

#if 0
#define RTRACE(x) printf x
#define RTR_CHAIN(x,y) rtr_chain(x,y)
static void rtr_chain(const char *s, RCPMFS_BUFFER *b)
{
	printf("%s: ", s);
	while (b)
	{
		printf("0x%lx", b->rcb_lsect);
		if (b->rcb_next) printf("->");
		b = b->rcb_next;
	}
	putchar('\n');
} 
#else
#define RTRACE(x)
#define RTR_CHAIN(x,y)
#endif

/* This struct contains function pointers to the driver's functions, and the
 * size of its DSK_DRIVER subclass */

DRV_CLASS dc_rcpmfs = 
{
	sizeof(RCPMFS_DSK_DRIVER),
	"rcpmfs",
	"Reverse CP/MFS driver",
	rcpmfs_open,	/* open */
	rcpmfs_creat,   /* create new */
	rcpmfs_close,   /* close */
	rcpmfs_read,	/* read sector, working from physical address */
	rcpmfs_write,   /* write sector, working from physical address */
	rcpmfs_format,  /* format track, physical */
	rcpmfs_getgeom, /* get geometry */
	rcpmfs_secid,	/* sector ID */
	rcpmfs_xseek,   /* seek to track */
	rcpmfs_status,  /* drive status */
	NULL,		/* xread */
	NULL,		/* xwrite */
	NULL,		/* tread */
	NULL,		/* xtread */
	rcpmfs_option_enum,	/* List options */
	rcpmfs_option_set,	/* Set option */
	rcpmfs_option_get,	/* Get option */

};


/* Forward declarations */
static dsk_err_t rcpmfs_flush(RCPMFS_DSK_DRIVER *self);

/******************** CP/M FILESYSTEM PARAMETERS **********************/

/* Get the extent mask */
static unsigned rcpmfs_get_exm(RCPMFS_DSK_DRIVER *self)
{
	if (self->rc_totalblocks < 256) 
	{
		return (self->rc_blocksize / 1024) - 1;
	}
	return (self->rc_blocksize / 2048) - 1;
}

static unsigned rcpmfs_blocks_per_extent(RCPMFS_DSK_DRIVER *self)
{
	if (self->rc_totalblocks < 256)  return 16;
	return 8;
}


static unsigned long rcpmfs_extent_size(RCPMFS_DSK_DRIVER *self)
{
	unsigned long blocks_per_extent = self->rc_blocksize;

	return blocks_per_extent * rcpmfs_blocks_per_extent(self);
}

static unsigned rcpmfs_secperblock(RCPMFS_DSK_DRIVER *self)
{
	return self->rc_blocksize / self->rc_geom.dg_secsize;
}

static unsigned rcpmfs_max_dirent(RCPMFS_DSK_DRIVER *self)
{
	return (self->rc_dirblocks * (self->rc_blocksize / 32));
}

/* Get the length of this extent, in bytes, to nearest 128 bytes */
static unsigned extent_bytes(RCPMFS_DSK_DRIVER *self, unsigned char *dirent)
{
	unsigned exm = rcpmfs_get_exm(self);

	return (dirent[DIR_RC] + ((dirent[DIR_EX] & exm) * 128)) * 128;
}


/********************* DATA CONVERSION FUNCTIONS *************************/

#define BCD(x) (((x % 10)+16*(x/10)) & 0xFF)
#define UNBCD(x) (((x % 16) + 10 * (x / 16)) & 0xFF)

void rcpmfs_time2cpm(time_t unixtime, unsigned char *cpmtime)
{
	long d  = (unixtime / 86400) - 2921;  /* CP/M day 0 is unix day 2921 */
	long h  = (unixtime % 86400) / 3600;  /* Hour, 0-23 */
	long m  = (unixtime % 3600)  / 60;    /* Minute, 0-59 */

	cpmtime[0] = (unsigned char)(d & 0xFF);
	cpmtime[1] = (unsigned char)((d >> 8) & 0xFF);
	cpmtime[2] = (unsigned char)(BCD(h));
	cpmtime[3] = (unsigned char)(BCD(m));
}

time_t rcpmfs_cpm2time(unsigned char *cpmtime)
{
	time_t t;
	unsigned short days;

	days = (cpmtime[0] + 256 * cpmtime[1]) + 2921;

	t =  60     * UNBCD(cpmtime[3]);
	t += 3600L  * UNBCD(cpmtime[2]);
	t += 86400L * days;
	return t;
}

#undef BCD
#undef UNBCD

/* Concatenate a name to the stored directory name. 
 * Pity libdsk isn't written in D; then I'd have proper dynamic
 * strings. */
static char *rcpmfs_mkname(RCPMFS_DSK_DRIVER *self, const char *filename)
{
	static char buf[PATH_MAX + 20];
	char *target;

	strncpy(buf, self->rc_dir, PATH_MAX - 1);
	buf[PATH_MAX - 1] = 0;
	target = buf + strlen(buf);
	target[0] = SEPARATOR;
	++target;
	strncpy(target, filename, 18);
	target[18] = 0;
	return buf;
}


/* Set file size. This works in two steps - firstly, it subtracts 'delta'
 * (a number of records to chop off the end of the file). Secondly, it
 * tweaks the exact size depending on the last record byte count */ 

static dsk_err_t rcpmfs_adjust_size(RCPMFS_DSK_DRIVER *self, 
		long delta, unsigned lrbc,
		const char *filename)
{
	struct stat st;
	long newsize;

	/* ISX, annoyingly, gives the last record byte count the opposite
	 * meaning. That is, it is the number of _unused_ bytes in the last
	 * record. */
	if (self->rc_fsversion == FSVERSION_ISX)
	{
		lrbc = (128 - lrbc) & 0x7F;
	}

/* Adjust the size of a file (to within 128 bytes). 
 * Rather than do this by working out how big CP/M thinks the file should
 * be, we stat() it, get the real size, and tweak that. */

	RTRACE(("Adjust file size (%s) delta=%ld lrbc=%d\n", filename, delta, lrbc));
	
	if (stat(filename, &st)) return DSK_ERR_SYSERR; /* Can't stat file! */

	/* Round up to nearest 128-byte record */   
	newsize = ((st.st_size + 0x7F) & (~0x7F));
	/* Subtract 'delta' records, if necessary */
	newsize -= delta;

	/* Adjust for new last-record-byte-count */
	if (lrbc)
	{
		newsize = (newsize - 0x80) + lrbc;
	}
	RTRACE(("old size=0x%lx new size=0x%lx\n", st.st_size, newsize));
	if (newsize != st.st_size)
	{
#ifdef HAVE_WINDOWS_H
		/* Why does win32 have to make everything so *difficult*? */
	HANDLE h = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h != INVALID_HANDLE_VALUE)
	{
		if (SetFilePointer(h, newsize, NULL, FILE_BEGIN) != 0xFFFFFFFFL)
		{
			SetEndOfFile(h);
		}
		CloseHandle(h);
	}
#else
	if (truncate(filename, newsize)) return DSK_ERR_SYSERR;
#endif
	}
	return DSK_ERR_OK;
}


static dsk_err_t rcpmfs_rename(RCPMFS_DSK_DRIVER *self, const char *oldname,
            const char *newname)
{
    char buf1[PATH_MAX + 20];
    char buf2[PATH_MAX + 20];
    int n, max;
    char *map_entry;

    strcpy(buf1, rcpmfs_mkname(self, oldname));
    strcpy(buf2, rcpmfs_mkname(self, newname));
    if (rename(buf1, buf2)) return DSK_ERR_SYSERR;

    max = rcpmfs_max_dirent(self);
    for (n = 0; n < max; n++)
    {
        map_entry = self->rc_namemap + NAMEMAP_ENTRYSIZE * n;
        if (!strcmp(map_entry, oldname))
        {
RTRACE(("Rename affects dirent %d: '%s' -> '%s'", n, map_entry, newname));
            strncpy(map_entry, newname, NAMEMAP_ENTRYSIZE-1);
            map_entry[NAMEMAP_ENTRYSIZE-1] = 0;
        }
    }
    return DSK_ERR_OK;  
}


/* This is used when CP/M writes back new filenames.
 * Convert a CP/M filename at dirent to plain text.  */
static void rcpmfs_cpmname(unsigned char *dirent, char *fnbuf)
{
	int n;
	char *dest = fnbuf;

	if (dirent[0] >= 1 && dirent[0] < 32)
	{
		sprintf(dest, "%02d..", dirent[0]);
		dest += strlen(dest);
	}

	for (n = 0; n < 8; n++)
	{
		if ((dirent[1+n] & 0x7F) != ' ')
		{
			*dest++ = tolower(dirent[1+n] & 0x7F);
		}
	}
	if ((dirent[9] & 0x7F) != ' ')
	{
		*dest++ = '.';
		for (n = 0; n < 3; n++)
		{
			if ((dirent[9+n] & 0x7F) != ' ')
			{
				*dest++ = tolower(dirent[9+n] & 0x7F);
			}
		}
	}
	*dest++ = 0;
}


/*********************** RCPMFS CONFIGURATION ***************************/

static dsk_err_t rcpmfs_option(RCPMFS_DSK_DRIVER *self, char *variable, 
        char *value)
{
	char *s;
	dsk_err_t err;
	char tempbuf[160];

	/* Trim spaces from the end of variable & start of value */
	s = strchr(variable, ' ');
	if (s) *s = 0;
	while (*value == ' ') ++value;
	s = strchr(value, ' ');
	if (s) *s = 0;
	s = strchr(value, '\n');
	if (s) *s = 0;

	if (!strcmp(variable, "blocksize") && atoi(value)) 
		   self->rc_blocksize = atoi(value);	
	if (!strcmp(variable, "dirblocks") && atoi(value)) 
		   self->rc_dirblocks = atoi(value);	
	if (!strcmp(variable, "totalblocks") && atoi(value)) 
		   self->rc_totalblocks = atoi(value);  
	if (!strcmp(variable, "systracks"))	/* We are allowed systracks=0*/ 
		   self->rc_systracks = atoi(value);	
	if (!strcmp(variable, "version") && atoi(value)) 
		   self->rc_fsversion = atoi(value);
	else if (!strcmp(variable, "version") && !strcmp(value, "isx"))
		   self->rc_fsversion = FSVERSION_ISX;
	else if (!strcmp(variable, "version") && !strcmp(value, "ISX"))
		   self->rc_fsversion = FSVERSION_ISX;

	if (!strcmp(variable, "format"))
	{
/* Try to find format by name */
		int fmt = 0;
		dsk_cchar_t fname;

		do
		{
			err = dg_stdformat(NULL, fmt, &fname, NULL);
	/*	  RTRACE(("value='%s' fname='%s'\n", value, fname)); */
			if (err == DSK_ERR_OK && fname &&
				!strcmp(value, fname))
			{
				return dg_stdformat(&self->rc_geom, fmt,
					NULL, NULL);	
			}
			++fmt;
		} while (err != DSK_ERR_BADFMT);
		return err;
	}
/* If line not recognised, see if the disk geometry parser recognises it */
	sprintf(tempbuf, "%s=%s", variable, value);
	return dg_parseline(tempbuf, &self->rc_geom, NULL);
}


static dsk_err_t rcpmfs_dump_options(RCPMFS_DSK_DRIVER *self, FILE *fp)
{
	dsk_err_t err;
	DSK_GEOMETRY geom;
	const char *fname;
	int fmt = 0;
	int format_found = 0;

	fprintf(fp, "; This file is formatted like a Windows INI file,\n"
		    "; but it must have only one section and that \n"
		    "; section must be:\n"
			"[RCPMFS]\n");
	fprintf(fp, "; Filesystem parameters are:\n");
	fprintf(fp, "BlockSize=%u	; Size of CP/M block\n", 
	                    self->rc_blocksize);
	fprintf(fp, "DirBlocks=%u    ; Number of blocks for the directory\n", 
	                    self->rc_dirblocks);
	fprintf(fp, "TotalBlocks=%u  ; Number of directory and data blocks\n",
	                        self->rc_totalblocks);
	fprintf(fp, "SysTracks=%u    ; Number of system tracks\n", 
	                    self->rc_systracks);
	if (self->rc_fsversion == FSVERSION_ISX)
		fprintf(fp, "Version=ISX    ; Filesystem version (CP/M 2 or 3, or ISX)\n");
	else	fprintf(fp, "Version=%u      ; Filesystem version (CP/M 2 or 3, or ISX)\n", self->rc_fsversion);

	do
	{
		err = dg_stdformat(&geom, fmt, &fname, NULL);
		if (err == DSK_ERR_OK && fname && 
			!memcmp(&geom, &self->rc_geom, sizeof(geom)))
		{
			format_found = 1;
			fprintf(fp, "Format=%s\n", fname);
			break;
		}
		fmt++;
	} while (err != DSK_ERR_BADFMT);

	if (format_found) err = DSK_ERR_OK;
	else err = dg_store(fp, &self->rc_geom, NULL);
	fflush(fp);
	return err;
}



static dsk_err_t rcpmfs_parse(RCPMFS_DSK_DRIVER *self, FILE *fp)
{
	char *s;
	char linebuf[160];
	int got_format = 0;
	dsk_err_t err;

	if (!self) return DSK_ERR_BADPTR;

	while (fgets(linebuf, sizeof(linebuf), fp))
	{
/* Force case-insensitivity */
		for (s = linebuf; *s; s++)
		{
			*s = tolower(*s);
		}
/* Allow comments to start with ; or # */
		s = strchr(linebuf, ';');
		if (s) *s = 0;  
		s = strchr(linebuf, '#');
		if (s) *s = 0;  

/* If we haven't got a format yet, see if this line has one */
		if (!got_format)
		{
			if (linebuf[0] != '[') continue;
/* Must be RCPMFS. I don't yet plan to support directories which can be multiple
 * FSs at once; it would ruin autodetection. */
			if (memcmp(linebuf + 1, "rcpmfs]", 7)) 
			{
				return DSK_ERR_NOTME;
			}
			got_format = 1;
			continue;
		}
		else
		{
			if (linebuf[0] == '[') return DSK_ERR_OK;
			s = strchr(linebuf, '=');
			if (s)
			{
				*s = 0;
				err = rcpmfs_option(self, linebuf, s + 1);
				if (err) return err;
			}
		}
	}
	return DSK_ERR_OK;
}

/******************** CP/M DIRECTORY ACCESS ************************/
/* Look up a block in the CP/M directory and say what file owns it */

#include <assert.h>

static dsk_err_t rcpmfs_writebuffer(RCPMFS_DSK_DRIVER *self,
		const void *data, dsk_lsect_t lsect)
{
	RCPMFS_BUFFER *rcb, *rcb2;

	RTR_CHAIN("rcpmfs_writebuffer", self->rc_bufhead);	
	rcb = self->rc_bufhead;
	while (rcb)
	{
		if (rcb->rcb_lsect == lsect)
		{
/* This is a "can't happen" error - trying to write a sector size other than
 * the one we're using */
			assert(rcb->rcb_size == self->rc_geom.dg_secsize);
			if (rcb->rcb_size != self->rc_geom.dg_secsize)
			{
				return DSK_ERR_ECHECK;
			}
			memcpy(rcb->rcb_data, data, self->rc_geom.dg_secsize);
			return DSK_ERR_OK;
		}
		rcb = rcb->rcb_next;
	}
	RTRACE(("rcpmfs_writebuffer: Allocating new buffer\n"));
	rcb = dsk_malloc(sizeof(RCPMFS_BUFFER) + self->rc_geom.dg_secsize);
	if (!rcb) return DSK_ERR_NOMEM;
	memcpy(rcb->rcb_data, data, self->rc_geom.dg_secsize);
	rcb->rcb_next = NULL;
	rcb->rcb_size = self->rc_geom.dg_secsize;
	rcb->rcb_lsect = lsect;
	RTRACE(("rcpmfs_writebuffer: Wrote %02x %02x %02x\n",
		rcb->rcb_data[0], rcb->rcb_data[1], rcb->rcb_data[2]));

	/* Add buffer at the tail of the chain. This is more complex, but
	 * means the directory comes at the beginning, which should be an
	 * optimisation */  
	if (!(rcb2 = self->rc_bufhead))
	{
		self->rc_bufhead = rcb;
		RTR_CHAIN("rcpmfs_writebuffer end1", self->rc_bufhead);	
		return DSK_ERR_OK;
	}
	while (rcb2->rcb_next)
	{
		rcb2 = rcb2->rcb_next;
	}
	rcb2->rcb_next = rcb;
	RTR_CHAIN("rcpmfs_writebuffer end2", self->rc_bufhead);	
	return DSK_ERR_OK;
}


/* If seeking to write, make sure that the file is at least 'offset' bytes
 * long. */
static dsk_err_t rcpmfs_wrseek(FILE *fp, unsigned long offset)
{
	long cursize;

	if (fseek(fp, 0, SEEK_END)) return DSK_ERR_SYSERR;
	cursize = ftell(fp);
	if (cursize == -1) return DSK_ERR_SYSERR;
	while (cursize < (long)offset)
	{
		if (fputc(0xE5, fp) == EOF) return DSK_ERR_SYSERR;
		++cursize;
	}   
	if (fseek(fp, offset, SEEK_SET)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}


static dsk_err_t rcpmfs_writefile(RCPMFS_DSK_DRIVER *self, char *filename,
		long offset, const void *buf, unsigned bufsize)
{
	FILE *fp;
	char *pathname;
	dsk_err_t err;

	RTRACE(("rcpmfs_writefile('%s' offset=0x%lx len=0x%x\n", filename, offset, bufsize));
	pathname = rcpmfs_mkname(self, filename);
	fp = fopen(pathname, "r+b");
	if (!fp) fp = fopen(pathname, "wb");
	if (fp && bufsize)
	{
		err = rcpmfs_wrseek(fp, offset);
		if (err)
		{
			fclose(fp);
			return err;
		}
		if (fwrite(buf, 1, bufsize, fp) < bufsize)
		{
			fclose(fp);
			return DSK_ERR_SYSERR;
		}
		fclose(fp);
		return DSK_ERR_OK;
	}
	if (fp) fclose(fp);

	return DSK_ERR_OK;
}


/* Given a filename, produce a CP/M 8.3 filename from it.
 * Returns 0 if the name cannot be converted to a CP/M 
 * filename, 1 if it can. st will be populated with 
 * the results of a stat() on the file. attributes
 * should be the DOS attributes of the file, if the 
 * host OS supports them */
static int rcpmfs_83name(RCPMFS_DSK_DRIVER *self, 
		char *name, unsigned char *dirent, struct stat *st, unsigned attributes)
{
	int n;
	static const char badchars[] = " :;<>[]";
	char *dot;
	int uid = 0;
	char *realname = name;

/* If the file begins with a CP/M user number (nn..) 
   then take account of it */
	if (isdigit(name[0]) && isdigit(name[1]) && 
			name[2] == '.' && name[3] == '.')
	{
		uid = atoi(name);
		name += 4;
		if (uid > 16 || uid < 0) return 0;
		RTRACE(("name=%s uid=%d\n", name, uid));
	}

/* Check that the filename doesn't contain characters which would be 
 * Abominations Unto CP/M. */
	for (n = 0; name[n]; n++)
	{
		if (strchr(badchars, name[n])) 
	{
		RTRACE(("Rejected because of Abomination: %c\n", name[n]));
		return 0;
	}
	}
/* If it hasn't got a dot, it can only be 8 chars */
	dot = strchr(name, '.');
	if (!dot)
	{
		if (strlen(name) > 8) return 0;
		dirent[0] = uid;
		for (n = 0; name[n]; n++)
		{
			dirent[n+1] = toupper(name[n]);
		}   
		for (; n < 11; n++) dirent[n+1] = ' ';

	}
	else
	{
		RTRACE(("dot = %p(%s) name = %p(%s)\n", dot, dot, name, name));
		if (dot == name) return 0;  /* Can't start with dot */
		if ((dot - name) > 8) return 0; /* 8 chars left of dot */
		if (strlen(dot) > 4) return 0;  /* 3 chars right of dot */
		if (strchr(dot+1,'.')) return 0; /* Only one dot */
		
		for (n = 0; name[n]; n++)
		{
			if (name[n] == '.') break;
			dirent[n+1] = toupper(name[n]);
		}
		for (; n < 8; n++) dirent[n+1] = ' ';
		for (n = 0; dot[n+1]; n++)
		{
			dirent[9 + n] = toupper(dot[n + 1]);
		}
		for (; n < 3; n++) dirent[n+9] = ' ';
		dirent[0] = uid;
	}
	if (stat(rcpmfs_mkname(self, realname), st)) 
	{
		RTRACE(("Failed to stat file\n"));
		return 0;   /* Can't stat file! */
	}
	if (!S_ISREG(st->st_mode))  
	{
		RTRACE(("Not a real file file\n"));
		return 0;   /* Not a real file */
	}

/* ISX gives the opposite meaning to the S1 byte */

	if (self->rc_fsversion == FSVERSION_ISX)
		dirent[DIR_S1] = (128 - (st->st_size & 0x7F)) & 0x7F;
	else	dirent[DIR_S1] = (st->st_size & 0x7F);

/* Set CP/M file attributes */
#if defined(HAVE_DIR_H)
	RTRACE(("Attributes from DOS attributes 0x%x\n", attributes)); 
	if (attributes & 1)	dirent[ 9] |= 0x80;
	if (attributes & 2)	dirent[10] |= 0x80;
	if (!(attributes & 32)) dirent[11] |= 0x80;
 #elif defined(HAVE_WINDOWS_H)
	RTRACE(("Attributes from Win32 attributes 0x%x\n", attributes)); 
	if (attributes   & FILE_ATTRIBUTE_READONLY)  dirent[ 9] |= 0x80;
	if (attributes   & FILE_ATTRIBUTE_HIDDEN)    dirent[10] |= 0x80;
	if (!(attributes & FILE_ATTRIBUTE_ARCHIVE))  dirent[11] |= 0x80;
#else
	RTRACE(("Attributes from Unix mode %o\n", st->st_mode)); 
	if (!(st->st_mode & S_IWUSR)) dirent[9] |= 0x80;
#endif
	return 1;
}


/* Read a specified directory entry */
static dsk_err_t rcpmfs_read_dirent(RCPMFS_DSK_DRIVER *self, unsigned entryno,
            unsigned char *entry, char *realname)
{
	dsk_lsect_t lsect;
	unsigned dirsecs;
	unsigned entriespersec;
	RCPMFS_BUFFER *rcb;
	char *map_entry;

	if (entryno >= rcpmfs_max_dirent(self)) 
	{
		fprintf(stderr,"Overrun: rcpmfs_read_dirent: entryno=%d max=%d\n",
				entryno, rcpmfs_max_dirent(self));
		return DSK_ERR_OVERRUN;
	}
	dirsecs	   = rcpmfs_secperblock(self) * self->rc_dirblocks;
	entriespersec = (self->rc_geom.dg_secsize) / 32;
	
	lsect = entryno / entriespersec;

/* Initialise the sector buffer. If it isn't found in the buffer chain, leave
 * it as 0xE5 */
	if (!self->rc_sectorbuf)
	{
		self->rc_sectorbuf = dsk_malloc(self->rc_geom.dg_secsize);
		if (!self->rc_sectorbuf) return DSK_ERR_NOMEM;
	}
	memset(self->rc_sectorbuf, 0xE5, self->rc_geom.dg_secsize);
	rcb = self->rc_bufhead;
	while (rcb)
	{
		if (rcb->rcb_lsect == lsect)
		{
			memcpy(self->rc_sectorbuf, rcb->rcb_data, 
				self->rc_geom.dg_secsize);
			break;
		}
		rcb = rcb->rcb_next;
	}
	/* Set the real name */
	if (realname)
	{
		map_entry = self->rc_namemap + NAMEMAP_ENTRYSIZE * entryno;
		strcpy(realname, map_entry);
	}
	/* Return the entry itself */
	if (entry)
	{
		entryno %= entriespersec;
		memcpy(entry, self->rc_sectorbuf + 32 * entryno, 32);
	}
	return DSK_ERR_OK;
}



/* Write a specified directory entry */
static dsk_err_t rcpmfs_write_dirent(RCPMFS_DSK_DRIVER *self, unsigned entryno,
	unsigned char *entry, char *realname)
{
	dsk_lsect_t lsect;
	dsk_err_t   err;
	unsigned dirsecs;
	unsigned entriespersec;
	RCPMFS_BUFFER *rcb;
	char *map_entry;

	RTRACE(("write dir entry: [%02x]%-11.11s %s\n", entry[0], entry + 1, realname));

	if (!realname) realname = "";

	if (entryno >= rcpmfs_max_dirent(self)) 
	{
		fprintf(stderr, "Overrun: rcpmfs_write_dirent: entryno=%d max=%d\n",
				entryno, rcpmfs_max_dirent(self));
		return DSK_ERR_OVERRUN;
	}

	dirsecs	   = rcpmfs_secperblock(self) * self->rc_dirblocks;
	entriespersec = (self->rc_geom.dg_secsize) / 32;
	
	lsect = entryno / entriespersec;

/* Initialise the sector buffer. If it isn't found in the buffer chain, this
 * is what gets written back */
	if (!self->rc_sectorbuf)
	{
		self->rc_sectorbuf = dsk_malloc(self->rc_geom.dg_secsize);
		if (!self->rc_sectorbuf) return DSK_ERR_NOMEM;
	}
	memset(self->rc_sectorbuf, 0xE5, self->rc_geom.dg_secsize);
	RTR_CHAIN("rcpmfs_write_dirent", self->rc_bufhead);	
	rcb = self->rc_bufhead;
	while (rcb)
	{
		if (rcb->rcb_lsect == lsect)
		{
			memcpy(self->rc_sectorbuf, rcb->rcb_data, 
				self->rc_geom.dg_secsize);
			break;
		}
		rcb = rcb->rcb_next;
	}
	/* Set the real name */
	map_entry = self->rc_namemap + NAMEMAP_ENTRYSIZE * entryno;
	strncpy(map_entry, realname, NAMEMAP_ENTRYSIZE-1);
	map_entry[NAMEMAP_ENTRYSIZE-1] = 0;

	entryno %= entriespersec;
	memcpy(self->rc_sectorbuf + 32 * entryno, entry, 32);
	err = rcpmfs_writebuffer(self, self->rc_sectorbuf, lsect);
	return err;
}

/* Add a new directory entry */
static dsk_err_t rcpmfs_add_dirent(RCPMFS_DSK_DRIVER *self, 
			unsigned char *entry, char *realname,
			struct stat *st)
{
	unsigned char timestamp[42];
	unsigned char *stamp;
	dsk_err_t err;

	/* See what we're going to write over */
	err = rcpmfs_read_dirent(self, self->rc_dirent, timestamp,NULL);
	if (err) return err;

	if (timestamp[0] != 0xE5) 
	{
	/* TODO: If the directory is full and there are timestamps, consider
	 * removing the timestamps to make more room */
		++self->rc_dirent;
		if (self->rc_dirent >= rcpmfs_max_dirent(self)) 
		{
			fprintf(stderr,"Overrun: rcpmfs_add_dirent: entryno=%d max=%d\n",
				self->rc_dirent, rcpmfs_max_dirent(self));
			return DSK_ERR_OVERRUN;
		}
	}
	/* Read the timestamp for the entry we're adding */
	err = rcpmfs_read_dirent(self, (self->rc_dirent | 3), timestamp, NULL);
	if (err) return err;
	err = rcpmfs_write_dirent(self, self->rc_dirent, entry, realname);
	if (err) return err;
	if (timestamp[0] == 0x21)
	{
		stamp = timestamp + 1 + 10 * (self->rc_dirent & 3);
		rcpmfs_time2cpm(st->st_atime, stamp);
		rcpmfs_time2cpm(st->st_mtime, stamp+4);
		stamp[8] = 0;   /* Protection mode */
		err = rcpmfs_write_dirent(self, (self->rc_dirent | 3), 
				timestamp, NULL);
		if (err) return err;
	}
	++self->rc_dirent;
	return DSK_ERR_OK;
}

/* Look up a block in the directory and find out what file owns it */
unsigned char *rcpmfs_lookup(RCPMFS_DSK_DRIVER *self, unsigned blockno,
		unsigned long *diroffs, char *filename)
{
	RCPMFS_BUFFER *rcb;
	dsk_err_t err;
	int blocks_per_extent, blkp, extsize;
	int nb;
	static unsigned char entry[32];
	unsigned entryno, entrymax;

	blocks_per_extent = rcpmfs_blocks_per_extent(self);
	extsize		   = rcpmfs_extent_size(self);
	RTR_CHAIN("rcpmfs_lookup", self->rc_bufhead);	
	rcb = self->rc_bufhead;
	entrymax = rcpmfs_max_dirent(self);
	for (entryno = 0; entryno < entrymax; entryno++)
	{
		err = rcpmfs_read_dirent(self, entryno, entry, filename);
		if (err) return NULL;

		/* Skip things that aren't files */
		if (entry[0] > 0x0F) continue;
		*diroffs = 0;
		for (nb = 0; nb < blocks_per_extent; nb++)
		{
/* For each block in each entry, see if it matches */
			if (blocks_per_extent == 16)
			{
				blkp = entry[16 + nb];
			}
			else
			{
				blkp = entry	   [16 + 2*nb];
				blkp += 256 * entry[17 + 2*nb];
			}
/* If block matches, find the mapped filename */
			if ((unsigned)blkp == blockno) 
			{
				return entry;
			}
			*diroffs += self->rc_blocksize;
		}
	}
	return NULL;
}

static dsk_err_t rcpmfs_chmod(RCPMFS_DSK_DRIVER *self, 
		unsigned char *dirent, const char *realname)
{
#ifdef HAVE_WINDOWS_H
	DWORD attrs = 0;
	if   (dirent[ 9] & 0x80)  attrs |= FILE_ATTRIBUTE_READONLY;
	if   (dirent[10] & 0x80)  attrs |= FILE_ATTRIBUTE_HIDDEN;
	if (!(dirent[11] & 0x80)) attrs |= FILE_ATTRIBUTE_ARCHIVE;
	if (!attrs) attrs |= FILE_ATTRIBUTE_NORMAL;

	SetFileAttributes(rcpmfs_mkname(self, realname), attrs);
#else
	mode_t attrs = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if (dirent[ 9] & 0x80)    attrs &= ~S_IWUSR;
	if (chmod(rcpmfs_mkname(self, realname), attrs))
	{
		RTRACE(("chmod('%s' -> '%s',%o) failed\n", 
			realname, rcpmfs_mkname(self, realname), attrs));
		return DSK_ERR_SYSERR;
	}
#endif
/* TODO: On systems with extended attributes, store F1-F4 and permissions
 * (and SYS/ARCV on Linux systems) */
	return DSK_ERR_OK;
}


/* Initialise CP/M directory */
static dsk_err_t rcpmfs_initdir(RCPMFS_DSK_DRIVER *self)
{
	unsigned entry;
	unsigned char label[32];
	char *sep, *dst;
	int n;
	dsk_err_t err = DSK_ERR_OK;
	struct stat st;

	self->rc_dirent = 0;
	if (self->rc_fsversion < FSVERSION_CPM3)
	{
		memset(label, 0x0E5, 32);
		/* 1.2.1: 'entry' is unsigned, so ">= 0" is always true */
		for (entry = rcpmfs_max_dirent(self); entry >= 1; entry--)
		{
			err = rcpmfs_write_dirent(self, entry - 1, label, NULL);
			if (err) break;
		}
		return err; 
	}
	/* Constructing a CP/M 3 filesystem. Let's start with the label. */
	memset(label, 0, sizeof(label));
	memset(label, 0x20, 12);
	sep = strrchr(self->rc_dir, SEPARATOR);
	if (sep) ++sep;
	else	 sep = self->rc_dir;
	dst = (char *)(label + 1);
	for (n = 0; n < 11; n++)
	{
		if (!sep[0]) break;
		/* If there's a dot, skip to the filetype */
		if (sep[0] == '.' && n > 0 && n < 8) 
		{
			n = 8; 
			++sep;
			continue;
		}
		dst[n] = toupper(*sep);
		++sep;  
	}
	label[12] = 0x61; /* Timestamp on access & update */
	self->rc_dirlabel = 0x61;
	if (!stat(self->rc_dir, &st))
	{
		rcpmfs_time2cpm(st.st_ctime, label + 0x18);
		rcpmfs_time2cpm(st.st_mtime, label + 0x1C);
	}
	err = rcpmfs_write_dirent(self, self->rc_dirent++, label, ".");
	if (err) return err;

	/* Write back an empty directory with timestamps every 4th slot */
	err = DSK_ERR_OK;
	for (entry = rcpmfs_max_dirent(self) - 1; entry >= 1; entry--)
	{
		if ((entry & 3) == 3) 
		{
			memset(label, 0, 32);
			label[0] = 0x21;
		}
		else
		{
			memset(label, 0x0E5, 32);
		}
		err = rcpmfs_write_dirent(self, entry, label, NULL);
		if (err) break;
	}
	return err;
}   




/* Read in the directory & convert to a CP/M directory. This is a rather
 * horrid mess of #defines because of the three not-quite-similar methods
 * various OSes give us for directory access. */
static dsk_err_t rcpmfs_readdir(RCPMFS_DSK_DRIVER *self)
{
#if defined(HAVE_DIRENT_H)
	DIR *direct;    /* We may need to emulate this for Windowses */
	struct dirent *entry;
#elif defined(HAVE_DIR_H)
	int done;
	struct ffblk entry;
	char findpath[PATH_MAX];
#elif defined(HAVE_WINDOWS_H)
	HANDLE direct;
	WIN32_FIND_DATA entry;
	char findpath[PATH_MAX];
#endif
	unsigned attributes;
	char *found;
	unsigned blockno, blocks_per_extent, exm, rollblock;
	unsigned char cpm_dirent[32];
	struct stat st;
	dsk_err_t err;
	unsigned numentries;	/* Number of dir entries for this file */
	unsigned numblocks; /* Number of blocks in this file */
	unsigned extent;
	int n, extblocks, rollback;
	unsigned long filesize, extsize;

	if (!self) return DSK_ERR_BADPTR;

	if (self->rc_namemap)
	{
		dsk_free(self->rc_namemap);
		self->rc_namemap = 0;
	}
	self->rc_namemap = dsk_malloc( NAMEMAP_ENTRYSIZE * 
				rcpmfs_max_dirent(self));
	if (!self->rc_namemap) return DSK_ERR_NOMEM;

	err = rcpmfs_initdir(self);
	if (err) return err;

	blockno = self->rc_dirblocks;

	exm		   = rcpmfs_get_exm(self);
	blocks_per_extent  = rcpmfs_blocks_per_extent(self);
	extsize		   = rcpmfs_extent_size(self);

#if defined(HAVE_DIRENT_H)
	direct = opendir(self->rc_dir);
	if (!direct) return DSK_ERR_SYSERR;
	while ((entry = readdir(direct)))
	{
		found = entry->d_name;
		attributes = 0;
#else
	strcpy(findpath, rcpmfs_mkname(self, "*.*"));
 #if defined(HAVE_DIR_H)
	done = findfirst(findpath, &entry, FA_RDONLY | FA_HIDDEN | FA_SYSTEM);
	while (!done)
	{
		found = entry.ff_name;
		attributes = entry.ff_attrib;
 #elif defined(HAVE_WINDOWS_H)
	direct = FindFirstFile(findpath, &entry);
	while (direct != INVALID_HANDLE_VALUE)
	{
		attributes = entry.dwFileAttributes;
		/* Win32: Prefer the short form of the filename */
		found = entry.cAlternateFileName;
		if (!found[0]) found = entry.cFileName;

	 /* If filename contains 2 dots, or starts with a dot, then it may
	  * be one of our specials (eg: .libdsk.ini, or a file in another
	  * user area. Either way, pass its real name, not the 8.3 short 
	  * version. */
		if (strstr(entry.cFileName, "..") || entry.cFileName[0] == '.')
		{
			found = entry.cFileName;
		}

 #endif
#endif
		RTRACE(("Scanning dir: Found %s\n", found));
		memset(cpm_dirent, 0, 32);
		/* Try to parse name as CP/M 8.3 name */
		if (rcpmfs_83name(self, found, cpm_dirent, &st, attributes)) 
		{
			RTRACE(("8.3 name is %-8.8s.%-3.3s\n", 
					cpm_dirent + 1, cpm_dirent + 9));
			/* Add this entry to the directory. Work out how many disk
			 * blocks it would get */
			numblocks = (st.st_size + (self->rc_blocksize -1)) /
					self->rc_blocksize;
			filesize = st.st_size;
/* numentries = number of directory entries for this file */
			numentries = (numblocks + blocks_per_extent - 1) / 
					blocks_per_extent;
   
			rollback = self->rc_dirent; 
			rollblock = blockno;
			if (numentries == 0) numentries = 1;
			extent = 0;
			while (numentries)
			{
/* Generate allocations for this extent */
				extblocks = numblocks;
				if ((unsigned)extblocks > blocks_per_extent) 
				extblocks = blocks_per_extent;
	
				memset(cpm_dirent + 16, 0, 16);
				for (n = 0; n < extblocks; n++)
				{
					if (blocks_per_extent == 16)
					{
						cpm_dirent[16+n] = blockno & 0xFF;
					}
					else
					{
						cpm_dirent[16+2*n] = blockno & 0xFF;
						cpm_dirent[17+2*n] = (blockno >>8 );
					}
					++blockno;
					if (blockno >= self->rc_totalblocks) break;
				}	/* end for */
				if (blockno >= self->rc_totalblocks) break;
/* Generate sizes for this extent */
				extsize = rcpmfs_extent_size(self);
				if (extsize > filesize) extsize = filesize;
				cpm_dirent[DIR_EX]  = (extent * (exm+1)) & 0x1F;
				cpm_dirent[DIR_EX] |= ((extsize + 127) / 16384) & exm;
				if (self->rc_fsversion == FSVERSION_ISX)
					cpm_dirent[DIR_S1]  = (unsigned char)(128 - (filesize & 0x7F)) & 0x7F;
				else	cpm_dirent[DIR_S1]  = (unsigned char)(filesize & 0x7F);
				cpm_dirent[DIR_S2]  = (extent * (exm+1)) / 32;
				cpm_dirent[DIR_RC]  = (unsigned char)((extsize + 127) / 128);
				filesize -= extsize;
				++extent;
/* Add extent to the directory */
				err = rcpmfs_add_dirent(self, cpm_dirent, found, &st);
				if (err == DSK_ERR_OVERRUN)
				{
					err = DSK_ERR_OK;
					break;
				}
				if (err) return err;

				numentries --;
				numblocks -= extblocks;
				/* If buffer full, add it to the chain */
			}	/* end while (numentris) */
		/* If disc full, rollback part-created file */
			if (blockno >= self->rc_totalblocks)
			{
				unsigned rnum = rollback;
RTRACE(("Disc full, rolling back this entry\n"));
				while (rnum < self->rc_dirent)
				{
					err = rcpmfs_read_dirent(self, rnum, cpm_dirent ,NULL);
					if (err) return err;
					if (cpm_dirent[0] < 16)
					{
						cpm_dirent[0] = 0xE5;
						err = rcpmfs_write_dirent(self, rnum, cpm_dirent ,NULL);
						if (err) return err;
					}
					++rnum;
				}
				blockno = rollblock;
				self->rc_dirent = rollback;
				break;
			} /* End if blockno >= self->rc_totalblocks */
		} /* End if 8.3 name valid */
/* The other end of the while loop */
#if defined(HAVE_DIRENT_H)
	}
	closedir(direct);
#elif defined(HAVE_DIR_H)
		done = findnext(&entry);
	}
#elif defined(HAVE_WINDOWS_H)
		if (!FindNextFile(direct, &entry))
		{
			FindClose(direct);
			direct = INVALID_HANDLE_VALUE;
		}
 	}
#endif
	return DSK_ERR_OK;
}





dsk_err_t rcpmfs_open(DSK_DRIVER *self, const char *passed)
{
	dsk_err_t err;
	struct stat st;
	RCPMFS_DSK_DRIVER *rcself;
	FILE *fp;
	char *dirname, *filename;

	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	/* Now find out if what we've been passed is a directory */
	if (stat(passed, &st)) return DSK_ERR_NOTME;
	if (!S_ISDIR(st.st_mode)) return DSK_ERR_NOTME;

	dirname = rcself->rc_dir;
	strncpy(dirname, passed, PATH_MAX - 1);
	dirname[PATH_MAX-1] = 0;
#ifdef HAVE_WINDOWS_H
	{ 
		char *s;
		/* Turn backslashes into nice sane forward slashes */
		while ( (s = strchr(dirname, '\\')) )
		{
			*s = SEPARATOR;
		}   
	}
#endif
	/* Chop off trailing directory separators */
	if (dirname[strlen(dirname)-1] == SEPARATOR)
	{
		dirname[strlen(dirname)-1] = 0;
	}

	/* Set geometry to sane defaults */
/* PCW720 defaults 
	err = dg_stdformat(&rcself->rc_geom, FMT_720K, NULL, NULL);
	if (err) return err;
	rcself->rc_blocksize = 2048;
	rcself->rc_dirblocks = 4;
	rcself->rc_totalblocks = 357;
	rcself->rc_systracks = 1;
	rcself->rc_fsversion = 3;
*/
/* PCW180 defaults */
	err = dg_stdformat(&rcself->rc_geom, FMT_180K, NULL, NULL);
	if (err) return err;
	rcself->rc_blocksize = 1024;
	rcself->rc_dirblocks = 2;
	rcself->rc_totalblocks = 175;
	rcself->rc_systracks = 1;
	rcself->rc_fsversion = FSVERSION_CPM3;
	rcself->rc_namemap = NULL;

	/* Now we have to find out if there's a configuration file */
	filename = rcpmfs_mkname(rcself, CONFIGFILE);
	fp = fopen(filename, "r");
	if (fp)
	{
		err = rcpmfs_parse(rcself, fp);
		fclose(fp);
		if (err) return err;
	}
	return rcpmfs_readdir(rcself);
}


dsk_err_t rcpmfs_creat(DSK_DRIVER *self, const char *passed)
{
	dsk_err_t err;
	struct stat st;
	RCPMFS_DSK_DRIVER *rcself;
	FILE *fp;
	char *dirname, *filename;

	/* Sanity check: Is this meant for our driver? */
	if (self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	/* Now find out if what we've been passed is a directory */
	if (stat(passed, &st)) 
	{
#ifdef HAVE_DIRECT_H
	if (_mkdir(passed)) return DSK_ERR_SYSERR;
#elif defined HAVE_WINDOWS_H
	if (mkdir(passed)) return DSK_ERR_SYSERR;
#else
	if (mkdir(passed, 0755)) return DSK_ERR_SYSERR;
#endif
	}
	else	if (!S_ISDIR(st.st_mode)) return DSK_ERR_NOTME;

	dirname = rcself->rc_dir;
	strncpy(dirname, passed, PATH_MAX - 1);
	dirname[PATH_MAX-1] = 0;

#ifdef HAVE_WINDOWS_H
	{ 
		char *s;
		/* Turn backslashes into nice sane forward slashes */
		while ( (s = strchr(dirname, '\\')) )
		{
			*s = SEPARATOR;
		}   
	}
#endif
	/* Chop off trailing directory separators */
	if (dirname[strlen(dirname)-1] == SEPARATOR)
	{
		dirname[strlen(dirname)-1] = 0;
	}

	/* Set geometry to sane defaults */
	err = dg_stdformat(&rcself->rc_geom, FMT_180K, NULL, NULL);
	if (err) return err;
	rcself->rc_blocksize = 1024;
	rcself->rc_dirblocks = 2;
	rcself->rc_totalblocks = 175;
	rcself->rc_systracks = 1;
	rcself->rc_fsversion = FSVERSION_CPM3;
	rcself->rc_namemap = NULL;
/* 720k defaults 
	err = dg_stdformat(&rcself->rc_geom, FMT_720K, NULL, NULL);
	if (err) return err;
	rcself->rc_blocksize = 2048;
	rcself->rc_dirblocks = 4;
	rcself->rc_totalblocks = 357;
	rcself->rc_systracks = 1;
	rcself->rc_fsversion = FSVERSION_CPM3;
*/
	/* Now we have to find out if there's a configuration file */
	filename = rcpmfs_mkname(rcself, CONFIGFILE);
	fp = fopen(filename, "r");
	if (fp)
	{
		err = rcpmfs_parse(rcself, fp);
		fclose(fp);
		if (err) return err;
	}
	else
	{
		fp = fopen(filename, "w");
		if (!fp) return DSK_ERR_SYSERR;
		err = rcpmfs_dump_options(rcself, fp);
		fclose(fp);
		if (err) return err;
	}
	/* Read in the directory, which we hope will be empty (but need not
	 * be) */
	return rcpmfs_readdir(rcself);
}



static void rcpmfs_free_buffers(RCPMFS_DSK_DRIVER *self)
{
	RCPMFS_BUFFER *rcb, *rcb2;

	/* Free buffers. The sector size may have changed */
	rcb = self->rc_bufhead;
	while (rcb)
	{
		rcb2 = rcb->rcb_next;
		dsk_free(rcb);
		rcb = rcb2;	
	}
	self->rc_bufhead = NULL;
}


dsk_err_t rcpmfs_close(DSK_DRIVER *self)
{
	RCPMFS_DSK_DRIVER *rcself;
	dsk_err_t err;

	if (self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	RTR_CHAIN("rcpmfs_close1", rcself->rc_bufhead);
	err = rcpmfs_flush(rcself);
	RTR_CHAIN("rcpmfs_close2", rcself->rc_bufhead);
	if (rcself->rc_namemap)
	{
		dsk_free(rcself->rc_namemap);
		rcself->rc_namemap = NULL;
	}
	if (rcself->rc_sectorbuf)
	{
		dsk_free(rcself->rc_sectorbuf);
		rcself->rc_sectorbuf = NULL;
	}
	return err;
}

/* Given a physical sector (with 0 being the first sector in the directory)
 * see if a disc file owns it. If so, sets filename / offset / bufsize 
 * appropriately. If not, sets filename to NULL. */
static dsk_err_t rcpmfs_psfind2(RCPMFS_DSK_DRIVER *self,
		char **filename, long *offset,
		dsk_lsect_t lsect, unsigned *bufsize)
{
	unsigned long blockno;
	unsigned secperblock;
	unsigned blockoffs;
	static char fnbuf[20];
	unsigned char *dirent;
	unsigned exm, extent;
	unsigned long diroffs, extent_len;

	if (!self || !filename || !offset || !bufsize) 
		return DSK_ERR_BADPTR;
	
	*filename = NULL;
	exm	    = rcpmfs_get_exm(self);
	secperblock = rcpmfs_secperblock(self);

	blockno   =  lsect / secperblock;
	blockoffs = (lsect - (blockno * secperblock)) * 
			self->rc_geom.dg_secsize;

	dirent = rcpmfs_lookup(self, blockno, &diroffs, fnbuf);

	/* No file owns this sector */
	if (!dirent)
	{
		return DSK_ERR_OK;
	}
	/* Now to find the offset */
	extent  = (dirent[DIR_EX] & 0x1F) + (dirent[DIR_S2] * 32);
	extent /= (exm + 1);

	*offset  = diroffs;
	*offset += blockoffs;
	*offset += (rcpmfs_extent_size(self) * extent);
	*filename = fnbuf;  
	*bufsize  = self->rc_geom.dg_secsize;

/* See how many bytes there are in the extent */
	extent_len = extent_bytes(self, dirent);
	if (dirent[DIR_S1])
	{
		if (self->rc_fsversion == FSVERSION_ISX)
			extent_len -= dirent[DIR_S1];
		else	extent_len = (extent_len - 128) + dirent[DIR_S1];
	}
/* And reduce buffer size accordingly */
	if (extent_len < (diroffs + blockoffs + bufsize[0]))
	{
		if (extent_len <= diroffs + blockoffs) bufsize[0] = 0;
		else	bufsize[0] = extent_len - (blockoffs + diroffs);	
	}
	RTRACE(("Sector 0x%lx (block %lx) is in file %s 0x%02x 0x%02x 0x%02x "
		" -> 0x%lx len 0x%lx\n", 
			lsect, blockno, fnbuf,
			dirent[DIR_EX], dirent[DIR_S1], dirent[DIR_S2],
			(unsigned long)(*offset), (unsigned long)(*bufsize)));

	return DSK_ERR_OK;
}


/* Given a physical sector, find out which file it's in. For metadata 
 * (the directory) and sectors pending writes, this will refer to a memory
 * buffer rather than a disk file.
 */
static dsk_err_t rcpmfs_psfind(RCPMFS_DSK_DRIVER *self,
		dsk_pcyl_t cylinder, dsk_phead_t head, 
		dsk_psect_t sector, char **filename, long *offset,
		unsigned char **buffer, dsk_lsect_t *lsect,
		unsigned *bufsize)
{
	dsk_lsect_t dir0;
	RCPMFS_BUFFER *rcb;

	if (!self || !filename || !offset || !buffer || !lsect || !bufsize) 
		return DSK_ERR_BADPTR;


	*filename = NULL;
	*buffer   = NULL;

	/* Find out what logical sector's wanted. */
	dg_ps2ls(&self->rc_geom, cylinder, head, sector, lsect);
	/* Find out where the directory starts */
	dir0 = self->rc_systracks * self->rc_geom.dg_sectors;

	RTRACE(("rcpmfs_psfind: cyl=%d hd=%d sec=%d -> sector %ld\n",
			cylinder, head, sector, *lsect));
	RTRACE(("dir0=%ld\n", dir0));

	if (lsect[0] < dir0) /* System track */
	{
		*filename = BOOTFILE;
		*offset   = lsect[0] * self->rc_geom.dg_secsize;
		*bufsize  = self->rc_geom.dg_secsize;
		RTRACE(("Sector found in boot image at %ld\n", *offset));
		return DSK_ERR_OK;
	}
	lsect[0] -= dir0;   /* Offset from directory */

	RTRACE(("\nLookup for sector: %ld\n", lsect[0]));
	/* See if it's in the buffer chain */
	RTR_CHAIN("rcpmfs_psfind", self->rc_bufhead);	
	rcb = self->rc_bufhead;
	while (rcb)
	{
		if (rcb->rcb_lsect == lsect[0])
		{
			*buffer   = rcb->rcb_data;
			*bufsize  = self->rc_geom.dg_secsize;
			return DSK_ERR_OK;
		}
		rcb = rcb->rcb_next;
	}
	return rcpmfs_psfind2(self, filename, offset, *lsect, bufsize);
}





dsk_err_t rcpmfs_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			void *buf, dsk_pcyl_t cylinder,
			dsk_phead_t head, dsk_psect_t sector)
{
	RCPMFS_DSK_DRIVER *rcself;
	long offset;
	char *filename;
	unsigned char *buffer;
	unsigned bufsize;
	dsk_err_t err;
	dsk_lsect_t lsect;
	int fr;
	if (!buf || !self || !geom || self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	/* Make sure that if we're pretending to be DD, we don't 
	 * return anything useful to someone asking about HD (or vice versa) */
	if (geom->dg_datarate != rcself->rc_geom.dg_datarate)
		return DSK_ERR_NOADDR;

	err = rcpmfs_psfind(rcself, cylinder, head, sector, 
			&filename, &offset, &buffer, &lsect, &bufsize);
	if (err) return err;

	RTRACE(("Read: Cyl %d Head %d Sec %d :: filename=%s offset=%ld buffer=%p\n",
			cylinder, head, sector, 
			filename ? filename : "(null)", offset, buffer));

	memset(buf, 0xE5, rcself->rc_geom.dg_secsize);
	if (buffer)
	{
		memcpy(buf, buffer, rcself->rc_geom.dg_secsize);
		return DSK_ERR_OK;
	}
	if (filename)
	{
		FILE *fp = fopen(rcpmfs_mkname(rcself, filename), "rb");
		if (fp)
		{
			if (fseek(fp, offset, SEEK_SET))
			{
				fprintf(stderr, "fseek failed: file=%s offset=%ld\n", filename, offset);
				fclose(fp);
				return DSK_ERR_SYSERR;
			}
/* If fread() fails, ignore it & just return a blank sector. It can also 
 * return a partial sector, if the file size isn't an exact multiple of 
 * the sector size. */
			fr = fread(buf, 1, rcself->rc_geom.dg_secsize, fp);
			if (fr < (int)rcself->rc_geom.dg_secsize)
			{
/* Pack the last 128-byte record of the file with 0x1A. This means text files 
 * as seen from CP/M will be terminated with 0x1A rather than 0xE5. */
				while (fr & 0x7F)
				{
					((unsigned char *)buf)[fr++] = 0x1A;
				}
			}	
			fclose(fp);
			return DSK_ERR_OK;
		}
	}
/* No buffer, no filename - just return a blank sector */
	return DSK_ERR_OK;
}

static void dump_dirent(unsigned char *entry)
{
	int n;
	RTRACE(("        "));
	for (n = 0; n < 16; n++)
	{
		RTRACE(("%02x ", entry[n]));
	}
	for (n = 0; n < 16; n++)
	{
		RTRACE(("%c", isprint(entry[n]) ? entry[n] : '.'));
	}
	RTRACE(("\n        "));
	for (n = 16; n < 32; n++)
	{
		RTRACE(("%02x ", entry[n]));
	}
	for (n = 16; n < 32; n++)
	{
		RTRACE(("%c", isprint(entry[n]) ? entry[n] : '.'));
	}
	RTRACE(("\n"));
}

/* Deal with a change to a directory entry */
static dsk_err_t rcpmfs_chgdir(RCPMFS_DSK_DRIVER *self, 
		unsigned entryno, unsigned char *old, unsigned char *new)
{
	dsk_err_t err;
	// JSIE-Fix: buffer too short to hold cp/m file names including user 
	// number and two more dots to separate user number from file name 
	//  char realname[14], newname[14];
	char realname[17], newname[17];
	FILE *fp;
	unsigned exm;
	unsigned oldextent, newextent;
	long oldlen, newlen;
	unsigned char entry[32]; 
	int n;

	RTRACE(("rcpmfs_chgdir (entry %d):\n", entryno));
	dump_dirent(old);
	RTRACE(("... to ...\n"));
	dump_dirent(new);


	exm	  = rcpmfs_get_exm(self);
	oldextent = ((old[DIR_S2] * 32) + (old[DIR_EX] & 31)) / (exm+1);
	newextent = ((new[DIR_S2] * 32) + (new[DIR_EX] & 31)) / (exm+1);

	RTRACE(("oldextent=%d newextent=%d\n", oldextent, newextent));

	if (old[0] == 0x20 && new[0] != 0x20) /* Directory label being removed */
	{
		self->rc_dirlabel = 0;
	}
	if (new[0] == 0x20) /* Directory label being written */
	{
		self->rc_dirlabel = new[DIR_EX];
	}
/* Creation and deletion requests. We only handle these for extent 0 */
	if (old[0] >= 0x10 && new[0] < 0x10 && (newextent == 0)) /* new file */
	{
		rcpmfs_cpmname(new, realname);
		RTRACE(("Create file: '%s'\n", realname));
		fp = fopen(rcpmfs_mkname(self, realname), "wb");
		if (!fp) return DSK_ERR_RDONLY;
		fclose(fp);
		return rcpmfs_write_dirent(self, entryno, new, realname);
	}
	else if (old[0] >= 0x10 && new[0] < 0x10) /* new extent */
	{
		rcpmfs_cpmname(new, realname);
		return rcpmfs_write_dirent(self, entryno, new, realname);
	}
	if (old[0] < 0x10 && new[0] >= 0x10 && (oldextent == 0)) 
	/* Erase file */
	{
		strcpy(realname, self->rc_namemap+ NAMEMAP_ENTRYSIZE* entryno);
		RTRACE(("Unlink file: '%s'\n", realname));
		if (remove(rcpmfs_mkname(self,realname))) return DSK_ERR_RDONLY;
		return rcpmfs_write_dirent(self, entryno, new, NULL);
	}
#if defined (HAVE_UTIME_H) || defined (HAVE_SYS_UTIME_H)
/* Timestamps. If writing back altered timestamps, and time stamping is 
 * turned on, use utime() on the disc files */
	if (new[0] == 0x21 && (self->rc_dirlabel & 0x60))  
	{
		unsigned eno = entryno - 3;
		struct utimbuf ut;

		for (n = 0; n < 3; n++)
		{
			if (memcmp(old + 1 + 10 * n, new + 1 + 10 * n, 8) ||
					old[0] != 0x21)
			{
				err = rcpmfs_read_dirent(self, eno+n, 
						entry, realname);
				if (!err)
				{
/* If CP/M has a particular type of time stamp turned off, stamp with now */
					if (self->rc_dirlabel & 0x40) 
						ut.actime  = rcpmfs_cpm2time(new + 10*n + 1);
					else	time(&ut.actime);
					if (self->rc_dirlabel & 0x20) 
						ut.modtime  = rcpmfs_cpm2time(new + 10*n + 1);
					else	time(&ut.modtime);
					utime(rcpmfs_mkname(self, realname), &ut);
				}
			}
		}
	}
#endif  /* def HAVE_UTIME_H */
	if (old[0] < 0x10 && new[0] < 0x10)
	{
		int renamed, chmodded;
		for (renamed = chmodded = 0, n = 1; n < 12; n++)
		{
			if ((old[n] & 0x7F) != (new[n] & 0x7F)) renamed = 1;
			if ((old[n] & 0x80) != (new[n] & 0x80)) chmodded = 1;
		}
		if (old[0] != new[0]) renamed = 1;	/* User number has changed */
		if (renamed)
		{
			err = rcpmfs_read_dirent(self, entryno, old, realname);
			if (err) return err;
			strcpy(realname, self->rc_namemap+ NAMEMAP_ENTRYSIZE* entryno);
			rcpmfs_cpmname(new, newname);

RTRACE(("Rename '%s' as '%s'", realname, newname));
			/* Rename extent 0. The others will follow suit. */
			if (newextent == 0)
				err = rcpmfs_rename(self, realname, newname);
/* This should not be necessary; rcpmfs_rename() should 
 * handle it.	
 * if (!err) err = rcpmfs_write_dirent(self, entryno, new, newname);
 */
			if (!err) err = rcpmfs_chmod(self, new, newname);
			if (err) return err;
		}
		else if (chmodded)
		{
			err = rcpmfs_read_dirent(self, entryno, old, realname);
			if (err) return err;
			err = rcpmfs_chmod(self, new, realname);
			if (err) return err;
		}
	}
	/* Change to a file's allocation. rcpmfs_flush() will
	 * handle files getting bigger; it can't handle files 
	 * getting smaller. */

	newlen = oldlen = 0;
	if (old[0] < 0x10 && new[0] < 0x10)
	{
		/* Check if length of this extent has changed */
		oldlen = extent_bytes(self, old);
		newlen = extent_bytes(self, new);
	}
	/* Whole extent has been blown away. Note the check for oldextent != 0;
	 * if extent zero is zapped, the whole file is assumed to have gone */
	else if (old[0] < 0x10 && new[0] == 0xE5 && oldextent != 0)
	{
		oldlen = extent_bytes(self, old);
		newlen = 0;
	}
	if (newlen < oldlen)
	{
		rcpmfs_cpmname(new, realname);
		RTRACE(("Reduce file size: %s oldlen=%ld newlen=%ld\n", rcpmfs_mkname(self,realname), oldlen, newlen));
		err = rcpmfs_adjust_size(self, oldlen - newlen, new[DIR_S1], rcpmfs_mkname(self,realname));
	}
/* File remains roughly, the same size, but Last Record Byte Count tweaked. */
	else if (old[0x0d] != new[0x0d] && (newextent == 0))	
	{
		rcpmfs_cpmname(new, realname);
		err = rcpmfs_adjust_size(self, 0, new[DIR_S1], rcpmfs_mkname(self,realname));
	}
	return DSK_ERR_OK;
}


/* For each buffered sector, see if it's in a file. If so, write it out.
 * If the whole sector (rather than just part) is in a file, then remove the
 * buffered copy */
static dsk_err_t rcpmfs_flush(RCPMFS_DSK_DRIVER *self)
{
	/* See if it's in the buffer chain */
	long dir_sectors = rcpmfs_secperblock(self) * self->rc_dirblocks;
	RCPMFS_BUFFER *rcb, *rcb2;
	dsk_err_t err;
	char *filename;
	long offset;
	unsigned bufsize;

RTR_CHAIN("rcpmfs_flush", self->rc_bufhead);
	rcb = self->rc_bufhead;
	while (rcb)
	{
		RTRACE(("rcpmfs_flush: Logical sector 0x%lx / 0x%lx\n", 
				rcb->rcb_lsect, (dsk_lsect_t)dir_sectors));
		RTR_CHAIN("rcpmfs_flush(1745)", self->rc_bufhead);
		if (rcb->rcb_lsect >= (dsk_lsect_t)dir_sectors)
		{
/* Find out if this is now part of a file  */
			RTR_CHAIN("rcpmfs_flush(1749)", self->rc_bufhead);
			err = rcpmfs_psfind2(self, &filename, &offset,
					rcb->rcb_lsect, &bufsize);
			if (err) return err;

			RTR_CHAIN("rcpmfs_flush(1754)", self->rc_bufhead);
			RTRACE(("rcpmfs_flush: bufsize=%d filename=%s\n",
						bufsize, filename));
			if (bufsize > 0 && filename != NULL)
			{
				err = rcpmfs_writefile(self, filename, offset, 
						rcb->rcb_data, bufsize);
			RTR_CHAIN("rcpmfs_flush(1761)", self->rc_bufhead);
				if (err) return err;
				if (bufsize == self->rc_geom.dg_secsize)
				{
RTR_CHAIN("Before drop", self->rc_bufhead);
/* Detach this buffer from the chain */
					if (self->rc_bufhead == rcb)
					{
						self->rc_bufhead = rcb->rcb_next;
					}
					for (rcb2 = self->rc_bufhead; 
						rcb2 != NULL;
						rcb2 = rcb2->rcb_next)
					{
						if (rcb2->rcb_next == rcb)
						rcb2->rcb_next = rcb->rcb_next;
					}
/* Free the buffer that's been written back */
					rcb2 = rcb;
					rcb = rcb->rcb_next;
					dsk_free(rcb2);
RTR_CHAIN("After drop", self->rc_bufhead);
					continue;
				}
			}
		}
		rcb = rcb->rcb_next;
		RTR_CHAIN("rcpmfs_flush(1790)", self->rc_bufhead);
	}
	RTR_CHAIN("rcpmfs_flush(1792)", self->rc_bufhead);
	return DSK_ERR_OK;
}



dsk_err_t rcpmfs_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			const void *buf, dsk_pcyl_t cylinder,
			dsk_phead_t head, dsk_psect_t sector)
{
	RCPMFS_DSK_DRIVER *rcself;
	long offset;
	char *filename;
	unsigned char *buffer;
	dsk_err_t err;
	dsk_lsect_t lsect;
	unsigned long dir_sectors;
	unsigned dir_entries;   /* Dir entries per sector */
	unsigned dir_first;
	unsigned entry;
	unsigned char *new, *old;
	unsigned bufsize;

	if (!buf || !self || !geom || self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	/* Make sure that if we're pretending to be DD, we don't 
	 * return anything useful to someone asking about HD (or vice versa) */
	if (geom->dg_datarate != rcself->rc_geom.dg_datarate)
		return DSK_ERR_NOADDR;

	offset = 0;
	err = rcpmfs_psfind(rcself, cylinder, head, sector, 
			&filename, &offset, &buffer, &lsect, &bufsize);
	if (err) return err;

	RTRACE(("Write: Cyl %d Head %d Sec %d :: filename=%s offset=0x%lx buffer=%p\n",
			cylinder, head, sector, 
			filename ? filename : "(null)", offset, buffer));

	dir_sectors = rcpmfs_secperblock(rcself) * rcself->rc_dirblocks;

/* Directory writes have to follow through to the underlying directory */
	if (buffer && lsect < dir_sectors)
	{
		dir_entries  = (rcself->rc_geom.dg_secsize / 32);   
		dir_first	= lsect * dir_entries;

		for (entry = 0; entry < dir_entries; entry++)
		{
			new = ((unsigned char *)buf) + 32 * entry;
			old = ((unsigned char *)buffer) + 32 * entry;

			if (memcmp(old, new, 32))
			{
				err = rcpmfs_chgdir(rcself, entry + dir_first,
						old, new);
				if (err) return err;
			}
		}
		memcpy(buffer, buf, rcself->rc_geom.dg_secsize);
	RTR_CHAIN("rcpmfs_write1", rcself->rc_bufhead);
		err =  rcpmfs_flush(rcself);
	RTR_CHAIN("rcpmfs_write2", rcself->rc_bufhead);
		return err;
	}

	if (buffer) /* Writing to a file buffer */
	{
		RTRACE(("Write: Write to buffer %p\n", buffer));
		memcpy(buffer, buf, rcself->rc_geom.dg_secsize);
		RTRACE(("rcpmfs_write: Wrote %02x %02x %02x\n",
			buffer[0], buffer[1], buffer[2]));
		return DSK_ERR_OK;
	}
	if (filename)   /* Writing to a disc file */
	{
/* If the full sector is not written to the file, then save a memory copy.
 * The file's directory entry may be extended after the file itself is 
 * written. */
		RTRACE(("Write: Write to file %s\n", filename));
		if (bufsize < rcself->rc_geom.dg_secsize)
		{
			rcpmfs_writebuffer(rcself, buf, lsect);
		}
		return rcpmfs_writefile(rcself, filename, offset, buf, bufsize);
	}
	RTRACE(("Write: No file or buffer found\n"));
	return rcpmfs_writebuffer(rcself, buf, lsect);
}






dsk_err_t rcpmfs_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
			dsk_pcyl_t cylinder, dsk_phead_t head,
			const DSK_FORMAT *format, unsigned char filler)
{
	RCPMFS_DSK_DRIVER *rcself;
	dsk_err_t err;
	dsk_psect_t sec;
	char *filename;
	FILE *fp;

	if (!self || !geom || self->dr_class != &dc_rcpmfs) 
		return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

/* 
 *  1. If the geometry doesn't match our current internal geometry, adopt
 *    the passed geometry and rewrite .libdsk.ini. Perhaps this should only
 *    be done if C0H0 is formatted?
 *
 *    Also: This will rewrite the geometry parameters, but not the CP/M 
 *    filesystem parameters. 
 */
	if (geom->dg_cylinders != rcself->rc_geom.dg_cylinders ||
	    geom->dg_sectors   != rcself->rc_geom.dg_sectors   ||
	    geom->dg_heads     != rcself->rc_geom.dg_heads     ||
	    geom->dg_sidedness != rcself->rc_geom.dg_sidedness ||
	    geom->dg_secsize   != rcself->rc_geom.dg_secsize   ||
	    geom->dg_secbase   != rcself->rc_geom.dg_secbase)
	{
/* If the sector size has changed, trash all buffers because they're going
 * to be the wrong size */
		if (geom->dg_secsize != rcself->rc_geom.dg_secsize)
		{
			if (rcself->rc_sectorbuf)
			{
				dsk_free(rcself->rc_sectorbuf);
				rcself->rc_sectorbuf
					= dsk_malloc(geom->dg_secsize);
				if (!rcself->rc_sectorbuf) return DSK_ERR_NOMEM;
				memset(rcself->rc_sectorbuf, 0xE5,
						geom->dg_secsize);
			}
			rcpmfs_free_buffers(rcself);
		}
		memcpy(&rcself->rc_geom, geom, sizeof(DSK_GEOMETRY));
    /* Recreate .libdsk.ini */
		filename = rcpmfs_mkname(rcself, CONFIGFILE);
		fp = fopen(filename, "w");
		if (fp)
		{
			err = rcpmfs_dump_options(rcself, fp);
			fclose(fp);
	 		if (err) return err;
		}
	}
/*
 *  2. Simulate a write of a buffer filled with the filler byte to each
 *    sector in the specified track.
 */ 
	if (rcself->rc_geom.dg_secsize < geom->dg_secsize)
	{
		dsk_free(rcself->rc_sectorbuf);
		rcself->rc_sectorbuf = NULL;		
	}
	if (!rcself->rc_sectorbuf)
	{ 
		rcself->rc_sectorbuf = dsk_malloc(geom->dg_secsize);
		if (!rcself->rc_sectorbuf) return DSK_ERR_NOMEM;
	}
	memset(rcself->rc_sectorbuf, filler, geom->dg_secsize);
	for (sec = 0; sec < geom->dg_sectors; sec++)
	{
		err = rcpmfs_write(self, geom, rcself->rc_sectorbuf,
			       cylinder, head, sec + geom->dg_secbase);	
		if (err) return err;	
	}
	return DSK_ERR_OK;
/*

    if (!rcself->px_fp) return DSK_ERR_NOTRDY;
    if (rcself->px_readonly) return DSK_ERR_RDONLY;

    // Convert from physical to logical sector. However, unlike the dg_* 
    // functions, this _always_ uses "SIDES_ALT" mapping; this is the 
    // mapping that both the Linux and NT floppy drivers use to convert 
    // offsets back into C/H/S. 
    offset = (cylinder * geom->dg_heads) + head;
    trklen = geom->dg_sectors * geom->dg_secsize;
    offset *= trklen;

    err = seekto(rcself, offset);
    if (err) return err;
    if (rcself->px_filesize < offset + trklen)
        rcself->px_filesize = offset + trklen;

    while (trklen--) 
        if (fputc(filler, rcself->px_fp) == EOF) return DSK_ERR_SYSERR; 

    return DSK_ERR_OK; */
}

    

dsk_err_t rcpmfs_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_pcyl_t cylinder, dsk_phead_t head)
{
	RCPMFS_DSK_DRIVER *rcself;
	long offset;
	char *filename;
	unsigned char *buffer;
	unsigned bufsize;
	dsk_err_t err;
	dsk_lsect_t lsect;

	if (!self || !geom || self->dr_class != &dc_rcpmfs) 
		return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	err = rcpmfs_psfind(rcself, cylinder, head, rcself->rc_geom.dg_secbase,
			&filename, &offset, &buffer, &lsect, &bufsize);
	if (err) return err;

	return DSK_ERR_OK;
}




dsk_err_t rcpmfs_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
				   dsk_phead_t head, unsigned char *result)
{
	RCPMFS_DSK_DRIVER *rcself;

	if (!self || !geom || self->dr_class != &dc_rcpmfs) 
		return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

/*  if (!rcself->px_fp) *result &= ~DSK_ST3_READY;
	if (rcself->px_readonly) *result |= DSK_ST3_RO; */
	return DSK_ERR_OK;
}


dsk_err_t rcpmfs_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	RCPMFS_DSK_DRIVER *rcself;

	if (!self || !geom || self->dr_class != &dc_rcpmfs)
			    return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;

	memcpy(geom, &rcself->rc_geom, sizeof(DSK_GEOMETRY));

	return DSK_ERR_OK;
}

dsk_err_t rcpmfs_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                     dsk_pcyl_t cyl, dsk_phead_t head, DSK_FORMAT *result)
{
	RCPMFS_DSK_DRIVER *rcself;

	if (!self || !geom || self->dr_class != &dc_rcpmfs || !result)
		return DSK_ERR_BADPTR;
	rcself = (RCPMFS_DSK_DRIVER *)self;
	if (cyl >= geom->dg_cylinders || head >= geom->dg_heads) 
		return DSK_ERR_NOADDR;
	result->fmt_sector = (rcself->rc_secid % geom->dg_sectors) 
		+ geom->dg_secbase;
	result->fmt_cylinder = cyl;
	result->fmt_head     = head;
	result->fmt_secsize  = geom->dg_secsize;
	return DSK_ERR_OK;
}


static dsk_err_t rcpmfs_update_config(RCPMFS_DSK_DRIVER *self)
{
	char *filename;
	FILE *fp;
	dsk_err_t err;

	/* System option changed. Update the configuration file. */
	filename = rcpmfs_mkname(self, CONFIGFILE);
	fp = fopen(filename, "w");
	if (!fp) return DSK_ERR_SYSERR;
	err = rcpmfs_dump_options(self, fp);
	fclose(fp);
	if (err) return err;


	return rcpmfs_readdir(self);
}

/* CP/M-specific filesystem parameters */
static char *option_names[] = 
{
	"FS:CP/M:BSH", "FS:CP/M:BLM", "FS:CP/M:EXM",
	"FS:CP/M:DSM", "FS:CP/M:DRM", "FS:CP/M:AL0", "FS:CP/M:AL1",
	"FS:CP/M:CKS", "FS:CP/M:OFF", "FS:CP/M:VERSION"
};

#define MAXOPTION (sizeof(option_names) / sizeof(option_names[0]))

dsk_err_t rcpmfs_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	if (!self) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;

	if (idx >= 0 && idx < MAXOPTION)
	{
		if (optname) *optname = option_names[idx];
		return DSK_ERR_OK;
	}
	return DSK_ERR_BADOPT;	

}

/* Set a driver-specific option */
dsk_err_t rcpmfs_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	RCPMFS_DSK_DRIVER *rcpmfs_self;
	int idx;
	unsigned dirents;

	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcpmfs_self = (RCPMFS_DSK_DRIVER *)self;

	for (idx = 0; idx < MAXOPTION; idx++)
	{
		if (!strcmp(optname, option_names[idx]))
			break;
	}
	if (idx >= MAXOPTION) return DSK_ERR_BADOPT;

	dirents = (rcpmfs_self->rc_dirblocks * rcpmfs_self->rc_blocksize) / 32;
	switch(idx)
	{

		/* If we change the block size, try to keep the directory
		 * size constant.
		 *
		 * If setting an option doesn't actually change anything, 
		 * return and don't bother rewriting .libdsk.ini 
		 */
		case 0: if (rcpmfs_self->rc_blocksize == (128 << value))
				return DSK_ERR_OK;
			rcpmfs_self->rc_blocksize = (128 << value);
			rcpmfs_self->rc_dirblocks = (dirents * 32) / 
				rcpmfs_self->rc_blocksize;
			break;
		case 1: if (rcpmfs_self->rc_blocksize == (128 * (1+value)))
				return DSK_ERR_OK;
			rcpmfs_self->rc_blocksize = 128 * (1 + value);
			rcpmfs_self->rc_dirblocks = (dirents * 32) / 
				rcpmfs_self->rc_blocksize;
			break;
		case 2: return DSK_ERR_RDONLY;	/* EXM can't be changed */
		case 3: if (rcpmfs_self->rc_totalblocks == (1 + value))
				return DSK_ERR_OK;
			rcpmfs_self->rc_totalblocks = value + 1;
			break;
		case 4: dirents = (32 * (value+1)) / rcpmfs_self->rc_blocksize;
			if (rcpmfs_self->rc_dirblocks == dirents)
				return DSK_ERR_OK;
			rcpmfs_self->rc_dirblocks = dirents;
			break;
		case 5: return DSK_ERR_RDONLY; /* AL0 can't be changed. */
		case 6: return DSK_ERR_RDONLY; /* AL1 can't be changed. */
		case 7: return DSK_ERR_RDONLY; /* CKS can't be changed. */
		case 8: if (rcpmfs_self->rc_systracks == value) 
				return DSK_ERR_OK;
			rcpmfs_self->rc_systracks = value;
			break;
		case 9:	if (rcpmfs_self->rc_fsversion == value)
				return DSK_ERR_OK;
			rcpmfs_self->rc_fsversion = value;
			break;
	}
	return rcpmfs_update_config(rcpmfs_self);
}


dsk_err_t rcpmfs_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	RCPMFS_DSK_DRIVER *rcpmfs_self;
	int idx, v = 0;
	unsigned al, dirents;

	if (!self || !optname) return DSK_ERR_BADPTR;
	if (self->dr_class != &dc_rcpmfs) return DSK_ERR_BADPTR;
	rcpmfs_self = (RCPMFS_DSK_DRIVER *)self;

	for (idx = 0; idx < MAXOPTION; idx++)
	{
		if (!strcmp(optname, option_names[idx]))
			break;
	}
	if (idx >= MAXOPTION) return DSK_ERR_BADOPT;

	al = (1 << 16) - (1 << (16 - rcpmfs_self->rc_dirblocks));
	dirents = (rcpmfs_self->rc_dirblocks * rcpmfs_self->rc_blocksize) / 32;
	switch(idx)
	{
		case 0: v = dsk_get_psh(rcpmfs_self->rc_blocksize); // BSH
			break;
		case 1: v = (rcpmfs_self->rc_blocksize / 128) - 1; // BLM
			break;
		case 2: v = rcpmfs_get_exm(rcpmfs_self);	// EXM
			break;
		case 3: v = rcpmfs_self->rc_totalblocks - 1;	// DSM
			break;
		case 4: v = dirents - 1;
			break;
		case 5: v = (al >> 8) & 0xFF;	// AL0
			break;
		case 6: v = (al & 0xFF);	// AL1
			break;
		case 7: v = (dirents / 4);	// CKS
			break;
		case 8: v = rcpmfs_self->rc_systracks;	// OFF
			break;
		case 9: v = rcpmfs_self->rc_fsversion;	// Filesystem version
			break;
	}
	if (value) *value = v;
	return DSK_ERR_OK;
}



#endif /* def HAVE_RCPMFS */
