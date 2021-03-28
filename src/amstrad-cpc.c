/*
 * amstrad-cpc.c - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xem/Emulator.h>
#include "amstrad-cpc.h"

enum RamSize
{
    RAM_16K  = ( 16 * 1024),
    RAM_32K  = ( 32 * 1024),
    RAM_48K  = ( 48 * 1024),
    RAM_64K  = ( 64 * 1024),
    RAM_72K  = ( 72 * 1024),
    RAM_128K = (128 * 1024)
};

#define SELF(user_data) ((AMSTRAD_CPC_EMULATOR*)(user_data))

AMSTRAD_CPC_EMULATOR amstrad_cpc = {
    NULL,     /* options  */
    NULL,     /* monitor  */
    NULL,     /* keyboard */
    NULL,     /* joystick */
    NULL,     /* cpu_z80a */
    NULL,     /* vga_core */
    NULL,     /* vdc_6845 */
    NULL,     /* ppi_8255 */
    NULL,     /* psg_8910 */
    NULL,     /* fdc_765a */
    { NULL }, /* ram_bank */
    { NULL }, /* rom_bank */
    { NULL }, /* exp_bank */
};

static int is_set(const char* value)
{
    if(value == NULL) {
        return 0;
    }
    if(strcmp(value, "{not-set}") == 0) {
        return 0;
    }
    return 1;
}

static char* build_filename(const char* directory, const char* filename)
{
    char buffer[PATH_MAX + 1];

    (void) snprintf(buffer, sizeof(buffer), "%s/%s/%s", XCPC_RESDIR, directory, filename);

    return strdup(buffer);
}

static void compute_stats(AMSTRAD_CPC_EMULATOR* self)
{
    struct timeval prev_time = self->timer.profiler;
    struct timeval curr_time = self->timer.profiler;
    unsigned long elapsed_us = 0;

    /* get the current time */ {
        if(gettimeofday(&curr_time, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
    }
    /* compute the elapsed time in us */ {
        const long long t1 = (((long long) prev_time.tv_sec) * 1000000LL) + ((long long) prev_time.tv_usec);
        const long long t2 = (((long long) curr_time.tv_sec) * 1000000LL) + ((long long) curr_time.tv_usec);
        if(t2 >= t1) {
            elapsed_us = ((unsigned long)(t2 - t1));
        }
        else {
            elapsed_us = 0UL;
        }
    }
    /* compute and build the statistics */ {
        if(elapsed_us != 0) {
            const double stats_frames  = (double) (self->frame.drawn * 1000000UL);
            const double stats_elapsed = (double) elapsed_us;
            const double stats_fps     = (stats_frames / stats_elapsed);
            (void) snprintf(self->stats, sizeof(self->stats), "refresh = %2d Hz, framerate = %.2f fps", self->frame.rate, stats_fps);
        }
        else {
            (void) snprintf(self->stats, sizeof(self->stats), "refresh = %2d Hz", self->frame.rate);
        }
    }
    /* print statistics */ {
        if(self->options->state.fps != 0) {
            xcpc_log_print(self->stats);
        }
    }
    /* set the new reference */ {
        self->timer.profiler = curr_time;
        self->frame.count    = 0;
        self->frame.drawn    = 0;
    }
}

static void cpc_mem_select(AMSTRAD_CPC_EMULATOR* self)
{
    if(self->ramsize >= RAM_128K) {
        switch(self->memory.ram.config) {
            case 0x00:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[1]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
                }
                break;
            case 0x01:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[1]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[7]->state.data;
                }
                break;
            case 0x02:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[4]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[5]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[6]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[7]->state.data;
                }
                break;
            case 0x03:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[3]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[7]->state.data;
                }
                break;
            case 0x04:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[4]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
                }
                break;
            case 0x05:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[5]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
                }
                break;
            case 0x06:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[6]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
                }
                break;
            case 0x07:
                {
                    self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
                    self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[7]->state.data;
                    self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
                    self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
                }
                break;
            default:
                xcpc_log_alert("ram-select: unsupported ram configuration (%02x)", self->memory.ram.config);
                break;
        }
    }
    else {
        self->memory.rd.bank[0] = self->memory.wr.bank[0] = self->ram_bank[0]->state.data;
        self->memory.rd.bank[1] = self->memory.wr.bank[1] = self->ram_bank[1]->state.data;
        self->memory.rd.bank[2] = self->memory.wr.bank[2] = self->ram_bank[2]->state.data;
        self->memory.rd.bank[3] = self->memory.wr.bank[3] = self->ram_bank[3]->state.data;
    }
    if((self->vga_core->state.rmr & 0x04) == 0) {
        if(self->rom_bank[0] != NULL) {
            self->memory.rd.bank[0] = self->rom_bank[0]->state.data;
        }
    }
    if((self->vga_core->state.rmr & 0x08) == 0) {
        if(self->rom_bank[1] != NULL) {
            self->memory.rd.bank[3] = self->rom_bank[1]->state.data;
        }
        if(self->exp_bank[self->memory.rom.config] != NULL) {
            self->memory.rd.bank[3] = self->exp_bank[self->memory.rom.config]->state.data;
        }
    }
}

static uint8_t cpu_mreq_m1(XcpcCpuZ80a* cpu_z80a, uint16_t addr)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(cpu_z80a->iface.user_data);

    return self->memory.rd.bank[addr >> 14][addr & 0x3fff];
}

static uint8_t cpu_mreq_rd(XcpcCpuZ80a* cpu_z80a, uint16_t addr)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(cpu_z80a->iface.user_data);

    return self->memory.rd.bank[addr >> 14][addr & 0x3fff];
}

static uint8_t cpu_mreq_wr(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(cpu_z80a->iface.user_data);

    self->memory.wr.bank[addr >> 14][addr & 0x3fff] = data;

    return data;
}

static uint8_t cpu_iorq_m1(XcpcCpuZ80a* cpu_z80a, uint16_t port)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(cpu_z80a->iface.user_data);

    self->vga_core->state.counter &= 0x1f;

    return 0x00;
}

static uint8_t cpu_iorq_rd(XcpcCpuZ80a* cpu_z80a, uint16_t port)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(cpu_z80a->iface.user_data);
    uint8_t data = 0x00;

    /* vga-core [0-------xxxxxxxx] [0x7fxx] */ {
        if((port & 0x8000) == 0) {
            xcpc_log_alert("cpu_iorq_rd[0x%04x]: vga-core [---- illegal ----]\n", port);
        }
    }
    /* vdc-6845 [-0------xxxxxxxx] [0xbfxx] */ {
        if((port & 0x4000) == 0) {
            switch((port >> 8) & 3) {
                case 0:  /* [-0----00xxxxxxxx] [0xbcxx] */
                    xcpc_log_alert("cpu_iorq_rd[0x%04x]: vdc-6845 [---- illegal ----]\n", port);
                    break;
                case 1:  /* [-0----01xxxxxxxx] [0xbdxx] */
                    xcpc_log_alert("cpu_iorq_rd[0x%04x]: vdc-6845 [---- illegal ----]\n", port);
                    break;
                case 2:  /* [-0----10xxxxxxxx] [0xbexx] */
                    xcpc_log_alert("cpu_iorq_rd[0x%04x]: vdc-6845 [- not supported -]\n", port);
                    break;
                case 3:  /* [-0----11xxxxxxxx] [0xbfxx] */
                    data = xcpc_vdc_6845_rd(self->vdc_6845, 0xff);
                    break;
            }
        }
    }
    /* rom-conf [--0-----xxxxxxxx] [0xdfxx] */ {
        if((port & 0x2000) == 0) {
            xcpc_log_alert("cpu_iorq_rd[0x%04x]: rom-conf [---- illegal ----]\n", port);
        }
    }
    /* prt-port [---0----xxxxxxxx] [0xefxx] */ {
        if((port & 0x1000) == 0) {
            xcpc_log_alert("cpu_iorq_rd[0x%04x]: prt-port [---- illegal ----]\n", port);
        }
    }
    /* ppi-8255 [----0---xxxxxxxx] [0xf7xx] */ {
        if((port & 0x0800) == 0) {
            switch((port >> 8) & 3) {
                case 0:  /* [----0-00xxxxxxxx] [0xf4xx] */
                    self->ppi_8255->state.port_a = self->keyboard->state.keys[self->keyboard->state.line];
                    data = self->ppi_8255->state.port_a;
                    break;
                case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
                    self->ppi_8255->state.port_b = ((0                                             & 0x01) << 7)
                                                 | ((1                                             & 0x01) << 6)
                                                 | ((1                                             & 0x01) << 5)
                                                 | ((self->refresh_rate                            & 0x01) << 4)
                                                 | ((self->manufacturer                            & 0x07) << 1)
                                                 | ((self->vdc_6845->state.ctrs.named.vsync_signal & 0x01) << 0);
                    data = self->ppi_8255->state.port_b;
                    break;
                case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
                    data = self->ppi_8255->state.port_c;
                    break;
                case 3:  /* [----0-11xxxxxxxx] [0xf7xx] */
                    xcpc_log_alert("cpu_iorq_rd[0x%04x]: ppi-8255 [---- illegal ----]\n", port);
                    break;
            }
        }
    }
    /* fdc-765a [-----0--0xxxxxxx] [0xfb7f] */ {
        if((port & 0x0480) == 0) {
            switch(((port >> 7) & 2) | (port & 1)) {
                case 0:  /* [-----0-00xxxxxx0] [0xfa7e] */
                    xcpc_log_alert("cpu_iorq_rd[0x%04x]: fdc-765a [---- illegal ----]\n", port);
                    break;
                case 1:  /* [-----0-00xxxxxx1] [0xfa7f] */
                    xcpc_log_alert("cpu_iorq_rd[0x%04x]: fdc-765a [---- illegal ----]\n", port);
                    break;
                case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
                    xcpc_fdc_765a_rd_stat(self->fdc_765a, &data);
                    break;
                case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
                    xcpc_fdc_765a_rd_data(self->fdc_765a, &data);
                    break;
            }
        }
    }
    return data;
}

static uint8_t cpu_iorq_wr(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(cpu_z80a->iface.user_data);

    /* vga-core [0-------xxxxxxxx] [0x7fxx] */ {
        if((port & 0x8000) == 0) {
            switch((data >> 6) & 3) {
                case 0: /* Select pen */
                    self->vga_core->state.pen = (data & 0x10 ? 0x10 : data & 0x0f);
                    break;
                case 1: /* Select color */
                    self->vga_core->state.ink[self->vga_core->state.pen] = data & 0x1f;
                    break;
                case 2: /* Interrupt control, ROM configuration and screen mode */
                    if((data & 0x10) != 0) {
                        self->vga_core->state.counter = 0;
                    }
                    self->vga_core->state.rmr = data & 0x1f;
                    cpc_mem_select(self);
                    break;
                case 3: /* RAM memory management */
                    self->memory.ram.config = data & 0x3f;
                    cpc_mem_select(self);
                    break;
            }
        }
    }
    /* vdc-6845 [-0------xxxxxxxx] [0xbfxx] */ {
        if((port & 0x4000) == 0) {
            switch((port >> 8) & 3) {
                case 0:  /* [-0----00xxxxxxxx] [0xbcxx] */
                    xcpc_vdc_6845_rs(self->vdc_6845, data);
                    break;
                case 1:  /* [-0----01xxxxxxxx] [0xbdxx] */
                    xcpc_vdc_6845_wr(self->vdc_6845, data);
                    break;
                case 2:  /* [-0----10xxxxxxxx] [0xbexx] */
                    xcpc_log_alert("cpu_iorq_wr[0x%04x]: vdc-6845 [- Not Supported -]\n", port);
                    break;
                case 3:  /* [-0----11xxxxxxxx] [0xbfxx] */
                    xcpc_log_alert("cpu_iorq_wr[0x%04x]: vdc-6845 [---- Illegal ----]\n", port);
                    break;
            }
        }
    }
    /* rom-conf [--0-----xxxxxxxx] [0xdfxx] */ {
        if((port & 0x2000) == 0) {
            self->memory.rom.config = data;
            cpc_mem_select(self);
        }
    }
    /* prt-port [---0----xxxxxxxx] [0xefxx] */ {
        if((port & 0x1000) == 0) {
            /* xxx */
        }
    }
    /* ppi-8255 [----0---xxxxxxxx] [0xf7xx] */ {
        if((port & 0x0800) == 0) {
            switch((port >> 8) & 3) {
                case 0:  /* [----0-00xxxxxxxx] [0xf4xx] */
                    self->ppi_8255->state.port_a = data;
                    break;
                case 1:  /* [----0-01xxxxxxxx] [0xf5xx] */
                    /*self->ppi_8255->state.port_b = data;*/
                    break;
                case 2:  /* [----0-10xxxxxxxx] [0xf6xx] */
                    self->ppi_8255->state.port_c = data;
                    self->keyboard->state.line = data & 0x0F;
                    break;
                case 3:  /* [----0-11xxxxxxxx] [0xf7xx] */
                    self->ppi_8255->state.ctrl_p = data;
                    break;
            }
        }
    }
    /* fdc-765a [-----0--0xxxxxxx] [0xfb7f] */ {
        if((port & 0x0480) == 0) {
            switch(((port >> 7) & 2) | ((port >> 0) & 1)) {
                case 0:  /* [-----0-00xxxxxx0] [0xfa7e] */
                    xcpc_fdc_765a_set_motor(self->fdc_765a, ((data & 1) << 1) | ((data & 1) << 0));
                    break;
                case 1:  /* [-----0-00xxxxxx1] [0xfa7f] */
                    xcpc_fdc_765a_set_motor(self->fdc_765a, ((data & 1) << 1) | ((data & 1) << 0));
                    break;
                case 2:  /* [-----0-10xxxxxx0] [0xfb7e] */
                    xcpc_fdc_765a_wr_stat(self->fdc_765a, &data);
                    break;
                case 3:  /* [-----0-10xxxxxx1] [0xfb7f] */
                    xcpc_fdc_765a_wr_data(self->fdc_765a, &data);
                    break;
            }
        }
    }
    return data;
}

static void vdc_hsync(XcpcVdc6845* vdc_6845, int hsync)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(vdc_6845->iface.user_data);
    XcpcMonitor* monitor  = self->monitor;
    XcpcCpuZ80a* cpu_z80a = self->cpu_z80a;
    XcpcVgaCore* vga_core = self->vga_core;

    if(hsync == 0) {
        /* falling edge */ {
            if(++vga_core->state.counter == 52) {
                xcpc_cpu_z80a_pulse_int(cpu_z80a);
                vga_core->state.counter = 0;
            }
            if(vga_core->state.delayed > 0) {
                if(--vga_core->state.delayed == 0) {
                    if(vga_core->state.counter >= 32) {
                        xcpc_cpu_z80a_pulse_int(cpu_z80a);
                    }
                    vga_core->state.counter = 0;
                }
            }
            /* update scanline */ {
                XcpcScanline* scanline = &self->scanlines.array[(self->scanlines.index + 1) % 312];
                /* update mode */ {
                    scanline->mode = vga_core->state.rmr & 0x03;
                }
                /* update colors */ {
                    int index = 0;
                    do {
                        const uint8_t ink = vga_core->state.ink[index];
                        scanline->ink[index].value = ink;
                        scanline->ink[index].pixel = monitor->state.palette[ink].pixel;
                    } while(++index < 17);
                }
            }
        }
    }
    else {
        /* rising edge */ {
            /* do nothing */
        }
    }
}

static void vdc_vsync(XcpcVdc6845* vdc_6845, int vsync)
{
    AMSTRAD_CPC_EMULATOR* self = SELF(vdc_6845->iface.user_data);
    XcpcVgaCore* vga_core = self->vga_core;

    if(vsync != 0) {
        /* rising edge */ {
            vga_core->state.delayed = 2;
        }
    }
    else {
        /* falling edge */ {
            /* do nothing */
        }
    }
}

static void amstrad_cpc_paint_default(AMSTRAD_CPC_EMULATOR* self)
{
}

static void amstrad_cpc_paint_08bpp(AMSTRAD_CPC_EMULATOR* self)
{
    XcpcMonitor* monitor = self->monitor;
    XcpcVdc6845* vdc_6845 = self->vdc_6845;
    XcpcVgaCore* vga_core = self->vga_core;
    unsigned int sa = ((vdc_6845->state.regs.named.start_address_high << 8) | vdc_6845->state.regs.named.start_address_low);
    unsigned int hd = (vdc_6845->state.regs.named.horizontal_displayed < 48 ? vdc_6845->state.regs.named.horizontal_displayed : 48);
    unsigned int hp = ((XCPC_MONITOR_WIDTH >> 0) - (hd << 4)) >> 1;
    unsigned int mr = vdc_6845->state.regs.named.maximum_scanline_address + 1;
    unsigned int vt = vdc_6845->state.regs.named.vertical_total + 1;
    unsigned int vd = (vdc_6845->state.regs.named.vertical_displayed < 39 ? vdc_6845->state.regs.named.vertical_displayed : 39);
    unsigned int vp = ((XCPC_MONITOR_HEIGHT >> 1) - (vd * mr)) >> 1;
    XcpcScanline* scanline = self->scanlines.array;
    uint8_t* dst = (uint8_t*) monitor->state.image->data;
    uint8_t* nxt = dst;
    uint8_t pixel;
    unsigned int cx, cy, ra;
    uint16_t addr;
    uint16_t bank;
    uint16_t disp;
    uint8_t data;

    scanline = &self->scanlines.array[(vt * mr) - (1 * vp)];
    for(cy = 0; cy < vp; cy++) {
        nxt += XCPC_MONITOR_WIDTH;
        pixel = scanline->ink[16].pixel;
        for(cx = 0; cx < XCPC_MONITOR_WIDTH; cx++) {
            *dst++ = *nxt++ = pixel;
        }
        dst = nxt; scanline++;
    }
    scanline = &self->scanlines.array[6];
    for(cy = 0; cy < vd; cy++) {
        for(ra = 0; ra < mr; ra++) {
            nxt += XCPC_MONITOR_WIDTH;
            switch(scanline->mode) {
                case 0x00: /* mode 0 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode0[data];
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 4;
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode0[data];
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 4;
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
                case 0x01: /* mode 1 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode1[data];
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode1[data];
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
                case 0x02: /* mode 2 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode2[data];
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 4 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 5 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 6 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 7 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode2[data];
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 4 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 5 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 6 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 7 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
            }
            dst = nxt; scanline++;
        }
        sa += hd;
    }
    scanline = &self->scanlines.array[(vd * mr) + (0 * vp)];
    for(cy = 0; cy < vp; cy++) {
        nxt += XCPC_MONITOR_WIDTH;
        pixel = scanline->ink[16].pixel;
        for(cx = 0; cx < XCPC_MONITOR_WIDTH; cx++) {
            *dst++ = *nxt++ = pixel;
        }
        dst = nxt; scanline++;
    }
    (void) xcpc_monitor_put_image(self->monitor);
}

static void amstrad_cpc_paint_16bpp(AMSTRAD_CPC_EMULATOR* self)
{
    XcpcMonitor* monitor = self->monitor;
    XcpcVdc6845* vdc_6845 = self->vdc_6845;
    XcpcVgaCore* vga_core = self->vga_core;
    unsigned int sa = ((vdc_6845->state.regs.named.start_address_high << 8) | vdc_6845->state.regs.named.start_address_low);
    unsigned int hd = (vdc_6845->state.regs.named.horizontal_displayed < 48 ? vdc_6845->state.regs.named.horizontal_displayed : 48);
    unsigned int hp = ((XCPC_MONITOR_WIDTH >> 0) - (hd << 4)) >> 1;
    unsigned int mr = vdc_6845->state.regs.named.maximum_scanline_address + 1;
    unsigned int vt = vdc_6845->state.regs.named.vertical_total + 1;
    unsigned int vd = (vdc_6845->state.regs.named.vertical_displayed < 39 ? vdc_6845->state.regs.named.vertical_displayed : 39);
    unsigned int vp = ((XCPC_MONITOR_HEIGHT >> 1) - (vd * mr)) >> 1;
    XcpcScanline* scanline = self->scanlines.array;
    uint16_t* dst = (uint16_t*) monitor->state.image->data;
    uint16_t* nxt = dst;
    uint16_t pixel;
    unsigned int cx, cy, ra;
    uint16_t addr;
    uint16_t bank;
    uint16_t disp;
    uint8_t data;

    scanline = &self->scanlines.array[(vt * mr) - (1 * vp)];
    for(cy = 0; cy < vp; cy++) {
        nxt += XCPC_MONITOR_WIDTH;
        pixel = scanline->ink[16].pixel;
        for(cx = 0; cx < XCPC_MONITOR_WIDTH; cx++) {
            *dst++ = *nxt++ = pixel;
        }
        dst = nxt; scanline++;
    }
    scanline = &self->scanlines.array[6];
    for(cy = 0; cy < vd; cy++) {
        for(ra = 0; ra < mr; ra++) {
            nxt += XCPC_MONITOR_WIDTH;
            switch(scanline->mode) {
                case 0x00: /* mode 0 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode0[data];
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 4;
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode0[data];
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 4;
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
                case 0x01: /* mode 1 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode1[data];
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode1[data];
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
                case 0x02: /* mode 2 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode2[data];
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 4 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 5 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 6 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 7 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode2[data];
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 4 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 5 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 6 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 7 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
            }
            dst = nxt; scanline++;
        }
        sa += hd;
    }
    scanline = &self->scanlines.array[(vd * mr) + (0 * vp)];
    for(cy = 0; cy < vp; cy++) {
        nxt += XCPC_MONITOR_WIDTH;
        pixel = scanline->ink[16].pixel;
        for(cx = 0; cx < XCPC_MONITOR_WIDTH; cx++) {
            *dst++ = *nxt++ = pixel;
        }
        dst = nxt; scanline++;
    }
    (void) xcpc_monitor_put_image(self->monitor);
}

static void amstrad_cpc_paint_32bpp(AMSTRAD_CPC_EMULATOR* self)
{
    XcpcMonitor* monitor = self->monitor;
    XcpcVdc6845* vdc_6845 = self->vdc_6845;
    XcpcVgaCore* vga_core = self->vga_core;
    unsigned int sa = ((vdc_6845->state.regs.named.start_address_high << 8) | vdc_6845->state.regs.named.start_address_low);
    unsigned int hd = (vdc_6845->state.regs.named.horizontal_displayed < 48 ? vdc_6845->state.regs.named.horizontal_displayed : 48);
    unsigned int hp = ((XCPC_MONITOR_WIDTH >> 0) - (hd << 4)) >> 1;
    unsigned int mr = vdc_6845->state.regs.named.maximum_scanline_address + 1;
    unsigned int vt = vdc_6845->state.regs.named.vertical_total + 1;
    unsigned int vd = (vdc_6845->state.regs.named.vertical_displayed < 39 ? vdc_6845->state.regs.named.vertical_displayed : 39);
    unsigned int vp = ((XCPC_MONITOR_HEIGHT >> 1) - (vd * mr)) >> 1;
    XcpcScanline* scanline = self->scanlines.array;
    uint32_t* dst = (uint32_t*) monitor->state.image->data;
    uint32_t* nxt = dst;
    uint32_t pixel;
    unsigned int cx, cy, ra;
    uint16_t addr;
    uint16_t bank;
    uint16_t disp;
    uint8_t data;

    scanline = &self->scanlines.array[(vt * mr) - (1 * vp)];
    for(cy = 0; cy < vp; cy++) {
        nxt += XCPC_MONITOR_WIDTH;
        pixel = scanline->ink[16].pixel;
        for(cx = 0; cx < XCPC_MONITOR_WIDTH; cx++) {
            *dst++ = *nxt++ = pixel;
        }
        dst = nxt; scanline++;
    }
    scanline = &self->scanlines.array[6];
    for(cy = 0; cy < vd; cy++) {
        for(ra = 0; ra < mr; ra++) {
            nxt += XCPC_MONITOR_WIDTH;
            switch(scanline->mode) {
                case 0x00: /* mode 0 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode0[data];
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 4;
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode0[data];
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 4;
                                    pixel = scanline->ink[data & 0x0f].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
                case 0x01: /* mode 1 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode1[data];
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode1[data];
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 2;
                                    pixel = scanline->ink[data & 0x03].pixel;
                                    *dst++ = *nxt++ = pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
                case 0x02: /* mode 2 */
                    {
                        /* left border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                        /* active display */ {
                            for(cx = 0; cx < hd; cx++) {
                                /* decode */ {
                                    addr = ((sa & 0x3000) << 2) | ((ra & 0x0007) << 11) | (((sa + cx) & 0x03ff) << 1);
                                    bank = (addr >> 14);
                                    disp = (addr & 0x3fff);
                                }
                                /* fetch 1st byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 0];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode2[data];
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 4 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 5 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 6 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 7 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* fetch 2nd byte */ {
                                    data = self->ram_bank[bank]->state.data[disp | 1];
                                }
                                /* pixel 0 */ {
                                    data = vga_core->state.mode2[data];
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 1 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 2 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 3 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 4 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 5 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 6 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                                /* pixel 7 */ {
                                    data >>= 1;
                                    pixel = scanline->ink[data & 0x01].pixel;
                                    *dst++ = *nxt++ = pixel;
                                }
                            }
                        }
                        /* right border */ {
                            pixel = scanline->ink[16].pixel;
                            for(cx = 0; cx < hp; cx++) {
                                *dst++ = *nxt++ = pixel;
                            }
                        }
                    }
                    break;
            }
            dst = nxt; scanline++;
        }
        sa += hd;
    }
    scanline = &self->scanlines.array[(vd * mr) + (0 * vp)];
    for(cy = 0; cy < vp; cy++) {
        nxt += XCPC_MONITOR_WIDTH;
        pixel = scanline->ink[16].pixel;
        for(cx = 0; cx < XCPC_MONITOR_WIDTH; cx++) {
            *dst++ = *nxt++ = pixel;
        }
        dst = nxt; scanline++;
    }
    (void) xcpc_monitor_put_image(self->monitor);
}

static void amstrad_cpc_keybd_default(AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
}

static void amstrad_cpc_keybd_qwerty(AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    (void) xcpc_keyboard_qwerty(self->keyboard, &event->xkey);
}

static void amstrad_cpc_keybd_azerty(AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    (void) xcpc_keyboard_azerty(self->keyboard, &event->xkey);
}

static void amstrad_cpc_mouse_default(AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
}

extern void amstrad_cpc_new(int* argc, char*** argv)
{
    AMSTRAD_CPC_EMULATOR* self = &amstrad_cpc;

    /* memset */ {
        (void) memset(self, 0, sizeof(AMSTRAD_CPC_EMULATOR()));
    }
    /* create options */ {
        self->options = xcpc_options_new();
    }
    /* parse options */ {
        (void) xcpc_options_parse(self->options, argc, argv);
    }
}

extern void amstrad_cpc_delete(void)
{
    AMSTRAD_CPC_EMULATOR* self = &amstrad_cpc;

    /* delete options */ {
        self->options = xcpc_options_delete(self->options);
    }
    /* memset */ {
        (void) memset(self, 0, sizeof(AMSTRAD_CPC_EMULATOR()));
    }
}

void amstrad_cpc_start(AMSTRAD_CPC_EMULATOR* self)
{
    char* system_rom = NULL;
    char* amsdos_rom = NULL;
    const char* cpc_model        = self->options->state.model;
    const char* cpc_monitor      = self->options->state.monitor;
    const char* cpc_keyboard     = self->options->state.keyboard;
    const char* cpc_refresh      = self->options->state.refresh;
    const char* cpc_manufacturer = self->options->state.manufacturer;
    const char* cpc_sysrom       = self->options->state.sysrom;
    const char* cpc_rom000       = self->options->state.rom000;
    const char* cpc_rom001       = self->options->state.rom001;
    const char* cpc_rom002       = self->options->state.rom002;
    const char* cpc_rom003       = self->options->state.rom003;
    const char* cpc_rom004       = self->options->state.rom004;
    const char* cpc_rom005       = self->options->state.rom005;
    const char* cpc_rom006       = self->options->state.rom006;
    const char* cpc_rom007       = self->options->state.rom007;
    const char* cpc_rom008       = self->options->state.rom008;
    const char* cpc_rom009       = self->options->state.rom009;
    const char* cpc_rom010       = self->options->state.rom010;
    const char* cpc_rom011       = self->options->state.rom011;
    const char* cpc_rom012       = self->options->state.rom012;
    const char* cpc_rom013       = self->options->state.rom013;
    const char* cpc_rom014       = self->options->state.rom014;
    const char* cpc_rom015       = self->options->state.rom015;
    const char* cpc_expansions[16] = {
        cpc_rom000, cpc_rom001, cpc_rom002, cpc_rom003,
        cpc_rom004, cpc_rom005, cpc_rom006, cpc_rom007,
        cpc_rom008, cpc_rom009, cpc_rom010, cpc_rom011,
        cpc_rom012, cpc_rom013, cpc_rom014, cpc_rom015,
    };

    /* init machine */ {
        self->computer_model = xcpc_computer_model(cpc_model, XCPC_COMPUTER_MODEL_6128);
        switch(self->computer_model) {
            case XCPC_COMPUTER_MODEL_464:
                {
                    self->handlers.paint  = &amstrad_cpc_paint_default;
                    self->handlers.keybd  = &amstrad_cpc_keybd_default;
                    self->handlers.mouse  = &amstrad_cpc_mouse_default;
                    self->ramsize         = RAM_64K;
                    self->monitor_model   = xcpc_monitor_model   (cpc_monitor     , XCPC_MONITOR_MODEL_CTM644  );
                    self->keyboard_layout = xcpc_keyboard_layout (cpc_keyboard    , XCPC_KEYBOARD_LAYOUT_QWERTY);
                    self->refresh_rate    = xcpc_refresh_rate    (cpc_refresh     , XCPC_REFRESH_RATE_50HZ     );
                    self->manufacturer    = xcpc_manufacturer    (cpc_manufacturer, XCPC_MANUFACTURER_AMSTRAD  );
                    system_rom            = (is_set(cpc_sysrom) ? strdup(cpc_sysrom) : build_filename("roms", "cpc464.rom"));
                    amsdos_rom            = (is_set(cpc_rom007) ? strdup(cpc_rom007) : NULL                                );
                }
                break;
            case XCPC_COMPUTER_MODEL_664:
                {
                    self->handlers.paint  = &amstrad_cpc_paint_default;
                    self->handlers.keybd  = &amstrad_cpc_keybd_default;
                    self->handlers.mouse  = &amstrad_cpc_mouse_default;
                    self->ramsize         = RAM_64K;
                    self->monitor_model   = xcpc_monitor_model   (cpc_monitor     , XCPC_MONITOR_MODEL_CTM644  );
                    self->keyboard_layout = xcpc_keyboard_layout (cpc_keyboard    , XCPC_KEYBOARD_LAYOUT_QWERTY);
                    self->refresh_rate    = xcpc_refresh_rate    (cpc_refresh     , XCPC_REFRESH_RATE_50HZ     );
                    self->manufacturer    = xcpc_manufacturer    (cpc_manufacturer, XCPC_MANUFACTURER_AMSTRAD  );
                    system_rom            = (is_set(cpc_sysrom) ? strdup(cpc_sysrom) : build_filename("roms", "cpc664.rom"));
                    amsdos_rom            = (is_set(cpc_rom007) ? strdup(cpc_rom007) : build_filename("roms", "amsdos.rom"));
                }
                break;
            case XCPC_COMPUTER_MODEL_6128:
                {
                    self->handlers.paint  = &amstrad_cpc_paint_default;
                    self->handlers.keybd  = &amstrad_cpc_keybd_default;
                    self->handlers.mouse  = &amstrad_cpc_mouse_default;
                    self->ramsize         = RAM_128K;
                    self->monitor_model   = xcpc_monitor_model   (cpc_monitor     , XCPC_MONITOR_MODEL_CTM644  );
                    self->keyboard_layout = xcpc_keyboard_layout (cpc_keyboard    , XCPC_KEYBOARD_LAYOUT_QWERTY);
                    self->refresh_rate    = xcpc_refresh_rate    (cpc_refresh     , XCPC_REFRESH_RATE_50HZ     );
                    self->manufacturer    = xcpc_manufacturer    (cpc_manufacturer, XCPC_MANUFACTURER_AMSTRAD  );
                    system_rom            = (is_set(cpc_sysrom) ? strdup(cpc_sysrom) : build_filename("roms", "cpc6128.rom"));
                    amsdos_rom            = (is_set(cpc_rom007) ? strdup(cpc_rom007) : build_filename("roms", "amsdos.rom" ));
                }
                break;
            default:
                xcpc_log_error("unknown computer model");
                break;
        }
    }
    /* create monitor */ {
        self->monitor = xcpc_monitor_new();
        self->monitor->iface.user_data = self;
    }
    /* create keyboard */ {
        self->keyboard = xcpc_keyboard_new();
        self->keyboard->iface.user_data = self;
    }
    /* create joystick */ {
        self->joystick = xcpc_joystick_new();
        self->joystick->iface.user_data = self;
    }
    /* create cpu_z80a */ {
        self->cpu_z80a = xcpc_cpu_z80a_new();
        self->cpu_z80a->iface.user_data = self;
        self->cpu_z80a->iface.mreq_m1   = &cpu_mreq_m1;
        self->cpu_z80a->iface.mreq_rd   = &cpu_mreq_rd;
        self->cpu_z80a->iface.mreq_wr   = &cpu_mreq_wr;
        self->cpu_z80a->iface.iorq_m1   = &cpu_iorq_m1;
        self->cpu_z80a->iface.iorq_rd   = &cpu_iorq_rd;
        self->cpu_z80a->iface.iorq_wr   = &cpu_iorq_wr;
    }
    /* create vga_core */ {
        self->vga_core = xcpc_vga_core_new();
        self->vga_core->iface.user_data = self;
    }
    /* create vdc_6845 */ {
        self->vdc_6845 = xcpc_vdc_6845_new();
        self->vdc_6845->iface.user_data = self;
        self->vdc_6845->iface.hsync_callback = &vdc_hsync;
        self->vdc_6845->iface.vsync_callback = &vdc_vsync;
    }
    /* create ppi_8255 */ {
        self->ppi_8255 = xcpc_ppi_8255_new();
        self->ppi_8255->iface.user_data = self;
    }
    /* create psg_8910 */ {
        self->psg_8910 = xcpc_psg_8910_new();
        self->psg_8910->iface.user_data = self;
    }
    /* create fdc_765a */ {
        self->fdc_765a = xcpc_fdc_765a_new();
        self->fdc_765a->iface.user_data = self;
        (void) xcpc_fdc_765a_attach(self->fdc_765a, 0);
        (void) xcpc_fdc_765a_attach(self->fdc_765a, 1);
    }
    /* create ram banks */ {
        size_t requested = self->ramsize;
        size_t allocated = 0UL;
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->ram_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(allocated < requested) {
                self->ram_bank[bank_index] = xcpc_ram_bank_new();
                allocated += sizeof(self->ram_bank[bank_index]->state.data);
            }
            else {
                self->ram_bank[bank_index] = NULL;
                allocated += 0UL;
            }
        }
    }
    /* create lower rom bank */ {
        XcpcRomBank*      rom_bank   = xcpc_rom_bank_new();
        XcpcRomBankStatus rom_status = xcpc_rom_bank_load(rom_bank, system_rom, 0x0000);
        if((rom_bank == NULL) || (rom_status != XCPC_ROM_BANK_STATUS_SUCCESS)) {
            xcpc_log_error("lower-rom: loading error (%s)", system_rom);
        }
        self->rom_bank[0] = rom_bank;
    }
    /* create upper rom bank */ {
        XcpcRomBank*      rom_bank   = xcpc_rom_bank_new();
        XcpcRomBankStatus rom_status = xcpc_rom_bank_load(rom_bank, system_rom, 0x4000);
        if((rom_bank == NULL) || (rom_status != XCPC_ROM_BANK_STATUS_SUCCESS)) {
            xcpc_log_error("upper-rom: loading error (%s)", system_rom);
        }
        self->rom_bank[1] = rom_bank;
    }
    /* create expansion roms banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->exp_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            const char* filename = NULL;
            if(bank_index < countof(cpc_expansions)) {
                filename = cpc_expansions[bank_index];
                if((bank_index == 7) && (is_set(amsdos_rom))) {
                    filename = amsdos_rom;
                }
            }
            if(is_set(filename)) {
                XcpcRomBank*      rom_bank   = xcpc_rom_bank_new();
                XcpcRomBankStatus rom_status = xcpc_rom_bank_load(rom_bank, filename, 0x0000);
                if((rom_bank == NULL) || (rom_status != XCPC_ROM_BANK_STATUS_SUCCESS)) {
                    xcpc_log_error("expansion-rom: loading error (%s)", filename);
                }
                self->exp_bank[bank_index] = rom_bank;
            }
            else {
                self->exp_bank[bank_index] = NULL;
            }
        }
    }
    /* initialize memory pager */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = 4;
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            self->memory.rd.bank[bank_index] = NULL;
            self->memory.wr.bank[bank_index] = NULL;
        }
        self->memory.ram.config = 0x00;
        self->memory.rom.config = 0x00;
    }
    /* initialize timers */ {
        if(gettimeofday(&self->timer.deadline, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
        if(gettimeofday(&self->timer.profiler, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
    }
    /* initialize frame */ {
        self->frame.rate  = 0;
        self->frame.time  = 0;
        self->frame.count = 0;
        self->frame.drawn = 0;
    }
    /* compute frame rate/time and cpu period */ {
        switch(self->refresh_rate) {
            case XCPC_REFRESH_RATE_50HZ:
                self->frame.rate = 50;
                self->frame.time = 20000;
                self->cpu_period = (int) (4000000.0 / (50.0 * 312.5));
                break;
            case XCPC_REFRESH_RATE_60HZ:
                self->frame.rate = 60;
                self->frame.time = 16667;
                self->cpu_period = (int) (4000000.0 / (60.0 * 262.5));
                break;
            default:
                xcpc_log_error("unsupported refresh rate %d", self->refresh_rate);
                break;
        }
        if(self->options->state.turbo != 0) {
            self->frame.time = 1000;
        }
    }
    /* reset instance */ {
        amstrad_cpc_reset(self);
    }
    /* load initial drive0 */ {
        const char* drive0 = self->options->state.drive0;
        if(is_set(drive0)) {
            amstrad_cpc_insert_drive0(self, drive0);
        }
    }
    /* load initial drive1 */ {
        const char* drive1 = self->options->state.drive1;
        if(is_set(drive1)) {
            amstrad_cpc_insert_drive1(self, drive1);
        }
    }
    /* load initial snapshot */ {
        const char* snapshot = self->options->state.snapshot;
        if(is_set(snapshot)) {
            amstrad_cpc_load_snapshot(self, snapshot);
        }
    }
    /* cleanup */ {
        if(system_rom != NULL) {
            system_rom = (free(system_rom), NULL);
        }
        if(amsdos_rom != NULL) {
            amsdos_rom = (free(amsdos_rom), NULL);
        }
    }
}

void amstrad_cpc_close(AMSTRAD_CPC_EMULATOR* self)
{
    /* cleanup handlers */ {
        self->handlers.mouse = &amstrad_cpc_mouse_default;
        self->handlers.keybd = &amstrad_cpc_keybd_default;
        self->handlers.paint = &amstrad_cpc_paint_default;
    }
    /* cleanup memory pager */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = 4;
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            self->memory.rd.bank[bank_index] = NULL;
            self->memory.wr.bank[bank_index] = NULL;
        }
        self->memory.ram.config = 0x00;
        self->memory.rom.config = 0x00;
    }
    /* destroy expansion roms banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->exp_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(self->exp_bank[bank_index] != NULL) {
                self->exp_bank[bank_index] = xcpc_rom_bank_delete(self->exp_bank[bank_index]);
            }
        }
    }
    /* destroy lower/upper rom banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->rom_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(self->rom_bank[bank_index] != NULL) {
                self->rom_bank[bank_index] = xcpc_rom_bank_delete(self->rom_bank[bank_index]);
            }
        }
    }
    /* destroy ram banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->ram_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(self->ram_bank[bank_index] != NULL) {
                self->ram_bank[bank_index] = xcpc_ram_bank_delete(self->ram_bank[bank_index]);
            }
        }
    }
    /* destroy fdc_765a */ {
        self->fdc_765a = xcpc_fdc_765a_delete(self->fdc_765a);
    }
    /* destroy psg_8910 */ {
        self->psg_8910 = xcpc_psg_8910_delete(self->psg_8910);
    }
    /* destroy ppi_8255 */ {
        self->ppi_8255 = xcpc_ppi_8255_delete(self->ppi_8255);
    }
    /* destroy vdc_6845 */ {
        self->vdc_6845 = xcpc_vdc_6845_delete(self->vdc_6845);
    }
    /* destroy vga_core */ {
        self->vga_core = xcpc_vga_core_delete(self->vga_core);
    }
    /* destroy cpu_z80a */ {
        self->cpu_z80a = xcpc_cpu_z80a_delete(self->cpu_z80a);
    }
    /* destroy joystick */ {
        self->joystick = xcpc_joystick_delete(self->joystick);
    }
    /* destroy keyboard */ {
        self->keyboard = xcpc_keyboard_delete(self->keyboard);
    }
    /* destroy monitor */ {
        self->monitor = xcpc_monitor_delete(self->monitor);
    }
}

void amstrad_cpc_reset(AMSTRAD_CPC_EMULATOR* self)
{
    /* reset monitor */ {
        (void) xcpc_monitor_reset(self->monitor);
    }
    /* reset keyboard */ {
        (void) xcpc_keyboard_reset(self->keyboard);
    }
    /* reset joystick */ {
        (void) xcpc_joystick_reset(self->joystick);
    }
    /* reset cpu_z80a */ {
        (void) xcpc_cpu_z80a_reset(self->cpu_z80a);
    }
    /* reset vga_core */ {
        (void) xcpc_vga_core_reset(self->vga_core);
    }
    /* reset vdc_6845 */ {
        (void) xcpc_vdc_6845_reset(self->vdc_6845);
    }
    /* reset ppi_8255 */ {
        (void) xcpc_ppi_8255_reset(self->ppi_8255);
    }
    /* reset psg_8910 */ {
        (void) xcpc_psg_8910_reset(self->psg_8910);
    }
    /* reset fdc_765a */ {
        (void) xcpc_fdc_765a_reset(self->fdc_765a);
    }
    /* reset ram banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->ram_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(self->ram_bank[bank_index] != NULL) {
                (void) xcpc_ram_bank_reset(self->ram_bank[bank_index]);
            }
        }
    }
    /* reset lower/upper rom banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->rom_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(self->rom_bank[bank_index] != NULL) {
                (void) xcpc_rom_bank_reset(self->rom_bank[bank_index]);
            }
        }
    }
    /* reset expansion roms banks */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = countof(self->exp_bank);
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            if(self->exp_bank[bank_index] != NULL) {
                (void) xcpc_rom_bank_reset(self->exp_bank[bank_index]);
            }
        }
    }
    /* reset memory pager */ {
        unsigned int bank_index = 0;
        unsigned int bank_count = 4;
        for(bank_index = 0; bank_index < bank_count; ++bank_index) {
            self->memory.rd.bank[bank_index] = NULL;
            self->memory.wr.bank[bank_index] = NULL;
        }
        self->memory.ram.config = 0x00;
        self->memory.rom.config = 0x00;
        cpc_mem_select(self);
    }
    /* timer */ {
        if(gettimeofday(&self->timer.deadline, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
        if(gettimeofday(&self->timer.profiler, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
    }
    /* frame */ {
        self->frame.rate  |= 0; /* no reset */
        self->frame.time  |= 0; /* no reset */
        self->frame.count &= 0; /* do reset */
        self->frame.drawn &= 0; /* do reset */
    }
    /* stats */ {
        self->stats[0]  = '\0';
    }
}

void amstrad_cpc_load_snapshot(AMSTRAD_CPC_EMULATOR* self, const char* filename)
{
    XcpcSnapshot*      snapshot = xcpc_snapshot_new();
    XcpcSnapshotStatus status   = XCPC_SNAPSHOT_STATUS_SUCCESS;
    uint32_t           ram_size = self->ramsize;

    /* load snapshot */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            status = xcpc_snapshot_load(snapshot, filename);
        }
    }
    /* fetch devices */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_fetch_cpu_z80a(snapshot, self->cpu_z80a);
            (void) xcpc_snapshot_fetch_vga_core(snapshot, self->vga_core);
            (void) xcpc_snapshot_fetch_vdc_6845(snapshot, self->vdc_6845);
            (void) xcpc_snapshot_fetch_ppi_8255(snapshot, self->ppi_8255);
            (void) xcpc_snapshot_fetch_psg_8910(snapshot, self->psg_8910);
            (void) xcpc_snapshot_fetch_fdc_765a(snapshot, self->fdc_765a);
        }
    }
    /* fetch ram/rom */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_fetch_ram_conf(snapshot, &self->memory.ram.config);
            (void) xcpc_snapshot_fetch_rom_conf(snapshot, &self->memory.rom.config);
            (void) xcpc_snapshot_fetch_ram_size(snapshot, &ram_size);
        }
    }
    /* fetch ram */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            uint32_t     snap_size  = self->ramsize;
            uint32_t     bank_size  = RAM_16K;
            unsigned int bank_index = 0;
            while(snap_size >= bank_size) {
                (void) xcpc_snapshot_fetch_ram_bank(snapshot, self->ram_bank[bank_index++]);
                snap_size -= bank_size;
            }
            if(snap_size != 0) {
                status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
            }
        }
    }
    /* cleanup */ {
        snapshot = xcpc_snapshot_delete(snapshot);
    }
    /* check for error */ {
        if(status != XCPC_SNAPSHOT_STATUS_SUCCESS) {
            xcpc_log_error("load snapshot : %s", xcpc_snapshot_strerror(status));
        }
    }
    /* perform memory mapping or reset */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            cpc_mem_select(self);
        }
        else {
            amstrad_cpc_reset(self);
        }
    }
}

void amstrad_cpc_save_snapshot(AMSTRAD_CPC_EMULATOR* self, const char* filename)
{
    XcpcSnapshot*      snapshot = xcpc_snapshot_new();
    XcpcSnapshotStatus status   = XCPC_SNAPSHOT_STATUS_SUCCESS;
    uint32_t           ram_size = self->ramsize;

    /* store devices */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_store_cpu_z80a(snapshot, self->cpu_z80a);
            (void) xcpc_snapshot_store_vga_core(snapshot, self->vga_core);
            (void) xcpc_snapshot_store_vdc_6845(snapshot, self->vdc_6845);
            (void) xcpc_snapshot_store_ppi_8255(snapshot, self->ppi_8255);
            (void) xcpc_snapshot_store_psg_8910(snapshot, self->psg_8910);
            (void) xcpc_snapshot_store_fdc_765a(snapshot, self->fdc_765a);
        }
    }
    /* store ram/rom */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_store_ram_conf(snapshot, &self->memory.ram.config);
            (void) xcpc_snapshot_store_rom_conf(snapshot, &self->memory.rom.config);
            (void) xcpc_snapshot_store_ram_size(snapshot, &ram_size);
        }
    }
    /* store ram */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            uint32_t     snap_size  = self->ramsize;
            uint32_t     bank_size  = RAM_16K;
            unsigned int bank_index = 0;
            while(snap_size >= bank_size) {
                (void) xcpc_snapshot_store_ram_bank(snapshot, self->ram_bank[bank_index++]);
                snap_size -= bank_size;
            }
            if(snap_size != 0) {
                status = XCPC_SNAPSHOT_STATUS_MEMORY_ERROR;
            }
        }
    }
    /* save snapshot */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            status = xcpc_snapshot_save(snapshot, filename);
        }
    }
    /* cleanup */ {
        snapshot = xcpc_snapshot_delete(snapshot);
    }
    /* check for error */ {
        if(status != XCPC_SNAPSHOT_STATUS_SUCCESS) {
            xcpc_log_error("save snapshot : %s", xcpc_snapshot_strerror(status));
        }
    }
}

void amstrad_cpc_insert_drive0(AMSTRAD_CPC_EMULATOR* self, const char* filename)
{
    (void) xcpc_fdc_765a_insert(self->fdc_765a, 0, filename);
}

void amstrad_cpc_remove_drive0(AMSTRAD_CPC_EMULATOR* self)
{
    (void) xcpc_fdc_765a_remove(self->fdc_765a, 0);
}

void amstrad_cpc_insert_drive1(AMSTRAD_CPC_EMULATOR* self, const char* filename)
{
    (void) xcpc_fdc_765a_insert(self->fdc_765a, 1, filename);
}

void amstrad_cpc_remove_drive1(AMSTRAD_CPC_EMULATOR* self)
{
    (void) xcpc_fdc_765a_remove(self->fdc_765a, 1);
}

unsigned long amstrad_cpc_create_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    if(self != NULL) {
        amstrad_cpc_start(self);
    }
    return 0UL;
}

unsigned long amstrad_cpc_destroy_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    if(self != NULL) {
        amstrad_cpc_close(self);
    }
    return 0UL;
}

unsigned long amstrad_cpc_realize_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    int use_xshm = self->options->state.xshm;

    if(self != NULL) {
        /* realize */ {
            (void) xcpc_monitor_realize ( self->monitor
                                        , self->monitor_model
                                        , XtDisplay(widget)
                                        , XtWindow(widget)
                                        , (use_xshm != 0 ? True : False) );
        }
        /* init paint handler */ {
            switch(self->monitor->state.image->bits_per_pixel) {
                case 8:
                    self->handlers.paint = &amstrad_cpc_paint_08bpp;
                    break;
                case 16:
                    self->handlers.paint = &amstrad_cpc_paint_16bpp;
                    break;
                case 32:
                    self->handlers.paint = &amstrad_cpc_paint_32bpp;
                    break;
                default:
                    self->handlers.paint = &amstrad_cpc_paint_default;
                    break;
            }
        }
        /* init keybd handler */ {
            switch(self->keyboard_layout) {
                case XCPC_KEYBOARD_LAYOUT_QWERTY:
                    self->handlers.keybd = &amstrad_cpc_keybd_qwerty;
                    break;
                case XCPC_KEYBOARD_LAYOUT_AZERTY:
                    self->handlers.keybd = &amstrad_cpc_keybd_azerty;
                    break;
                default:
                    self->handlers.keybd = &amstrad_cpc_keybd_default;
                    break;
            }
        }
    }
    return 0UL;
}

unsigned long amstrad_cpc_resize_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    if(self != NULL) {
        (void) xcpc_monitor_resize(self->monitor, event);
    }
    return 0UL;
}

unsigned long amstrad_cpc_redraw_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    if(self != NULL) {
        (void) xcpc_monitor_expose(self->monitor, event);
    }
    return 0UL;
}

unsigned long amstrad_cpc_timer_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    XcpcCpuZ80a* cpu_z80a = self->cpu_z80a;
    XcpcVdc6845* vdc_6845 = self->vdc_6845;
    XcpcFdc765a* fdc_765a = self->fdc_765a;
    unsigned long elapsed = 0;
    unsigned long timeout = 0;

    /* process each scanline */ {
        self->scanlines.index = 0;
        do {
            int cpu_tick;
            for(cpu_tick = 0; cpu_tick < self->cpu_period; cpu_tick += 4) {
                xcpc_vdc_6845_clock(vdc_6845);
                if((cpu_z80a->state.ctrs.i_period += 4) > 0) {
                    int32_t i_period = cpu_z80a->state.ctrs.i_period;
                    xcpc_cpu_z80a_clock(self->cpu_z80a);
                    cpu_z80a->state.ctrs.i_period = i_period - (((i_period - cpu_z80a->state.ctrs.i_period) + 3) & (~3));
                }
            }
        } while(++self->scanlines.index < 312);
    }
    /* clock the fdc */ {
        xcpc_fdc_765a_clock(fdc_765a);
    }
    /* compute the elapsed time in us */ {
        struct timeval prev_time = self->timer.deadline;
        struct timeval curr_time;
        if(gettimeofday(&curr_time, NULL) == 0) {
            const long long t1 = (((long long) prev_time.tv_sec) * 1000000LL) + ((long long) prev_time.tv_usec);
            const long long t2 = (((long long) curr_time.tv_sec) * 1000000LL) + ((long long) curr_time.tv_usec);
            if(t2 >= t1) {
                elapsed = ((unsigned long)(t2 - t1));
            }
            else {
                elapsed = 0UL;
            }
            if(elapsed >= 1000000UL) {
                self->timer.deadline = curr_time;
                elapsed              = 0UL;
            }
        }
    }
    /* draw the frame and compute stats if needed */ {
        if((self->frame.count == 0) || (elapsed <= self->frame.time)) {
            (*self->handlers.paint)(self);
            ++self->frame.drawn;
        }
        if(++self->frame.count == self->frame.rate) {
            compute_stats(self);
        }
    }
    /* compute the next frame absolute time */ {
        if((self->timer.deadline.tv_usec += self->frame.time) >= 1000000) {
            self->timer.deadline.tv_usec -= 1000000;
            self->timer.deadline.tv_sec  += 1;
        }
    }
    /* compute the deadline timeout in us */ {
        struct timeval next_time = self->timer.deadline;
        struct timeval curr_time;
        if(gettimeofday(&curr_time, NULL) == 0) {
            const long long t1 = (((long long) curr_time.tv_sec) * 1000000LL) + ((long long) curr_time.tv_usec);
            const long long t2 = (((long long) next_time.tv_sec) * 1000000LL) + ((long long) next_time.tv_usec);
            if(t2 >= t1) {
                timeout = ((unsigned long)(t2 - t1));
            }
            else {
                timeout = 0UL;
            }
        }
    }
    /* schedule the next frame in ms */ {
        if((timeout /= 1000UL) == 0UL) {
            timeout = 1UL;
        }
    }
    return timeout;
}

unsigned long amstrad_cpc_input_proc(Widget widget, AMSTRAD_CPC_EMULATOR* self, XEvent* event)
{
    switch(event->type) {
        case KeyPress:
            (*self->handlers.keybd)(self, event);
            break;
        case KeyRelease:
            (*self->handlers.keybd)(self, event);
            break;
        case ButtonPress:
            (*self->handlers.mouse)(self, event);
            break;
        case ButtonRelease:
            (*self->handlers.mouse)(self, event);
            break;
        case MotionNotify:
            (*self->handlers.mouse)(self, event);
            break;
        default:
            break;
    }
    return 0UL;
}
