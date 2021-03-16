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

#include "drvi.h"
#include "remote.h"
#include "rpcfork.h"

#ifdef HAVE_FORK
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

REMOTE_CLASS rpc_fork =
{
	sizeof(FORK_REMOTE_DATA),
	"fork",
	"UNIX client using fork",
	fork_open, 
	fork_close,
	fork_call
};

typedef unsigned short word16;
typedef unsigned char byte;



dsk_err_t fork_open(DSK_PDRIVER pDriver, const char *name, char *nameout)
{	
	FORK_REMOTE_DATA *self;
	pid_t pidchild;
	int pipes[4];
	unsigned char fork_err[2];
	dsk_err_t err;
	char *comma;

	self = (FORK_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_fork) return DSK_ERR_BADPTR;
	if (strncmp(name, "fork:", 5)) return DSK_ERR_NOTME;
	name += 5;
	self->filename = dsk_malloc(strlen(name) + 1);
	if (!self->filename) return DSK_ERR_NOMEM;
	strcpy(self->filename, name);
	comma = strchr(self->filename, ',');
	if (comma) 
	{
		strcpy(nameout, comma + 1);
		comma[0] = 0;
	}
	else	strcpy(nameout, "");
/* We have a filename. Let's open it up... */

	if (pipe(pipes) || pipe(&pipes[2]))
	{
		return DSK_ERR_SYSERR;
	}
	pidchild = fork();
	if (pidchild < 0) 
	{
		dsk_free(self->filename);
		self->filename = NULL;
		return DSK_ERR_SYSERR;
	}

	if (pidchild == 0)
	{
/* We're the child process. Switch stdin and stdout 
 * over to the pipes, and then run the server. */
		dup2(pipes[0], 0);
		dup2(pipes[3], 1);
		execlp(self->filename, self->filename, NULL);
/* We're still running, so the server didn't launch. Report
 * error to the parent and then terminate. */
		fork_err[0] = (DSK_ERR_NOTME >> 8) & 0xFF;
		fork_err[1] = (DSK_ERR_NOTME     ) & 0xFF;
		write(pipes[3], fork_err, 2);
		exit(1);
	}
/* We're the parent process. Read error number from 
 * initial startup (or lack of startup) of child. */
	self->infd  = pipes[2];
	self->outfd = pipes[1];
	read(pipes[2], fork_err, 2);
	err = fork_err[0];
	err = (signed short)((err << 8) | fork_err[1]);
	return err;	/* DSK_ERR_OK; */
}


dsk_err_t fork_close(DSK_PDRIVER pDriver)
{
	FORK_REMOTE_DATA *self = (FORK_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_fork) return DSK_ERR_BADPTR;
	if (close(self->outfd)) return DSK_ERR_SYSERR;
	if (close(self->infd)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}


dsk_err_t fork_call(DSK_PDRIVER pDriver, unsigned char *input, 
		int inp_len, unsigned char *output, int *out_len)
{
	word16 wire_len;
	byte var;
	byte wvar[2];
	unsigned char *tmpbuf;

	FORK_REMOTE_DATA *self = (FORK_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_fork) return DSK_ERR_BADPTR;
	/* CRC tbe input... */
	wire_len = inp_len;
/* 
	printf("rpc_tios: Input packet: ");
	for (n = 0; n < inp_len; n++) printf("%02x ", input[n]);
	printf("\n");
 */
	/* Write packet length (network byte order) */
	var = wire_len >> 8; 
	if (write(self->outfd, &var, 1) < 1) return DSK_ERR_SYSERR;
	var = wire_len & 0xFF; 
	if (write(self->outfd, &var, 1) < 1) return DSK_ERR_SYSERR;
	if (write(self->outfd, input, inp_len) < inp_len) return DSK_ERR_SYSERR;

	/* Outgoing packet sent. Await response */
	if (read(self->infd, wvar, 2) < 2) return DSK_ERR_SYSERR;
	wire_len   = wvar[0];
	wire_len   = (wire_len << 8) | wvar[1];
	tmpbuf = dsk_malloc(wire_len);
	if (!tmpbuf) return DSK_ERR_NOMEM;
	if (read(self->infd, tmpbuf, wire_len) < 2) return DSK_ERR_SYSERR;
/* Copy packet to waiting output buffer */
	if (wire_len < *out_len) *out_len = wire_len;
	memcpy(output, tmpbuf, *out_len);
	dsk_free(tmpbuf);
	return DSK_ERR_OK;
}





#endif /* def HAVE_FORK */
