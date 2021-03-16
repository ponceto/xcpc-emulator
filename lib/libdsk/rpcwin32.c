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
#include "rpcwin32.h"
#include "crc16.h"

#if defined(HAVE_WINDOWS_H) && defined (_WIN32)

#define SOH 1
#define STX 2
#define ACK 6
#define NAK 21

REMOTE_CLASS rpc_w32serial =
{
	sizeof(W32SERIAL_REMOTE_DATA),
	"serial",
	"Win32 serial port",
	w32serial_open, 
	w32serial_close,
	w32serial_call
};

static dsk_err_t read_bytes(W32SERIAL_REMOTE_DATA *self, int count, 
		unsigned char *c);
static dsk_err_t write_bytes(W32SERIAL_REMOTE_DATA *self, int count, 
		unsigned char *c);

static dsk_err_t wrbyte(W32SERIAL_REMOTE_DATA *self, unsigned char c)
{
	return write_bytes(self, 1, &c);
}

static unsigned char crc16tab[512];
static COMMTIMEOUTS timeouts = { 0, 1000, 300000, 1000, 30000 };

dsk_err_t w32serial_open(DSK_PDRIVER pDriver, const char *name, char *nameout)
{	
	char *sep;
	W32SERIAL_REMOTE_DATA *self;
 
	self = (W32SERIAL_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_w32serial) return DSK_ERR_BADPTR;
	if (strncmp(name, "serial:", 7)) return DSK_ERR_NOTME;
	name += 7;
	self->filename = dsk_malloc(strlen(name) + 1);
	if (!self->filename) return DSK_ERR_NOMEM;
	strcpy(self->filename, name);
	sep = strchr(self->filename, ',');
	if (sep) *sep = 0;
/* We have a filename. Let's open it up... */
	self->hComm = CreateFile(self->filename, GENERIC_READ | GENERIC_WRITE,
			0,      /* Exclusive access */
			NULL,   /* No security */
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if (self->hComm == INVALID_HANDLE_VALUE) 
	{
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
        memset(&self->dcb, 0, sizeof(self->dcb));
	self->dcb.DCBlength = sizeof(self->dcb);
	self->dcb.Parity   = NOPARITY;
	self->dcb.fParity  = 0;
	self->dcb.fBinary  = 1;
	self->dcb.StopBits = ONESTOPBIT;
	self->dcb.ByteSize = 8;
	self->dcb.BaudRate = self->baud;
	if (self->crtscts)
	{
		self->dcb.fOutxCtsFlow = 1;
		self->dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	}
	SetCommState(self->hComm, &self->dcb);
	SetCommTimeouts(self->hComm, &timeouts);

	sep = strchr(name, ',');
	if (sep) strcpy(nameout, sep + 1);
	else	 strcpy(nameout, "");	
	CRC_Init(crc16tab);
	return DSK_ERR_OK;
}


dsk_err_t w32serial_close(DSK_PDRIVER pDriver)
{
	W32SERIAL_REMOTE_DATA *self = (W32SERIAL_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_w32serial) return DSK_ERR_BADPTR;
	if (!CloseHandle(self->hComm)) return DSK_ERR_SYSERR;
	return DSK_ERR_OK;
}


dsk_err_t w32serial_call(DSK_PDRIVER pDriver, unsigned char *input, 
		int inp_len, unsigned char *output, int *out_len)
{
	word16 wire_len;
	word16 crc;
	byte var;
	byte wvar[2];
	int n;
	dsk_err_t err;
	unsigned char *tmpbuf;

	W32SERIAL_REMOTE_DATA *self = (W32SERIAL_REMOTE_DATA *)pDriver->dr_remote;	
	if (!self || self->super.rd_class != &rpc_w32serial) return DSK_ERR_BADPTR;
	/* CRC tbe input... */
	wire_len = inp_len;
	CRC_Clear();
	for (n = 0; n < inp_len; n++) CRC_Update(input[n]);
	crc = CRC_Done();
/* 
	printf("rpc_w32serial: Input packet: ");
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
	printf("rpc_w32serial: Result packet: ");
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


static dsk_err_t write_bytes(W32SERIAL_REMOTE_DATA *self, 
		int count, unsigned char *c)
{
	BOOL b;
	DWORD nwritten;

	b = WriteFile(self->hComm, c, count, &nwritten, NULL);
	if (b) return DSK_ERR_OK;
	return DSK_ERR_SYSERR;
}

static dsk_err_t read_bytes(W32SERIAL_REMOTE_DATA *self, 
		int count, unsigned char *c)
{
	BOOL b;
	DWORD nread;

	b = ReadFile(self->hComm, c, count, &nread, NULL);
	if (b) return DSK_ERR_OK;
	return DSK_ERR_SYSERR;
}

#endif /* def HAVE_WINDOWS_H */
