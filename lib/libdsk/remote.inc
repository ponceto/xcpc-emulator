/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2005  John Elliott <seasip.webmaster@gmail.com>            *
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

/* RPC engines supported by LibDsk */

#if defined(HAVE_WINDOWS_H) && defined(_WIN32)
	&rpc_w32serial,	/* Win32 serial */
#endif
#ifdef HAVE_TERMIOS_H
	&rpc_termios,	/* TERMIOS serial */
#endif
#ifdef HAVE_FORK
	&rpc_fork,	/* Fork */
#endif
#ifdef HAVE_DOS_H
	&rpc_fossil,	/* MS-DOS FOSSIL */
#endif
/* XXX Let's have some others here, like rpc_tcp, rpc_laplink etc. */

