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

/* Verify a sector against the disc. Does not use native "verify" facilities -
 * just does a read followed by memcmp() */

#include "drvi.h"

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_pcheck(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
    DRV_CLASS *dc;
    void *buf2;
    dsk_err_t e = DSK_ERR_UNKNOWN;
    unsigned n;

    if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

    dc = self->dr_class;

        if (!dc->dc_read) return DSK_ERR_NOTIMPL;
    buf2 = dsk_malloc(geom->dg_secsize); 
    if (!buf2) return DSK_ERR_NOMEM;

    for (n = 0; n < self->dr_retry_count; n++)
    {
        e = (dc->dc_read)(self,geom,buf2,cylinder,head,sector);
        if (!DSK_TRANSIENT_ERROR(e)) break;
    }
    if (e == DSK_ERR_OK && memcmp(buf, buf2, geom->dg_secsize)) 
        e = DSK_ERR_MISMATCH;
    dsk_free(buf2);
    return e;   
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_lcheck(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_lsect_t sector)
{
    void *buf2;
    dsk_err_t e;

    if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

    buf2 = dsk_malloc(geom->dg_secsize);
    if (!buf2) return DSK_ERR_NOMEM;

    e = dsk_lread(self,geom,buf2,sector);
    if (e == 0 && memcmp(buf, buf2, geom->dg_secsize)) e = DSK_ERR_MISMATCH;
    dsk_free(buf2);
    return e;   
}


LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_xcheck(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
            const void *buf, 
                        dsk_pcyl_t cylinder,   dsk_phead_t head,
                        dsk_pcyl_t cyl_expect, dsk_phead_t head_expect,
                        dsk_psect_t sector, size_t sector_len)
{
    DRV_CLASS *dc;
    void *buf2;
    dsk_err_t e = DSK_ERR_UNKNOWN;
    unsigned n;

    if (!self || !geom || !buf || !self->dr_class) return DSK_ERR_BADPTR;

    dc = self->dr_class;

        if (!dc->dc_xread) return DSK_ERR_NOTIMPL;
    buf2 = dsk_malloc(geom->dg_secsize); 
    if (!buf2) return DSK_ERR_NOMEM;

    for (n = 0; n < self->dr_retry_count; n++)
    {
        e = (dc->dc_xread)(self,geom,buf2,cylinder,head,
                cyl_expect, head_expect, sector, sector_len, 0);
        if (!DSK_TRANSIENT_ERROR(e)) break;
    }
    if (e == DSK_ERR_OK && memcmp(buf, buf2, geom->dg_secsize)) 
        e = DSK_ERR_MISMATCH;
    dsk_free(buf2);
    return e;   
}

