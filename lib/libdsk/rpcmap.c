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

/* Mapping functions to map a DSK_PDRIVER to/from an int. */

#include "drvi.h"


static DSK_PDRIVER *mapping = NULL;
unsigned int maplen = 0;

/* Initialise the "mapping" table */
static dsk_err_t check_mapping()
{
	unsigned int n;
	if (!mapping)
	{
		mapping = dsk_malloc(16 * sizeof(DSK_PDRIVER));
		if (!mapping) 
		{
			maplen = 0;
			return DSK_ERR_NOMEM;
		}
		maplen = 16;
		for (n = 0; n < maplen; n++) mapping[n] = NULL;
	}
	return DSK_ERR_OK;
}


/* Note that we leave entry 0 as NULL. 
 * Add a new integer -> DSK_PDRIVER mapping */
/* No need for this function to be public */
/* LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_add(DSK_PDRIVER ptr, unsigned int *n) */
static dsk_err_t dsk_map_add(DSK_PDRIVER ptr, unsigned int *n)
{
	unsigned int m;
	DSK_PDRIVER *newmap;
	dsk_err_t err;

	if (!n) return DSK_ERR_BADPTR;

	err = check_mapping();
	if (err) 
	{
		*n = 0;
		return err;
	}

	for (*n = 1; (*n) < maplen; (*n)++) if (mapping[*n] == NULL)
	{
		mapping[*n] = ptr;
		return DSK_ERR_OK;	
	}
	/* All slots used... */
	newmap = dsk_malloc(maplen * 2 * sizeof(DSK_PDRIVER));
	if (!newmap) 
	{
		*n = 0;
		return DSK_ERR_NOMEM;
	}
	for (m = 1; m < maplen; m++) newmap[m] = mapping[m];
	free(mapping);
	mapping[*n = maplen] = ptr;
	maplen *= 2;
	return DSK_ERR_OK;
}





/* Given a DSK_PDRIVER, find its integer mapping. Allocate a new entry
 * if required. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_dtoi(DSK_PDRIVER ptr, unsigned int *n)
{
	unsigned int m;
	dsk_err_t err;

	if (!n)  return DSK_ERR_BADPTR;
	if (!ptr)	/* Map NULL to ID zero */
	{
		*n = 0; 
		return DSK_ERR_OK;
	}
	err = check_mapping();
	if (err) 
	{
		*n = 0;
		return err;
	}
	for (m = 0; m < maplen; m++)
	{
		if (mapping[m] == ptr) 
		{
			*n = m;
			return DSK_ERR_OK;
		}
	}
	/* Not found. Add a new mapping. */
	return dsk_map_add(ptr, n);
}



/* Given an integer, return the corresponding DSK_PDRIVER. */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_itod(unsigned int n, DSK_PDRIVER *ptr)
{
	dsk_err_t err;

	if (!ptr)  return DSK_ERR_BADPTR;
	err = check_mapping();
	if (err) 
	{
		*ptr = NULL;
		return err;
	}
	if (!n || n >= maplen)	/* Map NULL to ID zero */
	{
		*ptr = NULL; 
		return DSK_ERR_OK;
	}
	*ptr = mapping[n];
	return DSK_ERR_OK;
}



/* Remove an integer <--> DSK_DRIVER mapping. If it was the last one, free
 * all the memory used by the mapping */
LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_map_delete(unsigned int index)
{
	unsigned int n;

	if (mapping)
	{
		/* Trying to unmap something that isn't mapped... */
		if (index == 0 || index >= maplen) return DSK_ERR_BADPTR;
		if (mapping[index] == NULL)        return DSK_ERR_BADPTR;

		mapping[index] = NULL;
		for (n = 0; n < maplen; n++) 
		{
			if (mapping[n]) return DSK_ERR_OK;
		}
		dsk_free(mapping);
		mapping = NULL;
		maplen = 0;
	}	
	return DSK_ERR_OK;
}
