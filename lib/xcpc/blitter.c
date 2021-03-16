/*
 * blitter.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "blitter-priv.h"

#define BadPixel ((unsigned long)(~0L))

static XcpcBlitter* xcpc_blitter_init(XcpcBlitter* self, Display* display, Window window, Bool try_xshm)
{
    if((display == NULL) || (window  == None)) {
        return self;
    }
    /* init attributes */ {
        XWindowAttributes attributes;
        Status status = XGetWindowAttributes(display, window, &attributes);
        if(status != 0) {
            self->display  = display;
            self->screen   = attributes.screen;
            self->visual   = attributes.visual;
            self->image    = NULL;
            self->gc       = DefaultGCOfScreen(attributes.screen);
            self->window   = window;
            self->colormap = attributes.colormap;
            self->depth    = attributes.depth;
            self->px       = 0;
            self->py       = 0;
            self->try_xshm = try_xshm;
            self->has_xshm = False;
            self->use_xshm = False;
        }
        if(status != 0) {
            if(self->try_xshm != False) {
                self->has_xshm = XcpcQueryShmExtension(self->display);
            }
        }
    }
    return self;
}

static XcpcBlitter* xcpc_blitter_fini(XcpcBlitter* self)
{
    /* clear attributes */ {
        self->display  = NULL;
        self->screen   = NULL;
        self->visual   = NULL;
        self->image    = NULL;
        self->gc       = NULL;
        self->window   = None;
        self->colormap = None;
        self->depth    = 0;
        self->px       = 0;
        self->py       = 0;
        self->try_xshm = False;
        self->has_xshm = False;
        self->use_xshm = False;
    }
    return self;
}

static XcpcBlitter* xcpc_blitter_init_image(XcpcBlitter* self)
{
    if((self->display == NULL) || (self->visual  == NULL)) {
        return self;
    }
    /* create xshm image */ {
        if(self->image == NULL) {
            if(self->has_xshm != False) {
                self->image = XcpcCreateShmImage ( self->display
                                                 , self->visual
                                                 , self->depth
                                                 , ZPixmap
                                                 , XCPC_BLITTER_WIDTH
                                                 , XCPC_BLITTER_HEIGHT );
                if(self->image != NULL) {
                    self->use_xshm = XcpcAttachShmImage(self->display, self->image);
                    if(self->use_xshm == False) {
                        self->image = (((void) XDestroyImage(self->image)), NULL);
                    }
                }
            }
        }
    }
    /* create image */ {
        if(self->image == NULL) {
            self->image = XcpcCreateImage ( self->display
                                          , self->visual
                                          , self->depth
                                          , ZPixmap
                                          , XCPC_BLITTER_WIDTH
                                          , XCPC_BLITTER_HEIGHT );
        }
    }
    /* resize window */ {
        if(self->image != NULL) {
            (void) XResizeWindow ( self->display
                                 , self->window
                                 , self->image->width
                                 , self->image->height );
        }
    }
    return self;
}

static XcpcBlitter* xcpc_blitter_fini_image(XcpcBlitter* self)
{
    if((self->display == NULL) || (self->image == NULL)) {
        return self;
    }
    /* detach xshm */ {
        if(self->use_xshm != False) {
            self->use_xshm = (((void) XcpcDetachShmImage(self->display, self->image)), False);
        }
    }
    /* destroy image */ {
        self->image = (((void) XDestroyImage(self->image)), NULL);
    }
    return self;
}

static XcpcBlitter* xcpc_blitter_init_palette(XcpcBlitter* self, XcpcMonitorModel monitor_model)
{
    if((self->display == NULL) || (self->colormap == None)) {
        return self;
    }
    /* init palette */ {
        unsigned short color_index = 0;
        unsigned short color_count = countof(self->palette);
        for(color_index = 0; color_index < color_count; ++color_index) {
            XColor *color = &self->palette[color_index];
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
                Status allocated = XAllocColor(self->display, self->colormap, color);
                if(allocated != 0) {
                    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
                          , "%s::init_palette(), color allocated (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                          , "XcpcBlitter"
                          , color->pixel
                          , color->red
                          , color->green
                          , color->blue );
                }
                else {
                    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_WARNING
                          , "%s::init_palette(), unable to allocate color (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                          , "XcpcBlitter"
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

static XcpcBlitter* xcpc_blitter_fini_palette(XcpcBlitter* self)
{
    if((self->display == NULL) || (self->colormap == None)) {
        return self;
    }
    /* free palette */ {
        unsigned short color_index = 0;
        unsigned short color_count = countof(self->palette);
        for(color_index = 0; color_index < color_count; ++color_index) {
            XColor* color = &self->palette[color_index];
            /* free color */ {
                Status freed = XFreeColors(self->display, self->colormap, &color->pixel, 1, 0);
                if(freed != 0) {
                    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
                          , "%s::fini_palette(), color freed (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                          , "XcpcBlitter"
                          , color->pixel
                          , color->red
                          , color->green
                          , color->blue );
                }
                else {
                    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_WARNING
                          , "%s::fini_palette(), unable to free color (pixel = 0x%08lx, rgb = [0x%04x, 0x%04x, 0x%04x])"
                          , "XcpcBlitter"
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

XcpcBlitter* xcpc_blitter_alloc(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::alloc()"
              , "XcpcBlitter" );
    }
    return xcpc_new(XcpcBlitter);
}

XcpcBlitter* xcpc_blitter_free(XcpcBlitter* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::free()"
              , "XcpcBlitter" );
    }
    return xcpc_delete(XcpcBlitter, self);
}

XcpcBlitter* xcpc_blitter_construct(XcpcBlitter* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::construct()"
              , "XcpcBlitter" );
    }
    if(self != NULL) {
        /* init attributes */ {
            self->display  = NULL;
            self->screen   = NULL;
            self->visual   = NULL;
            self->image    = NULL;
            self->gc       = NULL;
            self->window   = None;
            self->colormap = None;
            self->depth    = 0;
            self->px       = 0;
            self->py       = 0;
            self->try_xshm = False;
            self->has_xshm = False;
            self->use_xshm = False;
        }
        /* init palette */ {
            unsigned short color_index = 0;
            unsigned short color_count = countof(self->palette);
            for(color_index = 0; color_index < color_count; ++color_index) {
                XColor* color = &self->palette[color_index];
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
    }
    return xcpc_blitter_reset(self);
}

XcpcBlitter* xcpc_blitter_destruct(XcpcBlitter* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::destruct()"
              , "XcpcBlitter" );
    }
    return xcpc_blitter_unrealize(self);
}

XcpcBlitter* xcpc_blitter_new(void)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::new()"
              , "XcpcBlitter" );
    }
    return xcpc_blitter_construct(xcpc_blitter_alloc());
}

XcpcBlitter* xcpc_blitter_delete(XcpcBlitter* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::delete()"
              , "XcpcBlitter" );
    }
    return xcpc_blitter_free(xcpc_blitter_destruct(self));
}

XcpcBlitter* xcpc_blitter_reset(XcpcBlitter* self)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::reset()"
              , "XcpcBlitter" );
    }
    return self;
}

XcpcBlitter* xcpc_blitter_realize(XcpcBlitter* self, XcpcMonitorModel monitor_model, Display* display, Window window, Bool try_xshm)
{
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::realize()"
              , "XcpcBlitter" );
    }
    if(self != NULL) {
        /* unrealize */ {
            (void) xcpc_blitter_unrealize(self);
        }
        /* initialize attributes */ {
            (void) xcpc_blitter_init(self, display, window, try_xshm);
        }
        /* initialize image */ {
            (void) xcpc_blitter_init_image(self);
        }
        /* initialize palette */ {
            (void) xcpc_blitter_init_palette(self, monitor_model);
        }
    }
    return self;
}

XcpcBlitter* xcpc_blitter_unrealize(XcpcBlitter* self)
{
    /* unrealize */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::unrealize()"
              , "XcpcBlitter" );
    }
    if(self != NULL) {
        /* finalize palette */ {
            (void) xcpc_blitter_fini_palette(self);
        }
        /* finalize image */ {
            (void) xcpc_blitter_fini_image(self);
        }
        /* finalize attributes */ {
            (void) xcpc_blitter_fini(self);
        }
    }
    return self;
}

XcpcBlitter* xcpc_blitter_is_realized(XcpcBlitter* self)
{
    if(self == NULL) {
        return NULL;
    }
    if((self->display == NULL)
    || (self->screen  == NULL)
    || (self->visual  == NULL)
    || (self->image   == NULL)) {
        return NULL;
    }
    return self;
}

XcpcBlitter* xcpc_blitter_put_image(XcpcBlitter* self)
{
    if(self == NULL) {
        return NULL;
    }
    /* blit the image */ {
        Display* display = self->display;
        Window   window  = self->window;
        XImage*  image   = self->image;
        GC       gc      = self->gc;
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
                                , self->px
                                , self->py
                                , image->width
                                , image->height
                                , self->use_xshm
                                , False );
            (void) XFlush(display);
        }
    }
    return self;
}

XcpcBlitter* xcpc_blitter_resize(XcpcBlitter* self, XEvent* event)
{
    if((self == NULL) || (event == NULL) || (event->type != ConfigureNotify)) {
        return self;
    }
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::resize() (position = [%d, %d], dimension = [%d, %d])"
              , "XcpcBlitter"
              , event->xconfigure.x
              , event->xconfigure.y
              , event->xconfigure.width
              , event->xconfigure.height );
    }
    /* compute px */ {
        if(event->xconfigure.width > self->image->width) {
            self->px = (event->xconfigure.width - self->image->width) / 2;
        }
        else {
            self->px = 0;
        }
    }
    /* compute py */ {
        if(event->xconfigure.height > self->image->height) {
            self->py = (event->xconfigure.height - self->image->height) / 2;
        }
        else {
            self->py = 0;
        }
    }
    return self;
}

XcpcBlitter* xcpc_blitter_expose(XcpcBlitter* self, XEvent* event)
{
    if((self == NULL) || (event == NULL) || (event->type != Expose)) {
        return self;
    }
    /* debug */ {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "%s::expose() (position = [%d, %d], dimension = [%d, %d])"
              , "XcpcBlitter"
              , event->xexpose.x
              , event->xexpose.y
              , event->xexpose.width
              , event->xexpose.height );
    }
    /* expose */ {
        Display* display = self->display;
        Window   window  = self->window;
        XImage*  image   = self->image;
        GC       gc      = self->gc;
        XcpcBlitterArea area1;
        XcpcBlitterArea area2;
        /* is realized ? */ {
            if((display == NULL) || (window == None) || (image == NULL)) {
                return self;
            }
        }
        /* init area1 */ {
            area1.x1 = self->px;
            area1.y1 = self->py;
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
                                , area2.x1 - self->px
                                , area2.y1 - self->py
                                , area2.x1
                                , area2.y1
                                , ((area2.x2 - area2.x1) + 1)
                                , ((area2.y2 - area2.y1) + 1)
                                , self->use_xshm
                                , False );
            (void) XFlush(display);
        }
    }
    return self;
}
