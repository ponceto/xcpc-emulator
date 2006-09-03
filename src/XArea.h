#ifndef __XAREA_H__
#define __XAREA_H__

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

Widget XtCreateXArea(Widget p, String name, ArgList args, Cardinal n);

#endif
