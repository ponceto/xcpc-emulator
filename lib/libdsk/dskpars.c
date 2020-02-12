/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001, 2008  John Elliott <seasip.webmaster@gmail.com>      *
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

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_set_forcehead(DSK_DRIVER *self, int force)
{
	if (!self) return DSK_ERR_BADPTR;
	return dsk_set_option(self, "HEAD", force);
}

LDPUBLIC32 dsk_err_t LDPUBLIC16 dsk_get_forcehead(DSK_DRIVER *self, int *force)
{
	if (!self) return DSK_ERR_BADPTR;
	return dsk_get_option(self, "HEAD", force);
}

dsk_err_t dsk_isetoption(DSK_DRIVER *self, const char *name, int value, 
		int add_if_not_present)
{
	DSK_OPTION *opt, *last;
	int dummy;

	last = NULL;
	for (opt = self->dr_options; opt != NULL; opt = opt->do_next)
	{
		last = opt;
		if (!strcmp(opt->do_name, name))
		{
			opt->do_value = value;
			return DSK_ERR_OK;
		}
	}
	/* Option does not exist and cannot be added */
	if (!add_if_not_present) return DSK_ERR_BADOPT;

	/* Option does not exist in the generic store, but if its value can
	 * be obtained from the driver then we must not shadow it in the 
	 * generic store */
	if (dsk_get_option(self, name, &dummy) != DSK_ERR_BADOPT)
		return DSK_ERR_RDONLY;

	/* All right. Add the option to the generic store */
	opt = dsk_malloc(sizeof(DSK_OPTION) + strlen(name));
	if (opt == NULL) return DSK_ERR_NOMEM;
	opt->do_next = NULL;
	opt->do_value = value;
	strcpy(opt->do_name, name);
	if (last != NULL) last->do_next = opt;
	else		self->dr_options = opt;
	return DSK_ERR_OK;
}

LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_set_option(DSK_PDRIVER self, const char *name, int value)
{
        DRV_CLASS *dc;
	dsk_err_t err;

        if (!self || !name || !self->dr_class) return DSK_ERR_BADPTR;

        dc = self->dr_class;
/* First, give the driver class a crack at the option. */
	if (dc->dc_option_set) 
	{
		err = (*dc->dc_option_set)(self, name, value);	
		if (err != DSK_ERR_BADOPT)
		{
			return err;
		}
	}
/* Failing that, try the generic option handler */
	return dsk_isetoption(self, name, value, 0);
}


LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_get_option(DSK_PDRIVER self, const char *name, int *value)
{
	dsk_err_t err;
        DRV_CLASS *dc;
	DSK_OPTION *opt;

        if (!self || !name || !self->dr_class || !value) return DSK_ERR_BADPTR;

        dc = self->dr_class;

/* If a driver has a custom option getter/setter, use that */
	if (dc->dc_option_get)
	{
		err = (*dc->dc_option_get)(self, name, value);
		if (!err) return DSK_ERR_OK;
	}
/* Use the ones attached to the driver, if any */
	for (opt = self->dr_options; opt != NULL; opt = opt->do_next)
	{
		if (!strcmp(opt->do_name, name))
		{
			*value = opt->do_value;
			return DSK_ERR_OK;
		}
	}
	return DSK_ERR_BADOPT;
}


/* If "index" is in range, returns the n'th option name in (*optname).
 *  * Else sets (*optname) to null. */
LDPUBLIC32 dsk_err_t  LDPUBLIC16 dsk_option_enum(DSK_PDRIVER self, int idx, char **name)
{
        DRV_CLASS *dc;
	int optionals = 0;
	DSK_OPTION *opt;

        if (!self || !name || !self->dr_class) return DSK_ERR_BADPTR;

        dc = self->dr_class;
	*name = NULL;

	/* If we have any optional properties in the general store, 
	 * enumerate them first */
	for (opt = self->dr_options; opt != NULL; opt = opt->do_next)
	{
		if (optionals == idx)
		{
			*name = opt->do_name;
			return DSK_ERR_OK;
		}
		++optionals;
	}

	if (!dc->dc_option_enum) return DSK_ERR_OK;
	return (*dc->dc_option_enum)(self, idx - optionals, name);	
}

