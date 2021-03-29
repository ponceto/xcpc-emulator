/*
 * monitor.c - Copyright (c) 2001-2021 - Olivier Poncet
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

#define BadPixel ((unsigned long)(~0L))

static void xcpc_monitor_trace(const char* function)
{
    xcpc_log_trace("XcpcMonitor::%s()", function);
}

static XcpcMonitor* xcpc_monitor_init(XcpcMonitor* self, Display* display, Window window, Bool try_xshm)
{
    xcpc_monitor_trace("init");

    /* check parameters */ {
        if((display == NULL) || (window  == None)) {
            return self;
        }
    }
    /* init attributes */ {
        XWindowAttributes attributes;
        Status status = XGetWindowAttributes(display, window, &attributes);
        if(status != 0) {
            self->state.display  = display;
            self->state.screen   = attributes.screen;
            self->state.visual   = attributes.visual;
            self->state.image    = NULL;
            self->state.gc       = DefaultGCOfScreen(attributes.screen);
            self->state.window   = window;
            self->state.colormap = attributes.colormap;
            self->state.depth    = attributes.depth;
            self->state.px       = 0;
            self->state.py       = 0;
            self->state.try_xshm = try_xshm;
            self->state.has_xshm = False;
            self->state.use_xshm = False;
        }
        if(status != 0) {
            if(self->state.try_xshm != False) {
                self->state.has_xshm = XcpcQueryShmExtension(self->state.display);
            }
        }
    }
    return self;
}

static XcpcMonitor* xcpc_monitor_fini(XcpcMonitor* self)
{
    xcpc_monitor_trace("fini");

    /* clear attributes */ {
        self->state.display  = NULL;
        self->state.screen   = NULL;
        self->state.visual   = NULL;
        self->state.image    = NULL;
        self->state.gc       = NULL;
        self->state.window   = None;
        self->state.colormap = None;
        self->state.depth    = 0;
        self->state.px       = 0;
        self->state.py       = 0;
        self->state.try_xshm = False;
        self->state.has_xshm = False;
        self->state.use_xshm = False;
    }
    return self;
}

static XcpcMonitor* xcpc_monitor_init_image(XcpcMonitor* self)
{
    xcpc_monitor_trace("init_image");

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
                                                       , XCPC_MONITOR_WIDTH
                                                       , XCPC_MONITOR_HEIGHT );
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
                                                , XCPC_MONITOR_WIDTH
                                                , XCPC_MONITOR_HEIGHT );
        }
    }
    /* resize window */ {
        if(self->state.image != NULL) {
            (void) XResizeWindow ( self->state.display
                                 , self->state.window
                                 , self->state.image->width
                                 , self->state.image->height );
        }
    }
    return self;
}

static XcpcMonitor* xcpc_monitor_fini_image(XcpcMonitor* self)
{
    xcpc_monitor_trace("fini_image");

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

static XcpcMonitor* xcpc_monitor_init_palette(XcpcMonitor* self, XcpcMonitorModel monitor_model)
{
    xcpc_monitor_trace("init_palette");

    /* check if realized */ {
        if((self->state.display == NULL) || (self->state.colormap == None)) {
            return self;
        }
    }
    /* init palette */ {
        unsigned short color_index = 0;
        unsigned short color_count = countof(self->state.palette);
        for(color_index = 0; color_index < color_count; ++color_index) {
            XColor *color = &self->state.palette[color_index];
            /* clear color */ {
                color->pixel = BadPixel;
                color->red   = 0U;
                color->green = 0U;
                color->blue  = 0U;
                color->flags = DoRed | DoGreen | DoBlue;
                color->pad   = 0;
            }
            /* init color */ {
                (void) xcpc_color_get_values ( monitor_model
                                             , color_index
                                             , &color->red
                                             , &color->green
                                             , &color->blue );
            }
            /* alloc color */ {
                Status allocated = XAllocColor(self->state.display, self->state.colormap, color);
                if(allocated != 0) {
                    xcpc_log_debug ( "%s::init_palette(), color allocated (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                                   , "XcpcMonitor"
                                   , color->pixel
                                   , color->red
                                   , color->green
                                   , color->blue );
                }
                else {
                    xcpc_log_error ( "%s::init_palette(), unable to allocate color (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                                   , "XcpcMonitor"
                                   , color->pixel
                                   , color->red
                                   , color->green
                                   , color->blue );
                }
            }
        }
    }
    return self;
}

static XcpcMonitor* xcpc_monitor_fini_palette(XcpcMonitor* self)
{
    xcpc_monitor_trace("fini_palette");

    /* check if realized */ {
        if((self->state.display == NULL) || (self->state.colormap == None)) {
            return self;
        }
    }
    /* free palette */ {
        unsigned short color_index = 0;
        unsigned short color_count = countof(self->state.palette);
        for(color_index = 0; color_index < color_count; ++color_index) {
            XColor* color = &self->state.palette[color_index];
            /* free color */ {
                Status freed = XFreeColors(self->state.display, self->state.colormap, &color->pixel, 1, 0);
                if(freed != 0) {
                    xcpc_log_debug ( "%s::fini_palette(), color freed (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                                   , "XcpcMonitor"
                                   , color->pixel
                                   , color->red
                                   , color->green
                                   , color->blue );
                }
                else {
                    xcpc_log_error ( "%s::fini_palette(), unable to free color (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                                   , "XcpcMonitor"
                                   , color->pixel
                                   , color->red
                                   , color->green
                                   , color->blue );
                }
            }
            /* clear color */ {
                color->pixel = BadPixel;
                color->red   = 0U;
                color->green = 0U;
                color->blue  = 0U;
                color->flags = DoRed | DoGreen | DoBlue;
                color->pad   = 0;
            }
        }
    }
    return self;
}

XcpcMonitor* xcpc_monitor_alloc(void)
{
    xcpc_monitor_trace("alloc");

    return xcpc_new(XcpcMonitor);
}

XcpcMonitor* xcpc_monitor_free(XcpcMonitor* self)
{
    xcpc_monitor_trace("free");

    return xcpc_delete(XcpcMonitor, self);
}

XcpcMonitor* xcpc_monitor_construct(XcpcMonitor* self)
{
    xcpc_monitor_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcMonitorIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcMonitorState));
    }
    /* initialize iface */ {
        (void) xcpc_monitor_set_iface(self, NULL);
    }
    /* init attributes */ {
        self->state.display  = NULL;
        self->state.screen   = NULL;
        self->state.visual   = NULL;
        self->state.image    = NULL;
        self->state.gc       = NULL;
        self->state.window   = None;
        self->state.colormap = None;
        self->state.depth    = 0;
        self->state.px       = 0;
        self->state.py       = 0;
        self->state.try_xshm = False;
        self->state.has_xshm = False;
        self->state.use_xshm = False;
    }
    /* init palette */ {
        unsigned short color_index = 0;
        unsigned short color_count = countof(self->state.palette);
        for(color_index = 0; color_index < color_count; ++color_index) {
            XColor* color = &self->state.palette[color_index];
            /* init color */ {
                color->pixel = BadPixel;
                color->red   = 0U;
                color->green = 0U;
                color->blue  = 0U;
                color->flags = DoRed | DoGreen | DoBlue;
                color->pad   = 0;
            }
        }
    }
    return xcpc_monitor_reset(self);
}

XcpcMonitor* xcpc_monitor_destruct(XcpcMonitor* self)
{
    xcpc_monitor_trace("destruct");

    return xcpc_monitor_unrealize(self);
}

XcpcMonitor* xcpc_monitor_new(void)
{
    xcpc_monitor_trace("new");

    return xcpc_monitor_construct(xcpc_monitor_alloc());
}

XcpcMonitor* xcpc_monitor_delete(XcpcMonitor* self)
{
    xcpc_monitor_trace("delete");

    return xcpc_monitor_free(xcpc_monitor_destruct(self));
}

XcpcMonitor* xcpc_monitor_set_iface(XcpcMonitor* self, const XcpcMonitorIface* iface)
{
    xcpc_monitor_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = self;
    }
    return self;
}

XcpcMonitor* xcpc_monitor_reset(XcpcMonitor* self)
{
    xcpc_monitor_trace("reset");

    return self;
}

XcpcMonitor* xcpc_monitor_realize(XcpcMonitor* self, XcpcMonitorModel monitor_model, Display* display, Window window, Bool try_xshm)
{
    xcpc_monitor_trace("realize");

    /* unrealize */ {
        (void) xcpc_monitor_unrealize(self);
    }
    /* initialize attributes */ {
        (void) xcpc_monitor_init(self, display, window, try_xshm);
    }
    /* initialize image */ {
        (void) xcpc_monitor_init_image(self);
    }
    /* initialize palette */ {
        (void) xcpc_monitor_init_palette(self, monitor_model);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_unrealize(XcpcMonitor* self)
{
    xcpc_monitor_trace("unrealize");

    /* finalize palette */ {
        (void) xcpc_monitor_fini_palette(self);
    }
    /* finalize image */ {
        (void) xcpc_monitor_fini_image(self);
    }
    /* finalize attributes */ {
        (void) xcpc_monitor_fini(self);
    }
    return self;
}

XcpcMonitor* xcpc_monitor_is_realized(XcpcMonitor* self)
{
    if((self->state.display == NULL)
    || (self->state.screen  == NULL)
    || (self->state.visual  == NULL)
    || (self->state.image   == NULL)) {
        return NULL;
    }
    return self;
}

XcpcMonitor* xcpc_monitor_put_image(XcpcMonitor* self)
{
    /* blit the image */ {
        Display* display = self->state.display;
        Window   window  = self->state.window;
        XImage*  image   = self->state.image;
        GC       gc      = self->state.gc;
        if((display != NULL)
        && (window  != None)
        && (image   != NULL)) {
            (void) XSync(display, False);
            (void) XcpcPutImage ( display
                                , window
                                , gc
                                , image
                                , 0
                                , 0
                                , self->state.px
                                , self->state.py
                                , image->width
                                , image->height
                                , self->state.use_xshm
                                , False );
            (void) XFlush(display);
        }
    }
    return self;
}

XcpcMonitor* xcpc_monitor_resize(XcpcMonitor* self, XEvent* event)
{
    xcpc_monitor_trace("resize");

    /* check event */ {
        if((event == NULL) || (event->type != ConfigureNotify)) {
            return self;
        }
    }
    /* compute px */ {
        if(event->xconfigure.width > self->state.image->width) {
            self->state.px = (event->xconfigure.width - self->state.image->width) / 2;
        }
        else {
            self->state.px = 0;
        }
    }
    /* compute py */ {
        if(event->xconfigure.height > self->state.image->height) {
            self->state.py = (event->xconfigure.height - self->state.image->height) / 2;
        }
        else {
            self->state.py = 0;
        }
    }
    return self;
}

XcpcMonitor* xcpc_monitor_expose(XcpcMonitor* self, XEvent* event)
{
    xcpc_monitor_trace("expose");

    /* check event */ {
        if((event == NULL) || (event->type != Expose)) {
            return self;
        }
    }
    /* expose */ {
        Display* display = self->state.display;
        Window   window  = self->state.window;
        XImage*  image   = self->state.image;
        GC       gc      = self->state.gc;
        XcpcMonitorArea area1;
        XcpcMonitorArea area2;
        /* is realized ? */ {
            if((display == NULL) || (window == None) || (image == NULL)) {
                return self;
            }
        }
        /* init area1 */ {
            area1.x1 = self->state.px;
            area1.y1 = self->state.py;
            area1.x2 = ((area1.x1 + image->width ) - 1);
            area1.y2 = ((area1.y1 + image->height) - 1);
        }
        /* init area2 */ {
            area2.x1 = event->xexpose.x;
            area2.y1 = event->xexpose.y;
            area2.x2 = ((area2.x1 + event->xexpose.width ) - 1);
            area2.y2 = ((area2.y1 + event->xexpose.height) - 1);
        }
        /* intersect areas */ {
            if((area2.x1 > area1.x2)
            || (area2.x2 < area1.x1)
            || (area2.y1 > area1.y2)
            || (area2.y2 < area1.y1)) {
                (void) XClearArea ( display
                                  , window
                                  , area2.x1
                                  , area2.y1
                                  , ((area2.x2 - area2.x1) + 1)
                                  , ((area2.y2 - area2.y1) + 1)
                                  , False );
                (void) XFlush(display);
                return self;
            }
            if(area2.x1 < area1.x1) {
                area2.x1 = area1.x1;
            }
            if(area2.x2 > area1.x2) {
                area2.x2 = area1.x2;
            }
            if(area2.y1 < area1.y1) {
                area2.y1 = area1.y1;
            }
            if(area2.y2 > area1.y2) {
                area2.y2 = area1.y2;
            }
        }
        /* put image */ {
            (void) XSync(display, False);
            (void) XcpcPutImage ( display
                                , window
                                , gc
                                , image
                                , area2.x1 - self->state.px
                                , area2.y1 - self->state.py
                                , area2.x1
                                , area2.y1
                                , ((area2.x2 - area2.x1) + 1)
                                , ((area2.y2 - area2.y1) + 1)
                                , self->state.use_xshm
                                , False );
            (void) XFlush(display);
        }
    }
    return self;
}
