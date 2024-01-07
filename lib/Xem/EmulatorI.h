/*
 * EmulatorI.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XemEmulatorI_h__
#define __XemEmulatorI_h__

#include <Xem/Emulator.h>
#include <xcpc/glue/backend.h>
#include <xcpc/glue/frontend.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XEM_EMULATOR_DATA(data) ((XemEmulatorData)(data))
#define XEM_EMULATOR_FUNC(func) ((XemEmulatorFunc)(func))

typedef XtPointer XemEmulatorData;

typedef unsigned long (*XemEmulatorFunc)(XtPointer data, XEvent* event, void* extra);

typedef struct _XemVideo    XemVideo;
typedef struct _XemAudio    XemAudio;
typedef struct _XemEvents   XemEvents;
typedef struct _XemKeyboard XemKeyboard;
typedef struct _XemJoystick XemJoystick;
typedef struct _XcpcBackend XemBackend;

struct _XemVideo
{
    Display* display;
    Window   window;
};

struct _XemAudio
{
#ifdef HAVE_PORTAUDIO
    PaStream* stream;
#else
    void*     stream;
#endif
};

struct _XemEvents
{
    XEvent       last_rcv_event;
    XEvent       last_key_event;
    XEvent       list[256];
    unsigned int head;
    unsigned int tail;
};

struct _XemKeyboard
{
    Boolean js_enabled;
    int     js_id;
    int     js_axis_x;
    int     js_axis_y;
    int     js_button0;
    int     js_button1;
};

struct _XemJoystick
{
    String         device;
    String         identifier;
    int            fd;
    XtInputId      input_id;
    int            js_id;
    int            js_axis_x;
    int            js_axis_y;
    int            js_button0;
    int            js_button1;
    int            js_buttons;
    unsigned short js_mapping[1024];
};

extern XemVideo*    XemVideoConstruct          (Widget widget, XemVideo* video);
extern XemVideo*    XemVideoDestruct           (Widget widget, XemVideo* video);
extern XemVideo*    XemVideoRealize            (Widget widget, XemVideo* video);
extern XemVideo*    XemVideoUnrealize          (Widget widget, XemVideo* video);

extern XemAudio*    XemAudioConstruct          (Widget widget, XemAudio* audio);
extern XemAudio*    XemAudioDestruct           (Widget widget, XemAudio* audio);
extern XemAudio*    XemAudioRealize            (Widget widget, XemAudio* audio);
extern XemAudio*    XemAudioUnrealize          (Widget widget, XemAudio* audio);

extern XemEvents*   XemEventsConstruct         (Widget widget, XemEvents* events);
extern XemEvents*   XemEventsDestruct          (Widget widget, XemEvents* events);
extern XemEvents*   XemEventsDispatch          (Widget widget, XemEvents* events, XEvent* event);
extern XemEvents*   XemEventsThrottle          (Widget widget, XemEvents* events, XEvent* event);
extern XemEvents*   XemEventsProcess           (Widget widget, XemEvents* events);
extern XEvent*      XemEventsCopyOrFill        (Widget widget, XemEvents* events, XEvent* event);

extern XemKeyboard* XemKeyboardConstruct       (Widget widget, XemKeyboard* keyboard, int id);
extern XemKeyboard* XemKeyboardDestruct        (Widget widget, XemKeyboard* keyboard);
static Boolean      XemKeyboardPreprocessEvent (Widget widget, XemKeyboard* keyboard, XEvent* event);

extern XemJoystick* XemJoystickConstruct       (Widget widget, XemJoystick* joystick, const char* device, int id);
extern XemJoystick* XemJoystickDestruct        (Widget widget, XemJoystick* joystick);
extern XemJoystick* XemJoystickLookupByFd      (Widget widget, int fd);
extern void         XemJoystickHandler         (Widget widget, int* source, XtInputId* input_id);
extern XemJoystick* XemJoystickDump            (Widget widget, XemJoystick* joystick, unsigned char button);

extern XemBackend*  XemBackendConstruct        (Widget widget, XemBackend* backend);
extern XemBackend*  XemBackendDestruct         (Widget widget, XemBackend* backend);

#ifdef __cplusplus
}
#endif

#endif /* __XemEmulatorI_h__ */
