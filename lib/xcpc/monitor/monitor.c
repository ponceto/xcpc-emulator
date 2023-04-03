/*
 * monitor.c - Copyright (c) 2001-2023 - Olivier Poncet
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
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#include "monitor-priv.h"

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcMonitor::%s()", function);
}

static XColor* clear_color(XColor* color)
{
    if(color != NULL) {
        color->pixel = ((unsigned long)(~0L));
        color->red   = 0U;
        color->green = 0U;
        color->blue  = 0U;
        color->flags = DoRed | DoGreen | DoBlue;
        color->pad   = 0;
    }
    return color;
}

static XColor* alloc_color(Display* display, Colormap colormap, XColor* color)
{
    if(color != NULL) {
        Status status = XAllocColor(display, colormap, color);
        if(status != 0) {
            xcpc_log_debug ( "%s::alloc_color(), color allocated (pixel = 0x%08lx, rgb = { 0x%04x, 0x%04x, 0x%04x })"
                           , "XcpcMonitor"
                           , color->pixel
                           , color->red
                           , color->green
                           , color->blue );
        }
        else {
            xcpc_log_error ( "%s::alloc_color(), unable to allocate color (pixel = 0x%08lx, rgb = { 0x%04x, 0x%04x, 0x%04x })"
                           , "XcpcMonitor"
                           , color->pixel
                           , color->red
                           , color->green
                           , color->blue );
        }
    }
    return color;
}

static XColor* free_color(Display* display, Colormap colormap, XColor* color)
{
    if(color != NULL) {
        Status status = XFreeColors(display, colormap, &color->pixel, 1, 0);
        if(status != 0) {
            xcpc_log_debug ( "%s::free_color(), color freed (pixel = 0x%08lx, rgb = { 0x%04x, 0x%04x, 0x%04x })"
                           , "XcpcMonitor"
                           , color->pixel
                           , color->red
                           , color->green
                           , color->blue );
        }
        else {
            xcpc_log_error ( "%s::free_color(), unable to free color (pixel = 0x%08lx, rgb = { 0x%04x, 0x%04x, 0x%04x })"
                           , "XcpcMonitor"
                           , color->pixel
                           , color->red
                           , color->green
                           , color->blue );
        }
    }
    return color;
}

static XcpcMonitor* init_attributes(XcpcMonitor* self, Display* display, Window window, Bool try_xshm)
{
    log_trace("init");

    /* check parameters */ {
        if((display == NULL) || (window  == None)) {
            return self;
        }
    }
    /* init attributes */ {
        XWindowAttributes attributes;
        Status status = XGetWindowAttributes(display, window, &attributes);
        if(status != 0) {
            self->state.display   = display;
            self->state.screen    = attributes.screen;
            self->state.visual    = attributes.visual;
            self->state.image     = NULL;
            self->state.gc        = DefaultGCOfScreen(attributes.screen);
            self->state.window    = window;
            self->state.colormap  = attributes.colormap;
            self->state.depth     = attributes.depth;
            self->state.image_x   = 0;
            self->state.image_y   = 0;
            self->state.total_w   = XCPC_MONITOR_50HZ_TOTAL_WIDTH;
            self->state.total_h   = XCPC_MONITOR_50HZ_TOTAL_HEIGHT;
            self->state.visible_x = XCPC_MONITOR_50HZ_VISIBLE_X;
            self->state.visible_y = XCPC_MONITOR_50HZ_VISIBLE_Y;
            self->state.visible_w = XCPC_MONITOR_50HZ_VISIBLE_WIDTH;
            self->state.visible_h = XCPC_MONITOR_50HZ_VISIBLE_HEIGHT;
            self->state.try_xshm  = try_xshm;
            self->state.has_xshm  = False;
            self->state.use_xshm  = False;
            switch(self->setup.refresh_rate) {
                case XCPC_REFRESH_RATE_50HZ:
                    self->state.total_w   = XCPC_MONITOR_50HZ_TOTAL_WIDTH;
                    self->state.total_h   = XCPC_MONITOR_50HZ_TOTAL_HEIGHT;
                    self->state.visible_x = XCPC_MONITOR_50HZ_VISIBLE_X;
                    self->state.visible_y = XCPC_MONITOR_50HZ_VISIBLE_Y;
                    self->state.visible_w = XCPC_MONITOR_50HZ_VISIBLE_WIDTH;
                    self->state.visible_h = XCPC_MONITOR_50HZ_VISIBLE_HEIGHT;
                    break;
                case XCPC_REFRESH_RATE_60HZ:
                    self->state.total_w   = XCPC_MONITOR_60HZ_TOTAL_WIDTH;
                    self->state.total_h   = XCPC_MONITOR_60HZ_TOTAL_HEIGHT;
                    self->state.visible_x = XCPC_MONITOR_60HZ_VISIBLE_X;
                    self->state.visible_y = XCPC_MONITOR_60HZ_VISIBLE_Y;
                    self->state.visible_w = XCPC_MONITOR_60HZ_VISIBLE_WIDTH;
                    self->state.visible_h = XCPC_MONITOR_60HZ_VISIBLE_HEIGHT;
                    break;
                default:
                    break;
            }
        }
        if(status != 0) {
            if(self->state.try_xshm != False) {
                self->state.has_xshm = XcpcQueryShmExtension(self->state.display);
            }
        }
    }
    return self;
}

static XcpcMonitor* fini_attributes(XcpcMonitor* self)
{
    log_trace("fini");

    /* clear attributes */ {
        self->state.display   = NULL;
        self->state.screen    = NULL;
        self->state.visual    = NULL;
        self->state.image     = NULL;
        self->state.gc        = NULL;
        self->state.window    = None;
        self->state.colormap  = None;
        self->state.depth     = 0;
        self->state.image_x   = 0;
        self->state.image_y   = 0;
        self->state.total_w   = 0;
        self->state.total_h   = 0;
        self->state.visible_x = 0;
        self->state.visible_y = 0;
        self->state.visible_w = 0;
        self->state.visible_h = 0;
        self->state.try_xshm  = False;
        self->state.has_xshm  = False;
        self->state.use_xshm  = False;
    }
    return self;
}

static XcpcMonitor* init_image(XcpcMonitor* self)
{
    log_trace("init_image");

    /* check if realized */ {
        if((self->state.display == NULL) || (self->state.visual == NULL)) {
            return self;
        }
    }
    /* create xshm image */ {
        if(self->state.image == NULL) {
            if(self->state.has_xshm != False) {
                self->state.image = XcpcCreateShmImage ( self->state.display
                                                       , self->state.visual
                                                       , self->state.depth
                                                       , ZPixmap
                                                       , self->state.total_w
                                                       , self->state.total_h );
                if(self->state.image != NULL) {
                    self->state.use_xshm = XcpcAttachShmImage(self->state.display, self->state.image);
                    if(self->state.use_xshm == False) {
                        self->state.image = (((void) XDestroyImage(self->state.image)), NULL);
                    }
                }
            }
        }
    }
    /* create image */ {
        if(self->state.image == NULL) {
            self->state.image = XcpcCreateImage ( self->state.display
                                                , self->state.visual
                                                , self->state.depth
                                                , ZPixmap
                                                , self->state.total_w
                                                , self->state.total_h );
        }
    }
    /* resize window */ {
        if(self->state.image != NULL) {
            (void) XResizeWindow ( self->state.display
                                 , self->state.window
                                 , self->state.visible_w
                                 , self->state.visible_h );
        }
    }
    return self;
}

static XcpcMonitor* fini_image(XcpcMonitor* self)
{
    log_trace("fini_image");

    /* check if realized */ {
        if((self->state.display == NULL) || (self->state.image == NULL)) {
            return self;
        }
    }
    /* detach xshm */ {
        if(self->state.use_xshm != False) {
            self->state.use_xshm = (((void) XcpcDetachShmImage(self->state.display, self->state.image)), False);
        }
    }
    /* destroy image */ {
        self->state.image = (((void) XDestroyImage(self->state.image)), NULL);
    }
    return self;
}

static XcpcMonitor* init_palette(XcpcMonitor* self)
{
    log_trace("init_palette");

    /* check if realized */ {
        if((self->state.display == NULL) || (self->state.colormap == None)) {
            return self;
        }
    }
    /* init palette */ {
        int color_index = 0;
        int color_count = countof(self->state.palette1);
        do {
            XColor* color = &self->state.palette1[color_index];
            /* clear color */ {
                (void) clear_color(color);
            }
            /* get color */ {
                (void) xcpc_color_get_values ( self->setup.monitor_type
                                             , color_index
                                             , &color->red
                                             , &color->green
                                             , &color->blue );
            }
            /* alloc color */ {
                (void) alloc_color(self->state.display, self->state.colormap, color);
            }
        } while(++color_index < color_count);
    }
    /* init palette2 */ {
        int color_index = 0;
        int color_count = countof(self->state.palette2);
        do {
            XColor* color = &self->state.palette2[color_index];
            /* clear color */ {
                (void) clear_color(color);
            }
            /* get color */ {
                (void) xcpc_color_get_values ( self->setup.monitor_type
                                             , color_index
                                             , &color->red
                                             , &color->green
                                             , &color->blue );
            }
            /* adjust rgb */ {
                color->red   = ((((uint32_t) color->red  ) * 5) / 8);
                color->green = ((((uint32_t) color->green) * 5) / 8);
                color->blue  = ((((uint32_t) color->blue ) * 5) / 8);
            }
            /* alloc color */ {
                (void) alloc_color(self->state.display, self->state.colormap, color);
            }
        } while(++color_index < color_count);
    }
    return self;
}

static XcpcMonitor* fini_palette(XcpcMonitor* self)
{
    log_trace("fini_palette");

    /* check if realized */ {
        if((self->state.display == NULL) || (self->state.colormap == None)) {
            return self;
        }
    }
    /* free palette */ {
        int color_index = 0;
        int color_count = countof(self->state.palette1);
        do {
            XColor* color = &self->state.palette1[color_index];
            /* free color */ {
                (void) free_color(self->state.display, self->state.colormap, color);
            }
            /* clear color */ {
                (void) clear_color(color);
            }
        } while(++color_index < color_count);
    }
    /* free palette2 */ {
        int color_index = 0;
        int color_count = countof(self->state.palette2);
        do {
            XColor* color = &self->state.palette2[color_index];
            /* free color */ {
                (void) free_color(self->state.display, self->state.colormap, color);
            }
            /* clear color */ {
                (void) clear_color(color);
            }
        } while(++color_index < color_count);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcMonitor);
}

XcpcMonitor* xcpc_monitor_free(XcpcMonitor* self)
{
    log_trace("free");

    return xcpc_delete(XcpcMonitor, self);
}

XcpcMonitor* xcpc_monitor_construct(XcpcMonitor* self, const XcpcMonitorIface* iface)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcMonitorIface));
        (void) memset(&self->setup, 0, sizeof(XcpcMonitorSetup));
        (void) memset(&self->state, 0, sizeof(XcpcMonitorState));
    }
    /* initialize iface */ {
        if(iface != NULL) {
            *(&self->iface) = *(iface);
        }
        else {
            self->iface.user_data = NULL;
        }
    }
    /* init setup */ {
        self->setup.monitor_type = XCPC_MONITOR_TYPE_DEFAULT;
        self->setup.refresh_rate = XCPC_REFRESH_RATE_DEFAULT;
    }
    /* init attributes */ {
        self->state.display   = NULL;
        self->state.screen    = NULL;
        self->state.visual    = NULL;
        self->state.image     = NULL;
        self->state.gc        = NULL;
        self->state.window    = None;
        self->state.colormap  = None;
        self->state.depth     = 0;
        self->state.image_x   = 0;
        self->state.image_y   = 0;
        self->state.total_w   = 0;
        self->state.total_h   = 0;
        self->state.visible_x = 0;
        self->state.visible_y = 0;
        self->state.visible_w = 0;
        self->state.visible_h = 0;
        self->state.try_xshm  = False;
        self->state.has_xshm  = False;
        self->state.use_xshm  = False;
    }
    /* init palette1 */ {
        int color_index = 0;
        int color_count = countof(self->state.palette1);
        do {
            (void) clear_color(&self->state.palette1[color_index]);
        } while(++color_index < color_count);
    }
    /* init palette2 */ {
        int color_index = 0;
        int color_count = countof(self->state.palette2);
        do {
            (void) clear_color(&self->state.palette2[color_index]);
        } while(++color_index < color_count);
    }
    /* reset */ {
        (void) xcpc_monitor_reset(self);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_destruct(XcpcMonitor* self)
{
    log_trace("destruct");

    /* unrealize */ {
        (void) xcpc_monitor_unrealize(self);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_new(const XcpcMonitorIface* iface)
{
    log_trace("new");

    return xcpc_monitor_construct(xcpc_monitor_alloc(), iface);
}

XcpcMonitor* xcpc_monitor_delete(XcpcMonitor* self)
{
    log_trace("delete");

    return xcpc_monitor_free(xcpc_monitor_destruct(self));
}

XcpcMonitor* xcpc_monitor_reset(XcpcMonitor* self)
{
    log_trace("reset");

    return self;
}

XcpcMonitor* xcpc_monitor_realize(XcpcMonitor* self, XcpcMonitorType monitor_type, XcpcRefreshRate refresh_rate, Display* display, Window window, Bool try_xshm)
{
    log_trace("realize");

    /* unrealize */ {
        (void) xcpc_monitor_unrealize(self);
    }
    /* adjust setup */ {
        self->setup.monitor_type = monitor_type;
        self->setup.refresh_rate = refresh_rate;
    }
    /* initialize attributes */ {
        (void) init_attributes(self, display, window, try_xshm);
    }
    /* initialize image */ {
        (void) init_image(self);
    }
    /* initialize palette */ {
        (void) init_palette(self);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_unrealize(XcpcMonitor* self)
{
    log_trace("unrealize");

    /* finalize palette */ {
        (void) fini_palette(self);
    }
    /* finalize image */ {
        (void) fini_image(self);
    }
    /* finalize attributes */ {
        (void) fini_attributes(self);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_put_image(XcpcMonitor* self)
{
    Display* display = self->state.display;
    Window   window  = self->state.window;
    XImage*  image   = self->state.image;
    GC       gc      = self->state.gc;

    /* blit the image */ {
        if((display != NULL)
        && (window  != None)
        && (image   != NULL)) {
            const int src_x = self->state.visible_x;
            const int src_y = self->state.visible_y;
            const int dst_x = self->state.image_x;
            const int dst_y = self->state.image_y;
            const int dst_w = self->state.visible_w;
            const int dst_h = self->state.visible_h;
            (void) XSync(display, False);
            (void) XcpcPutImage ( display
                                , window
                                , gc
                                , image
                                , src_x
                                , src_y
                                , dst_x
                                , dst_y
                                , dst_w
                                , dst_h
                                , self->state.use_xshm
                                , False );
            (void) XFlush(display);
        }
    }
    return self;
}

XcpcMonitor* xcpc_monitor_expose(XcpcMonitor* self, XExposeEvent* event)
{
    Display* display = self->state.display;
    Window   window  = self->state.window;
    XImage*  image   = self->state.image;
    GC       gc      = self->state.gc;

    log_trace("expose");
    /* is realized ? */ {
        if((display == NULL) || (window == None) || (image == NULL)) {
            return self;
        }
    }
    /* expose */ {
        XcpcMonitorArea monitor;
        XcpcMonitorArea refresh;
        /* init monitor area */ {
            monitor.x1 = self->state.image_x;
            monitor.y1 = self->state.image_y;
            monitor.x2 = ((monitor.x1 + self->state.visible_w) - 1);
            monitor.y2 = ((monitor.y1 + self->state.visible_h) - 1);
        }
        /* init refresh area */ {
            refresh.x1 = event->x;
            refresh.y1 = event->y;
            refresh.x2 = ((refresh.x1 + event->width ) - 1);
            refresh.y2 = ((refresh.y1 + event->height) - 1);
        }
        /* intersect both areas */ {
            if((refresh.x1 > monitor.x2)
            || (refresh.x2 < monitor.x1)
            || (refresh.y1 > monitor.y2)
            || (refresh.y2 < monitor.y1)) {
                return self;
            }
            if(refresh.x1 < monitor.x1) {
                refresh.x1 = monitor.x1;
            }
            if(refresh.x2 > monitor.x2) {
                refresh.x2 = monitor.x2;
            }
            if(refresh.y1 < monitor.y1) {
                refresh.y1 = monitor.y1;
            }
            if(refresh.y2 > monitor.y2) {
                refresh.y2 = monitor.y2;
            }
        }
        /* put image */ {
            const int src_x = self->state.visible_x + (refresh.x1 - self->state.image_x);
            const int src_y = self->state.visible_y + (refresh.y1 - self->state.image_y);
            const int dst_x = refresh.x1;
            const int dst_y = refresh.y1;
            const int dst_w = ((refresh.x2 - refresh.x1) + 1);
            const int dst_h = ((refresh.y2 - refresh.y1) + 1);
            (void) XSync(display, False);
            (void) XcpcPutImage ( display
                                , window
                                , gc
                                , image
                                , src_x
                                , src_y
                                , dst_x
                                , dst_y
                                , dst_w
                                , dst_h
                                , self->state.use_xshm
                                , False );
            (void) XFlush(display);
        }
    }
    return self;
}

XcpcMonitor* xcpc_monitor_resize(XcpcMonitor* self, XConfigureEvent* event)
{
    log_trace("resize");
    /* resize */ {
        /* compute image_x */ {
            const int event_width   = event->width;
            const int visible_width = self->state.visible_w;
            if(event_width >= visible_width) {
                self->state.image_x = +((event_width - visible_width) / 2);
            }
            else {
                self->state.image_x = -((visible_width - event_width) / 2);
            }
        }
        /* compute image_y */ {
            const int event_height   = event->height;
            const int visible_height = self->state.visible_h;
            if(event_height >= visible_height) {
                self->state.image_y = +((event_height - visible_height) / 2);
            }
            else {
                self->state.image_y = -((visible_height - event_height) / 2);
            }
        }
    }
    return self;
}
