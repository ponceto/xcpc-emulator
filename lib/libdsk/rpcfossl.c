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
#include "rpcfossl.h"
#include "crc16.h"

#ifdef HAVE_DOS_H
#include <dos.h>

#define MAXTRIES 30

#define SOH 1
#define STX 2
#define ACK 6
#define NAK 21

REMOTE_CLASS rpc_fossil =
{
	sizeof(FOSSIL_REMOTE_DATA),
	"serial",
	"DOS16 serial port (FOSSIL)",
	fossil_open, 
	fossil_close,
	fossil_call
};

static dsk_err_t read_bytes(FOSSIL_REMOTE_DATA *self, int count, 
		unsigned char *c);
static dsk_err_t write_bytes(FOSSIL_REMOTE_DATA *self, int count, 
		unsigned char *c);

void sleep1()
{
	int n;
	unsigned cx, dx;	
	union REGS rg;

	rg.h.ah = 0;
	int86(0x1A, &rg, &rg);
	cx = rg.x.cx;
	dx = rg.x.dx;
	for (n = 0; n < 18; n++)
	{
/* Busywait for one system clock tick */
		do
		{
			rg.h.ah = 0;
			int86(0x1A, &rg, &rg);
		}
		while (rg.x.cx == cx && rg.x.dx == dx);
		cx = rg.x.cx;
		dx = rg.x.dx;
	}
}

static dsk_err_t wrbyte(FOSSIL_REMOTE_DATA *self, unsigned char c)
{
	union REGS rg;
	int tries;

	for (tries = 0; tries < MAXTRIES; tries++)
	{
		rg.h.ah = 3;
		rg.x.dx = self->portno;
/*		printf("W:INT 0x14: ah=0x03 dx=0x%04x\n", self->portno); */
		int86(0x14, &rg, &rg);
/*		printf("W:       -> ah=0x%02x\n", rg.h.ah); */
		if (rg.h.ah & 0x20)	/* Can output data */ 
		{
			rg.h.ah = 1;
			rg.h.al = c;
			rg.x.dx = self->portno;
			int86(0x14, &rg, &rg);
			return DSK_ERR_OK;
		}
		sleep1();
	}
	return DSK_ERR_TIMEOUT;
}


static dsk_err_t rdbyte(FOSSIL_REMOTE_DATA *self, unsigned char *c)
{
	union REGS rg;
	int tries;

	for (tries = 0; tries < MAXTRIES; tries++)
	{
		rg.h.ah = 3;
		rg.x.dx = self->portno;
/*		printf("R:INT 0x14: ah=0x03 dx=0x%04x\n", self->portno); */
		int86(0x14, &rg, &rg);
/*		printf("R:       -> ah=0x%02x\n", rg.h.ah); */
		if (rg.h.ah & 1) 	/* Input available */
		{
			rg.h.ah = 2;
			rg.x.dx = self->portno;
			int86(0x14, &rg, &rg);
			*c = rg.h.al;
			return DSK_ERR_OK;
		}
		sleep1();
	}
	return DSK_ERR_TIMEOUT;
}


static unsigned char crc16tab[512];

#ifdef HAVE_SYS_FARPTR_H
/* DPMI / far-pointer version; has to check for FOSSIL using farpeek 
 *  * functions.  */
#include <go32.h>
#include <sys/farptr.h>

static dsk_err_t check_fossil(void)
{
	unsigned short int14off = _farpeekl(_dos_ds, 4 * 0x14);
	unsigned short int14seg = _farpeekl(_dos_ds, 4 * 0x14 + 2);
	unsigned long  int14    = int14seg * 16 + int14off;

	if (_farpeekb(_dos_ds, int14 + 6) != 0x54 ||
	     _farpeekb(_dos_ds, int14 + 7) != 0x19)
	{
		return DSK_ERR_NOTME;
	}
	return DSK_ERR_OK;
}
#else

typedef unsigned char far *LPBYTE;
typedef LPBYTE far *LPLPBYTE;

static dsk_err_t check_fossil(void)
{
	LPLPBYTE int14vec = MK_FP(0, 4 * 0x14);
	LPBYTE int14 = int14vec[0];	
	if (int14[6] != 0x54 || int14[7] != 0x19)
	{
		return DSK_ERR_NOTME;
	}
	return DSK_ERR_OK;
}
#endif /* def HAVE_SYS_FARPTR_H */

dsk_err_t fossil_open(DSK_PDRIVER pDriver, const char *name, char *nameout)
{	
	char *sep;
	union REGS rg;
	FOSSIL_REMOTE_DATA *self;

	if (check_fossil()) return DSK_ERR_NOTME;
	self = (FOSSIL_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_fossil) return DSK_ERR_BADPTR;
	if (strncmp(name, "serial:", 7)) return DSK_ERR_NOTME;
	name += 7;
	self->filename = dsk_malloc(strlen(name) + 1);
	if (!self->filename) return DSK_ERR_NOMEM;
	strcpy(self->filename, name);
	sep = strchr(self->filename, ',');
	if (sep) *sep = 0;
	self->portno = atoi(self->filename);
/* We have a filename. Let's open it up... */

	rg.h.ah = 4;			/* Initialise driver */
	rg.x.dx = self->portno;		/* Port no. */
	int86(0x14, &rg, &rg);
	if (rg.x.ax != 0x1954)		/* FOSSIL init failed */
	{
		dsk_free(self->filename);
		self->filename = NULL;
		return DSK_ERR_NOTME;
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
		self->baud = atol(sep);
		name = sep;
	}
	rg.h.ah = 0;	/* Set baud rate */
	rg.x.dx = self->portno;
	switch(self->baud)
	{
/* Set up framing -- no parity, 1 stop bit, 8 bits, and specified baud rate */
		case 19200L: rg.h.al = 0x03; break;
		case 38400L: rg.h.al = 0x23; break;
		case   300L: rg.h.al = 0x43; break;
		case   600L: rg.h.al = 0x63; break;
		case  1200L: rg.h.al = 0x83; break;
		case  2400L: rg.h.al = 0xA3; break;
		case  4800L: rg.h.al = 0xC3; break;
		default:
			    fprintf(stderr, "Baud defaulting to 9600\n");
		case  9600L: rg.h.al = 0xE3; break;
	}
	int86(0x14, &rg, &rg);

/* Set handshaking */
	rg.h.ah = 0x0F;
	rg.x.dx = self->portno;
	rg.h.al = self->crtscts ? 2 : 0;
	int86(0x14, &rg, &rg);

	sep = strchr(name, ',');
	if (sep) strcpy(nameout, sep + 1);
	else	 strcpy(nameout, "");	
	CRC_Init(crc16tab);
	return DSK_ERR_OK;
}


dsk_err_t fossil_close(DSK_PDRIVER pDriver)
{
	union REGS rg;
	FOSSIL_REMOTE_DATA *self = (FOSSIL_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_fossil) return DSK_ERR_BADPTR;

	/* FOSSIL close */
	rg.h.ah = 5;
	rg.x.dx = self->portno;
	int86(0x14, &rg, &rg);

	return DSK_ERR_OK;
}


dsk_err_t fossil_call(DSK_PDRIVER pDriver, unsigned char *input, 
		int inp_len, unsigned char *output, int *out_len)
{
	word16 wire_len;
	word16 crc;
	byte var;
	byte wvar[2];
	int n;
	dsk_err_t err;
	unsigned char *tmpbuf;

	FOSSIL_REMOTE_DATA *self = (FOSSIL_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_fossil) return DSK_ERR_BADPTR;
	/* CRC tbe input... */
	wire_len = inp_len;
	CRC_Clear();
	for (n = 0; n < inp_len; n++) CRC_Update(input[n]);
	crc = CRC_Done();
/* 
	printf("rpc_fossil: Input packet: ");
	for (n = 0; n < inp_len; n++) printf("%02x ", input[n]);
	printf("\n");
*/
	while (1)
	{
		/* Write SOH (start of request) */
		err = wrbyte(self, SOH); if (err) return err;
		/* Write packet length (network byte order) */
		wvar[0] = wire_len >> 8; 
		wvar[1] = wire_len & 0xFF;
		err = write_bytes(self, 2, wvar); 
		if (err) return err;
		err = write_bytes(self, inp_len, input);
		if (err) return err;
		wvar[0] = crc >> 8; 
		wvar[1] = crc & 0xFF;
		err = write_bytes(self, 2, wvar); 
		if (err) return err;

/*
		err = read_bytes(self, 1, &var); if (err) return err;
*/
/* Wait 'til we get an ACK or a NAK. */
		do
		{
			err = read_bytes(self, 1, &var); 
			if (err) return err;
		}
		while(var != ACK && var != NAK);
		if (var == ACK) break;
		if (var == NAK) continue;
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
	printf("rpc_fossil: Result packet: ");
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


static dsk_err_t write_bytes(FOSSIL_REMOTE_DATA *self, 
		int count, unsigned char *c)
{
	int n;
	dsk_err_t err;

	for (n = 0; n < count; n++)
	{
		err = wrbyte(self, c[n]);
		if (err) return err;
	}
	return DSK_ERR_OK;
}

static dsk_err_t read_bytes(FOSSIL_REMOTE_DATA *self, 
		int count, unsigned char *c)
{
	int n;
	dsk_err_t err;

	for (n = 0; n < count; n++)
	{
		err = rdbyte(self, &c[n]);
		if (err) return err;
	}
	return DSK_ERR_OK;
}

#endif /* def HAVE_WINDOWS_H */
