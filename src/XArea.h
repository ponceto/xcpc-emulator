/*
 * XArea.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __XAREA_H__
#define __XAREA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Resources:

 Name                  Class              RepType         Default Value
 ----                  -----              -------         -------------
 background            Background         Pixel           XtDefaultBackground
 border                BorderColor        Pixel           XtDefaultForeground
 borderWidth           BorderWidth        Dimension       1
 buttonPressCallback   Callback           Callback        NULL
 buttonReleaseCallback Callback           Callback        NULL
 destroyCallback       Callback           Pointer         NULL
 enterNotifyCallback   Callback           Callback        NULL
 exposeCallback        Callback           Callback        NULL
 height                Height             Dimension       0
 keyPressCallback      Callback           Callback        NULL
 keyReleaseCallback    Callback           Callback        NULL
 leaveNotifyCallback   Callback           Callback        NULL
 mappedWhenManaged     MappedWhenManaged  Boolean         True
 motionNotifyCallback  Callback           Callback        NULL
 sensitive             Sensitive          Boolean         True
 width                 Width              Dimension       0
 x                     Position           Position        0
 y                     Position           Position        0

*/

#define XtNkeyPressCallback      "keyPressCallback"
#define XtNkeyReleaseCallback    "keyReleaseCallback"
#define XtNbuttonPressCallback   "buttonPressCallback"
#define XtNbuttonReleaseCallback "buttonReleaseCallback"
#define XtNmotionNotifyCallback  "motionNotifyCallback"
#define XtNenterNotifyCallback   "enterNotifyCallback"
#define XtNleaveNotifyCallback   "leaveNotifyCallback"
#define XtNexposeCallback        "exposeCallback"

typedef struct _XAreaClassRec *XAreaWidgetClass;
typedef struct _XAreaRec *XAreaWidget;

extern WidgetClass xareaWidgetClass;

extern Widget XtCreateXArea(Widget p, String name, ArgList args, Cardinal n);

#ifdef __cplusplus
}
#endif

#endif
