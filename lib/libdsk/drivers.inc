/***************************************************************************
 *                                                                         *
 *    LIBDSK: General floppy and diskimage access library                  *
 *    Copyright (C) 2001  John Elliott <seasip.webmaster@gmail.com>            *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
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

/* Note: Order does matter in this file.
 * This is the order in which drivers will be tested when format is being
 * autodetected. The order is:
 *
 * 1. Devices whose names aren't the names of files - ie, the Windows floppy
 */
#ifdef NTWDMFLOPPY
    &dc_ntwdm,
#endif
#ifdef WIN32FLOPPY
    &dc_win32,
#endif
#ifdef WIN16FLOPPY
    &dc_win16,
#endif
#ifdef DOS16FLOPPY
    &dc_dos16,
    &dc_dosint25,
#endif
#ifdef DOS32FLOPPY
    &dc_dos32,
    &dc_dosint25,
#endif
    &dc_remote,
/* 2. Directory-based backends */
#ifdef HAVE_RCPMFS
    &dc_rcpmfs,         /* rcpmfs needs unistd.h for truncate() */    
#endif

/* 3. Disc images which can be detected by magic numbers or other means. This 
 *    includes CPCEMU discs and Linux floppy drives. Linux floppy comes first
 *    because it makes detection of CPC formats much faster. */
#ifdef LINUXFLOPPY
    &dc_linux,
#endif
    &dc_cpcemu, /* Normal .DSK */
    &dc_cpcext, /* Extended .DSK */
/*  &dc_dqk,     * Compressed .DSK - now subsumed by compression
                         * library. */
    &dc_adisk,  /* APRIDISK image */
    &dc_qm,     /* CopyQM */
    &dc_tele,	/* Teledisk */
    &dc_imd,	/* IMAGEDISK IMD */
/*  &dc_dskf,	   IBM LoadDskF */
    &dc_ydsk,	/* YAZE YDSK */

/* 4. Raw files with no magic number. */
    &dc_posix,
/* 5. Raw files that will never be selected automatically. For example, 
 *   the MYZ80 drive image cannot be reliably identified, so it must be 
 *   explicitly specified. */
    &dc_myz80,
    &dc_simh,
    &dc_nwasp,
    &dc_logical,
    &dc_jv3,
    &dc_cfi,    /* RLE-compressed raw. The reasons that this isn't 
             * handled by the general compression system are:
             * 1. No magic number, so can't autodetect.
             * 2. Compression is per track, not for the file. 
             */
