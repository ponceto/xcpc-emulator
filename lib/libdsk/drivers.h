/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001-2  John Elliott <seasip.webmaster@gmail.com>          *
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

/* This file lists all possible types of floppy driver in libdsk. Order does
 * not matter in this file. */

extern DRV_CLASS dc_cpcemu;	/* CPCEMU DSK driver */
extern DRV_CLASS dc_cpcext;	/* CPCEMU DSK driver: create in ext. format */
extern DRV_CLASS dc_dqk;	/* Compressed CPCEMU driver */
extern DRV_CLASS dc_posix;	/* POSIX driver */
extern DRV_CLASS dc_nwasp;	/* NanoWasp driver */
extern DRV_CLASS dc_myz80;	/* MYZ80 driver */
extern DRV_CLASS dc_cfi;	/* CFI driver */
extern DRV_CLASS dc_adisk;	/* APRIDISK driver */
extern DRV_CLASS dc_imd;	/* IMAGEDISK driver */
extern DRV_CLASS dc_jv3;	/* JV3 TRS-80 driver */
extern DRV_CLASS dc_qm;		/* CopyQM driver */
extern DRV_CLASS dc_tele;	/* Teledisk driver */
extern DRV_CLASS dc_dskf;	/* LoadDskF driver */
extern DRV_CLASS dc_ydsk;	/* YAZE YDSK driver */
extern DRV_CLASS dc_simh;	/* SIMH Altair driver */
extern DRV_CLASS dc_logical;	/* Raw, in logical sector order */
extern DRV_CLASS dc_rcpmfs;	/* Reverse-CP/MFS driver */
extern DRV_CLASS dc_remote;	/* All remote drivers */
#ifdef LINUXFLOPPY
extern DRV_CLASS dc_linux;	/* Linux driver */
#endif
#ifdef NTWDMFLOPPY
extern DRV_CLASS dc_ntwdm;	/* NT WDM driver */
#endif
#ifdef WIN32FLOPPY
extern DRV_CLASS dc_win32;	/* Win32 driver */
#endif
#ifdef WIN16FLOPPY
extern DRV_CLASS dc_win16;	/* Win16 driver */
#endif
#ifdef DOS32FLOPPY
extern DRV_CLASS dc_dos32;	/* DOS32 driver */
extern DRV_CLASS dc_dosint25;	/* DOS (INT 25h) driver */
#endif
#ifdef DOS16FLOPPY
extern DRV_CLASS dc_dos16;	/* DOS16 driver */
extern DRV_CLASS dc_dosint25;	/* DOS (INT 25h) driver */
#endif

