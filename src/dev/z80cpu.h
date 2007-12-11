/*
 * z80cpu.h - Copyright (c) 2001, 2006, 2007 Olivier Poncet
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

#define S_FLAG  0x80 /* 1: Result negative         */
#define Z_FLAG  0x40 /* 1: Result is zero          */
#define H_FLAG  0x10 /* 1: Halfcarry/Halfborrow    */
#define P_FLAG  0x04 /* 1: Result is even          */
#define V_FLAG  0x04 /* 1: Overflow occured        */
#define N_FLAG  0x02 /* 1: Subtraction occured     */
#define C_FLAG  0x01 /* 1: Carry/Borrow occured    */

#define IFF_1   0x01 /* IFF1 flip-flop             */
#define IFF_2   0x02 /* IFF2 flip-flop             */
#define IFF_IM1 0x08 /* IM1 mode                   */
#define IFF_IM2 0x10 /* IM2 mode                   */
#define IFF_INT 0x20 /* Pending INT                */
#define IFF_NMI 0x40 /* Pending NMI                */
#define IFF_HLT 0x80 /* CPU HALTed                 */

typedef union _GdevZ80REG {
  guint16 W;
#ifdef MSB_FIRST
  struct { guint8 h, l; } B;
#endif
#ifdef LSB_FIRST
  struct { guint8 l, h; } B;
#endif
} GdevZ80REG;

struct _GdevZ80CPU {
  GdevDevice device;
  GdevZ80REG AF, BC, DE, HL ; /* Main registers      */
  GdevZ80REG AF1,BC1,DE1,HL1; /* Prime registers     */
  GdevZ80REG IX, IY;          /* Index registers     */
  GdevZ80REG SP, PC;          /* Control registers   */
  GdevZ80REG IR, IF;          /* Interrupt & Refresh */
  gint32     m_cycles;        /* M-Cycles counter    */
  gint32     t_states;        /* T-States counter    */
  /* User functions */
  guint8 (*mreq_rd)(GdevZ80CPU *z80cpu, guint16 addr);
  void   (*mreq_wr)(GdevZ80CPU *z80cpu, guint16 addr, guint8 data);
  guint8 (*iorq_rd)(GdevZ80CPU *z80cpu, guint16 addr);
  void   (*iorq_wr)(GdevZ80CPU *z80cpu, guint16 addr, guint8 data);
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
