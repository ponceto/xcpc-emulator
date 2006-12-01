/* lib765.h - Copyright (c) 2002, 2003, 2004 John Elliott - 2005 Philip Kendall
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __LIB765_H__
#define __LIB765_H__

#include <stdarg.h>
#include <limits.h>
#include <libdsk.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ############################################################################
 * Defines
 * ############################################################################
 */

#define MAX_SECTOR_LEN  8192
#define SHORT_TIMEOUT   1000
#define LONGER_TIMEOUT  1333333

/* Floppy drive types */
#define FD_NONE          ( 0)
#define FD_30            ( 1)
#define FD_35            ( 2)
#define FD_525           ( 3)

/* Floppy drive errors */
#define FD_E_OK          ( 0)
#define FD_E_SEEKFAIL    (-1)
#define FD_E_NOADDR      (-2)
#define FD_E_NODATA      (-3)
#define FD_E_DATAERR     (-4)
#define FD_E_NOSECTOR    (-5)
#define FD_E_NOTRDY      (-6)
#define FD_E_READONLY    (-7)

/* Floppy drive dirty flags */
#define FD_D_UNAVAILABLE (-1)
#define FD_D_CLEAN       ( 0)
#define FD_D_DIRTY       ( 1)

/*
 * ############################################################################
 * Types declarations
 * ############################################################################
 */
typedef unsigned char fdc_byte;
typedef short         fd_err_t;

/*
 * ############################################################################
 * FDD declarations
 * ############################################################################
 */
typedef struct fdd_765        FDD_765;
typedef struct fdd_765_vtable FDD_765_VTABLE;

struct fdd_765 {
  FDD_765_VTABLE *fd_vtable;
  int             fd_type;
  int             fd_heads;
  int             fd_cylinders;
  int             fd_readonly;
  int             fd_motor;
  int             fd_cylinder;
  char            fdl_filename[PATH_MAX + 1];
  const char     *fdl_type;
  const char     *fdl_compress;
  DSK_PDRIVER     fdl_diskp;
  DSK_GEOMETRY    fdl_diskg;
};

struct fdd_765_vtable {
  fd_err_t  (*fdv_seek_cylinder)(FDD_765 *fdd, int cylinder);
  fd_err_t  (*fdv_read_id      )(FDD_765 *fdd, int head, int sector, fdc_byte *buf);
  fd_err_t  (*fdv_read_sector  )(FDD_765 *fdd, int xcylinder, int xhead, int head, int sector, fdc_byte *buf, int len, int *deleted, int skip_deleted, int mfm, int multi);
  fd_err_t  (*fdv_read_track   )(FDD_765 *fdd, int xcylinder, int xhead, int head, fdc_byte *buf, int *len);
  fd_err_t  (*fdv_write_sector )(FDD_765 *fdd, int xcylinder, int xhead, int head, int sector, fdc_byte *buf, int len, int deleted, int skip_deleted, int mfm, int multi);
  fd_err_t  (*fdv_format_track )(FDD_765 *fdd, int head, int sectors, fdc_byte *buf, fdc_byte filler);
  fdc_byte  (*fdv_drive_status )(FDD_765 *fdd);
  int       (*fdv_ready        )(FDD_765 *fdd);
  void      (*fdv_eject        )(FDD_765 *fdd);
};

extern void fdd_init_impl (FDD_765 *fdd);
extern void fdd_reset_impl(FDD_765 *fdd);

extern fd_err_t fdd_seek_cylinder(FDD_765 *fdd, int cylinder);
extern fd_err_t fdd_read_id      (FDD_765 *fdd, int head, int sector, fdc_byte *buf);
extern fd_err_t fdd_read_sector  (FDD_765 *fdd, int xcylinder, int xhead, int head, int sector, fdc_byte *buf, int len, int *deleted, int skip_deleted, int mfm, int multi);
extern fd_err_t fdd_read_track   (FDD_765 *fdd, int xcylinder, int xhead, int head, fdc_byte *buf, int *len);
extern fd_err_t fdd_write_sector (FDD_765 *fdd, int xcylinder, int xhead, int head, int sector, fdc_byte *buf, int len, int deleted, int skip_deleted, int mfm, int multi);
extern fd_err_t fdd_format_track (FDD_765 *fdd, int head, int sectors, fdc_byte *track, fdc_byte filler);
extern int      fdd_ready        (FDD_765 *fdd);

/*
 * ############################################################################
 * FDC declarations
 * ############################################################################
 */
typedef struct fdc_765        FDC_765;
typedef void (*FDC_ISR)(FDC_765 *fdc, int status);

extern void fdc_init_impl (FDC_765 *fdc);
extern void fdc_reset_impl(FDC_765 *fdc);

extern fdc_byte fdc_rd_stat  (FDC_765 *fdc);
extern fdc_byte fdc_rd_data  (FDC_765 *fdc);
extern void     fdc_wr_data  (FDC_765 *fdc, fdc_byte value);
extern void     fdc_set_motor(FDC_765 *fdc, fdc_byte state);
extern void     fdc_tick     (FDC_765 *fdc);

struct fdc_765 {
  FDC_ISR  fdc_isr;
  FDD_765 *fdc_drive[4];
  guint8 unit_id;
  guint8 head_id;
  struct {
    gint   cmd;       /* current command           */
    guint8 msr;       /* main status register      */
    guint8 st0;       /* status register: ST0      */
    guint8 st1;       /* status register: ST1      */
    guint8 st2;       /* status register: ST2      */
    guint8 st3;       /* status register: ST3      */
    guint8 srt;       /* Step Rate Time            */
    guint8 hlt;       /* Head Load Time            */
    guint8 hut;       /* Head Unload Time          */
    guint8 ndm;       /* Non-DMA Mode              */
  } reg;
  struct {
    guint8 buf[16];   /* command buffer            */
    gint   len;       /* command buffer length     */
    gint   pos;       /* command buffer position   */
  } cmd;
  struct {
    guint8 buf[8192]; /* execution buffer          */
    gint   len;       /* execution buffer length   */
    gint   pos;       /* execution buffer position */
  } exe;
  struct {
    guint8 buf[16];   /* result buffer             */
    gint   len;       /* result buffer length      */
    gint   pos;       /* result buffer position    */
  } res;
  struct {
    guint state;      /* interrupt state           */
    guint count;      /* interrupt countdown       */
  } isr;
  int fdc_write_deleted;
  int fdc_lastidread;
};

#ifdef __cplusplus
}
#endif

#endif /* __LIB765_H__ */
