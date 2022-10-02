/*
 * xlib.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <climits>
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#include <stdexcept>
#include "xlib.h"

// ---------------------------------------------------------------------------
// <anonymous>::heap
// ---------------------------------------------------------------------------

namespace {

struct heap
{
    template <typename T>
    static T* alloc(const size_t count)
    {
        return static_cast<T*>(::malloc(count * sizeof(T)));
    }

    template <typename T>
    static T* dealloc(T* data)
    {
        if(data != nullptr) {
            data = (::free(data), nullptr);
        }
        return data;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::x11_error
// ---------------------------------------------------------------------------

namespace {

XErrorHandler x11_error_handler = nullptr;
Bool          x11_error_occured = False;

class x11_error
{
public: // public interface
    x11_error()
    {
        x11_error_occured = False;
        x11_error_handler = XSetErrorHandler(&error_handler);
    }

   ~x11_error()
    {
        x11_error_occured = False;
        x11_error_handler = XSetErrorHandler(x11_error_handler);
    }

    static auto has_error() -> Bool
    {
        return x11_error_occured;
    }

private: // private interface
   static int error_handler(Display* display, XErrorEvent* event)
   {
       if((display != nullptr) && (event != nullptr)) {
           x11_error_occured = True;
       }
       return 0;
   }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::x11_traits
// ---------------------------------------------------------------------------

namespace {

struct x11_traits
{
    static void flush(Display* display)
    {
        static_cast<void>(XFlush(display));
    }

    static void sync(Display* display)
    {
        static_cast<void>(XSync(display, False));
    }

    static XImage* create_image ( Display*     display
                                , Visual*      visual
                                , unsigned int depth
                                , int          format
                                , unsigned int width
                                , unsigned int height )
    {
        XImage* image = nullptr;

        if(image == nullptr) {
            image = XCreateImage ( display
                                 , visual
                                 , depth
                                 , format
                                 , 0
                                 , nullptr
                                 , width
                                 , height
                                 , 32
                                 , 0 );
        }
        if(image != nullptr) {
            image->data = heap::alloc<char>(image->height * image->bytes_per_line);
            if(image->data != nullptr) {
                image->f.destroy_image = &destroy_image;
            }
            else {
                image = (static_cast<void>(XDestroyImage(image)), nullptr);
            }
        }
        return image;
    }

    static int destroy_image(XImage *image)
    {
        if(image != nullptr) {
            image->obdata = heap::dealloc(image->obdata);
            image->data   = heap::dealloc(image->data);
            image         = heap::dealloc(image);
        }
        return 1;
    }

    static XImage* create_shm_image ( Display*     display
                                    , Visual*      visual
                                    , unsigned int depth
                                    , int          format
                                    , unsigned int width
                                    , unsigned int height )
    {
        XImage* image = nullptr;
#ifdef HAVE_XSHM
        if(image == nullptr) {
            image = XShmCreateImage ( display
                                    , visual
                                    , depth
                                    , format
                                    , nullptr
                                    , nullptr
                                    , width
                                    , height );
        }
        if(image != nullptr) {
            XShmSegmentInfo* xshm_segment_info = heap::alloc<XShmSegmentInfo>(1);
            if(xshm_segment_info != nullptr) {
                xshm_segment_info->shmseg   = None;
                xshm_segment_info->shmid    = ::shmget(IPC_PRIVATE, (image->height * image->bytes_per_line), (IPC_CREAT | 0666));
                xshm_segment_info->shmaddr  = reinterpret_cast<char*>(::shmat(xshm_segment_info->shmid, nullptr, 0));
                xshm_segment_info->readOnly = False;
                image->obdata               = reinterpret_cast<char*>(xshm_segment_info);
                image->data                 = reinterpret_cast<char*>(xshm_segment_info->shmaddr);
                image->f.destroy_image      = &destroy_shm_image;
            }
            else {
                image = (static_cast<void>(XDestroyImage(image)), nullptr);
            }
        }
#endif
        return image;
    }

    static int destroy_shm_image(XImage *image)
    {
        if(image != nullptr) {
#ifdef HAVE_XSHM
            if(image->obdata != nullptr) {
                constexpr int bad_shmid   = -1;
                char* const   bad_address = reinterpret_cast<char*>(-1);
                XShmSegmentInfo* xshm_segment_info = reinterpret_cast<XShmSegmentInfo*>(image->obdata);
                image->obdata = nullptr;
                image->data   = nullptr;
                if(xshm_segment_info->shmid != bad_shmid) {
                    const int rc = ::shmctl(xshm_segment_info->shmid, IPC_RMID, nullptr);
                    if(rc != -1) {
                        xshm_segment_info->shmid = bad_shmid;
                    }
                    else {
                        throw std::runtime_error("shmctl() has failed");
                    }
                }
                if(xshm_segment_info->shmaddr != bad_address) {
                    const int rc = ::shmdt(xshm_segment_info->shmaddr);
                    if(rc != -1) {
                        xshm_segment_info->shmaddr = bad_address;
                    }
                    else {
                        throw std::runtime_error("shmdt() has failed");
                    }
                }
                xshm_segment_info = heap::dealloc(xshm_segment_info);
            }
#endif
            image->obdata = heap::dealloc(image->obdata);
            image->data   = heap::dealloc(image->data);
            image         = heap::dealloc(image);
        }
        return 1;
    }

    static Bool query_shm_extension(Display* display)
    {
        Bool status = False;
#ifdef HAVE_XSHM
        if(status == False) {
            status = XShmQueryExtension(display);
        }
        if(status != False) {
            int  major_version  = 0;
            int  minor_version  = 0;
            Bool shared_pixmaps = False;
            status = XShmQueryVersion(display, &major_version, &minor_version, &shared_pixmaps);
        }
#endif
        return status;
    }

    static Bool shm_attach(Display* display, void* shminfo)
    {
        Bool status = False;
#ifdef HAVE_XSHM
        if(shminfo != nullptr) {
            const x11_error scoped_error;
            sync(display);
            status = XShmAttach(display, static_cast<XShmSegmentInfo*>(shminfo));
            sync(display);
        }
#endif
        return status;
    }

    static Bool shm_detach(Display* display, void* shminfo)
    {
        Bool status = False;
#ifdef HAVE_XSHM
        if(shminfo != nullptr) {
            const x11_error scoped_error;
            sync(display);
            status = XShmDetach(display, static_cast<XShmSegmentInfo*>(shminfo));
            sync(display);
        }
#endif
        return status;
    }
};

}

// ---------------------------------------------------------------------------
// XcpcCreateImage
// ---------------------------------------------------------------------------

XImage* XcpcCreateImage ( Display*     display
                        , Visual*      visual
                        , unsigned int depth
                        , int          format
                        , unsigned int width
                        , unsigned int height )
{
    return x11_traits::create_image(display, visual, depth, format, width, height);
}

// ---------------------------------------------------------------------------
// XcpcDestroyImage
// ---------------------------------------------------------------------------

int XcpcDestroyImage(XImage *image)
{
    return x11_traits::destroy_image(image);
}

// ---------------------------------------------------------------------------
// XcpcCreateShmImage
// ---------------------------------------------------------------------------

XImage* XcpcCreateShmImage ( Display*     display
                           , Visual*      visual
                           , unsigned int depth
                           , int          format
                           , unsigned int width
                           , unsigned int height )
{
    return x11_traits::create_shm_image(display, visual, depth, format, width, height);
}

// ---------------------------------------------------------------------------
// XcpcDestroyShmImage
// ---------------------------------------------------------------------------

int XcpcDestroyShmImage(XImage *image)
{
    return x11_traits::destroy_shm_image(image);
}

// ---------------------------------------------------------------------------
// XcpcQueryShmExtension
// ---------------------------------------------------------------------------

Bool XcpcQueryShmExtension(Display* display)
{
    return x11_traits::query_shm_extension(display);
}

// ---------------------------------------------------------------------------
// XcpcAttachShmImage
// ---------------------------------------------------------------------------

Bool XcpcAttachShmImage(Display* display, XImage* image)
{
    return x11_traits::shm_attach(display, image->obdata);
}

// ---------------------------------------------------------------------------
// XcpcDetachShmImage
// ---------------------------------------------------------------------------

Bool XcpcDetachShmImage(Display* display, XImage* image)
{
    return x11_traits::shm_detach(display, image->obdata);
}

// ---------------------------------------------------------------------------
// XcpcPutImage
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
