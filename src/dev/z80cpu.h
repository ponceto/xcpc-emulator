/*
 * z80cpu.h - Copyright (c) 2001, 2006 Olivier Poncet
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

#define INT_RST00   0x00c7     /* RST 00h                    */
#define INT_RST08   0x00cf     /* RST 08h                    */
#define INT_RST10   0x00d7     /* RST 10h                    */
#define INT_RST18   0x00df     /* RST 18h                    */
#define INT_RST20   0x00e7     /* RST 20h                    */
#define INT_RST28   0x00ef     /* RST 28h                    */
#define INT_RST30   0x00f7     /* RST 30h                    */
#define INT_RST38   0x00ff     /* RST 38h                    */
#define INT_IRQ     0x00ff     /* Default IRQ opcode is 0xff */
#define INT_NMI     0xfffd     /* Non-maskable interrupt     */

#define S_FLAG      0x80       /* 1: Result negative         */
#define Z_FLAG      0x40       /* 1: Result is zero          */
#define H_FLAG      0x10       /* 1: Halfcarry/Halfborrow    */
#define P_FLAG      0x04       /* 1: Result is even          */
#define V_FLAG      0x04       /* 1: Overflow occured        */
#define N_FLAG      0x02       /* 1: Subtraction occured     */
#define C_FLAG      0x01       /* 1: Carry/Borrow occured    */

#define IFF_1       0x01       /* IFF1 flip-flop             */
#define IFF_2       0x02       /* IFF2 flip-flop             */
#define IFF_3       0x04       /* After EI instruction       */
#define IFF_IM1     0x08       /* IM1 mode                   */
#define IFF_IM2     0x10       /* IM2 mode                   */
#define IFF_IRQ     0x20       /* Pending IRQ                */
#define IFF_NMI     0x40       /* Pending NMI                */
#define IFF_HALT    0x80       /* CPU HALTed                 */

typedef union _GdevZ80REG {
#ifdef LSB_FIRST
  struct { guint8 l, h; } B;
#endif
#ifdef MSB_FIRST
  struct { guint8 h, l; } B;
#endif
  guint16 W;
} GdevZ80REG;

struct _GdevZ80CPU {
  GdevDevice device;
  GdevZ80REG AF, BC, DE, HL ; /* Main registers      */
  GdevZ80REG AF1,BC1,DE1,HL1; /* Prime registers     */
  GdevZ80REG IX, IY;          /* Index registers     */
  GdevZ80REG SP, PC;          /* Control registers   */
  GdevZ80REG IR;              /* Interrupt & Refresh */
  guint8     IFF;             /* Interrupt Flip-Flop */
  gint       m_cycles;         /* Z80 m-cycles        */
  gint       t_states;         /* Z80 t-states        */
  /* User functions */
  guint8 (*mm_rd)(GdevZ80CPU *z80cpu, guint16 addr);
  void   (*mm_wr)(GdevZ80CPU *z80cpu, guint16 addr, guint8 data);
  guint8 (*io_rd)(GdevZ80CPU *z80cpu, guint16 addr);
  void   (*io_wr)(GdevZ80CPU *z80cpu, guint16 addr, guint8 data);
};

struct _GdevZ80CPUClass {
  GdevDeviceClass parent_class;
};

extern GType       gdev_z80cpu_get_type   (void);
extern GdevZ80CPU *gdev_z80cpu_new        (void);
extern void        gdev_z80cpu_intr       (GdevZ80CPU *z80cpu, guint16 vector);
extern void        gdev_z80cpu_assert_irq (GdevZ80CPU *z80cpu);
extern void        gdev_z80cpu_assert_nmi (GdevZ80CPU *z80cpu);

G_END_DECLS

#endif /* __GDEV_Z80CPU_H__ */
