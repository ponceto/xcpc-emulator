/*
 * xcpc-intrinsic.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "xcpc-intrinsic-priv.h"

#ifndef _
#define _(string) (string)
#endif

/*
 * ---------------------------------------------------------------------------
 * options
 * ---------------------------------------------------------------------------
 */

static XrmOptionDescRec options[] = {
    { "-quiet", ".xcpcQuietFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-trace", ".xcpcTraceFlag", XrmoptionNoArg, (XPointer) "true" },
    { "-debug", ".xcpcDebugFlag", XrmoptionNoArg, (XPointer) "true" },
};

/*
 * ---------------------------------------------------------------------------
 * fallback resources
 * ---------------------------------------------------------------------------
 */

static String fallback_resources[] = {
    "Xcpc*title: Xcpc - Amstrad CPC emulator",
    NULL
};

/*
 * ---------------------------------------------------------------------------
 * application resources
 * ---------------------------------------------------------------------------
 */

static XtResource application_resources[] = {
    /* xcpcQuietFlag */ {
        "xcpcQuietFlag", "XcpcQuietFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, quiet_flag),
        XtRImmediate, (XtPointer) False
    },
    /* xcpcTraceFlag */ {
        "xcpcTraceFlag", "XcpcTraceFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, trace_flag),
        XtRImmediate, (XtPointer) False
    },
    /* xcpcDebugFlag */ {
        "xcpcDebugFlag", "XcpcDebugFlag", XtRBoolean,
        sizeof(Boolean), XtOffsetOf(XcpcResourcesRec, debug_flag),
        XtRImmediate, (XtPointer) False
    },
};

/*
 * ---------------------------------------------------------------------------
 * Toolkit utilities
 * ---------------------------------------------------------------------------
 */

#if 0
static void ShowWidget(Widget widget)
{
    if(widget != NULL) {
        XtManageChild(widget);
    }
}
#endif

#if 0
static void HideWidget(Widget widget)
{
    if(widget != NULL) {
        XtUnmanageChild(widget);
    }
}
#endif

#if 1
static Widget FindShell(Widget widget)
{
    while((widget != NULL) && (XtIsShell(widget) == False)) {
        widget = XtParent(widget);
    }
    return widget;
}
#endif

#if 0
static Widget FindTopLevelShell(Widget widget)
{
    while((widget != NULL) && (XtIsTopLevelShell(widget) == False)) {
        widget = XtParent(widget);
    }
    return widget;
}
#endif

/*
 * ---------------------------------------------------------------------------
 * Controls
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* SetMachine(XcpcApplication* self)
{
#if 0
    Arg      arglist[4];
    Cardinal argcount = 0;
    char     buffer[256];

    /* build string */ {
        (void) snprintf ( buffer, sizeof(buffer)
                        , "%s %s %s, %s @ %s, %s"
                        , xcpc_company_name_to_string(xcpc_machine_company_name(self->machine))
                        , xcpc_machine_type_to_string(xcpc_machine_machine_type(self->machine))
                        , xcpc_memory_size_to_string(xcpc_machine_memory_size(self->machine))
                        , xcpc_monitor_type_to_string(xcpc_machine_monitor_type(self->machine))
                        , xcpc_refresh_rate_to_string(xcpc_machine_refresh_rate(self->machine))
                        , xcpc_keyboard_type_to_string(xcpc_machine_keyboard_type(self->machine)) );
    }
    if(self->infobar.system != NULL) {
        XtUnmanageChild(self->infobar.system);
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.system, arglist, argcount);
        XtManageChild(self->infobar.system);
    }
#endif
    return self;
}

static XcpcApplication* SetTitle(XcpcApplication* self, const char* string)
{
    Arg      arglist[4];
    Cardinal argcount = 0;
    char     buffer[256];

    /* inititialize buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "Xcpc - Amstrad CPC emulator - %s", string);
    }
    if(self->layout.toplevel != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNtitle, buffer); ++argcount;
        XtSetValues(self->layout.toplevel, arglist, argcount);
    }
    return self;
}

static XcpcApplication* SetStatus(XcpcApplication* self, const char* string)
{
#if 0
    Arg      arglist[4];
    Cardinal argcount = 0;
    char     buffer[256];

    /* inititialize buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "%s", string);
    }
    if(self->infobar.status != NULL) {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.status, arglist, argcount);
    }
#endif
    return SetTitle(SetMachine(self), string);
}

static XcpcApplication* SetDrive0(XcpcApplication* self)
{
#if 0
    Arg      arglist[4];
    Cardinal argcount = 0;
    const char* filename = NULL;
    char        buffer[256];

    /* fetch filename */ {
        filename = xcpc_machine_filename_drive0(self->machine);
        if((filename != NULL) && (*filename != '\0')) {
            const char* slash = strrchr(filename, '/');
            if(slash != NULL) {
                filename = (slash + 1);
            }
        }
        else {
            filename = _("{empty}");
        }
    }
    /* init buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "%s %s", _("A:"), filename);
    }
    if(self->infobar.drive0 != NULL) {
        XtUnmanageChild(self->infobar.drive0);
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.drive0, arglist, argcount);
        XtManageChild(self->infobar.drive0);
    }
#endif
    return self;
}

static XcpcApplication* SetDrive1(XcpcApplication* self)
{
#if 0
    Arg      arglist[4];
    Cardinal argcount = 0;
    const char* filename = NULL;
    char        buffer[256];

    /* fetch filename */ {
        filename = xcpc_machine_filename_drive1(self->machine);
        if((filename != NULL) && (*filename != '\0')) {
            const char* slash = strrchr(filename, '/');
            if(slash != NULL) {
                filename = (slash + 1);
            }
        }
        else {
            filename = _("{empty}");
        }
    }
    /* init buffer */ {
        (void) snprintf(buffer, sizeof(buffer), "%s %s", _("B:"), filename);
    }
    if(self->infobar.drive1 != NULL) {
        XtUnmanageChild(self->infobar.drive1);
        argcount = 0;
        XtSetArg(arglist[argcount], XtNlabel, buffer); ++argcount;
        XtSetValues(self->infobar.drive1, arglist, argcount);
        XtManageChild(self->infobar.drive1);
    }
#endif
    return self;
}

#if 0
static XcpcApplication* Exit(XcpcApplication* self)
{
    if(self->appcontext != NULL) {
        XtAppSetExitFlag(self->appcontext);
    }
    return self;
}
#endif

static XcpcApplication* Play(XcpcApplication* self)
{
#if 0
    /* show/hide controls */ {
        HideWidget(self->menubar.ctrl.play_emulator);
        ShowWidget(self->menubar.ctrl.pause_emulator);
        HideWidget(self->toolbar.play_emulator);
        ShowWidget(self->toolbar.pause_emulator);
    }
#endif
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, True);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    /* set status */ {
        (void) SetStatus(self, _("Playing"));
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

static XcpcApplication* Pause(XcpcApplication* self)
{
#if 0
    /* show/hide controls */ {
        ShowWidget(self->menubar.ctrl.play_emulator);
        HideWidget(self->menubar.ctrl.pause_emulator);
        ShowWidget(self->toolbar.play_emulator);
        HideWidget(self->toolbar.pause_emulator);
    }
#endif
    if(self->layout.emulator != NULL) {
        XtSetSensitive(self->layout.emulator, False);
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    /* set status */ {
        (void) SetStatus(self, _("Paused"));
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

static XcpcApplication* Reset(XcpcApplication* self)
{
    if(self->layout.emulator != NULL) {
        (void) xcpc_machine_reset(self->machine);
    }
    /* set status */ {
        (void) SetStatus(self, _("Reset"));
        (void) SetDrive0(self);
        (void) SetDrive1(self);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Snapshot
 * ---------------------------------------------------------------------------
 */

#if 1
static XcpcApplication* LoadSnapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_load_snapshot(self->machine, filename);
    }
    return self;
}
#endif

#if 0
static XcpcApplication* SaveSnapshot(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_save_snapshot(self->machine, filename);
    }
    return self;
}
#endif

/*
 * ---------------------------------------------------------------------------
 * Drive0
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* InsertDiskIntoDrive0(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive0(self->machine, filename);
    }
    return SetDrive0(self);
}

static XcpcApplication* RemoveDiskFromDrive0(XcpcApplication* self)
{
    xcpc_machine_remove_drive0(self->machine);

    return SetDrive0(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drive1
 * ---------------------------------------------------------------------------
 */

#if 0
static XcpcApplication* InsertDiskIntoDrive1(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        xcpc_machine_insert_drive1(self->machine, filename);
    }
    return SetDrive1(self);
}
#endif

#if 0
static XcpcApplication* RemoveDiskFromDrive1(XcpcApplication* self)
{
    xcpc_machine_remove_drive1(self->machine);

    return SetDrive1(self);
}
#endif

/*
 * ---------------------------------------------------------------------------
 * Drive by default
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* InsertOrRemoveDisk(XcpcApplication* self, const char* filename)
{
    if((filename != NULL) && (*filename != '\0')) {
        (void) InsertDiskIntoDrive0(self, filename);
    }
    else {
        (void) RemoveDiskFromDrive0(self);
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * Generic callbacks
 * ---------------------------------------------------------------------------
 */

static void DestroyCallback(Widget widget, Widget* reference, XtPointer pointer)
{
    if(XtIsApplicationShell(widget) != False) {
        XtAppSetExitFlag(XtWidgetToApplicationContext(widget));
    }
    if((widget != NULL) && (reference != NULL) && (widget == *reference)) {
        *reference = NULL;
    }
}

#if 0
static void DismissCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    Widget shell = FindShell(widget);

    /* popdown shell */ {
        XtPopdown(shell);
    }
    /* destroy shell */ {
        XtDestroyWidget(shell);
    }
    /* play */ {
        (void) Play(self);
    }
}
#endif

/*
 * ---------------------------------------------------------------------------
 * Controls callbacks
 * ---------------------------------------------------------------------------
 */

static void ResetCallback(Widget widget, XcpcApplication* self, XtPointer info)
{
    (void) Reset(self);
    (void) Play(self);
}

/*
 * ---------------------------------------------------------------------------
 * Drag & Drop callbacks
 * ---------------------------------------------------------------------------
 */

static int ConvertHexChar(const char* string)
{
    int  value     = 0;
    char character = *string++;

    /* check character */ {
        if(character != '%') {
            return -1;
        }
    }
    /* convert 1st digit */ {
        switch(character = *string++) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                value = ((value << 4) | (0x0 + (character - '0')));
                break;
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                value = ((value << 4) | (0xa + (character - 'a')));
                break;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                value = ((value << 4) | (0xa + (character - 'A')));
                break;
            default:
                return -1;
        }
    }
    /* convert 2nd digit */ {
        switch(character = *string++) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                value = ((value << 4) | (0x0 + (character - '0')));
                break;
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                value = ((value << 4) | (0xa + (character - 'a')));
                break;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                value = ((value << 4) | (0xa + (character - 'A')));
                break;
            default:
                return -1;
        }
    }
    return value;
}

static void DeserializeURI(char* buffer, size_t buflen, const char* uri)
{
    const char nul = '\0';
    const char cr  = '\r';
    const char lf  = '\n';
    char character = *uri;

    /* deserialize */ {
        if(character != nul) {
            do {
                if(buflen <= 1) {
                    break;
                }
                if((character == cr) || (character == lf)) {
                    break;
                }
                else if(character == '%') {
                    const int hexval = ConvertHexChar(uri);
                    if(hexval != -1) {
                        *buffer = ((char)(hexval & 0xff));
                        uri += 2;
                    }
                    else {
                        *buffer = character;
                    }
                }
                else {
                    *buffer = character;
                }
                ++buffer;
                --buflen;
            } while((character = *++uri) != nul);
        }
    }
    /* terminate buffer */ {
        *buffer = nul;
    }
}

static int CheckExtension(const char* filename, const char* extension)
{
    const int filename_length  = strlen(filename);
    const int extension_length = strlen(extension);

    if(filename_length >= extension_length) {
        if(strcasecmp(&filename[filename_length - extension_length], extension) == 0) {
            return 1;
        }
    }
    return 0;
}

static void DropUriCallback(Widget widget, XcpcApplication* self, const char* uri)
{
    char filename[PATH_MAX];

    if((uri != NULL) && (strncmp(uri, "file://", 7) == 0)) {
        DeserializeURI(filename, sizeof(filename), &uri[7]);
        if(CheckExtension(filename, ".sna") != 0) {
            (void) LoadSnapshot(self, filename);
            (void) Play(self);
        }
        else if(CheckExtension(filename, ".dsk") != 0) {
            (void) InsertOrRemoveDisk(self, filename);
            (void) Play(self);
        }
        else if(CheckExtension(filename, ".dsk.gz") != 0) {
            (void) InsertOrRemoveDisk(self, filename);
            (void) Play(self);
        }
        else if(CheckExtension(filename, ".dsk.bz2") != 0) {
            (void) InsertOrRemoveDisk(self, filename);
            (void) Play(self);
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * Hotkey callbacks
 * ---------------------------------------------------------------------------
 */

static void HotkeyCallback(Widget widget, XcpcApplication* self, KeySym* keysym)
{
    if(keysym != NULL) {
        switch(*keysym) {
            case XK_Pause:
                if(XtIsSensitive(self->layout.emulator) == False) {
                    Play(self);
                }
                else {
                    Pause(self);
                }
                break;
            case XK_F1:
                break;
            case XK_F2:
                break;
            case XK_F3:
                break;
            case XK_F4:
                break;
            case XK_F5:
                ResetCallback(widget, self, NULL);
                break;
            case XK_F6:
                break;
            case XK_F7:
                break;
            case XK_F8:
                break;
            case XK_F9:
                break;
            case XK_F10:
                break;
            case XK_F11:
                break;
            case XK_F12:
                break;
            default:
                break;
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication private methods
 * ---------------------------------------------------------------------------
 */

static XcpcApplication* BuildEmulator(XcpcApplication* self)
{
    XcpcLayoutRec* layout = &self->layout;
    Arg      arglist[24];
    Cardinal argcount = 0;

    /* emulator */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNborderWidth       , 0                         ); ++argcount;
        XtSetArg(arglist[argcount], XtNsensitive         , False                     ); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineInstance   , self->machine             ); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineCreateProc , &xcpc_machine_create_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineDestroyProc, &xcpc_machine_destroy_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineRealizeProc, &xcpc_machine_realize_proc); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineResizeProc , &xcpc_machine_resize_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineExposeProc , &xcpc_machine_expose_proc ); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineInputProc  , &xcpc_machine_input_proc  ); ++argcount;
        XtSetArg(arglist[argcount], XtNmachineClockProc  , &xcpc_machine_clock_proc  ); ++argcount;
        XtSetArg(arglist[argcount], XtNjoystick0         , xcpc_get_joystick0()      ); ++argcount;
        XtSetArg(arglist[argcount], XtNjoystick1         , xcpc_get_joystick1()      ); ++argcount;
        layout->emulator = XemCreateEmulator(self->layout.toplevel, "emulator", arglist, argcount);
        XtAddCallback(layout->emulator, XtNhotkeyCallback , (XtCallbackProc) &HotkeyCallback , (XtPointer) self);
        XtAddCallback(layout->emulator, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &layout->emulator);
        XtManageChild(layout->emulator);
    }
    return self;
}

static XcpcApplication* BuildLayout(XcpcApplication* self)
{
    /* emulator */ {
        (void) BuildEmulator(self);
    }
    return self;
}

static void DeferredStartHandler(XcpcApplication* self, XtIntervalId* intervalId)
{
    if(*intervalId == self->intervalId) {
        self->intervalId = ((XtIntervalId)(0));
    }
    /* set initial keyboard focus */ {
        XtSetKeyboardFocus(FindShell(self->layout.emulator), self->layout.emulator);
    }
    /* play */ {
        (void) Play(self);
    }
}

static XcpcApplication* Construct(XcpcApplication* self, int* argc, char*** argv)
{
    Arg      arglist[16];
    Cardinal argcount = 0;

    /* clear instance */ {
        (void) memset(self, 0, sizeof(XcpcApplication));
    }
    /* intialize options */ {
        self->options = xcpc_options_new();
    }
    /* parse the command-line */ {
        (void) xcpc_options_parse(self->options, argc, argv);
    }
    /* intialize machine */ {
        const XcpcMachineIface machine_iface = {
            self, /* user_data */
            NULL, /* reserved0 */
            NULL, /* reserved1 */
            NULL, /* reserved2 */
            NULL, /* reserved3 */
            NULL, /* reserved4 */
            NULL, /* reserved5 */
            NULL, /* reserved6 */
            NULL, /* reserved7 */
        };
        self->machine = xcpc_machine_new(&machine_iface, self->options);
    }
    /* set language proc */ {
        (void) XtSetLanguageProc(NULL, NULL, NULL);
    }
    /* create application context and toplevel shell */ {
        argcount = 0;
        XtSetArg(arglist[argcount], XtNmappedWhenManaged, True); ++argcount;
        XtSetArg(arglist[argcount], XtNallowShellResize , True); ++argcount;
        self->layout.toplevel = XtOpenApplication(&self->appcontext, "Xcpc", options, XtNumber(options), argc, *argv, fallback_resources, xemAppShellWidgetClass, arglist, argcount);
        XtAddCallback(self->layout.toplevel, XtNdestroyCallback, (XtCallbackProc) &DestroyCallback, (XtPointer) &self->layout.toplevel);
        XtAddCallback(self->layout.toplevel, XtNdropURICallback, (XtCallbackProc) &DropUriCallback, (XtPointer) self);
    }
    /* get application resources */ {
        argcount = 0;
        XtGetApplicationResources(self->layout.toplevel, (XtPointer) &self->resources, application_resources, XtNumber(application_resources), arglist, argcount);
    }
    /* get application name and class */ {
        XtGetApplicationNameAndClass(XtDisplay(self->layout.toplevel), &self->resources.app_name, &self->resources.app_class);
    }
    /* build user interface */ {
        (void) BuildLayout(self);
    }
    /* deferred start */ {
        self->intervalId = XtAppAddTimeOut(self->appcontext, 25UL, (XtTimerCallbackProc) &DeferredStartHandler, self);
    }
    return self;
}

static XcpcApplication* Destruct(XcpcApplication* self)
{
    /* destroy toplevel shell */ {
        if(self->layout.toplevel != NULL) {
            self->layout.toplevel = (XtDestroyWidget(self->layout.toplevel), NULL);
        }
    }
    /* destroy application context */ {
        if(self->appcontext != NULL) {
            self->appcontext = (XtDestroyApplicationContext(self->appcontext), NULL);
        }
    }
    /* finalize machine */ {
        self->machine = xcpc_machine_delete(self->machine);
    }
    /* finalize options */ {
        self->options = xcpc_options_delete(self->options);
    }
    return self;
}

static XcpcApplication* MainLoop(XcpcApplication* self)
{
    if(XtAppGetExitFlag(self->appcontext) == False) {
        /* realize toplevel shell */ {
            if((self->layout.toplevel != NULL)) {
                XtRealizeWidget(self->layout.toplevel);
            }
        }
        /* run application loop  */ {
            if((self->appcontext != NULL)) {
                XtAppMainLoop(self->appcontext);
            }
        }
    }
    return self;
}

/*
 * ---------------------------------------------------------------------------
 * XcpcApplication public methods
 * ---------------------------------------------------------------------------
 */

XcpcApplication* XcpcApplicationNew(int* argc, char*** argv)
{
    return Construct(xcpc_new(XcpcApplication), argc, argv);
}

XcpcApplication* XcpcApplicationDelete(XcpcApplication* self)
{
    return xcpc_delete(XcpcApplication, Destruct(self));
}

XcpcApplication* XcpcApplicationRun(XcpcApplication* self)
{
    return MainLoop(self);
}

/*
 * ---------------------------------------------------------------------------
 * xcpc_main
 * ---------------------------------------------------------------------------
 */

int xcpc_main(int* argc, char*** argv)
{
    XcpcApplication* self = XcpcApplicationNew(argc, argv);

    if(self != NULL) {
        (void) XcpcApplicationRun(self);
        self = XcpcApplicationDelete(self);
    }
    return EXIT_SUCCESS;
}

/*
 * ---------------------------------------------------------------------------
 * End-Of-File
 * ---------------------------------------------------------------------------
 */
