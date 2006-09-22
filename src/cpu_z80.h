/*
 * cpu_z80.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __CPU_Z80_H__
#define __CPU_Z80_H__

#ifdef __cplusplus
extern "C" {
#endif

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
#define INT_NONE    0xffff     /* No interrupt required      */
#define INT_QUIT    0xfffe     /* Exit the emulation         */

#define S_FLAG      0x80       /* 1: Result negative         */
#define Z_FLAG      0x40       /* 1: Result is zero          */
#define H_FLAG      0x10       /* 1: Halfcarry/Halfborrow    */
#define P_FLAG      0x04       /* 1: Result is even          */
#define V_FLAG      0x04       /* 1: Overflow occured        */
#define N_FLAG      0x02       /* 1: Subtraction occured     */
#define C_FLAG      0x01       /* 1: Carry/Borrow occured    */

#define IFF_1       0x01       /* IFF1 flip-flop             */
#define IFF_2       0x02       /* IFF2 flip-flop             */
#define IFF_EI      0x04       /* After EI                   */
#define IFF_IM1     0x10       /* IM1 mode                   */
#define IFF_IM2     0x20       /* IM2 mode                   */
#define IFF_HALT    0x80       /* CPU HALTed                 */

typedef union _pair {
#ifdef LSB_FIRST
  struct { byte l, h; } B;
#endif
#ifdef MSB_FIRST
  struct { byte h, l; } B;
#endif
  word W;
} pair;

typedef struct _CPU_Z80 {
  pair AF, BC, DE, HL ; /* Main registers      */
  pair AF1,BC1,DE1,HL1; /* Prime registers     */
  pair IX, IY;          /* Index registers     */
  pair SP, PC;          /* Control registers   */
  pair IR;              /* Interrupt & Refresh */
  byte IFF;             /* Interrupt Flip-Flop */
  int  IPeriod;
  int  TStates;
} CPU_Z80;

extern void cpu_z80_init (CPU_Z80 *cpu_z80);
extern void cpu_z80_clock(CPU_Z80 *cpu_z80);
extern void cpu_z80_reset(CPU_Z80 *cpu_z80);
extern void cpu_z80_exit (CPU_Z80 *cpu_z80);

extern void cpu_z80_mm_wr(CPU_Z80 *cpu_z80, word addr, byte data);
extern byte cpu_z80_mm_rd(CPU_Z80 *cpu_z80, word addr);
extern void cpu_z80_io_wr(CPU_Z80 *cpu_z80, word addr, byte data);
extern byte cpu_z80_io_rd(CPU_Z80 *cpu_z80, word addr);
extern word cpu_z80_timer(CPU_Z80 *cpu_z80);

#ifdef __cplusplus
}
#endif

#endif
