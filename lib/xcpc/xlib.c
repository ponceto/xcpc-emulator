/*
 * xlib.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "xlib-priv.h"

#ifdef HAVE_XSHM
static XErrorHandler xshm_error_handler = NULL;
static Bool          xshm_error_occured = False;

static int XShmErrorHandler(Display* display, XErrorEvent* event)
{
    if((display != NULL) && (event != NULL)) {
        xshm_error_occured = True;
    }
    return 0;
}
#endif

static int _XcpcDestroyImage(XImage *image)
{
#ifndef NDEBUG
    if(image != NULL) {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "destroying XImage (shm = %s, depth = %d, width = %d, height = %d)"
              , "no"
              , image->depth
              , image->width
              , image->height );
    }
#endif
    if(image != NULL) {
        if(image->obdata != NULL) {
            image->obdata = Xdelete(char, image->obdata);
        }
        if(image->data != NULL) {
            image->data = Xdelete(char, image->data);
        }
        image = Xdelete(XImage, image);
    }
    return 1;
}

XImage* XcpcCreateImage ( Display*     display
                        , Visual*      visual
                        , unsigned int depth
                        , int          format
                        , unsigned int width
                        , unsigned int height )
{
    XImage* image = XCreateImage ( display
                                 , visual
                                 , depth
                                 , format
                                 , 0
                                 , NULL
                                 , width
                                 , height
                                 , 32
                                 , 0 );
    if(image != NULL) {
        image->data = Xmalloc(image->height * image->bytes_per_line);
        if(image->data != NULL) {
            image->f.destroy_image = &_XcpcDestroyImage;
        }
        else {
            image = ((void) XDestroyImage(image), NULL);
        }
    }
#ifndef NDEBUG
    if(image != NULL) {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "XImage created (shm = %s, depth = %d, width = %d, height = %d)"
              , "no"
              , depth
              , width
              , height );
    }
    else {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "unable to create XImage (shm = %s, depth = %d, width = %d, height = %d)"
              , "no"
              , depth
              , width
              , height );
    }
#endif
    return image;
}

static int _XcpcDestroyShmImage(XImage *image)
{
#ifndef NDEBUG
    if(image != NULL) {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "destroying XImage (shm = %s, depth = %d, width = %d, height = %d)"
              , "yes"
              , image->depth
              , image->width
              , image->height );
    }
#endif
    if(image != NULL) {
#ifdef HAVE_XSHM
        if(image->obdata != NULL) {
            XShmSegmentInfo* xshm_segment_info = XSHM_SEGMENT_INFO(image->obdata);
            image->obdata = NULL;
            image->data   = NULL;
            if(xshm_segment_info->shmid != -1) {
                (void) shmctl(xshm_segment_info->shmid, IPC_RMID, NULL);
                xshm_segment_info->shmid = -1;
            }
            if(xshm_segment_info->shmaddr != ((char*)(-1))) {
                (void) shmdt(xshm_segment_info->shmaddr);
                xshm_segment_info->shmaddr = ((char*)(-1));
            }
            xshm_segment_info = Xdelete(XShmSegmentInfo, xshm_segment_info);
        }
#endif
        if(image->obdata != NULL) {
            image->obdata = Xdelete(char, image->obdata);
        }
        if(image->data != NULL) {
            image->data = Xdelete(char, image->data);
        }
        image = Xdelete(XImage, image);
    }
    return 1;
}

XImage* XcpcCreateShmImage ( Display*     display
                           , Visual*      visual
                           , unsigned int depth
                           , int          format
                           , unsigned int width
                           , unsigned int height )
{
#ifndef HAVE_XSHM
    XImage* image = NULL;
#else
    XImage* image = XShmCreateImage ( display
                                    , visual
                                    , depth
                                    , format
                                    , NULL
                                    , NULL
                                    , width
                                    , height );
    if(image != NULL) {
        XShmSegmentInfo* xshm_segment_info = Xnew(XShmSegmentInfo);
        if(xshm_segment_info != NULL) {
            xshm_segment_info->shmseg   = None;
            xshm_segment_info->shmid    = shmget(IPC_PRIVATE, (image->height * image->bytes_per_line), (IPC_CREAT | 0666));
            xshm_segment_info->shmaddr  = shmat(xshm_segment_info->shmid, NULL, 0);
            xshm_segment_info->readOnly = False;
            image->obdata = ((char*)(xshm_segment_info));
            image->data   = (xshm_segment_info->shmaddr);
            image->f.destroy_image = &_XcpcDestroyShmImage;
        }
        else {
            image = ((void) XDestroyImage(image), NULL);
        }
    }
#endif
#ifndef NDEBUG
    if(image != NULL) {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "XImage created (shm = %s, depth = %d, width = %d, height = %d)"
              , "yes"
              , depth
              , width
              , height );
    }
    else {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "unable to create XImage (shm = %s, depth = %d, width = %d, height = %d)"
              , "yes"
              , depth
              , width
              , height );
    }
#endif
    return image;
}

Bool XcpcAttachShmImage(Display* display, XImage* image)
{
    Bool xshm_attached = False;

#ifdef HAVE_XSHM
    if(image->obdata == NULL) {
        return False;
    }
    else {
        /* synchronize the display queue */ {
            (void) XSync(display, False);
        }
        /* install X11/XShm error handler */ {
            xshm_error_occured = False;
            xshm_error_handler = &XShmErrorHandler;
            xshm_error_handler = XSetErrorHandler(xshm_error_handler);
        }
        /* attach the XShm segment */ {
            (void) XShmAttach(display, XSHM_SEGMENT_INFO(image->obdata));
            (void) XSync(display, False);
        }
        /* check for error */ {
            if(xshm_error_occured == False) {
                xshm_attached = True;
            }
        }
        /* restore X11/XShm error handler */ {
            xshm_error_occured = False;
            xshm_error_handler = XSetErrorHandler(xshm_error_handler);
        }
    }
#endif
#ifndef NDEBUG
    if(xshm_attached != False) {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "XShm segment attached (shmseg = %lu, shmid = %d, shmaddr = %p, readOnly = %d)"
              , XSHM_SEGMENT_INFO(image->obdata)->shmseg
              , XSHM_SEGMENT_INFO(image->obdata)->shmid
              , XSHM_SEGMENT_INFO(image->obdata)->shmaddr
              , XSHM_SEGMENT_INFO(image->obdata)->readOnly );
    }
    else {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "unable to attach XShm segment (shmseg = %lu, shmid = %d, shmaddr = %p, readOnly = %d)"
              , XSHM_SEGMENT_INFO(image->obdata)->shmseg
              , XSHM_SEGMENT_INFO(image->obdata)->shmid
              , XSHM_SEGMENT_INFO(image->obdata)->shmaddr
              , XSHM_SEGMENT_INFO(image->obdata)->readOnly );
    }
#endif
    return xshm_attached;
}

Bool XcpcDetachShmImage(Display* display, XImage* image)
{
    Bool xshm_detached = False;

#ifdef HAVE_XSHM
    if(image->obdata == NULL) {
        return False;
    }
    else {
        /* synchronize the display queue */ {
            (void) XSync(display, False);
        }
        /* install X11/XShm error handler */ {
            xshm_error_occured = False;
            xshm_error_handler = &XShmErrorHandler;
            xshm_error_handler = XSetErrorHandler(xshm_error_handler);
        }
        /* detach the XShm segment */ {
            (void) XShmDetach(display, XSHM_SEGMENT_INFO(image->obdata));
            (void) XSync(display, False);
        }
        /* check for error */ {
            if(xshm_error_occured == False) {
                xshm_detached = True;
            }
        }
        /* restore X11/XShm error handler */ {
            xshm_error_occured = False;
            xshm_error_handler = XSetErrorHandler(xshm_error_handler);
        }
    }
#endif
#ifndef NDEBUG
    if(xshm_detached != False) {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "XShm segment detached (shmseg = %lu, shmid = %d, shmaddr = %p, readOnly = %d)"
              , XSHM_SEGMENT_INFO(image->obdata)->shmseg
              , XSHM_SEGMENT_INFO(image->obdata)->shmid
              , XSHM_SEGMENT_INFO(image->obdata)->shmaddr
              , XSHM_SEGMENT_INFO(image->obdata)->readOnly );
    }
    else {
        g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
              , "unable to detach XShm segment (shmseg = %lu, shmid = %d, shmaddr = %p, readOnly = %d)"
              , XSHM_SEGMENT_INFO(image->obdata)->shmseg
              , XSHM_SEGMENT_INFO(image->obdata)->shmid
              , XSHM_SEGMENT_INFO(image->obdata)->shmaddr
              , XSHM_SEGMENT_INFO(image->obdata)->readOnly );
    }
#endif
    return xshm_detached;
}

Bool XcpcQueryShmExtension(Display* display)
{
    Bool has_xshm = False;

#ifdef HAVE_XSHM
    if(has_xshm == False) {
        has_xshm = XShmQueryExtension(display);
    }
    if(has_xshm != False) {
        int  major_version  = 0;
        int  minor_version  = 0;
        Bool shared_pixmaps = False;
        has_xshm = XShmQueryVersion(display, &major_version, &minor_version, &shared_pixmaps);
#ifndef NDEBUG
        if(has_xshm != False) {
            g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
                    , "the XShm extension is available (version %d.%d, shared pixmaps = %s)"
                    , major_version
                    , minor_version
                    , (shared_pixmaps != False ? "yes" : "no") );
        }
#endif
    }
#endif
    return has_xshm;
}

int XcpcPutImage ( Display*     display
                 , Drawable     drawable
                 , GC           gc
                 , XImage*      image
                 , int          src_x
                 , int          src_y
                 , int          dst_x
                 , int          dst_y
                 , unsigned int width
                 , unsigned int height
                 , Bool         xshm_image
                 , Bool         send_event )
{
#ifdef HAVE_XSHM
    if(xshm_image != False) {
        return XShmPutImage ( display
                            , drawable
                            , gc
                            , image
                            , src_x
                            , src_y
                            , dst_x
                            , dst_y
                            , width
                            , height
                            , send_event );
    }
#endif
    return XPutImage ( display
                     , drawable
                     , gc
                     , image
                     , src_x
                     , src_y
                     , dst_x
                     , dst_y
                     , width
                     , height );
}
