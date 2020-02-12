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
#include "rpctios.h"
#include "crc16.h"

#ifdef HAVE_TERMIOS_H
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <termios.h>
#include <sys/time.h>
#include <errno.h>

#define SOH 1
#define STX 2
#define ACK 6
#define NAK 21

REMOTE_CLASS rpc_termios =
{
	sizeof(TERMIOS_REMOTE_DATA),
	"serial",
	"UNIX serial port",
	tios_open, 
	tios_close,
	tios_call
};

static void set_params (TERMIOS_REMOTE_DATA *self, struct termios *termios_p);
static dsk_err_t wrbyte(TERMIOS_REMOTE_DATA *self, unsigned char c);
static dsk_err_t read_bytes(TERMIOS_REMOTE_DATA *self, int count, 
		unsigned char *c);

static unsigned char crc16tab[512];

dsk_err_t tios_open(DSK_PDRIVER pDriver, const char *name, char *nameout)
{	
	char *sep;
	struct termios t;
	TERMIOS_REMOTE_DATA *self;
 
	self = (TERMIOS_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_termios) return DSK_ERR_BADPTR;
	if (strncmp(name, "serial:", 7)) return DSK_ERR_NOTME;
	name += 7;
	self->filename = dsk_malloc(strlen(name) + 1);
	if (!self->filename) return DSK_ERR_NOMEM;
	strcpy(self->filename, name);
	sep = strchr(self->filename, ',');
	if (sep) *sep = 0;
/* We have a filename. Let's open it up... */
	self->infd = open(self->filename, O_RDONLY | O_NONBLOCK);
	if (self->infd < 0) 
	{
		dsk_free(self->filename);
		self->filename = NULL;
		return DSK_ERR_SYSERR;
	}	
	self->outfd = open(self->filename, O_WRONLY | O_APPEND | O_NONBLOCK);
	if (self->outfd < 0) 
	{
		close(self->infd);
		dsk_free(self->filename);
		self->filename = NULL;
		return DSK_ERR_SYSERR;
	}	
/* Comms channels open. See if the user provided a baud rate */	
	self->baud = 9600;
	sep = strchr(name, ',');
/* If the filename has a comma, then the bit after the comma is options. 
 *
 * Option syntax is:
 * {baud}{+crtscts|-crtscts}
 *
 */ 
	if (sep)
	{
		char *opt1, *opt2, *opte;

		++sep;
		opt1 = strstr(sep, "+crtscts");
		opt2 = strstr(sep, "-crtscts");
		opte = strchr(sep, ',');
		if (!opte) opte = sep + strlen(sep);
		if      (opt1 && opt1 < opte) self->crtscts = 1;
		else if (opt2 && opt2 < opte) self->crtscts = 0;
		else                          self->crtscts = 1;
		self->baud = atoi(sep);
		name = sep;
	}
	tcgetattr(self->infd, &t);
	set_params(self, &t);
	tcsetattr(self->infd, TCSADRAIN, &t);
	tcgetattr(self->outfd, &t);
	set_params(self, &t);
	tcsetattr(self->outfd, TCSADRAIN, &t);
	sep = strchr(name, ',');
	if (sep) strcpy(nameout, sep + 1);
	else	 strcpy(nameout, "");	
	CRC_Init(crc16tab);
	return DSK_ERR_OK;
}


dsk_err_t tios_close(DSK_PDRIVER pDriver)
{
	TERMIOS_REMOTE_DATA *self = (TERMIOS_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_termios) return DSK_ERR_BADPTR;
	if (close(self->outfd)) return DSK_ERR_SYSERR;
	if (close(self->infd)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}


dsk_err_t tios_call(DSK_PDRIVER pDriver, unsigned char *input, 
		int inp_len, unsigned char *output, int *out_len)
{
	word16 wire_len;
	word16 crc;
	byte var;
	byte wvar[2];
	int n;
	dsk_err_t err;
	unsigned char *tmpbuf;

	TERMIOS_REMOTE_DATA *self = (TERMIOS_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_termios) return DSK_ERR_BADPTR;
	/* CRC tbe input... */
	wire_len = inp_len;
	CRC_Clear();
	for (n = 0; n < inp_len; n++) CRC_Update(input[n]);
	crc = CRC_Done();
/* 
	printf("rpc_tios: Input packet: ");
	for (n = 0; n < inp_len; n++) printf("%02x ", input[n]);
	printf("\n");
*/
	while (1)
	{
		/* Write SOH (start of request) */
		err = wrbyte(self, SOH); if (err) return err;
		/* Write packet length (network byte order) */
		var = wire_len >> 8; 
		err = wrbyte(self, var); if (err) return err;
		var = wire_len & 0xFF; 
		err = wrbyte(self, var); if (err) return err;
		for (n = 0; n < inp_len; n++) 
		{
			err = wrbyte(self, input[n]);
			if (err) return err;
		}
		var = crc >> 8; 
		err = wrbyte(self, var); if (err) return err;
		var = crc & 0xFF; 
		err = wrbyte(self, var); if (err) return err;

		tcdrain(self->infd);
		err = read_bytes(self, 1, &var); if (err) return err;
		if (var == ACK) break;
		if (var == NAK) continue;

		/* Not ACK or NAK. Could be a screaming TTY or anything. 
		 * Swallow all input. */
		while(read(self->infd, &var, 1) > 0);
	}
	/* Outgoing packet sent. Await response */
	while (1)
	{
		/* First byte of response must be STX. */
		err = read_bytes(self, 1, &var); if (err) return err;
		if (var != STX) continue;
		err = read_bytes(self, 2, wvar); if (err) return err;
		wire_len   = wvar[0];
		wire_len   = (wire_len << 8) | wvar[1];
		tmpbuf = dsk_malloc(wire_len + 2);
		if (!tmpbuf) return DSK_ERR_NOMEM;
		CRC_Clear();
		err = read_bytes(self, wire_len + 2, tmpbuf); 
		if (err) { dsk_free(tmpbuf); return err; }
		for (n = 0; n < wire_len; n++)
		{
			CRC_Update(tmpbuf[n]);
		}
		crc = tmpbuf[wire_len];
		crc = (crc << 8) | tmpbuf[wire_len + 1];
		/* If CRC matches, send ACK and return. Else send NAK. */
		if (crc == CRC_Done())
		{
/*
	printf("rpc_tios: Result packet: ");
	for (n = 0; n < wire_len; n++) printf("%02x ", tmpbuf[n]);
	printf("\n");
*/
			err = wrbyte(self, ACK);
			if (err) { dsk_free(tmpbuf); return err; }
/* Copy packet to waiting output buffer */
			if (wire_len < *out_len) *out_len = wire_len;
			memcpy(output, tmpbuf, *out_len);
			dsk_free(tmpbuf);
			return DSK_ERR_OK;
		}
/* Packet was garbled. NAK it and try again. */
		dsk_free(tmpbuf);
		err = wrbyte(self, NAK);
		if (err) return err;
	}
	/* Should never happen */
	return DSK_ERR_NOTIMPL;
}

static void set_params(TERMIOS_REMOTE_DATA *self, struct termios *termios_p)
{
	termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|
			IGNCR|ICRNL|IXON);
	termios_p->c_oflag &= ~OPOST;
	termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	/* Mask out parity and bits */
	termios_p->c_cflag &= ~(CSIZE|PARENB);
	/* Enable hardware handshaking */
	if (self->crtscts) termios_p->c_cflag |= CRTSCTS;
	else		   termios_p->c_cflag &= ~CRTSCTS;
	/* 1 stop bit */
	termios_p->c_cflag &= ~CSTOPB;
	/* 8 bits */
	termios_p->c_cflag |=  CS8;
	switch(self->baud)
	{
		case 50:     cfsetispeed(termios_p, B50); break;
		case 110:    cfsetispeed(termios_p, B110); break;
		case 134:    cfsetispeed(termios_p, B134); break;
		case 150:    cfsetispeed(termios_p, B150); break;
		case 300:    cfsetispeed(termios_p, B300); break;
		case 600:    cfsetispeed(termios_p, B600); break;
		case 1200:   cfsetispeed(termios_p, B1200); break;
		case 1800:   cfsetispeed(termios_p, B1800); break;
		case 2400:   cfsetispeed(termios_p, B2400); break;
		case 4800:   cfsetispeed(termios_p, B4800); break;
		case 19200:  cfsetispeed(termios_p, B19200); break;
		case 38400:  cfsetispeed(termios_p, B38400); break;
		case 57600:  cfsetispeed(termios_p, B57600); break;
		case 115200: cfsetispeed(termios_p, B115200); break;
		default:     fprintf(stderr, "%s: Unrecognised bitrate, using 9600\n", self->filename);
		case 9600:   cfsetispeed(termios_p, B9600); break;
	}
}

static dsk_err_t wrbyte(TERMIOS_REMOTE_DATA *self, unsigned char c)
{
	int tries, err;
	unsigned char var;

	for (tries = 0; tries < 30; tries++)
	{
		var = c;
		err = write(self->outfd, &var, 1);
		if (err == 1) 
		{
	/*		printf("Wrote %02x\n", c); fflush(stdout); */
			return DSK_ERR_OK;
		}
		if (err == -1 && errno == EAGAIN)
		{
			sleep(1);
			continue;
		}
		return DSK_ERR_SYSERR;
	}
	return DSK_ERR_TIMEOUT;	/* Timeout! */
}

/*
static dsk_err_t rdbyte(TERMIOS_REMOTE_DATA *self, unsigned char *c)
{
	int tries, err;

	printf("Reading\r"); fflush(stdout);
	for (tries = 0; tries < 30; tries++)
	{
		err = read(self->infd, c, 1);
		if (err > 0) 
		{
			printf("Read %02x\n", *c);
			fflush(stdout);
			return DSK_ERR_OK;
		}
		sleep(1);
	}
	return DSK_ERR_TIMEOUT;
}
*/


static dsk_err_t read_bytes(TERMIOS_REMOTE_DATA *self, int count, 
		unsigned char *c)
{
	int tries, err, count0;
	unsigned char *c0 ;

	c0 = c;
	count0 = count;
/*	printf("Reading %d bytes\n", count); fflush(stdout); */
	for (tries = 0; tries < 30;)
	{
		err = read(self->infd, c, count);
		if (err > 0) 
		{
			count -= err;
			c     += err;
/*
			printf("Got %d that time leaving %d\n", err, count);
			fflush(stdout);
*/
			if (count <= 0)
			{
/*				int n;
				printf("Read ");
				for (n = 0; n < count0; n++) printf("%02x ",
						c0[n]);
				printf("\n");
				fflush(stdout); */
				return DSK_ERR_OK;
			}
			continue;
		}
		sleep(1);
		++tries;
	}
	return DSK_ERR_TIMEOUT;
}



#endif /* def HAVE_TERMIOS_H */
