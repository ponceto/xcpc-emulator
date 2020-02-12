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

/* Functions to convert logical <--> physical tracks/sectors */

#include "drvi.h"

/* Convert physical sector to logical */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_ps2ls(const DSK_GEOMETRY *self,  
	/* in */	dsk_pcyl_t cyl, dsk_phead_t head, dsk_psect_t sec,
	/* out */	dsk_lsect_t *logical)
{
	dsk_ltrack_t track;
	dsk_lsect_t sector;
	dsk_err_t e;

	e = dg_pt2lt(self, cyl, head, &track);
   if (e) return e;

	if (sec  < self->dg_secbase || 
	    sec  >= self->dg_secbase + self->dg_sectors) return DSK_ERR_BADPTR;

	sector = track * self->dg_sectors;
	sector += (sec - self->dg_secbase);

	if (logical) *logical = sector;

	return DSK_ERR_OK;
}

/* Convert logical sector to physical */
LDPUBLIC32 dsk_err_t LDPUBLIC16  dg_ls2ps(const DSK_GEOMETRY *self, 
	/* in */	dsk_lsect_t logical, 
	/* out */	dsk_pcyl_t *cyl, dsk_phead_t *head, dsk_psect_t *sec)
{
	dsk_err_t err;

	if (!self) return DSK_ERR_BADPTR;
	if (!self->dg_sectors || !self->dg_heads) return DSK_ERR_DIVZERO;

	if (logical >= self->dg_cylinders * self->dg_heads * self->dg_sectors)
		return DSK_ERR_BADPARM;

	if (sec)
	{
		if (self->dg_sidedness == SIDES_EXTSURFACE)
		{
			dsk_phead_t h;

			logical /= self->dg_sectors;
			err = dg_lt2pt(self, (dsk_ltrack_t)logical, cyl, &h);
			if (err) return err;
		
			*sec = (dsk_psect_t)((logical % self->dg_sectors) 
				+ h * self->dg_sectors 
				+ self->dg_secbase);

		}
		else
		{
			*sec = (dsk_psect_t)((logical % self->dg_sectors) + self->dg_secbase);
		}
	}

	logical /= self->dg_sectors;
	return dg_lt2pt(self, (dsk_ltrack_t)logical, cyl, head);
}



 
/* Convert physical cyl/head to logical track */
LDPUBLIC32 dsk_err_t LDPUBLIC16  dg_pt2lt(const DSK_GEOMETRY *self,  
	/* in */	dsk_pcyl_t cyl, dsk_phead_t head,
	/* out */	dsk_ltrack_t *logical)
{
	dsk_ltrack_t track = 0;

	if (!self          ) return DSK_ERR_BADPTR;
	if (!self->dg_heads) return DSK_ERR_DIVZERO;

	if (head >= self->dg_heads ||
	    cyl  >= self->dg_cylinders) return DSK_ERR_BADPARM;

	switch(self->dg_sidedness)
	{
		case SIDES_EXTSURFACE:	/* [1.3.7] */
		case SIDES_ALT:		track = (cyl * self->dg_heads) + head; break;
		case SIDES_OUTBACK:	if (self->dg_heads > 2) return DSK_ERR_BADPARM;
					if (!head)	track = cyl;
					else		track = (2 * self->dg_cylinders) - (1 + cyl); 
					break;
		case SIDES_OUTOUT:	track = (head * self->dg_cylinders) + cyl;
					break;
	}

	if (logical) *logical = track;

	return DSK_ERR_OK;
}

/* Convert logical track to physical */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dg_lt2pt(const DSK_GEOMETRY *self, 
	/* in */	dsk_ltrack_t logical, 
	/* out */	dsk_pcyl_t *cyl, dsk_phead_t *head)
{
	dsk_pcyl_t c = 0;
	dsk_phead_t h = 0;

	if (!self) return DSK_ERR_BADPTR;
	if (!self->dg_heads) return DSK_ERR_DIVZERO;

	if (logical >= self->dg_cylinders * self->dg_heads)
		return DSK_ERR_BADPARM;

	switch(self->dg_sidedness)
	{
		case SIDES_EXTSURFACE:	/* [1.3.7] */
		case SIDES_ALT:		c = (logical / self->dg_heads); 
					h = (logical % self->dg_heads); 
					break;
		case SIDES_OUTBACK:	if (self->dg_heads > 2) return DSK_ERR_BADPARM;
					if (logical < self->dg_cylinders)
					{
						c = logical;
						h = 0;
					}
					else
					{
						c = (2 * self->dg_cylinders) - (1 + logical); 
						h = 1;
					}
					break;
		case SIDES_OUTOUT:	c = (logical % self->dg_cylinders);
					h = (logical / self->dg_cylinders);
					break;
	}
	if (cyl)  *cyl  = c;
	if (head) *head = h;
	return DSK_ERR_OK;
}


/* "Extended surface" discs have sector IDs that don't match the sectors' 
 * location on disc. This means, anywhere we reflect dsk_read() to dsk_xread(),
 * we have to translate the physical location to the expected sector ID rather 
 * than assuming a 1:1 mapping. */

dsk_phead_t dg_x_head(const DSK_GEOMETRY *dg, dsk_phead_t h)
{
	if (dg == NULL) return h;

	return (dg->dg_sidedness == SIDES_EXTSURFACE) ? 0 : h;
}

dsk_phead_t dg_x_sector(const DSK_GEOMETRY *dg, dsk_phead_t h, dsk_psect_t sec)
{
	if (dg == NULL) return sec;

	if (dg->dg_sidedness == SIDES_EXTSURFACE) 
		sec += h * dg->dg_sectors;
	return sec;
}


