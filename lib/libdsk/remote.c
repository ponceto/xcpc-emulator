/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001,2005  John Elliott <seasip.webmaster@gmail.com>       *
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

/*  LibDsk remoting is handled much like compression; the DSK_DRIVER contains
 *  a pointer to remote data, which is handled by 
 */
#include "drvi.h"
#include "rpcfuncs.h"
#include "remote.h"
#include "remall.h"	/* All remote drivers */

DRV_CLASS dc_remote =
{
	sizeof(DSK_DRIVER),	/* [deliberately] No extra data */
	"remote",
	"Remote LibDsk instance",
	remote_open,
	remote_creat,
	remote_close, 
	remote_read,
	remote_write,
	remote_format,
	remote_getgeom,
	remote_secid,
	remote_xseek,
	remote_status,
	remote_xread,
	remote_xwrite,
	remote_tread,
	remote_xtread,
	remote_option_enum,
	remote_option_set,
	remote_option_get,
	remote_trackids,
	remote_rtread
};

/* All classes of remote driver */
static REMOTE_CLASS *classes[] = {

#include "remote.inc"

	NULL
};

int implements(DSK_DRIVER *self, int function)
{
	int n;

	/* If we don't have the list of implemented functions, assume 
	 * everything is implemented */
	if (!self->dr_remote->rd_functions)  return 1;
	/* If in test mode, bypass this optimisation */
	if (self->dr_remote->rd_testing)     return 1;

	for (n = 0; self->dr_remote->rd_functions[n]; n++)
	{
		if ((int)self->dr_remote->rd_functions[n] == function)
			return 1;
	}	
	return 0;
}

static dsk_err_t remote_lookup(DSK_DRIVER *self, const char *filename, 
		char **fileout, char **outtype, char **outcomp)
{
	int n, len;
	dsk_err_t err;

	/* Note no class check on 'self'. All the required data for remote 
	 * operation are deliberately in the base class. */
	if (filename == NULL || self == NULL || fileout == NULL) 
		return DSK_ERR_BADPTR;

	*fileout = dsk_malloc(1 + strlen(filename));
	if (!*fileout) return DSK_ERR_NOMEM;

	for (n = 0; classes[n]; n++)
	{
		len = strlen(classes[n]->rc_name);
/* If filename starts "classname:" then we have a match */
		if (strncmp(filename, classes[n]->rc_name, len) 
			|| filename[len] != ':') continue;
/* Allocate the amount of data it wants */
		self->dr_remote = dsk_malloc(classes[n]->rc_selfsize);
		if (!self->dr_remote)
		{
			if (*fileout) dsk_free(*fileout);
			*fileout = NULL;
			return DSK_ERR_NOMEM;
		}
		memset(self->dr_remote, 0, classes[n]->rc_selfsize);
		self->dr_remote->rd_class = classes[n];
/* Try to open the connection. If it fails then tidy everything away */
		err = (*classes[n]->rc_open)(self, filename, *fileout);
		if (err)
		{
			dsk_free(self->dr_remote);
			self->dr_remote = NULL;
			if (err != DSK_ERR_NOTME)
			{
				dsk_free(*fileout);
				*fileout = NULL;
				return err;
			}
		}
/* Split the filename we've been given into (name,type,compression) */
		else 
		{
			char *comma;
			*outtype = NULL;
			*outcomp = NULL;

			comma = strchr(*fileout, ',');
			if (!comma) return DSK_ERR_OK;
			*outtype = comma + 1;
			*comma = 0;
			comma = strchr(*outtype, ',');
			if (!comma) return DSK_ERR_OK;
			*outcomp = comma + 1;
			*comma = 0;
			return DSK_ERR_OK;
		}
	}
	if (*fileout) dsk_free(*fileout);
	*fileout = NULL;
	return DSK_ERR_NOTME;
}

dsk_err_t remote_open(DSK_DRIVER *self, const char *filename)
{
	RPCFUNC function;
	char *outname, *outtype, *outcomp, *comment;

	dsk_err_t err = remote_lookup(self, filename, &outname, &outtype, &outcomp);

	if (err) return err;
	function = self->dr_remote->rd_class->rc_call;
	err = dsk_r_open(self, function, &self->dr_remote->rd_handle, 
			 	outname, outtype, outcomp);
	dsk_free(outname);
	if (err) return err;
	err = dsk_r_properties(self, function, self->dr_remote->rd_handle);
	if (err) return err;
/*	if (implements(self, RPC_DSK_GETCOMMENT))
	{ */
		err = dsk_r_get_comment(self, function, 
				self->dr_remote->rd_handle, &comment);
		if (err) return err;
		if (comment != NULL) err = dsk_set_comment(self, comment);
/*	}	 */
	return err;
}

dsk_err_t remote_creat(DSK_DRIVER *self, const char *filename)
{
	RPCFUNC function;
	char *outname, *outtype, *outcomp, *comment;

	dsk_err_t err = remote_lookup(self, filename, &outname, &outtype, &outcomp);

	if (err) return err;
	function = self->dr_remote->rd_class->rc_call;
	err = dsk_r_creat(self, function, &self->dr_remote->rd_handle, 
			 	outname, outtype, outcomp);
	dsk_free(outname);
	if (err) return err;
	err = dsk_r_properties(self, function, self->dr_remote->rd_handle);
	if (err) return err;
/*	if (implements(self, RPC_DSK_GETCOMMENT))
	{ */
		err = dsk_r_get_comment(self, function, 
				self->dr_remote->rd_handle, &comment);
		if (err) return err;
		if (comment != NULL) err = dsk_set_comment(self, comment);
/*	} */
	return err;
}


dsk_err_t remote_close(DSK_DRIVER *self)
{
	dsk_err_t err;
	RPCFUNC function;
	if (self == NULL || self->dr_remote == NULL) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	/* Update the comment (if any) */
	if (implements(self, RPC_DSK_SETCOMMENT))
	{
		char *cmt;
		err = dsk_get_comment(self, &cmt);
		if (!err) dsk_r_set_comment(self, function, 
			self->dr_remote->rd_handle, cmt);
	}	

	/* Close the file */
	dsk_r_close(self, function, self->dr_remote->rd_handle);
	/* Close the connection */
	err = (*self->dr_remote->rd_class->rc_close)(self);
	if (self->dr_remote->rd_functions)
		dsk_free(self->dr_remote->rd_functions);
	if (self->dr_remote->rd_name)
		dsk_free(self->dr_remote->rd_name);
	dsk_free(self->dr_remote);
	return err;
}

dsk_err_t remote_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	RPCFUNC function;
	if (!self || !geom || !buf || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_PREAD)) return DSK_ERR_NOTIMPL;

	return dsk_r_read(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head, sector);
}

dsk_err_t remote_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector)
{
	RPCFUNC function;
	if (!self || !geom || !buf || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_PWRITE)) return DSK_ERR_NOTIMPL;

	return dsk_r_write(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head, sector);
}



dsk_err_t remote_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler)
{
	RPCFUNC function;
	if (!self || !geom || !format || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_PFORMAT)) return DSK_ERR_NOTIMPL;
	return dsk_r_format(self, function, self->dr_remote->rd_handle,
			geom, cylinder, head, format, filler);
}


dsk_err_t remote_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result)
{
	RPCFUNC function;
	if (!self || !geom || !result || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_PSECID)) return DSK_ERR_NOTIMPL;
	return dsk_r_secid(self, function, self->dr_remote->rd_handle,
			geom, cylinder, head, result);
}


dsk_err_t remote_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom)
{
	RPCFUNC function;
	if (!self || !geom || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_GETGEOM)) return DSK_ERR_NOTIMPL;
	return dsk_r_getgeom(self, function, self->dr_remote->rd_handle,
			geom);
}

dsk_err_t remote_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		                dsk_pcyl_t cylinder, dsk_phead_t head)
{
	RPCFUNC function;
	if (!self || !geom || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_PSEEK)) return DSK_ERR_NOTIMPL;
	return dsk_r_pseek(self, function, self->dr_remote->rd_handle,
			geom, cylinder, head);

}


dsk_err_t remote_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result)
{
	RPCFUNC function;
	if (!self || !geom || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_DRIVE_STATUS)) return DSK_ERR_NOTIMPL;
	return dsk_r_drive_status(self, function, self->dr_remote->rd_handle,
			geom, head, result);
}


dsk_err_t remote_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                      dsk_pcyl_t cylinder, dsk_phead_t head, 
		      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
		      dsk_psect_t sector, size_t sector_size, int *deleted)
{
	RPCFUNC function;
	if (!self || !geom || !buf || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_XREAD)) return DSK_ERR_NOTIMPL;
	return dsk_r_xread(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head, cyl_expected, head_expected,
			sector, sector_size, deleted);

}



dsk_err_t remote_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			const void *buf,
                      dsk_pcyl_t cylinder, dsk_phead_t head, 
		      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			dsk_psect_t sector, size_t sector_size, int deleted)
{
	RPCFUNC function;
	if (!self || !geom || !buf || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_XWRITE)) return DSK_ERR_NOTIMPL;
	return dsk_r_xwrite(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head, cyl_expected, head_expected,
			sector, sector_size, deleted);

}


dsk_err_t remote_tread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
		                     dsk_pcyl_t cylinder, dsk_phead_t head)
{
	RPCFUNC function;
	if (!self || !geom || !buf || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_PTREAD)) return DSK_ERR_NOTIMPL;
	return dsk_r_tread(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head);
	
}

dsk_err_t remote_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
		        dsk_pcyl_t cylinder, dsk_phead_t head,
		        dsk_pcyl_t cyl_expected, dsk_phead_t head_expected)
{
	RPCFUNC function;
	if (!self || !geom || !buf || !self->dr_remote) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_XTREAD)) return DSK_ERR_NOTIMPL;
	return dsk_r_xtread(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head, cyl_expected, 
			head_expected);
	
}
/* List driver-specific options */
dsk_err_t remote_option_enum(DSK_DRIVER *self, int idx, char **optname)
{
	RPCFUNC function;
	if (!self || !optname) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_OPTION_ENUM)) return DSK_ERR_NOTIMPL;
	return dsk_r_option_enum(self, function, self->dr_remote->rd_handle,
			idx, optname);
}

/* Set a driver-specific option */
dsk_err_t remote_option_set(DSK_DRIVER *self, const char *optname, int value)
{
	RPCFUNC function;
	if (!self || !optname) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

/* We also support this option, which does not show up in dsk_option_enum
 * (because it would be quite tricky to get right) */
	if (!strcmp(optname, "REMOTE:TESTING"))
	{
		self->dr_remote->rd_testing = value;
		return DSK_ERR_OK;
	}

	if (!implements(self, RPC_DSK_OPTION_SET)) return DSK_ERR_NOTIMPL;
	return dsk_r_option_set(self, function, self->dr_remote->rd_handle,
			optname, value);
}
/* Get a driver-specific option */
dsk_err_t remote_option_get(DSK_DRIVER *self, const char *optname, int *value)
{
	RPCFUNC function;
	if (!self || !optname || !value) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!strcmp(optname, "REMOTE:TESTING"))
	{
		*value = self->dr_remote->rd_testing;
		return DSK_ERR_OK;
	}

	if (!implements(self, RPC_DSK_OPTION_GET)) return DSK_ERR_NOTIMPL;
	return dsk_r_option_get(self, function, self->dr_remote->rd_handle,
			optname, value);
}

dsk_err_t remote_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			  dsk_pcyl_t cylinder, dsk_phead_t head,
			  dsk_psect_t *count, DSK_FORMAT **result)
{
	RPCFUNC function;
	if (!self || !geom || !count || !result) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_TRACKIDS)) return DSK_ERR_NOTIMPL;
	return dsk_r_trackids(self, function, self->dr_remote->rd_handle,
			geom, cylinder, head, count, result);
}

/* Read raw track, including sector headers */
dsk_err_t remote_rtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		       void *buf, dsk_pcyl_t cylinder,  dsk_phead_t head,
		       int reserved, size_t *bufsize)
{
	RPCFUNC function;
	if (!self || !geom || !buf) return DSK_ERR_BADPTR;
	function = self->dr_remote->rd_class->rc_call;

	if (!implements(self, RPC_DSK_RTREAD)) return DSK_ERR_NOTIMPL;
	return dsk_r_rtread(self, function, self->dr_remote->rd_handle,
			geom, buf, cylinder, head, reserved, bufsize);
}

