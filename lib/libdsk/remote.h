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
 *  a pointer to remoting instance data (dr_remote). This in turn has a 
 *  pointer to the remoting function table. 
 */

typedef struct remote_data
{
	struct remote_class *rd_class;
/* rd_handle is a handle used by the remote end of the connection. Don't 
 * try to use it for local instance data. */ 
	unsigned rd_handle;
	unsigned *rd_functions;	/* Implemented functions */
	char *rd_name;		/* Remote system name */
	unsigned rd_testing;	/* Disable optimisations for testing? */
} REMOTE_DATA;

typedef struct remote_class
{
	size_t rc_selfsize;	/* Size of associated remote_data */

	const char *rc_name;	/* Short name of this protocol */
	const char *rc_desc;	/* Description of this protocol */

	/* Open the _connection_ (not anything at the target end; the 
	 * driver will then call dsk_r_open() or dsk_r_creat().) The
	 * filename passed in will be parsed, and nameout will be 
	 * the name that should be passed to dsk_r_open() / dsk_r_creat().
	 */
	dsk_err_t (*rc_open)(DSK_PDRIVER pDriver, const char *name,
				char *nameout);
	/* Close the connection. */
	dsk_err_t (*rc_close)(DSK_PDRIVER pDriver);

	/* Remote comms method. This must match the prototype of RPCFUNC 
	 *
	 * Entered with:
	 * 	input    = bytes to send
	 * 	inp_len  = number of bytes to send
	 * 	output   = receive buffer
	 *	*out_len = max number of bytes to receive
	 *
	 *  Return
	 *  	output contains received bytes
	 *  	*out_len = number of received bytes
	 * */
	dsk_err_t (*rc_call)(DSK_PDRIVER pDriver, unsigned char *input, 
		int inp_len, unsigned char *output, int *out_len);
} REMOTE_CLASS;


/* The 'remote' LibDsk driver: */

dsk_err_t remote_open(DSK_DRIVER *self, const char *filename);
dsk_err_t remote_creat(DSK_DRIVER *self, const char *filename);
dsk_err_t remote_close(DSK_DRIVER *self);
dsk_err_t remote_read(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t remote_write(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                              const void *buf, dsk_pcyl_t cylinder,
                              dsk_phead_t head, dsk_psect_t sector);
dsk_err_t remote_format(DSK_DRIVER *self, DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                const DSK_FORMAT *format, unsigned char filler);
dsk_err_t remote_secid(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                                dsk_pcyl_t cylinder, dsk_phead_t head,
                                DSK_FORMAT *result);
dsk_err_t remote_getgeom(DSK_DRIVER *self, DSK_GEOMETRY *geom);
dsk_err_t remote_status(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
                      dsk_phead_t head, unsigned char *result);

dsk_err_t remote_xseek(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		dsk_pcyl_t cylinder, dsk_phead_t head);

dsk_err_t remote_xread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
                      dsk_pcyl_t cylinder, dsk_phead_t head, 
		      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
		      dsk_psect_t sector, size_t sector_size, int *deleted);

dsk_err_t remote_xwrite(DSK_DRIVER *self, const DSK_GEOMETRY *geom, 
			const void *buf,
                      dsk_pcyl_t cylinder, dsk_phead_t head, 
		      dsk_pcyl_t cyl_expected, dsk_phead_t head_expected,
			dsk_psect_t sector, size_t sector_size, int deleted);

dsk_err_t remote_tread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
		                     dsk_pcyl_t cylinder, dsk_phead_t head);

dsk_err_t remote_xtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom, void *buf,
		        dsk_pcyl_t cylinder, dsk_phead_t head,
		        dsk_pcyl_t cyl_expected, dsk_phead_t head_expected);
/* List driver-specific options */
dsk_err_t remote_option_enum(DSK_DRIVER *self, int idx, char **optname);

/* Set a driver-specific option */
dsk_err_t remote_option_set(DSK_DRIVER *self, const char *optname, int value);
/* Get a driver-specific option */
dsk_err_t remote_option_get(DSK_DRIVER *self, const char *optname, int *value);

dsk_err_t remote_trackids(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
			  dsk_pcyl_t cylinder, dsk_phead_t head,
			  dsk_psect_t *count, DSK_FORMAT **result);

/* Read raw track, including sector headers */
dsk_err_t remote_rtread(DSK_DRIVER *self, const DSK_GEOMETRY *geom,
		       void *buf, dsk_pcyl_t cylinder,  dsk_phead_t head,
		       int reserved, size_t *bufsize);

