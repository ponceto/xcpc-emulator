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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "config.h"
#include "libdsk.h"
#include "drv.h"

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifdef HAVE_ASSERT_H
# include <assert.h>
#endif

#ifndef DISABLE_FLOPPY
# ifdef HAVE_LINUX_FD_H
#  include "linux/fd.h"
#  ifdef HAVE_LINUX_FDREG_H
#   define LINUXFLOPPY 
#   include "linux/fdreg.h"
#  endif
# endif

# ifdef HAVE_WINDOWS_H 
#  include <windows.h>
# endif

#ifdef HAVE_WINIOCTL_H
#  define WIN32FLOPPY 
#  define NTWDMFLOPPY 
#  include <winioctl.h>
# endif
#endif

/* See if we have any floppy drivers that take parameters of the form A: */
#ifdef WIN32FLOPPY 
# define ANYFLOPPY
#endif
#ifdef WIN16FLOPPY 
# define ANYFLOPPY
#endif
#ifdef DOS32FLOPPY 
# define ANYFLOPPY
#endif
#ifdef DOS16FLOPPY 
# define ANYFLOPPY
#endif

#if defined(HAVE_UNISTD_H)
#define HAVE_RCPMFS 1
#elif defined(_WIN32)
#define HAVE_RCPMFS 1
#elif defined(HAVE_DIR_H)
#define HAVE_RCPMFS 1
#endif


/* Initialise custom formats */
dsk_err_t dg_custom_init(void);
const char *dg_homedir(void);
const char *dg_sharedir(void);
dsk_err_t dg_parseline(char *linebuf, DSK_GEOMETRY *dg, char *description);
dsk_err_t dg_parse(FILE *fp, DSK_GEOMETRY *dg, char *description);
dsk_err_t dg_store(FILE *fp, DSK_GEOMETRY *dg, char *description);
/* The default geometry probe; driver geometry probes can call it */
dsk_err_t dsk_defgetgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);
/* The default system for storing optional integer properties */
dsk_err_t dsk_isetoption(DSK_DRIVER *self, const char *name, int value, 
		int add_if_not_present);


/* "Extended surface" discs have sector IDs that don't match the sectors' 
 * location on disc. This means, anywhere we reflect dsk_read() to dsk_xread(),
 * we have to translate the physical location to the sector ID field rather 
 * than assuming a 1:1 mapping. */

dsk_phead_t dg_x_head  (const DSK_GEOMETRY *dg, dsk_phead_t h);
dsk_psect_t dg_x_sector(const DSK_GEOMETRY *dg, dsk_phead_t h, dsk_psect_t s);


#ifdef AUTOSHARE
# define Q2(x) Q1(x)
# define Q1(x) #x
# define SHAREDIR Q2(AUTOSHARE)
#else
# define SHAREDIR NULL
#endif
