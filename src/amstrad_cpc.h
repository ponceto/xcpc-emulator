/*
 * amstrad_cpc.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __AMSTRAD_CPC_H__
#define __AMSTRAD_CPC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Computer version */
#define AMSTRAD_CPC_464         0x01
#define AMSTRAD_CPC_664         0x02
#define AMSTRAD_CPC_6128        0x03
#define AMSTRAD_CPC_464_PLUS    0x04
#define AMSTRAD_CPC_6128_PLUS   0x05

/* monitor type */
#define AMSTRAD_CPC_CTM65       0x00
#define AMSTRAD_CPC_CTM644      0x01

/* Computer clock */
#define AMSTRAD_CPC_1_6MHZ      0x00
#define AMSTRAD_CPC_2MHZ        0x01
#define AMSTRAD_CPC_3_3MHZ      0x02
#define AMSTRAD_CPC_4MHZ        0x03
#define AMSTRAD_CPC_6_6MHZ      0x04
#define AMSTRAD_CPC_8MHZ        0x05
#define AMSTRAD_CPC_9_9MHZ      0x06
#define AMSTRAD_CPC_16MHZ       0x07

/* Cassette read data */
#define AMSTRAD_CPC_NO_DATA     0x00
#define AMSTRAD_CPC_DATA        0x01

/* Printer configuration */
#define AMSTRAD_CPC_NOT_READY   0x01
#define AMSTRAD_CPC_READY       0x00

/* Expansion peripheral detect */
#define AMSTRAD_CPC_NOT_PRESENT 0x00
#define AMSTRAD_CPC_PRESENT     0x01

/* Screen refresh frequency */
#define AMSTRAD_CPC_50HZ        0x01
#define AMSTRAD_CPC_60HZ        0x00

/* Computer name on power-up */
#define AMSTRAD_CPC_ISP         0x00
#define AMSTRAD_CPC_TRIUMPH     0x01
#define AMSTRAD_CPC_SAISHO      0x02
#define AMSTRAD_CPC_SOLAVOX     0x03
#define AMSTRAD_CPC_AWA         0x04
#define AMSTRAD_CPC_SCHNEIDER   0x05
#define AMSTRAD_CPC_ORION       0x06
#define AMSTRAD_CPC_AMSTRAD     0x07

/* VSYNC status */
#define AMSTRAD_CPC_NOT_ACTIVE  0x00
#define AMSTRAD_CPC_ACTIVE      0x01

typedef struct {
  int version;
  int monitor;
  int clock;
  int cassette;
  int printer;
  int expansion;
  int refresh;
  int manufacturer;
  int vsync;
  unsigned int width;
  unsigned int height;
  int ramsize;
  char *rom[8];
} AMSTRAD_CPC_CFG;

typedef struct {
  int ticks;
  int cycle;
  struct {
    byte *lower_rom;
    byte *ram;
    byte *upper_rom[8];
    byte expansion;
  } memory;
  struct {
    byte row;
    byte line[16];
  } keyboard;
  struct {
    byte pen;
    byte ink[17];
    byte rom_cfg;
    byte ram_cfg;
    byte counter;
  } gate_array;
} AMSTRAD_CPC;

extern AMSTRAD_CPC amstrad_cpc;

extern void amstrad_cpc_init(void);
extern void amstrad_cpc_reset(void);
extern void amstrad_cpc_exit(void);
extern int  amstrad_cpc_main(int argc, char **argv);
extern void amstrad_cpc_load_snapshot(char *filename);
extern void amstrad_cpc_save_snapshot(char *filename);

#ifdef __cplusplus
}
#endif

#endif
