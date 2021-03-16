/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2003  John Elliott <seasip.webmaster@gmail.com>            *
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


static DSK_REPORTFUNC st_repfunc;
static DSK_REPORTEND  st_repend;

/* Register callbacks for LibDsk functions to display information on the
 *  * screen. */
LDPUBLIC32 void LDPUBLIC16 dsk_reportfunc_set(DSK_REPORTFUNC report, 
                                              DSK_REPORTEND  repend)
{
	st_repfunc = report;
	st_repend  = repend;
}


/* Retrieve the values of the callbacks */
LDPUBLIC32 void LDPUBLIC16 dsk_reportfunc_get(DSK_REPORTFUNC *report, 
                                              DSK_REPORTEND  *repend)
{
	if (report) *report = st_repfunc;
	if (repend) *repend = st_repend;
}


LDPUBLIC32 void LDPUBLIC16 dsk_report(const char *s)
{
	if (st_repfunc) (*st_repfunc)(s);
}


LDPUBLIC32 void LDPUBLIC16 dsk_report_end()
{
	if (st_repend) (*st_repend)();
}

