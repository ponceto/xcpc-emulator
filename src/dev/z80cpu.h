/*
 * z80cpu.h - Copyright (c) 2001, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Olivier Poncet
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
#ifndef __GDEV_Z80CPU_H__
#define __GDEV_Z80CPU_H__

#include <dev/device.h>

G_BEGIN_DECLS

#define GDEV_TYPE_Z80CPU            (gdev_z80cpu_get_type())
#define GDEV_Z80CPU(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GDEV_TYPE_Z80CPU, GdevZ80CPU))
#define GDEV_Z80CPU_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GDEV_TYPE_Z80CPU, GdevZ80CPUClass))
#define GDEV_IS_Z80CPU(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GDEV_TYPE_Z80CPU))
#define GDEV_IS_Z80CPU_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDEV_TYPE_Z80CPU))
#define GDEV_Z80CPU_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GDEV_TYPE_Z80CPU, GdevZ80CPUClass))

typedef struct _GdevZ80CPU      GdevZ80CPU;
typedef struct _GdevZ80CPUClass GdevZ80CPUClass;

#define _SF   0x80 /* Sign                   */
#define _ZF   0x40 /* Zero                   */
#define _5F   0x20 /* Undocumented           */
#define _HF   0x10 /* HalfCarry / HalfBorrow */
#define _3F   0x08 /* Undocumented           */
#define _PF   0x04 /* Parity                 */
#define _OF   0x04 /* Overflow               */
#define _NF   0x02 /* Add / Sub              */
#define _CF   0x01 /* Carry / Borrow         */

#define _HLT  0x80 /* CPU is HALTed          */
#define _NMI  0x40 /* Pending NMI            */
#define _INT  0x20 /* Pending INT            */
#define _XYZ  0x10 /* Not Used               */
#define _IM2  0x08 /* Interrupt Mode #2      */
#define _IM1  0x04 /* Interrupt Mode #1      */
#define _IFF2 0x02 /* Interrupt Flip-Flop #2 */
#define _IFF1 0x01 /* Interrupt Flip-Flop #1 */

typedef gint8   s_int08_t;
typedef guint8  u_int08_t;
typedef gint16  s_int16_t;
typedef guint16 u_int16_t;
typedef gint32  s_int32_t;
typedef guint32 u_int32_t;
typedef gint64  s_int64_t;
typedef guint64 u_int64_t;

typedef union _GdevZ80REG {
  u_int32_t q;
#if defined(MSB_FIRST) && !defined(LSB_FIRST)
  struct { u_int16_t h, l; } w;
  struct { u_int08_t x, y, h, l; } b;
#endif
#if !defined(MSB_FIRST) && defined(LSB_FIRST)
  struct { u_int16_t l, h; } w;
  struct { u_int08_t l, h, y, x; } b;
#endif
} GdevZ80REG;

struct _GdevZ80CPU {
  GdevDevice device;
  struct {
    GdevZ80REG AF; /* AF & AF'            */
    GdevZ80REG BC; /* BC & BC'            */
    GdevZ80REG DE; /* DE & DE'            */
    GdevZ80REG HL; /* HL & HL'            */
    GdevZ80REG IX; /* IX Index            */
    GdevZ80REG IY; /* IY Index            */
    GdevZ80REG SP; /* Stack Pointer       */
    GdevZ80REG PC; /* Program Counter     */
    GdevZ80REG IR; /* Interrupt & Refresh */
    GdevZ80REG IF; /* IFF, IM & Control   */
  } reg;
  u_int32_t m_cycles;
  u_int32_t t_states;
  s_int32_t ccounter;
  u_int08_t (*mreq_m1)(GdevZ80CPU *z80cpu, u_int16_t addr);
  u_int08_t (*mreq_rd)(GdevZ80CPU *z80cpu, u_int16_t addr);
  void      (*mreq_wr)(GdevZ80CPU *z80cpu, u_int16_t addr, u_int08_t data);
  u_int08_t (*iorq_m1)(GdevZ80CPU *z80cpu, u_int16_t addr);
  u_int08_t (*iorq_rd)(GdevZ80CPU *z80cpu, u_int16_t addr);
  void      (*iorq_wr)(GdevZ80CPU *z80cpu, u_int16_t addr, u_int08_t data);
};

struct _GdevZ80CPUClass {
  GdevDeviceClass parent_class;
};

extern GType       gdev_z80cpu_get_type   (void);
extern GdevZ80CPU *gdev_z80cpu_new        (void);
extern void        gdev_z80cpu_assert_int (GdevZ80CPU *z80cpu);
extern void        gdev_z80cpu_assert_nmi (GdevZ80CPU *z80cpu);

G_END_DECLS

#endif /* __GDEV_Z80CPU_H__ */
