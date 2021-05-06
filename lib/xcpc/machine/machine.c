/*
 * machine.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include <limits.h>
#include <sys/time.h>
#include "machine-priv.h"

#if 0
#define XCPC_DEBUG_IORQ
#define XCPC_DEBUG_VGA_CORE
#define XCPC_DEBUG_VDC_6845
#define XCPC_DEBUG_PSG_8910
#endif

#ifdef XCPC_DEBUG_IORQ
static void debug_iorq(const char* device, const char* reason, uint16_t port, uint8_t data)
{
    xcpc_log_debug("%s %04X : %s 0x%02x", device, port, reason, data);
}
#else
#define debug_iorq(device, reason, port, data) do { (void)(port); (void)(data); } while(0)
#endif

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcMachine::%s()", function);
}

static void default_handler(XcpcMachine* machine, void* user_data)
{
    log_trace("default_handler");
}

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

    (void) snprintf(buffer, sizeof(buffer), "%s/%s", directory, filename);

    return strdup(buffer);
}

static void compute_stats(XcpcMachine* self)
{
    unsigned long elapsed_us = 0;

    /* get the current time */ {
        if(gettimeofday(&self->timer.currtime, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
    }
    /* compute the elapsed time in us */ {
        const long long prevtime = XCPC_TIMESTAMP_OF(&self->timer.profiler);
        const long long currtime = XCPC_TIMESTAMP_OF(&self->timer.currtime);
        if(currtime >= prevtime) {
            elapsed_us = ((unsigned long)(currtime - prevtime));
        }
        else {
            elapsed_us = 0UL;
        }
    }
    /* compute and print the statistics */ {
        if(elapsed_us != 0) {
            const double stats_frames  = ((double)(self->stats.frame_drawn * 1000000UL));
            const double stats_elapsed = ((double)(elapsed_us));
            const double stats_fps     = (stats_frames / stats_elapsed);
            (void) snprintf ( self->stats.buffer, sizeof(self->stats.buffer)
                            , "refresh=%2d hz, framerate=%.2f fps, total-hsync=%d, total-vsync=%d, frames=%d/%d"
                            , self->frame.rate
                            , stats_fps
                            , self->stats.total_hsync
                            , self->stats.total_vsync
                            , self->stats.frame_drawn
                            , self->stats.frame_count );
        }
        else {
            (void) snprintf ( self->stats.buffer, sizeof(self->stats.buffer)
                            , "refresh=%2d hz, framerate=%.2f fps, total-hsync=%d, total-vsync=%d, frames=%d/%d"
                            , self->frame.rate
                            , 0.0f
                            , self->stats.total_hsync
                            , self->stats.total_vsync
                            , self->stats.frame_drawn
                            , self->stats.frame_count );
        }
    }
    /* print the statistics */ {
        if(self->setup.fps != 0) {
            xcpc_log_print(self->stats.buffer);
        }
    }
    /* set the new reference */ {
        self->timer.profiler = self->timer.currtime;
        self->stats.frame_count    = 0;
        self->stats.frame_drawn    = 0;
        self->stats.total_hsync    = 0;
        self->stats.total_vsync    = 0;
    }
#ifdef XCPC_DEBUG_VGA_CORE
    (void) xcpc_vga_core_debug(self->board.vga_core);
#endif
#ifdef XCPC_DEBUG_VDC_6845
    (void) xcpc_vdc_6845_debug(self->board.vdc_6845);
#endif
#ifdef XCPC_DEBUG_PSG_8910
    (void) xcpc_psg_8910_debug(self->board.psg_8910);
#endif
}

static void cpc_mem_select(XcpcMachine* self, uint8_t ram_conf, uint8_t rom_conf)
{
    self->pager.conf.ram = ram_conf;
    self->pager.conf.rom = rom_conf;
    if(self->setup.memory_size >= XCPC_MEMORY_SIZE_128K) {
        switch(self->pager.conf.ram & 0x3f) {
            case 0x00:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[1]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[3]->state.data;
                }
                break;
            case 0x01:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[1]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[7]->state.data;
                }
                break;
            case 0x02:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[4]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[5]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[6]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[7]->state.data;
                }
                break;
            case 0x03:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[3]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[7]->state.data;
                }
                break;
            case 0x04:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[4]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[3]->state.data;
                }
                break;
            case 0x05:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[5]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[3]->state.data;
                }
                break;
            case 0x06:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[6]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[3]->state.data;
                }
                break;
            case 0x07:
                {
                    self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
                    self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[7]->state.data;
                    self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
                    self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[3]->state.data;
                }
                break;
            default:
                xcpc_log_alert("cpc_mem_select() : unsupported ram configuration (%02x)", self->pager.conf.ram);
                break;
        }
    }
    else {
        self->pager.bank.rd[0] = self->pager.bank.wr[0] = self->board.ram_bank[0]->state.data;
        self->pager.bank.rd[1] = self->pager.bank.wr[1] = self->board.ram_bank[1]->state.data;
        self->pager.bank.rd[2] = self->pager.bank.wr[2] = self->board.ram_bank[2]->state.data;
        self->pager.bank.rd[3] = self->pager.bank.wr[3] = self->board.ram_bank[3]->state.data;
    }
    if((self->board.vga_core->state.rmr & 0x04) == 0) {
        if(self->board.rom_bank[0] != NULL) {
            self->pager.bank.rd[0] = self->board.rom_bank[0]->state.data;
        }
    }
    if((self->board.vga_core->state.rmr & 0x08) == 0) {
        if(self->board.rom_bank[1] != NULL) {
            self->pager.bank.rd[3] = self->board.rom_bank[1]->state.data;
        }
        if(self->board.exp_bank[self->pager.conf.rom] != NULL) {
            self->pager.bank.rd[3] = self->board.exp_bank[self->pager.conf.rom]->state.data;
        }
    }
}

static uint8_t cpu_mreq_m1(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data, XcpcMachine* self)
{
    const uint16_t index  = ((addr >> 14) & 0x0003);
    const uint16_t offset = ((addr >>  0) & 0x3fff);

    /* mreq m1 */ {
        data = self->pager.bank.rd[index][offset];
    }
    return data;
}

static uint8_t cpu_mreq_rd(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data, XcpcMachine* self)
{
    const uint16_t index  = ((addr >> 14) & 0x0003);
    const uint16_t offset = ((addr >>  0) & 0x3fff);

    /* mreq rd */ {
        data = self->pager.bank.rd[index][offset];
    }
    return data;
}

static uint8_t cpu_mreq_wr(XcpcCpuZ80a* cpu_z80a, uint16_t addr, uint8_t data, XcpcMachine* self)
{
    const uint16_t index  = ((addr >> 14) & 0x0003);
    const uint16_t offset = ((addr >>  0) & 0x3fff);

    /* mreq wr */ {
        self->pager.bank.wr[index][offset] = data;
    }
    return data;
}

static uint8_t cpu_iorq_m1(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data, XcpcMachine* self)
{
    /* clear data */ {
        data = 0x00;
    }
    /* iorq m1 */ {
        self->board.vga_core->state.counter &= 0x1f;
    }
    return data;
}

static uint8_t cpu_iorq_rd(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data, XcpcMachine* self)
{
    /* clear data */ {
        data = 0x00;
    }
    /* vga-core [0-------xxxxxxxx] [0x7fxx] */ {
        if((port & 0x8000) == 0) {
            const uint8_t function = ((data >> 6) & 3);
            switch(function) {
                case 0: /* select pen */
                    {
                        data = xcpc_vga_core_illegal(self->board.vga_core, data);
                        debug_iorq("vga-core", "set-pen (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 1: /* select ink */
                    {
                        data = xcpc_vga_core_illegal(self->board.vga_core, data);
                        debug_iorq("vga-core", "set-ink (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 2: /* interrupt control, rom configuration and screen mode */
                    {
                        data = xcpc_vga_core_illegal(self->board.vga_core, data);
                        debug_iorq("vga-core", "set-rmr (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 3: /* ram memory management */
                    {
                        data = xcpc_vga_core_illegal(self->board.vga_core, data);
                        debug_iorq("ram-conf", "set-ram (illegal read)", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    /* vdc-6845 [-0------xxxxxxxx] [0xbfxx] */ {
        if((port & 0x4000) == 0) {
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [-0----00xxxxxxxx] [0xbcxx] */
                    {
                        data = xcpc_vdc_6845_illegal(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "set-addr (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 1: /* [-0----01xxxxxxxx] [0xbdxx] */
                    {
                        data = xcpc_vdc_6845_illegal(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "set-data (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 2: /* [-0----10xxxxxxxx] [0xbexx] */
                    {
                        data = xcpc_vdc_6845_illegal(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "get-addr (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 3: /* [-0----11xxxxxxxx] [0xbfxx] */
                    {
                        data = xcpc_vdc_6845_rd_data(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "get-data", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    /* rom-conf [--0-----xxxxxxxx] [0xdfxx] */ {
        if((port & 0x2000) == 0) {
            debug_iorq("rom-conf", "illegal read", port, (data & 0xff));
        }
    }
    /* prt-port [---0----xxxxxxxx] [0xefxx] */ {
        if((port & 0x1000) == 0) {
            debug_iorq("prt-port", "get-data", port, (data & 0xff));
        }
    }
    /* ppi-8255 [----0---xxxxxxxx] [0xf7xx] */ {
        if((port & 0x0800) == 0) {
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [----0-00xxxxxxxx] [0xf4xx] */
                    {
                        data = xcpc_ppi_8255_rd_port_a(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "get-port-a", port, (data & 0xff));
                    }
                    break;
                case 1: /* [----0-01xxxxxxxx] [0xf5xx] */
                    {
                        data = xcpc_ppi_8255_rd_port_b(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "get-port-b", port, (data & 0xff));
                    }
                    break;
                case 2: /* [----0-10xxxxxxxx] [0xf6xx] */
                    {
                        data = xcpc_ppi_8255_rd_port_c(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "get-port-c", port, (data & 0xff));
                    }
                    break;
                case 3: /* [----0-11xxxxxxxx] [0xf7xx] */
                    {
                        data = xcpc_ppi_8255_illegal(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "set-ctrl-p (illegal read)", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    /* fdc-765a [-----0--0xxxxxxx] [0xfb7f] */ {
        if((port & 0x0480) == 0) {
            const uint8_t function = (((port >> 7) & 2) | (port & 1));
            switch(function) {
                case 0: /* [-----0-00xxxxxx0] [0xfa7e] */
                    {
                        data = xcpc_fdc_765a_illegal(self->board.fdc_765a, data);
                        debug_iorq("fdc-765a", "set-motor (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 1: /* [-----0-00xxxxxx1] [0xfa7f] */
                    {
                        data = xcpc_fdc_765a_illegal(self->board.fdc_765a, data);
                        debug_iorq("fdc-765a", "set-motor (illegal read)", port, (data & 0xff));
                    }
                    break;
                case 2: /* [-----0-10xxxxxx0] [0xfb7e] */
                    {
                        data = xcpc_fdc_765a_rd_stat(self->board.fdc_765a, data);
                        debug_iorq("fdc-765a", "get-stat", port, (data & 0xff));
                    }
                    break;
                case 3: /* [-----0-10xxxxxx1] [0xfb7f] */
                    {
                        data = xcpc_fdc_765a_rd_data(self->board.fdc_765a, data);
                        debug_iorq("fdc-765a", "get-data", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    return data;
}

static uint8_t cpu_iorq_wr(XcpcCpuZ80a* cpu_z80a, uint16_t port, uint8_t data, XcpcMachine* self)
{
    /* vga-core [0-------xxxxxxxx] [0x7fxx] */ {
        if((port & 0x8000) == 0) {
            const uint8_t function = ((data >> 6) & 3);
            switch(function) {
                case 0: /* select pen */
                    {
                        (void) xcpc_vga_core_set_pen(self->board.vga_core, data);
                        debug_iorq("vga-core", "set-pen", port, (data & 0x1f));
                    }
                    break;
                case 1: /* select ink */
                    {
                        (void) xcpc_vga_core_set_ink(self->board.vga_core, data);
                        debug_iorq("vga-core", "set-ink", port, (data & 0x1f));
                    }
                    break;
                case 2: /* interrupt control, rom configuration and screen mode */
                    {
                        (void) xcpc_vga_core_set_rmr(self->board.vga_core, data);
                        cpc_mem_select(self, self->pager.conf.ram, self->pager.conf.rom);
                        debug_iorq("vga-core", "set-rmr", port, (data & 0x1f));
                    }
                    break;
                case 3: /* ram memory management */
                    {
                        cpc_mem_select(self, (data & 0x3f), self->pager.conf.rom);
                        debug_iorq("ram-conf", "set-ram", port, (data & 0x1f));
                    }
                    break;
            }
        }
    }
    /* vdc-6845 [-0------xxxxxxxx] [0xbfxx] */ {
        if((port & 0x4000) == 0) {
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [-0----00xxxxxxxx] [0xbcxx] */
                    {
                        (void) xcpc_vdc_6845_wr_addr(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "set-addr", port, (data & 0xff));
                    }
                    break;
                case 1: /* [-0----01xxxxxxxx] [0xbdxx] */
                    {
                        (void) xcpc_vdc_6845_wr_data(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "set-data", port, (data & 0xff));
                    }
                    break;
                case 2: /* [-0----10xxxxxxxx] [0xbexx] */
                    {
                        (void) xcpc_vdc_6845_illegal(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "get-addr (illegal write)", port, (data & 0xff));
                    }
                    break;
                case 3: /* [-0----11xxxxxxxx] [0xbfxx] */
                    {
                        (void) xcpc_vdc_6845_illegal(self->board.vdc_6845, data);
                        debug_iorq("vdc-6845", "get-data (illegal write)", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    /* rom-conf [--0-----xxxxxxxx] [0xdfxx] */ {
        if((port & 0x2000) == 0) {
            cpc_mem_select(self, self->pager.conf.ram, (data & 0xff));
            debug_iorq("rom-conf", "set-rom", port, (data & 0xff));
        }
    }
    /* prt-port [---0----xxxxxxxx] [0xefxx] */ {
        if((port & 0x1000) == 0) {
            debug_iorq("prt-port", "set-data", port, (data & 0xff));
        }
    }
    /* ppi-8255 [----0---xxxxxxxx] [0xf7xx] */ {
        if((port & 0x0800) == 0) {
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [----0-00xxxxxxxx] [0xf4xx] */
                    {
                        (void) xcpc_ppi_8255_wr_port_a(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "set-port-a", port, (data & 0xff));
                    }
                    break;
                case 1: /* [----0-01xxxxxxxx] [0xf5xx] */
                    {
                        (void) xcpc_ppi_8255_wr_port_b(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "set-port-b", port, (data & 0xff));
                    }
                    break;
                case 2: /* [----0-10xxxxxxxx] [0xf6xx] */
                    {
                        (void) xcpc_ppi_8255_wr_port_c(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "set-port-c", port, (data & 0xff));
                    }
                    break;
                case 3: /* [----0-11xxxxxxxx] [0xf7xx] */
                    {
                        (void) xcpc_ppi_8255_wr_ctrl_p(self->board.ppi_8255, data);
                        debug_iorq("ppi-8255", "set-ctrl-p", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    /* fdc-765a [-----0--0xxxxxxx] [0xfb7f] */ {
        if((port & 0x0480) == 0) {
            const uint8_t function = (((port >> 7) & 2) | ((port >> 0) & 1));
            switch(function) {
                case 0: /* [-----0-00xxxxxx0] [0xfa7e] */
                    {
                        (void) xcpc_fdc_765a_set_motor(self->board.fdc_765a, (((data & 1) << 1) | ((data & 1) << 0)));
                        debug_iorq("fdc-765a", "set-motor", port, (data & 0xff));
                    }
                    break;
                case 1: /* [-----0-00xxxxxx1] [0xfa7f] */
                    {
                        (void) xcpc_fdc_765a_set_motor(self->board.fdc_765a, (((data & 1) << 1) | ((data & 1) << 0)));
                        debug_iorq("fdc-765a", "set-motor", port, (data & 0xff));
                    }
                    break;
                case 2: /* [-----0-10xxxxxx0] [0xfb7e] */
                    {
                        (void) xcpc_fdc_765a_wr_stat(self->board.fdc_765a, data);
                        debug_iorq("fdc-765a", "set-stat", port, (data & 0xff));
                    }
                    break;
                case 3: /* [-----0-10xxxxxx1] [0xfb7f] */
                    {
                        (void) xcpc_fdc_765a_wr_data(self->board.fdc_765a, data);
                        debug_iorq("fdc-765a", "set-DATA", port, (data & 0xff));
                    }
                    break;
            }
        }
    }
    return data;
}

static uint8_t vdc_frame(XcpcVdc6845* vdc_6845, int frame, XcpcMachine* self)
{
    return 0x00;
}

static uint8_t vdc_hsync(XcpcVdc6845* vdc_6845, int hsync, XcpcMachine* self)
{
    XcpcMonitor* monitor  = self->board.monitor;
    XcpcCpuZ80a* cpu_z80a = self->board.cpu_z80a;
    XcpcVgaCore* vga_core = self->board.vga_core;

    if((self->state.hsync = hsync) != 0) {
        const unsigned int last_scanline = countof(self->frame.scanline_array) - 1;
        /* rising edge */ {
            ++self->stats.total_hsync;
            if(++self->frame.beam_y > last_scanline) {
                self->frame.beam_y = last_scanline;
            }
            /* update scanline */ {
                XcpcScanline* scanline = &self->frame.scanline_array[self->frame.beam_y];
                /* update mode */ {
                    scanline->mode = vga_core->state.rmr & 0x03;
                }
                /* update colors */ {
                    int index = 0;
                    do {
                        const uint8_t ink = vga_core->state.ink[index];
                        scanline->color[index].value  = ink;
                        scanline->color[index].pixel1 = monitor->state.palette1[ink].pixel;
                        scanline->color[index].pixel2 = monitor->state.palette2[ink].pixel;
                    } while(++index < 17);
                }
            }
        }
    }
    else {
        /* falling edge */ {
            ++vga_core->state.counter;
            if(vga_core->state.delayed == 0) {
                if(vga_core->state.counter == 52) {
                    xcpc_cpu_z80a_pulse_int(cpu_z80a);
                    vga_core->state.counter = 0;
                }
            }
            else if(--vga_core->state.delayed == 0) {
                if(vga_core->state.counter >= 32) {
                    xcpc_cpu_z80a_pulse_int(cpu_z80a);
                }
                vga_core->state.counter = 0;
            }
        }
    }
    return 0x00;
}

static uint8_t vdc_vsync(XcpcVdc6845* vdc_6845, int vsync, XcpcMachine* self)
{
    if((self->state.vsync = vsync) != 0) {
        /* rising edge */ {
            ++self->stats.total_vsync;
            self->board.vga_core->state.delayed = 2;
        }
    }
    else {
        /* falling edge */ {
            self->frame.beam_y = 0;
        }
    }
    return 0x00;
}

static uint8_t ppi_rd_port_a(XcpcPpi8255* ppi_8255, uint8_t data, XcpcMachine* self)
{
    /* read from psg */ {
        const uint8_t psg_function = (self->state.psg_bdir << 2)
                                   | (self->state.psg_bc2  << 1)
                                   | (self->state.psg_bc1  << 0)
                                   ;
        data = self->state.psg_data;
        switch(psg_function & 0x07) {
            case 0x03: /* read from psg */
                data = xcpc_psg_8910_rd_data(self->board.psg_8910, data);
                break;
            case 0x06: /* write to psg  */
                data = xcpc_psg_8910_illegal(self->board.psg_8910, data);
                break;
            case 0x07: /* latch address */
                data = xcpc_psg_8910_illegal(self->board.psg_8910, data);
                break;
            default:   /* inactive      */
                break;
        }
    }
    return data;
}

static uint8_t ppi_wr_port_a(XcpcPpi8255* ppi_8255, uint8_t data, XcpcMachine* self)
{
    /* write to psg */ {
        const uint8_t psg_function = (self->state.psg_bdir << 2)
                                   | (self->state.psg_bc2  << 1)
                                   | (self->state.psg_bc1  << 0)
                                   ;
        self->state.psg_data = data;
        switch(psg_function & 0x07) {
            case 0x03: /* read from psg */
                data = xcpc_psg_8910_illegal(self->board.psg_8910, data);
                break;
            case 0x06: /* write to psg  */
                data = xcpc_psg_8910_wr_data(self->board.psg_8910, data);
                break;
            case 0x07: /* latch address */
                data = xcpc_psg_8910_wr_addr(self->board.psg_8910, data);
                break;
            default:   /* inactive      */
                break;
        }
    }
    return data;
}

static uint8_t ppi_rd_port_b(XcpcPpi8255* ppi_8255, uint8_t data, XcpcMachine* self)
{
    /* read port b */ {
        data = ((self->state.cas_read  & 0x01) << 7)
             | ((self->state.parallel  & 0x01) << 6)
             | ((self->state.expansion & 0x01) << 5)
             | ((self->state.refresh   & 0x01) << 4)
             | ((self->state.company   & 0x07) << 1)
             | ((self->state.vsync     & 0x01) << 0);
    }
    return data;
}

static uint8_t ppi_wr_port_b(XcpcPpi8255* ppi_8255, uint8_t data, XcpcMachine* self)
{
    /* write port b - illegal */ {
        data = ((self->state.cas_read  & 0x01) << 7)
             | ((self->state.parallel  & 0x01) << 6)
             | ((self->state.expansion & 0x01) << 5)
             | ((self->state.refresh   & 0x01) << 4)
             | ((self->state.company   & 0x07) << 1)
             | ((self->state.vsync     & 0x01) << 0);
    }
    return data;
}

static uint8_t ppi_rd_port_c(XcpcPpi8255* ppi_8255, uint8_t data, XcpcMachine* self)
{
    /* read port c */ {
        /* illegal */
    }
    return data;
}

static uint8_t ppi_wr_port_c(XcpcPpi8255* ppi_8255, uint8_t data, XcpcMachine* self)
{
    /* update state */ {
        self->state.psg_bdir  = ((data & 0x80) >> 7);
        self->state.psg_bc1   = ((data & 0x40) >> 6);
        self->state.cas_write = ((data & 0x20) >> 5);
        self->state.cas_motor = ((data & 0x10) >> 4);
    }
    /* update keyboard line */ {
        (void) xcpc_keyboard_set_line(self->board.keyboard, ((data & 0x0f) >> 0));
    }
    /* execute psg operation */ {
        const uint8_t psg_function = (self->state.psg_bdir << 2)
                                   | (self->state.psg_bc2  << 1)
                                   | (self->state.psg_bc1  << 0)
                                   ;
        switch(psg_function & 0x07) {
            case 0x03: /* read from psg */
                (void) xcpc_psg_8910_rd_data(self->board.psg_8910, self->state.psg_data);
                break;
            case 0x06: /* write to psg  */
                (void) xcpc_psg_8910_wr_data(self->board.psg_8910, self->state.psg_data);
                break;
            case 0x07: /* latch address */
                (void) xcpc_psg_8910_wr_addr(self->board.psg_8910, self->state.psg_data);
                break;
            default:   /* inactive      */
                break;
        }
    }
    return data;
}

static uint8_t psg_rd_port_a(XcpcPsg8910* psg_8910, uint8_t data, XcpcMachine* self)
{
    /* read keyboard */ {
        data = (self->state.psg_data = xcpc_keyboard_get_data(self->board.keyboard, 0xff));
    }
    return data;
}

static uint8_t psg_wr_port_a(XcpcPsg8910* psg_8910, uint8_t data, XcpcMachine* self)
{
    return 0xff;
}

static uint8_t psg_rd_port_b(XcpcPsg8910* psg_8910, uint8_t data, XcpcMachine* self)
{
    return 0xff;
}

static uint8_t psg_wr_port_b(XcpcPsg8910* psg_8910, uint8_t data, XcpcMachine* self)
{
    return 0xff;
}

static void paint_default(XcpcMachine* self)
{
}

static void paint_08bpp(XcpcMachine* self)
{
    XcpcMonitor*  monitor  = self->board.monitor;
    XcpcVdc6845*  vdc_6845 = self->board.vdc_6845;
    XcpcVgaCore*  vga_core = self->board.vga_core;
    XcpcScanline* scanline = self->frame.scanline_array;
    XImage*       ximage   = monitor->state.image;
    const uint8_t* const mode0 = vga_core->setup.mode0;
    const uint8_t* const mode1 = vga_core->setup.mode1;
    const uint8_t* const mode2 = vga_core->setup.mode2;
    const uint8_t* const ram[4] = {
        self->board.ram_bank[0]->state.data,
        self->board.ram_bank[1]->state.data,
        self->board.ram_bank[2]->state.data,
        self->board.ram_bank[3]->state.data,
    };
    const XcpcHorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc_6845->state.regs.named.horizontal_total     < 63 ? vdc_6845->state.regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc_6845->state.regs.named.horizontal_displayed < 52 ? vdc_6845->state.regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc_6845->state.regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + (((vdc_6845->state.regs.named.sync_width >> 0) & 0x0f) << 4)),
    };
    const XcpcVertProps v = {
        /* ch  : pixels */ (1 + (vdc_6845->state.regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc_6845->state.regs.named.vertical_total     < 40 ? vdc_6845->state.regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc_6845->state.regs.named.vertical_displayed < 40 ? vdc_6845->state.regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc_6845->state.regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + (((vdc_6845->state.regs.named.sync_width >> 4) & 0x0f) << 0)),
    };
    const XcpcBorders b = {
        /* top : pixels */ ((v.ch * (v.vt - v.vsp)) - (v.vsw != 0 ? v.vsw :  16)),
        /* bot : pixels */ ((v.ch * (v.vsp - v.vd)) + (v.vsw != 0 ? v.vsw :  16)),
        /* lft : pixels */ ((h.cw * (h.ht - h.hsp)) - (h.hsw != 0 ? h.hsw : 256)),
        /* rgt : pixels */ ((h.cw * (h.hsp - h.hd)) + (h.hsw != 0 ? h.hsw : 256)),
    };
    unsigned int address = ((vdc_6845->state.regs.named.start_address_high << 8) | (vdc_6845->state.regs.named.start_address_low  << 0));
    const unsigned int rowstride  = ximage->bytes_per_line;
    int                remaining_lines = ximage->height;
    uint8_t* data_iter = XCPC_BYTE_PTR(ximage->data);
    uint8_t* this_line = NULL;
    uint8_t* next_line = NULL;
    uint8_t  pixel1    = 0;
    uint8_t  pixel2    = 0;
    int      row       = 0;
    int      col       = 0;
    int      ras       = 0;

    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                this_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                next_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                pixel1 = scanline->color[16].pixel1;
                pixel2 = scanline->color[16].pixel2;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(col = 0; col < cols; ++col) {
                *this_line++ = pixel1;
                *next_line++ = pixel2;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(row = 0; row < rows; ++row) {
            for(ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    this_line = data_iter;
                    data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                    next_line = data_iter;
                    data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel1 = scanline->color[16].pixel1;
                    pixel2 = scanline->color[16].pixel2;
                    for(col = 0; col < lfts; ++col) {
                        *this_line++ = pixel1;
                        *next_line++ = pixel2;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel1 = scanline->color[16].pixel1;
                    pixel2 = scanline->color[16].pixel2;
                    for(col = 0; col < rgts; ++col) {
                        *this_line++ = pixel1;
                        *next_line++ = pixel2;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                this_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                next_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                pixel1 = scanline->color[16].pixel1;
                pixel2 = scanline->color[16].pixel2;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(col = 0; col < cols; ++col) {
                *this_line++ = pixel1;
                *next_line++ = pixel2;
            }
            ++scanline;
        }
    }
    /* put image */ {
        (void) xcpc_monitor_put_image(self->board.monitor);
    }
}

static void paint_16bpp(XcpcMachine* self)
{
    XcpcMonitor*  monitor  = self->board.monitor;
    XcpcVdc6845*  vdc_6845 = self->board.vdc_6845;
    XcpcVgaCore*  vga_core = self->board.vga_core;
    XcpcScanline* scanline = self->frame.scanline_array;
    XImage*       ximage   = monitor->state.image;
    const uint8_t* const mode0 = vga_core->setup.mode0;
    const uint8_t* const mode1 = vga_core->setup.mode1;
    const uint8_t* const mode2 = vga_core->setup.mode2;
    const uint8_t* const ram[4] = {
        self->board.ram_bank[0]->state.data,
        self->board.ram_bank[1]->state.data,
        self->board.ram_bank[2]->state.data,
        self->board.ram_bank[3]->state.data,
    };
    const XcpcHorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc_6845->state.regs.named.horizontal_total     < 63 ? vdc_6845->state.regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc_6845->state.regs.named.horizontal_displayed < 52 ? vdc_6845->state.regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc_6845->state.regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + (((vdc_6845->state.regs.named.sync_width >> 0) & 0x0f) << 4)),
    };
    const XcpcVertProps v = {
        /* ch  : pixels */ (1 + (vdc_6845->state.regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc_6845->state.regs.named.vertical_total     < 40 ? vdc_6845->state.regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc_6845->state.regs.named.vertical_displayed < 40 ? vdc_6845->state.regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc_6845->state.regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + (((vdc_6845->state.regs.named.sync_width >> 4) & 0x0f) << 0)),
    };
    const XcpcBorders b = {
        /* top : pixels */ ((v.ch * (v.vt - v.vsp)) - (v.vsw != 0 ? v.vsw :  16)),
        /* bot : pixels */ ((v.ch * (v.vsp - v.vd)) + (v.vsw != 0 ? v.vsw :  16)),
        /* lft : pixels */ ((h.cw * (h.ht - h.hsp)) - (h.hsw != 0 ? h.hsw : 256)),
        /* rgt : pixels */ ((h.cw * (h.hsp - h.hd)) + (h.hsw != 0 ? h.hsw : 256)),
    };
    unsigned int address = ((vdc_6845->state.regs.named.start_address_high << 8) | (vdc_6845->state.regs.named.start_address_low  << 0));
    const unsigned int rowstride  = ximage->bytes_per_line;
    int                remaining_lines = ximage->height;
    uint16_t* data_iter = XCPC_WORD_PTR(ximage->data);
    uint16_t* this_line = NULL;
    uint16_t* next_line = NULL;
    uint16_t  pixel1    = 0;
    uint16_t  pixel2    = 0;
    int       row       = 0;
    int       col       = 0;
    int       ras       = 0;

    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                this_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                next_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                pixel1 = scanline->color[16].pixel1;
                pixel2 = scanline->color[16].pixel2;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(col = 0; col < cols; ++col) {
                *this_line++ = pixel1;
                *next_line++ = pixel2;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(row = 0; row < rows; ++row) {
            for(ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    this_line = data_iter;
                    data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                    next_line = data_iter;
                    data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel1 = scanline->color[16].pixel1;
                    pixel2 = scanline->color[16].pixel2;
                    for(col = 0; col < lfts; ++col) {
                        *this_line++ = pixel1;
                        *next_line++ = pixel2;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel1 = scanline->color[16].pixel1;
                    pixel2 = scanline->color[16].pixel2;
                    for(col = 0; col < rgts; ++col) {
                        *this_line++ = pixel1;
                        *next_line++ = pixel2;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                this_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                next_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                pixel1 = scanline->color[16].pixel1;
                pixel2 = scanline->color[16].pixel2;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(col = 0; col < cols; ++col) {
                *this_line++ = pixel1;
                *next_line++ = pixel2;
            }
            ++scanline;
        }
    }
    /* put image */ {
        (void) xcpc_monitor_put_image(self->board.monitor);
    }
}

static void paint_32bpp(XcpcMachine* self)
{
    XcpcMonitor*  monitor  = self->board.monitor;
    XcpcVdc6845*  vdc_6845 = self->board.vdc_6845;
    XcpcVgaCore*  vga_core = self->board.vga_core;
    XcpcScanline* scanline = self->frame.scanline_array;
    XImage*       ximage   = monitor->state.image;
    const uint8_t* const mode0 = vga_core->setup.mode0;
    const uint8_t* const mode1 = vga_core->setup.mode1;
    const uint8_t* const mode2 = vga_core->setup.mode2;
    const uint8_t* const ram[4] = {
        self->board.ram_bank[0]->state.data,
        self->board.ram_bank[1]->state.data,
        self->board.ram_bank[2]->state.data,
        self->board.ram_bank[3]->state.data,
    };
    const XcpcHorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc_6845->state.regs.named.horizontal_total     < 63 ? vdc_6845->state.regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc_6845->state.regs.named.horizontal_displayed < 52 ? vdc_6845->state.regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc_6845->state.regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + (((vdc_6845->state.regs.named.sync_width >> 0) & 0x0f) << 4)),
    };
    const XcpcVertProps v = {
        /* ch  : pixels */ (1 + (vdc_6845->state.regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc_6845->state.regs.named.vertical_total     < 40 ? vdc_6845->state.regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc_6845->state.regs.named.vertical_displayed < 40 ? vdc_6845->state.regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc_6845->state.regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + (((vdc_6845->state.regs.named.sync_width >> 4) & 0x0f) << 0)),
    };
    const XcpcBorders b = {
        /* top : pixels */ ((v.ch * (v.vt - v.vsp)) - (v.vsw != 0 ? v.vsw :  16)),
        /* bot : pixels */ ((v.ch * (v.vsp - v.vd)) + (v.vsw != 0 ? v.vsw :  16)),
        /* lft : pixels */ ((h.cw * (h.ht - h.hsp)) - (h.hsw != 0 ? h.hsw : 256)),
        /* rgt : pixels */ ((h.cw * (h.hsp - h.hd)) + (h.hsw != 0 ? h.hsw : 256)),
    };
    unsigned int address = ((vdc_6845->state.regs.named.start_address_high << 8) | (vdc_6845->state.regs.named.start_address_low  << 0));
    const unsigned int rowstride  = ximage->bytes_per_line;
    int                remaining_lines = ximage->height;
    uint32_t* data_iter = XCPC_LONG_PTR(ximage->data);
    uint32_t* this_line = NULL;
    uint32_t* next_line = NULL;
    uint32_t  pixel1    = 0;
    uint32_t  pixel2    = 0;
    int       row       = 0;
    int       col       = 0;
    int       ras       = 0;

    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                this_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                next_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                pixel1 = scanline->color[16].pixel1;
                pixel2 = scanline->color[16].pixel2;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(col = 0; col < cols; ++col) {
                *this_line++ = pixel1;
                *next_line++ = pixel2;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(row = 0; row < rows; ++row) {
            for(ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    this_line = data_iter;
                    data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                    next_line = data_iter;
                    data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel1 = scanline->color[16].pixel1;
                    pixel2 = scanline->color[16].pixel2;
                    for(col = 0; col < lfts; ++col) {
                        *this_line++ = pixel1;
                        *next_line++ = pixel2;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            pixel2 = scanline->color[byte & 0x0f].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            pixel2 = scanline->color[byte & 0x03].pixel2;
                                            *this_line++ = pixel1; *this_line++ = pixel1;
                                            *next_line++ = pixel2; *next_line++ = pixel2;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            pixel2 = scanline->color[byte & 0x01].pixel2;
                                            *this_line++ = pixel1;
                                            *next_line++ = pixel2;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel1 = scanline->color[16].pixel1;
                    pixel2 = scanline->color[16].pixel2;
                    for(col = 0; col < rgts; ++col) {
                        *this_line++ = pixel1;
                        *next_line++ = pixel2;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                this_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                next_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + rowstride);
                pixel1 = scanline->color[16].pixel1;
                pixel2 = scanline->color[16].pixel2;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(col = 0; col < cols; ++col) {
                *this_line++ = pixel1;
                *next_line++ = pixel2;
            }
            ++scanline;
        }
    }
    /* put image */ {
        (void) xcpc_monitor_put_image(self->board.monitor);
    }
}

static void keybd_default(XcpcMachine* self, XEvent* event)
{
}

static void keybd_qwerty(XcpcMachine* self, XEvent* event)
{
    (void) xcpc_keyboard_qwerty(self->board.keyboard, &event->xkey);
}

static void keybd_azerty(XcpcMachine* self, XEvent* event)
{
    (void) xcpc_keyboard_azerty(self->board.keyboard, &event->xkey);
}

static void mouse_default(XcpcMachine* self, XEvent* event)
{
    (void) xcpc_keyboard_joystick(self->board.keyboard, event);
}

static void construct_iface(XcpcMachine* self, const XcpcMachineIface* iface)
{
    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = NULL;
        self->iface.reserved0 = NULL;
        self->iface.reserved1 = NULL;
        self->iface.reserved2 = NULL;
        self->iface.reserved3 = NULL;
        self->iface.reserved4 = NULL;
        self->iface.reserved5 = NULL;
        self->iface.reserved6 = NULL;
        self->iface.reserved7 = NULL;
    }
    if(self->iface.reserved0 == NULL) { self->iface.reserved0 = &default_handler; }
    if(self->iface.reserved1 == NULL) { self->iface.reserved1 = &default_handler; }
    if(self->iface.reserved2 == NULL) { self->iface.reserved2 = &default_handler; }
    if(self->iface.reserved3 == NULL) { self->iface.reserved3 = &default_handler; }
    if(self->iface.reserved4 == NULL) { self->iface.reserved4 = &default_handler; }
    if(self->iface.reserved5 == NULL) { self->iface.reserved5 = &default_handler; }
    if(self->iface.reserved6 == NULL) { self->iface.reserved6 = &default_handler; }
    if(self->iface.reserved7 == NULL) { self->iface.reserved7 = &default_handler; }
}

static void destruct_iface(XcpcMachine* self)
{
}

static void reset_iface(XcpcMachine* self)
{
}

static void construct_setup(XcpcMachine* self, XcpcOptions* options)
{
    self->setup.options       = options;
    self->setup.company_name  = XCPC_COMPANY_NAME_DEFAULT;
    self->setup.machine_type  = XCPC_MACHINE_TYPE_DEFAULT;
    self->setup.monitor_type  = XCPC_MONITOR_TYPE_DEFAULT;
    self->setup.refresh_rate  = XCPC_REFRESH_RATE_DEFAULT;
    self->setup.keyboard_type = XCPC_KEYBOARD_TYPE_DEFAULT;
    self->setup.memory_size   = XCPC_MEMORY_SIZE_DEFAULT;
    self->setup.turbo         = 0;
    self->setup.xshm          = 0;
    self->setup.fps           = 0;
}

static void destruct_setup(XcpcMachine* self)
{
}

static void reset_setup(XcpcMachine* self)
{
}

static void construct_state(XcpcMachine* self)
{
    self->state.hsync     = 0; /* no hsync      */
    self->state.vsync     = 0; /* no vsync      */
    self->state.refresh   = 1; /* 50Hz          */
    self->state.company   = 7; /* amstrad       */
    self->state.expansion = 1; /* present       */
    self->state.parallel  = 1; /* not connected */
    self->state.psg_data  = 0; /* no data       */
    self->state.psg_bdir  = 0; /* no data       */
    self->state.psg_bc1   = 0; /* no data       */
    self->state.psg_bc2   = 1; /* always set    */
    self->state.cas_read  = 0; /* no data       */
    self->state.cas_write = 0; /* no data       */
    self->state.cas_motor = 0; /* no data       */
}

static void destruct_state(XcpcMachine* self)
{
}

static void reset_state(XcpcMachine* self)
{
    self->state.hsync     &= 0; /* clear value  */
    self->state.vsync     &= 0; /* clear value  */
    self->state.refresh   |= 0; /* don't modify */
    self->state.company   |= 0; /* don't modify */
    self->state.expansion |= 0; /* don't modify */
    self->state.parallel  |= 0; /* don't modify */
    self->state.psg_data  &= 0; /* clear value  */
    self->state.psg_bdir  &= 0; /* clear value  */
    self->state.psg_bc1   &= 0; /* clear value  */
    self->state.psg_bc2   |= 0; /* don't modify */
    self->state.cas_read  &= 0; /* clear value  */
    self->state.cas_write &= 0; /* clear value  */
    self->state.cas_motor &= 0; /* clear value  */
}

static void construct_board(XcpcMachine* self)
{
    /* monitor */ {
        const XcpcMonitorIface monitor_iface = {
            self /* user_data */
        };
        if(self->board.monitor == NULL) {
            self->board.monitor = xcpc_monitor_new(&monitor_iface);
        }
    }
    /* keyboard */ {
        const XcpcKeyboardIface keyboard_iface = {
            self /* user_data */
        };
        if(self->board.keyboard == NULL) {
            self->board.keyboard = xcpc_keyboard_new(&keyboard_iface);
        }
    }
    /* joystick */ {
        const XcpcJoystickIface joystick_iface = {
            self /* user_data */
        };
        if(self->board.joystick == NULL) {
            self->board.joystick = xcpc_joystick_new(&joystick_iface);
        }
    }
    /* cpu_z80a */ {
        const XcpcCpuZ80aIface cpu_z80a_iface = {
            self,                                  /* user_data */
            XCPC_CPU_Z80A_MREQ_FUNC(&cpu_mreq_m1), /* mreq_m1   */
            XCPC_CPU_Z80A_MREQ_FUNC(&cpu_mreq_rd), /* mreq_rd   */
            XCPC_CPU_Z80A_MREQ_FUNC(&cpu_mreq_wr), /* mreq_wr   */
            XCPC_CPU_Z80A_IORQ_FUNC(&cpu_iorq_m1), /* iorq_m1   */
            XCPC_CPU_Z80A_IORQ_FUNC(&cpu_iorq_rd), /* iorq_rd   */
            XCPC_CPU_Z80A_IORQ_FUNC(&cpu_iorq_wr), /* iorq_wr   */
        };
        if(self->board.cpu_z80a == NULL) {
            self->board.cpu_z80a = xcpc_cpu_z80a_new(&cpu_z80a_iface);
        }
    }
    /* vga_core */ {
        const XcpcVgaCoreIface vga_core_iface = {
            self /* user_data */
        };
        if(self->board.vga_core == NULL) {
            self->board.vga_core = xcpc_vga_core_new(&vga_core_iface);
        }
    }
    /* vdc_6845 */ {
        const XcpcVdc6845Iface vdc_6845_iface = {
            self,                                 /* user_data */
            XCPC_VDC_6845_FRAME_FUNC(&vdc_frame), /* frame     */
            XCPC_VDC_6845_HSYNC_FUNC(&vdc_hsync), /* hsync     */
            XCPC_VDC_6845_VSYNC_FUNC(&vdc_vsync), /* vsync     */
        };
        if(self->board.vdc_6845 == NULL) {
            self->board.vdc_6845 = xcpc_vdc_6845_new(&vdc_6845_iface);
        }
    }
    /* ppi_8255 */ {
        const XcpcPpi8255Iface ppi_8255_iface = {
            self,                                  /* user_data */
            XCPC_PPI_8255_RD_FUNC(&ppi_rd_port_a), /* rd_port_a */
            XCPC_PPI_8255_WR_FUNC(&ppi_wr_port_a), /* wr_port_a */
            XCPC_PPI_8255_RD_FUNC(&ppi_rd_port_b), /* rd_port_b */
            XCPC_PPI_8255_WR_FUNC(&ppi_wr_port_b), /* wr_port_b */
            XCPC_PPI_8255_RD_FUNC(&ppi_rd_port_c), /* rd_port_c */
            XCPC_PPI_8255_WR_FUNC(&ppi_wr_port_c), /* wr_port_c */
        };
        if(self->board.ppi_8255 == NULL) {
            self->board.ppi_8255 = xcpc_ppi_8255_new(&ppi_8255_iface);
        }
    }
    /* psg_8910 */ {
        const XcpcPsg8910Iface psg_8910_iface = {
            self,                                  /* user_data */
            XCPC_PSG_8910_RD_FUNC(&psg_rd_port_a), /* rd_port_a */
            XCPC_PSG_8910_WR_FUNC(&psg_wr_port_a), /* wr_port_a */
            XCPC_PSG_8910_RD_FUNC(&psg_rd_port_b), /* rd_port_b */
            XCPC_PSG_8910_WR_FUNC(&psg_wr_port_b), /* wr_port_b */
        };
        if(self->board.psg_8910 == NULL) {
            self->board.psg_8910 = xcpc_psg_8910_new(&psg_8910_iface);
        }
    }
    /* fdc_765a */ {
        const XcpcFdc765aIface fdc_765a_iface = {
            self /* user_data */
        };
        if(self->board.fdc_765a == NULL) {
            self->board.fdc_765a = xcpc_fdc_765a_new(&fdc_765a_iface);
            (void) xcpc_fdc_765a_attach(self->board.fdc_765a, XCPC_FDC_765A_DRIVE0);
            (void) xcpc_fdc_765a_attach(self->board.fdc_765a, XCPC_FDC_765A_DRIVE1);
        }
    }
    /* ram_bank */ {
        const XcpcRamBankIface ram_bank_iface = {
            self /* user_data */
        };
        unsigned int ram_index = 0;
        unsigned int ram_count = countof(self->board.ram_bank);
        do {
            if(self->board.ram_bank[ram_index] == NULL) {
                self->board.ram_bank[ram_index] = xcpc_ram_bank_new(&ram_bank_iface);
            }
        } while(++ram_index < ram_count);
    }
    /* rom_bank */ {
        const XcpcRomBankIface rom_bank_iface = {
            self /* user_data */
        };
        unsigned int rom_index = 0;
        unsigned int rom_count = countof(self->board.rom_bank);
        do {
            if(self->board.rom_bank[rom_index] == NULL) {
                self->board.rom_bank[rom_index] = xcpc_rom_bank_new(&rom_bank_iface);
            }
        } while(++rom_index < rom_count);
    }
#if 0
    /* exp_bank */ {
        const XcpcRomBankIface exp_bank_iface = {
            self /* user_data */
        };
        unsigned int exp_index = 0;
        unsigned int exp_count = countof(self->board.exp_bank);
        do {
            if(self->board.exp_bank[exp_index] == NULL) {
                self->board.exp_bank[exp_index] = xcpc_rom_bank_new(&exp_bank_iface);
            }
        } while(++exp_index < exp_count);
    }
#endif
}

static void destruct_board(XcpcMachine* self)
{
    /* exp_bank */ {
        unsigned int exp_index = 0;
        unsigned int exp_count = countof(self->board.exp_bank);
        do {
            if(self->board.exp_bank[exp_index] != NULL) {
                self->board.exp_bank[exp_index] = xcpc_rom_bank_delete(self->board.exp_bank[exp_index]);
            }
        } while(++exp_index < exp_count);
    }
    /* rom_bank */ {
        unsigned int rom_index = 0;
        unsigned int rom_count = countof(self->board.rom_bank);
        do {
            if(self->board.rom_bank[rom_index] != NULL) {
                self->board.rom_bank[rom_index] = xcpc_rom_bank_delete(self->board.rom_bank[rom_index]);
            }
        } while(++rom_index < rom_count);
    }
    /* ram_bank */ {
        unsigned int ram_index = 0;
        unsigned int ram_count = countof(self->board.ram_bank);
        do {
            if(self->board.ram_bank[ram_index] != NULL) {
                self->board.ram_bank[ram_index] = xcpc_ram_bank_delete(self->board.ram_bank[ram_index]);
            }
        } while(++ram_index < ram_count);
    }
    /* fdc_765a */ {
        if(self->board.fdc_765a != NULL) {
            self->board.fdc_765a = xcpc_fdc_765a_delete(self->board.fdc_765a);
        }
    }
    /* psg_8910 */ {
        if(self->board.psg_8910 != NULL) {
            self->board.psg_8910 = xcpc_psg_8910_delete(self->board.psg_8910);
        }
    }
    /* ppi_8255 */ {
        if(self->board.ppi_8255 != NULL) {
            self->board.ppi_8255 = xcpc_ppi_8255_delete(self->board.ppi_8255);
        }
    }
    /* vdc_6845 */ {
        if(self->board.vdc_6845 != NULL) {
            self->board.vdc_6845 = xcpc_vdc_6845_delete(self->board.vdc_6845);
        }
    }
    /* vga_core */ {
        if(self->board.vga_core != NULL) {
            self->board.vga_core = xcpc_vga_core_delete(self->board.vga_core);
        }
    }
    /* cpu_z80a */ {
        if(self->board.cpu_z80a != NULL) {
            self->board.cpu_z80a = xcpc_cpu_z80a_delete(self->board.cpu_z80a);
        }
    }
    /* joystick */ {
        if(self->board.joystick != NULL) {
            self->board.joystick = xcpc_joystick_delete(self->board.joystick);
        }
    }
    /* keyboard */ {
        if(self->board.keyboard != NULL) {
            self->board.keyboard = xcpc_keyboard_delete(self->board.keyboard);
        }
    }
    /* monitor */ {
        if(self->board.monitor != NULL) {
            self->board.monitor = xcpc_monitor_delete(self->board.monitor);
        }
    }
}

static void reset_board(XcpcMachine* self)
{
    /* monitor */ {
        if(self->board.monitor != NULL) {
            (void) xcpc_monitor_reset(self->board.monitor);
        }
    }
    /* keyboard */ {
        if(self->board.keyboard != NULL) {
            (void) xcpc_keyboard_reset(self->board.keyboard);
        }
    }
    /* joystick */ {
        if(self->board.joystick != NULL) {
            (void) xcpc_joystick_reset(self->board.joystick);
        }
    }
    /* cpu_z80a */ {
        if(self->board.cpu_z80a != NULL) {
            (void) xcpc_cpu_z80a_reset(self->board.cpu_z80a);
        }
    }
    /* vga_core */ {
        if(self->board.vga_core != NULL) {
            (void) xcpc_vga_core_reset(self->board.vga_core);
        }
    }
    /* vdc_6845 */ {
        if(self->board.vdc_6845 != NULL) {
            (void) xcpc_vdc_6845_reset(self->board.vdc_6845);
        }
    }
    /* ppi_8255 */ {
        if(self->board.ppi_8255 != NULL) {
            (void) xcpc_ppi_8255_reset(self->board.ppi_8255);
        }
    }
    /* psg_8910 */ {
        if(self->board.psg_8910 != NULL) {
            (void) xcpc_psg_8910_reset(self->board.psg_8910);
        }
    }
    /* fdc_765a */ {
        if(self->board.fdc_765a != NULL) {
            (void) xcpc_fdc_765a_reset(self->board.fdc_765a);
        }
    }
    /* ram_bank */ {
        unsigned int ram_index = 0;
        unsigned int ram_count = countof(self->board.ram_bank);
        do {
            if(self->board.ram_bank[ram_index] != NULL) {
                (void) xcpc_ram_bank_reset(self->board.ram_bank[ram_index]);
            }
        } while(++ram_index < ram_count);
    }
    /* rom_bank */ {
        unsigned int rom_index = 0;
        unsigned int rom_count = countof(self->board.rom_bank);
        do {
            if(self->board.rom_bank[rom_index] != NULL) {
                (void) xcpc_rom_bank_reset(self->board.rom_bank[rom_index]);
            }
        } while(++rom_index < rom_count);
    }
    /* exp_bank */ {
        unsigned int exp_index = 0;
        unsigned int exp_count = countof(self->board.exp_bank);
        do {
            if(self->board.exp_bank[exp_index] != NULL) {
                (void) xcpc_rom_bank_reset(self->board.exp_bank[exp_index]);
            }
        } while(++exp_index < exp_count);
    }
}

static void construct_pager(XcpcMachine* self)
{
}

static void destruct_pager(XcpcMachine* self)
{
}

static void reset_pager(XcpcMachine* self)
{
    self->pager.bank.rd[0] = self->pager.bank.wr[0] = NULL;
    self->pager.bank.rd[1] = self->pager.bank.wr[1] = NULL;
    self->pager.bank.rd[2] = self->pager.bank.wr[2] = NULL;
    self->pager.bank.rd[3] = self->pager.bank.wr[3] = NULL;
    self->pager.conf.ram = 0x00;
    self->pager.conf.rom = 0x00;
    cpc_mem_select(self, self->pager.conf.ram, self->pager.conf.rom);
}

static void construct_frame(XcpcMachine* self)
{
    self->frame.scanline_count = 0;
    self->frame.beam_x         = 0;
    self->frame.beam_y         = 0;
    self->frame.rate           = 0;
    self->frame.duration       = 0;
    self->frame.cpu_ticks      = 0;
}

static void destruct_frame(XcpcMachine* self)
{
}

static void reset_frame(XcpcMachine* self)
{
    self->frame.scanline_count |= 0; /* don't modify */
    self->frame.beam_x         &= 0; /* clear value  */
    self->frame.beam_y         &= 0; /* clear value  */
    self->frame.rate           |= 0; /* don't modify */
    self->frame.duration       |= 0; /* don't modify */
    self->frame.cpu_ticks      |= 0; /* don't modify */
}

static void construct_stats(XcpcMachine* self)
{
    self->stats.frame_count = 0;
    self->stats.frame_drawn = 0;
    self->stats.total_hsync = 0;
    self->stats.total_vsync = 0;
    self->stats.buffer[0]   = '\0';
}

static void destruct_stats(XcpcMachine* self)
{
}

static void reset_stats(XcpcMachine* self)
{
    self->stats.frame_count &= 0; /* clear value  */
    self->stats.frame_drawn &= 0; /* clear value  */
    self->stats.total_hsync &= 0; /* clear value  */
    self->stats.total_vsync &= 0; /* clear value  */
    self->stats.buffer[0]   &= 0; /* clear value  */
}

static void construct_timer(XcpcMachine* self)
{
    self->timer.currtime.tv_sec  = 0;
    self->timer.currtime.tv_usec = 0;
    self->timer.deadline.tv_sec  = 0;
    self->timer.deadline.tv_usec = 0;
    self->timer.profiler.tv_sec  = 0;
    self->timer.profiler.tv_usec = 0;
}

static void destruct_timer(XcpcMachine* self)
{
}

static void reset_timer(XcpcMachine* self)
{
    struct timeval now;

    if(gettimeofday(&now, NULL) != 0) {
        xcpc_log_error("gettimeofday() has failed");
        now.tv_sec  = 0;
        now.tv_usec = 0;
    }
    self->timer.currtime = now;
    self->timer.deadline = now;
    self->timer.profiler = now;
}

static void construct_funcs(XcpcMachine* self)
{
    self->funcs.paint_func = &paint_default;
    self->funcs.keybd_func = &keybd_default;
    self->funcs.mouse_func = &mouse_default;
}

static void destruct_funcs(XcpcMachine* self)
{
}

static void reset_funcs(XcpcMachine* self)
{
}

static void initialize(XcpcMachine* self)
{
    char* system_rom = NULL;
    char* amsdos_rom = NULL;
    XcpcOptions* options     = self->setup.options;
    const char* opt_company  = options->state.company;
    const char* opt_machine  = options->state.machine;
    const char* opt_monitor  = options->state.monitor;
    const char* opt_refresh  = options->state.refresh;
    const char* opt_keyboard = options->state.keyboard;
    const char* opt_system   = options->state.sysrom;
    const char* opt_amsdos   = options->state.rom007;
    const char* opt_drive0   = options->state.drive0;
    const char* opt_drive1   = options->state.drive1;
    const char* opt_snapshot = options->state.snapshot;
    const int   opt_turbo    = options->state.turbo;
    const int   opt_xshm     = options->state.xshm;
    const int   opt_fps      = options->state.fps;

    /* initialize setup */ {
        self->setup.company_name  = xcpc_company_name(opt_company, XCPC_COMPANY_NAME_DEFAULT);
        self->setup.machine_type  = xcpc_machine_type(opt_machine, XCPC_MACHINE_TYPE_DEFAULT);
        self->setup.monitor_type  = xcpc_monitor_type(opt_monitor, XCPC_MONITOR_TYPE_DEFAULT);
        self->setup.refresh_rate  = xcpc_refresh_rate(opt_refresh, XCPC_REFRESH_RATE_DEFAULT);
        self->setup.keyboard_type = xcpc_keyboard_type(opt_keyboard, XCPC_KEYBOARD_TYPE_DEFAULT);
        self->setup.memory_size   = XCPC_MEMORY_SIZE_DEFAULT;
        self->setup.turbo         = opt_turbo;
        self->setup.xshm          = opt_xshm;
        self->setup.fps           = opt_fps;
    }
    /* adjust company name */ {
        if(self->setup.company_name == XCPC_COMPANY_NAME_DEFAULT) {
            self->setup.company_name = XCPC_COMPANY_NAME_AMSTRAD;
        }
    }
    /* adjust machine type */ {
        if(self->setup.machine_type == XCPC_MACHINE_TYPE_DEFAULT) {
            self->setup.machine_type = XCPC_MACHINE_TYPE_CPC6128;
        }
    }
    /* adjust monitor type */ {
        if(self->setup.monitor_type == XCPC_MONITOR_TYPE_DEFAULT) {
            self->setup.monitor_type = XCPC_MONITOR_TYPE_COLOR;
        }
    }
    /* adjust refresh rate */ {
        if(self->setup.refresh_rate == XCPC_REFRESH_RATE_DEFAULT) {
            self->setup.refresh_rate = XCPC_REFRESH_RATE_50HZ;
        }
    }
    /* adjust keyboard type */ {
        if(self->setup.keyboard_type == XCPC_KEYBOARD_TYPE_DEFAULT) {
            self->setup.keyboard_type = XCPC_KEYBOARD_TYPE_QWERTY;
        }
    }
    /* adjust memory size */ {
        if(self->setup.memory_size == XCPC_MEMORY_SIZE_DEFAULT) {
            if(self->setup.machine_type == XCPC_MACHINE_TYPE_CPC6128) {
                self->setup.memory_size = XCPC_MEMORY_SIZE_128K;
            }
            else {
                self->setup.memory_size = XCPC_MEMORY_SIZE_64K;
            }
        }
    }
    /* prepare roms */ {
        switch(self->setup.machine_type) {
            case XCPC_MACHINE_TYPE_CPC464:
                {
                    system_rom = (is_set(opt_system) ? strdup(opt_system) : build_filename(xcpc_get_romdir(), "cpc464.rom"));
                    amsdos_rom = (is_set(opt_amsdos) ? strdup(opt_amsdos) : NULL                                           );
                }
                break;
            case XCPC_MACHINE_TYPE_CPC664:
                {
                    system_rom = (is_set(opt_system) ? strdup(opt_system) : build_filename(xcpc_get_romdir(), "cpc664.rom"));
                    amsdos_rom = (is_set(opt_amsdos) ? strdup(opt_amsdos) : build_filename(xcpc_get_romdir(), "amsdos.rom"));
                }
                break;
            case XCPC_MACHINE_TYPE_CPC6128:
                {
                    system_rom = (is_set(opt_system) ? strdup(opt_system) : build_filename(xcpc_get_romdir(), "cpc6128.rom"));
                    amsdos_rom = (is_set(opt_amsdos) ? strdup(opt_amsdos) : build_filename(xcpc_get_romdir(), "amsdos.rom" ));
                }
                break;
            default:
                xcpc_log_error("unknown machine type");
                break;
        }
    }
    /* load lower rom */ {
        XcpcRomBankStatus status = xcpc_rom_bank_load(self->board.rom_bank[0], system_rom, 0x0000);
        if(status != XCPC_ROM_BANK_STATUS_SUCCESS) {
            xcpc_log_error("lower-rom: loading error (%s)", system_rom);
        }
    }
    /* load upper rom */ {
        XcpcRomBankStatus status = xcpc_rom_bank_load(self->board.rom_bank[1], system_rom, 0x4000);
        if(status != XCPC_ROM_BANK_STATUS_SUCCESS) {
            xcpc_log_error("upper-rom: loading error (%s)", system_rom);
        }
    }
    /* load expansion roms */ {
        const char* cpc_expansions[16] = {
            options->state.rom000,
            options->state.rom001,
            options->state.rom002,
            options->state.rom003,
            options->state.rom004,
            options->state.rom005,
            options->state.rom006,
            options->state.rom007,
            options->state.rom008,
            options->state.rom009,
            options->state.rom010,
            options->state.rom011,
            options->state.rom012,
            options->state.rom013,
            options->state.rom014,
            options->state.rom015,
        };
        /* create expansion roms banks */ {
            const XcpcRomBankIface exp_bank_iface = {
                self /* user_data */
            };
            unsigned int exp_index = 0;
            unsigned int exp_count = countof(self->board.exp_bank);
            for(exp_index = 0; exp_index < exp_count; ++exp_index) {
                const char* filename = NULL;
                if(exp_index < countof(cpc_expansions)) {
                    filename = cpc_expansions[exp_index];
                    if((exp_index == 7) && (is_set(amsdos_rom))) {
                        filename = amsdos_rom;
                    }
                }
                if(is_set(filename)) {
                    if(self->board.exp_bank[exp_index] == NULL) {
                        self->board.exp_bank[exp_index] = xcpc_rom_bank_new(&exp_bank_iface);
                    }
                }
                else {
                    if(self->board.exp_bank[exp_index] != NULL) {
                        self->board.exp_bank[exp_index] = xcpc_rom_bank_delete(self->board.exp_bank[exp_index]);
                    }
                }
                if(is_set(filename)) {
                    XcpcRomBankStatus status = xcpc_rom_bank_load(self->board.exp_bank[exp_index], filename, 0x0000);
                    if(status != XCPC_ROM_BANK_STATUS_SUCCESS) {
                        xcpc_log_error("expansion-rom: loading error (%s)", filename);
                    }
                }
            }
        }
    }
    /* compute frame rate/time and cpu period */ {
        switch(self->setup.refresh_rate) {
            case XCPC_REFRESH_RATE_50HZ:
                self->state.company        = ((self->setup.company_name - 1) & 7);
                self->state.refresh        = 1;
                self->frame.rate           = 50;
                self->frame.duration       = 20000;
                self->frame.scanline_count = 312;
                self->frame.beam_x         = 0;
                self->frame.beam_y         = 0;
                self->frame.cpu_ticks      = (int) (4000000.0 / (50.0 * 312.5));
                break;
            case XCPC_REFRESH_RATE_60HZ:
                self->state.company        = ((self->setup.company_name - 1) & 7);
                self->state.refresh        = 0;
                self->frame.rate           = 60;
                self->frame.duration       = 16667;
                self->frame.scanline_count = 262;
                self->frame.beam_x         = 0;
                self->frame.beam_y         = 0;
                self->frame.cpu_ticks      = (int) (4000000.0 / (60.0 * 262.5));
                break;
            default:
                xcpc_log_error("unsupported refresh rate %d", self->setup.refresh_rate);
                break;
        }
        if(self->setup.turbo != 0) {
            self->frame.rate     = 2000;
            self->frame.duration = 500;
        }
    }
    /* reset instance */ {
        (void) xcpc_machine_reset(self);
    }
    /* load initial snapshot */ {
        if(is_set(opt_snapshot)) {
            xcpc_machine_load_snapshot(self, opt_snapshot);
        }
    }
    /* load initial drive0 */ {
        if(is_set(opt_drive0)) {
            xcpc_machine_insert_drive0(self, opt_drive0);
        }
    }
    /* load initial drive1 */ {
        if(is_set(opt_drive1)) {
            xcpc_machine_insert_drive1(self, opt_drive1);
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

XcpcMachine* xcpc_machine_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcMachine);
}

XcpcMachine* xcpc_machine_free(XcpcMachine* self)
{
    log_trace("free");

    return xcpc_delete(XcpcMachine, self);
}

XcpcMachine* xcpc_machine_construct(XcpcMachine* self, const XcpcMachineIface* iface, XcpcOptions* options)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcMachineIface));
        (void) memset(&self->setup, 0, sizeof(XcpcMachineSetup));
        (void) memset(&self->state, 0, sizeof(XcpcMachineState));
        (void) memset(&self->board, 0, sizeof(XcpcMachineBoard));
        (void) memset(&self->pager, 0, sizeof(XcpcMachinePager));
        (void) memset(&self->frame, 0, sizeof(XcpcMachineFrame));
        (void) memset(&self->stats, 0, sizeof(XcpcMachineStats));
        (void) memset(&self->timer, 0, sizeof(XcpcMachineTimer));
        (void) memset(&self->funcs, 0, sizeof(XcpcMachineFuncs));
    }
    /* construct all subsystems */ {
        construct_iface(self, iface);
        construct_setup(self, options);
        construct_state(self);
        construct_board(self);
        construct_pager(self);
        construct_frame(self);
        construct_stats(self);
        construct_timer(self);
        construct_funcs(self);
    }
    /* initialize */ {
        (void) initialize(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_destruct(XcpcMachine* self)
{
    log_trace("destruct");

    /* destruct all subsystems */ {
        destruct_funcs(self);
        destruct_timer(self);
        destruct_stats(self);
        destruct_frame(self);
        destruct_pager(self);
        destruct_board(self);
        destruct_state(self);
        destruct_setup(self);
        destruct_iface(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_new(const XcpcMachineIface* iface, XcpcOptions* options)
{
    log_trace("new");

    return xcpc_machine_construct(xcpc_machine_alloc(), iface, options);
}

XcpcMachine* xcpc_machine_delete(XcpcMachine* self)
{
    log_trace("delete");

    return xcpc_machine_free(xcpc_machine_destruct(self));
}

XcpcMachine* xcpc_machine_reset(XcpcMachine* self)
{
    log_trace("reset");

    /* reset all subsystems */ {
        reset_iface(self);
        reset_setup(self);
        reset_state(self);
        reset_board(self);
        reset_pager(self);
        reset_frame(self);
        reset_stats(self);
        reset_timer(self);
        reset_funcs(self);
    }
    return self;
}

XcpcMachine* xcpc_machine_clock(XcpcMachine* self)
{
    XcpcCpuZ80a* cpu_z80a = self->board.cpu_z80a;
    XcpcVdc6845* vdc_6845 = self->board.vdc_6845;
    XcpcPsg8910* psg_8910 = self->board.psg_8910;
    XcpcFdc765a* fdc_765a = self->board.fdc_765a;

    /* process each scanline */ {
        int scanlines = self->frame.scanline_count;
        do {
            int32_t old_i_period;
            int32_t new_i_period;
            int cpu_ticks = self->frame.cpu_ticks;
            do {
                (void) xcpc_vdc_6845_clock(vdc_6845);
                if((cpu_z80a->state.ctrs.i_period += 4) > 0) {
                    old_i_period = cpu_z80a->state.ctrs.i_period;
                    (void) xcpc_cpu_z80a_clock(self->board.cpu_z80a);
                    new_i_period = cpu_z80a->state.ctrs.i_period;
                    cpu_z80a->state.ctrs.i_period = old_i_period - (((old_i_period - new_i_period) + 3) & (~3));
                }
                (void) xcpc_psg_8910_clock(psg_8910);
            } while((cpu_ticks -= 4) > 0);
        } while(--scanlines > 0);
    }
    /* clock the fdc */ {
        (void) xcpc_fdc_765a_clock(fdc_765a);
    }
    return self;
}

XcpcMachine* xcpc_machine_insert_drive0(XcpcMachine* self, const char* filename)
{
    log_trace("xcpc_machine_insert_drive0");

    if(self->board.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_insert(self->board.fdc_765a, XCPC_FDC_765A_DRIVE0, filename);
    }
    return self;
}

XcpcMachine* xcpc_machine_remove_drive0(XcpcMachine* self)
{
    log_trace("xcpc_machine_remove_drive0");

    if(self->board.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_remove(self->board.fdc_765a, XCPC_FDC_765A_DRIVE0);
    }
    return self;
}

const char* xcpc_machine_filename_drive0(XcpcMachine* self)
{
    log_trace("xcpc_machine_filename_drive0");

    if(self->board.fdc_765a != NULL) {
        return xcpc_fdc_765a_filename(self->board.fdc_765a, XCPC_FDC_765A_DRIVE0, NULL);
    }
    return NULL;
}

XcpcMachine* xcpc_machine_insert_drive1(XcpcMachine* self, const char* filename)
{
    log_trace("xcpc_machine_insert_drive1");

    if(self->board.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_insert(self->board.fdc_765a, XCPC_FDC_765A_DRIVE1, filename);
    }
    return self;
}

XcpcMachine* xcpc_machine_remove_drive1(XcpcMachine* self)
{
    log_trace("xcpc_machine_remove_drive1");

    if(self->board.fdc_765a != NULL) {
        (void) xcpc_fdc_765a_remove(self->board.fdc_765a, XCPC_FDC_765A_DRIVE1);
    }
    return self;
}

const char* xcpc_machine_filename_drive1(XcpcMachine* self)
{
    log_trace("xcpc_machine_filename_drive1");

    if(self->board.fdc_765a != NULL) {
        return xcpc_fdc_765a_filename(self->board.fdc_765a, XCPC_FDC_765A_DRIVE1, NULL);
    }
    return NULL;
}

XcpcMachine* xcpc_machine_load_snapshot(XcpcMachine* self, const char* filename)
{
    XcpcSnapshot*      snapshot = xcpc_snapshot_new();
    XcpcSnapshotStatus status   = XCPC_SNAPSHOT_STATUS_SUCCESS;
    uint32_t           ram_size = self->setup.memory_size;

    log_trace("xcpc_machine_load_snapshot");
    /* reset and clock just one frame to avoid a strange bug (eg. gryzor) */ {
        (void) xcpc_machine_reset(self);
        (void) xcpc_machine_clock(self);
    }
    /* load snapshot */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            status = xcpc_snapshot_load(snapshot, filename);
        }
    }
    /* fetch devices */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_fetch_cpu_z80a(snapshot, self->board.cpu_z80a);
            (void) xcpc_snapshot_fetch_vga_core(snapshot, self->board.vga_core);
            (void) xcpc_snapshot_fetch_vdc_6845(snapshot, self->board.vdc_6845);
            (void) xcpc_snapshot_fetch_ppi_8255(snapshot, self->board.ppi_8255);
            (void) xcpc_snapshot_fetch_psg_8910(snapshot, self->board.psg_8910);
            (void) xcpc_snapshot_fetch_fdc_765a(snapshot, self->board.fdc_765a);
        }
    }
    /* fetch ram/rom */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_fetch_ram_conf(snapshot, &self->pager.conf.ram);
            (void) xcpc_snapshot_fetch_rom_conf(snapshot, &self->pager.conf.rom);
            (void) xcpc_snapshot_fetch_ram_size(snapshot, &ram_size);
        }
    }
    /* fetch ram */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            uint32_t     snap_size  = self->setup.memory_size;
            uint32_t     bank_size  = 16384;
            unsigned int bank_index = 0;
            while(snap_size >= bank_size) {
                (void) xcpc_snapshot_fetch_ram_bank(snapshot, self->board.ram_bank[bank_index++]);
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
            cpc_mem_select(self, self->pager.conf.ram, self->pager.conf.rom);
            self->board.vga_core->state.counter = 32;
        }
        else {
            (void) xcpc_machine_reset(self);
        }
    }
    return self;
}

XcpcMachine* xcpc_machine_save_snapshot(XcpcMachine* self, const char* filename)
{
    XcpcSnapshot*      snapshot = xcpc_snapshot_new();
    XcpcSnapshotStatus status   = XCPC_SNAPSHOT_STATUS_SUCCESS;
    uint32_t           ram_size = self->setup.memory_size;

    log_trace("xcpc_machine_save_snapshot");
    /* store devices */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_store_cpu_z80a(snapshot, self->board.cpu_z80a);
            (void) xcpc_snapshot_store_vga_core(snapshot, self->board.vga_core);
            (void) xcpc_snapshot_store_vdc_6845(snapshot, self->board.vdc_6845);
            (void) xcpc_snapshot_store_ppi_8255(snapshot, self->board.ppi_8255);
            (void) xcpc_snapshot_store_psg_8910(snapshot, self->board.psg_8910);
            (void) xcpc_snapshot_store_fdc_765a(snapshot, self->board.fdc_765a);
        }
    }
    /* store ram/rom */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            (void) xcpc_snapshot_store_ram_conf(snapshot, &self->pager.conf.ram);
            (void) xcpc_snapshot_store_rom_conf(snapshot, &self->pager.conf.rom);
            (void) xcpc_snapshot_store_ram_size(snapshot, &ram_size);
        }
    }
    /* store ram */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            uint32_t     snap_size  = self->setup.memory_size;
            uint32_t     bank_size  = 16384;
            unsigned int bank_index = 0;
            while(snap_size >= bank_size) {
                (void) xcpc_snapshot_store_ram_bank(snapshot, self->board.ram_bank[bank_index++]);
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
    return self;
}

unsigned long xcpc_machine_create_proc(XcpcMachine* self, XEvent* event)
{
    return 0UL;
}

unsigned long xcpc_machine_destroy_proc(XcpcMachine* self, XEvent* event)
{
    /* unrealize */ {
        (void) xcpc_monitor_unrealize(self->board.monitor);
    }
    return 0UL;
}

unsigned long xcpc_machine_realize_proc(XcpcMachine* self, XEvent* event)
{
    /* realize */ {
        (void) xcpc_monitor_realize ( self->board.monitor
                                    , self->setup.monitor_type
                                    , self->setup.refresh_rate
                                    , event->xany.display
                                    , event->xany.window
                                    , (self->setup.xshm != 0 ? True : False) );
    }
    /* init paint handler */ {
        switch(self->board.monitor->state.image->bits_per_pixel) {
            case 8:
                self->funcs.paint_func = &paint_08bpp;
                break;
            case 16:
                self->funcs.paint_func = &paint_16bpp;
                break;
            case 32:
                self->funcs.paint_func = &paint_32bpp;
                break;
            default:
                self->funcs.paint_func = &paint_default;
                break;
        }
    }
    /* init keybd handler */ {
        switch(self->setup.keyboard_type) {
            case XCPC_KEYBOARD_TYPE_QWERTY:
                self->funcs.keybd_func = &keybd_qwerty;
                break;
            case XCPC_KEYBOARD_TYPE_AZERTY:
                self->funcs.keybd_func = &keybd_azerty;
                break;
            default:
                self->funcs.keybd_func = &keybd_default;
                break;
        }
    }
    return 0UL;
}

unsigned long xcpc_machine_resize_proc(XcpcMachine* self, XEvent* event)
{
    if(event->type == ConfigureNotify) {
        (void) xcpc_monitor_resize(self->board.monitor, &event->xconfigure);
    }
    return 0UL;
}

unsigned long xcpc_machine_expose_proc(XcpcMachine* self, XEvent* event)
{
    if(event->type == Expose) {
        (void) xcpc_monitor_expose(self->board.monitor, &event->xexpose);
    }
    return 0UL;
}

unsigned long xcpc_machine_input_proc(XcpcMachine* self, XEvent* event)
{
    switch(event->type) {
        case KeyPress:
            (*self->funcs.keybd_func)(self, event);
            break;
        case KeyRelease:
            (*self->funcs.keybd_func)(self, event);
            break;
        case ButtonPress:
            (*self->funcs.mouse_func)(self, event);
            break;
        case ButtonRelease:
            (*self->funcs.mouse_func)(self, event);
            break;
        case MotionNotify:
            (*self->funcs.mouse_func)(self, event);
            break;
        default:
            break;
    }
    return 0UL;
}

unsigned long xcpc_machine_clock_proc(XcpcMachine* self, XEvent* event)
{
    unsigned long timeout    = 0UL;
    unsigned long timedrift  = 0UL;
    unsigned int  skip_frame = 0;

    /* clock the machine */ {
        (void) xcpc_machine_clock(self);
    }
    /* compute the next deadline */ {
        if((self->timer.deadline.tv_usec += self->frame.duration) >= 1000000) {
            self->timer.deadline.tv_usec -= 1000000;
            self->timer.deadline.tv_sec  += 1;
        }
    }
    /* get the current time */ {
        if(gettimeofday(&self->timer.currtime, NULL) != 0) {
            xcpc_log_error("gettimeofday() has failed");
        }
    }
    /* compute the next deadline timeout in us */ {
        const long long currtime = XCPC_TIMESTAMP_OF(&self->timer.currtime);
        const long long deadline = XCPC_TIMESTAMP_OF(&self->timer.deadline);
        if(currtime <= deadline) {
            timeout     = ((unsigned long)(deadline - currtime));
            skip_frame |= 0;
        }
        else {
            timedrift   = ((unsigned long)(currtime - deadline));
            skip_frame |= 1;
        }
    }
    /* force always the first frame and skip other in turbo mode */ {
        if(self->stats.frame_count == 0) {
            skip_frame = 0;
        }
        else if(self->setup.turbo != 0) {
            skip_frame = 1;
        }
    }
    /* draw the frame and compute stats if needed */ {
        if(skip_frame == 0) {
            (*self->funcs.paint_func)(self);
            ++self->stats.frame_drawn;
        }
        if(++self->stats.frame_count == self->frame.rate) {
            compute_stats(self);
        }
    }
    /* check if the time has drifted for more than a second */ {
        if(timedrift >= 1000000UL) {
            timeout = self->frame.duration;
            self->timer.deadline = self->timer.currtime;
            if((self->timer.deadline.tv_usec += self->frame.duration) >= 1000000) {
                self->timer.deadline.tv_usec -= 1000000;
                self->timer.deadline.tv_sec  += 1;
            }
        }
    }
    /* schedule the next frame in ms */ {
        timeout /= 1000UL;
    }
    /* adjust timeout for the first frame */ {
        if((timeout == 0UL) && (self->stats.frame_count == 0)) {
            timeout = 1UL;
        }
    }
    return timeout;
}

XcpcCompanyName xcpc_machine_company_name(XcpcMachine* self)
{
    return self->setup.company_name;
}

XcpcMachineType xcpc_machine_machine_type(XcpcMachine* self)
{
    return self->setup.machine_type;
}

XcpcMonitorType xcpc_machine_monitor_type(XcpcMachine* self)
{
    return self->setup.monitor_type;
}

XcpcRefreshRate xcpc_machine_refresh_rate(XcpcMachine* self)
{
    return self->setup.refresh_rate;
}

XcpcKeyboardType xcpc_machine_keyboard_type(XcpcMachine* self)
{
    return self->setup.keyboard_type;
}

XcpcMemorySize xcpc_machine_memory_size(XcpcMachine* self)
{
    return self->setup.memory_size;
}
