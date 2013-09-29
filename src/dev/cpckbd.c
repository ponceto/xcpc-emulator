/*
 * cpckbd.c - Copyright (c) 2001-2013 Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpckbd.h"

#define SHFT_L_MASK 0x01
#define SHFT_R_MASK 0x02
#define CTRL_L_MASK 0x04
#define CTRL_R_MASK 0x08

static void gdev_cpckbd_reset(GdevCPCKBD *cpckbd);
static void gdev_cpckbd_clock(GdevCPCKBD *cpckbd);

G_DEFINE_TYPE(GdevCPCKBD, gdev_cpckbd, GDEV_TYPE_DEVICE)

/**
 * GdevCPCKBD::class_init()
 *
 * @param cpckbd_class specifies the GdevCPCKBD class
 */
static void gdev_cpckbd_class_init(GdevCPCKBDClass *cpckbd_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(cpckbd_class);

  device_class->reset = (GdevDeviceProc) gdev_cpckbd_reset;
  device_class->clock = (GdevDeviceProc) gdev_cpckbd_clock;
}

/**
 * GdevCPCKBD::init()
 *
 * @param cpckbd specifies the GdevCPCKBD instance
 */
static void gdev_cpckbd_init(GdevCPCKBD *cpckbd)
{
  gdev_cpckbd_reset(cpckbd);
}

/**
 * GdevCPCKBD::reset()
 *
 * @param cpckbd specifies the GdevCPCKBD instance
 */
static void gdev_cpckbd_reset(GdevCPCKBD *cpckbd)
{
  cpckbd->mods       = 0x00;
  cpckbd->line       = 0x00;
  cpckbd->bits[0x00] = 0xff;
  cpckbd->bits[0x01] = 0xff;
  cpckbd->bits[0x02] = 0xff;
  cpckbd->bits[0x03] = 0xff;
  cpckbd->bits[0x04] = 0xff;
  cpckbd->bits[0x05] = 0xff;
  cpckbd->bits[0x06] = 0xff;
  cpckbd->bits[0x07] = 0xff;
  cpckbd->bits[0x08] = 0xff;
  cpckbd->bits[0x09] = 0xff;
  cpckbd->bits[0x0a] = 0xff;
  cpckbd->bits[0x0b] = 0xff;
  cpckbd->bits[0x0c] = 0xff;
  cpckbd->bits[0x0d] = 0xff;
  cpckbd->bits[0x0e] = 0xff;
  cpckbd->bits[0x0f] = 0xff;
}

/**
 * GdevCPCKBD::clock()
 *
 * @param cpckbd specifies the GdevCPCKBD instance
 */
static void gdev_cpckbd_clock(GdevCPCKBD *cpckbd)
{
}

/**
 * GdevCPCKBD::new()
 *
 * @return the GdevCPCKBD instance
 */
GdevCPCKBD *gdev_cpckbd_new(void)
{
  return(g_object_new(GDEV_TYPE_CPCKBD, NULL));
}

/**
 * GdevCPCKBD::qwerty()
 *
 * @param cpckbd specifies the GdevCPCKBD instance
 * @param xevent specifies the XEvent structure pointer
 */
void gdev_cpckbd_qwerty(GdevCPCKBD *cpckbd, XEvent *xevent)
{
  KeySym keysym = XK_VoidSymbol;
  guint8 line = 0x09;
  guint8 mask = 0x40;
  guint8 mods = 0x00;

  if((cpckbd->mods & SHFT_R_MASK) == 0) {
    xevent->xkey.state &= ~ShiftMask;
  }
  if((cpckbd->mods & CTRL_R_MASK) == 0) {
    xevent->xkey.state &= ~ControlMask;
  }
  (void) XLookupString((XKeyEvent *) xevent, NULL, 0, &keysym, NULL);
  if((keysym >> 8) == 0x00) {
    switch(keysym) {
      case XK_space:
        line = 0x05; mask = 0x80;
        break;
      case XK_exclam:
        mods |= SHFT_L_MASK;
      case XK_1:
        line = 0x08; mask = 0x01;
        break;
      case XK_asciitilde:
        mods |= CTRL_L_MASK;
      case XK_quotedbl:
        mods |= SHFT_L_MASK;
      case XK_2:
        line = 0x08; mask = 0x02;
        break;
      case XK_numbersign:
        mods |= SHFT_L_MASK;
      case XK_3:
        line = 0x07; mask = 0x02;
        break;
      case XK_dollar:
        mods |= SHFT_L_MASK;
      case XK_4:
        line = 0x07; mask = 0x01;
        break;
      case XK_percent:
        mods |= SHFT_L_MASK;
      case XK_5:
        line = 0x06; mask = 0x02;
        break;
      case XK_ampersand:
        mods |= SHFT_L_MASK;
      case XK_6:
        line = 0x06; mask = 0x01;
        break;
      case XK_apostrophe:
        mods |= SHFT_L_MASK;
      case XK_7:
        line = 0x05; mask = 0x02;
        break;
      case XK_parenleft:
        mods |= SHFT_L_MASK;
      case XK_8:
        line = 0x05; mask = 0x01;
        break;
      case XK_parenright:
        mods |= SHFT_L_MASK;
      case XK_9:
        line = 0x04; mask = 0x02;
        break;
      case XK_underscore:
        mods |= SHFT_L_MASK;
      case XK_0:
        line = 0x04; mask = 0x01;
        break;
      case XK_equal:
        mods |= SHFT_L_MASK;
      case XK_minus:
        line = 0x03; mask = 0x02;
        break;
      case XK_sterling:
        mods |= SHFT_L_MASK;
      case XK_asciicircum:
        line = 0x03; mask = 0x01;
        break;
      case XK_Q:
        mods |= SHFT_L_MASK;
      case XK_q:
        line = 0x08; mask = 0x08;
        break;
      case XK_W:
        mods |= SHFT_L_MASK;
      case XK_w:
        line = 0x07; mask = 0x08;
        break;
      case XK_E:
        mods |= SHFT_L_MASK;
      case XK_e:
        line = 0x07; mask = 0x04;
        break;
      case XK_R:
        mods |= SHFT_L_MASK;
      case XK_r:
        line = 0x06; mask = 0x04;
        break;
      case XK_T:
        mods |= SHFT_L_MASK;
      case XK_t:
        line = 0x06; mask = 0x08;
        break;
      case XK_Y:
        mods |= SHFT_L_MASK;
      case XK_y:
        line = 0x05; mask = 0x08;
        break;
      case XK_U:
        mods |= SHFT_L_MASK;
      case XK_u:
        line = 0x05; mask = 0x04;
        break;
      case XK_I:
        mods |= SHFT_L_MASK;
      case XK_i:
        line = 0x04; mask = 0x08;
        break;
      case XK_O:
        mods |= SHFT_L_MASK;
      case XK_o:
        line = 0x04; mask = 0x04;
        break;
      case XK_P:
        mods |= SHFT_L_MASK;
      case XK_p:
        line = 0x03; mask = 0x08;
        break;
      case XK_bar:
        mods |= SHFT_L_MASK;
      case XK_at:
        line = 0x03; mask = 0x04;
        break;
      case XK_braceleft:
        mods |= SHFT_L_MASK;
      case XK_bracketleft:
        line = 0x02; mask = 0x02;
        break;
      case XK_A:
        mods |= SHFT_L_MASK;
      case XK_a:
        line = 0x08; mask = 0x20;
        break;
      case XK_S:
        mods |= SHFT_L_MASK;
      case XK_s:
        line = 0x07; mask = 0x10;
        break;
      case XK_D:
        mods |= SHFT_L_MASK;
      case XK_d:
        line = 0x07; mask = 0x20;
        break;
      case XK_F:
        mods |= SHFT_L_MASK;
      case XK_f:
        line = 0x06; mask = 0x20;
        break;
      case XK_G:
        mods |= SHFT_L_MASK;
      case XK_g:
        line = 0x06; mask = 0x10;
        break;
      case XK_H:
        mods |= SHFT_L_MASK;
      case XK_h:
        line = 0x05; mask = 0x10;
        break;
      case XK_J:
        mods |= SHFT_L_MASK;
      case XK_j:
        line = 0x05; mask = 0x20;
        break;
      case XK_K:
        mods |= SHFT_L_MASK;
      case XK_k:
        line = 0x04; mask = 0x20;
        break;
      case XK_L:
        mods |= SHFT_L_MASK;
      case XK_l:
        line = 0x04; mask = 0x10;
        break;
      case XK_asterisk:
        mods |= SHFT_L_MASK;
      case XK_colon:
        line = 0x03; mask = 0x20;
        break;
      case XK_plus:
        mods |= SHFT_L_MASK;
      case XK_semicolon:
        line = 0x03; mask = 0x10;
        break;
      case XK_braceright:
        mods |= SHFT_L_MASK;
      case XK_bracketright:
        line = 0x02; mask = 0x08;
        break;
      case XK_Z:
        mods |= SHFT_L_MASK;
      case XK_z:
        line = 0x08; mask = 0x80;
        break;
      case XK_X:
        mods |= SHFT_L_MASK;
      case XK_x:
        line = 0x07; mask = 0x80;
        break;
      case XK_C:
        mods |= SHFT_L_MASK;
      case XK_c:
        line = 0x07; mask = 0x40;
        break;
      case XK_V:
        mods |= SHFT_L_MASK;
      case XK_v:
        line = 0x06; mask = 0x80;
        break;
      case XK_B:
        mods |= SHFT_L_MASK;
      case XK_b:
        line = 0x06; mask = 0x40;
        break;
      case XK_N:
        mods |= SHFT_L_MASK;
      case XK_n:
        line = 0x05; mask = 0x40;
        break;
      case XK_M:
        mods |= SHFT_L_MASK;
      case XK_m:
        line = 0x04; mask = 0x40;
        break;
      case XK_less:
        mods |= SHFT_L_MASK;
      case XK_comma:
        line = 0x04; mask = 0x80;
        break;
      case XK_greater:
        mods |= SHFT_L_MASK;
      case XK_period:
        line = 0x03; mask = 0x80;
        break;
      case XK_question:
        mods |= SHFT_L_MASK;
      case XK_slash:
        line = 0x03; mask = 0x40;
        break;
      case XK_grave:
        mods |= SHFT_L_MASK;
      case XK_backslash:
        line = 0x02; mask = 0x40;
        break;
    }
  }
  else if((keysym >> 8) == 0xff) {
    switch(keysym) {
      case XK_BackSpace:
        line = 0x09; mask = 0x80;
        break;
      case XK_Tab:
        line = 0x08; mask = 0x10;
        break;
      case XK_Return:
        line = 0x02; mask = 0x04;
        break;
      case XK_Escape:
        line = 0x08; mask = 0x04;
        break;
      case XK_Delete:
        line = 0x02; mask = 0x01;
        break;
      case XK_Shift_L:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  SHFT_L_MASK;
        }
        else {
          cpckbd->mods &= ~SHFT_L_MASK;
        }
        break;
      case XK_Shift_R:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  SHFT_R_MASK;
        }
        else {
          cpckbd->mods &= ~SHFT_R_MASK;
        }
        break;
      case XK_Control_L:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  CTRL_L_MASK;
        }
        else {
          cpckbd->mods &= ~CTRL_L_MASK;
        }
        break;
      case XK_Control_R:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  CTRL_R_MASK;
        }
        else {
          cpckbd->mods &= ~CTRL_R_MASK;
        }
        break;
      case XK_Alt_L:
        line = 0x01; mask = 0x02;
        break;
      case XK_Caps_Lock:
        line = 0x08; mask = 0x40;
        break;
      case XK_Up:
        line = 0x00; mask = 0x01;
        break;
      case XK_Down:
        line = 0x00; mask = 0x04;
        break;
      case XK_Left:
        line = 0x01; mask = 0x01;
        break;
      case XK_Right:
        line = 0x00; mask = 0x02;
        break;
      case XK_KP_Up:
        line = 0x09; mask = 0x01;
        break;
      case XK_KP_Down:
        line = 0x09; mask = 0x02;
        break;
      case XK_KP_Left:
        line = 0x09; mask = 0x04;
        break;
      case XK_KP_Right:
        line = 0x09; mask = 0x08;
        break;
      case XK_KP_Insert:
        line = 0x09; mask = 0x10;
        break;
      case XK_KP_Delete:
        line = 0x09; mask = 0x20;
        break;
      case XK_KP_Enter:
        line = 0x00; mask = 0x40;
        break;
      case XK_KP_Decimal:
        line = 0x00; mask = 0x80;
        break;
      case XK_KP_0:
        line = 0x01; mask = 0x80;
        break;
      case XK_KP_1:
        line = 0x01; mask = 0x20;
        break;
      case XK_KP_2:
        line = 0x01; mask = 0x40;
        break;
      case XK_KP_3:
        line = 0x00; mask = 0x20;
        break;
      case XK_KP_4:
        line = 0x02; mask = 0x10;
        break;
      case XK_KP_5:
        line = 0x01; mask = 0x10;
        break;
      case XK_KP_6:
        line = 0x00; mask = 0x10;
        break;
      case XK_KP_7:
        line = 0x01; mask = 0x04;
        break;
      case XK_KP_8:
        line = 0x01; mask = 0x08;
        break;
      case XK_KP_9:
        line = 0x00; mask = 0x08;
        break;
    }
  }
  if((cpckbd->mods & SHFT_L_MASK) != 0) {
    cpckbd->bits[0x02] &= ~0x20;
  }
  else {
    cpckbd->bits[0x02] |=  0x20;
  }
  if((cpckbd->mods & CTRL_L_MASK) != 0) {
    cpckbd->bits[0x02] &= ~0x80;
  }
  else {
    cpckbd->bits[0x02] |=  0x80;
  }
  if(xevent->type == KeyPress) {
    if((mods & SHFT_L_MASK) != 0) {
      cpckbd->bits[0x02] &= ~0x20;
    }
    if((mods & CTRL_L_MASK) != 0) {
      cpckbd->bits[0x02] &= ~0x80;
    }
    cpckbd->bits[line] &= ~mask;
  }
  else {
    cpckbd->bits[line] |=  mask;
  }
}

/**
 * GdevCPCKBD::azerty()
 *
 * @param cpckbd specifies the GdevCPCKBD instance
 * @param xevent specifies the XEvent structure pointer
 */
void gdev_cpckbd_azerty(GdevCPCKBD *cpckbd, XEvent *xevent)
{
  KeySym keysym = XK_VoidSymbol;
  guint8 line = 0x09;
  guint8 mask = 0x40;
  guint8 mods = 0x00;

  if((cpckbd->mods & SHFT_R_MASK) == 0) {
    xevent->xkey.state &= ~ShiftMask;
  }
  if((cpckbd->mods & CTRL_R_MASK) == 0) {
    xevent->xkey.state &= ~ControlMask;
  }
  (void) XLookupString((XKeyEvent *) xevent, NULL, 0, &keysym, NULL);
  if((keysym >> 8) == 0x00) {
    switch(keysym) {
      case XK_space:
        line = 0x05; mask = 0x80;
        break;
      case XK_1:
        mods |= SHFT_L_MASK;
      case XK_ampersand:
        line = 0x08; mask = 0x01;
        break;
      case XK_asciitilde:
        mods |= CTRL_L_MASK;
      case XK_2:
        mods |= SHFT_L_MASK;
      case XK_eacute:
        line = 0x08; mask = 0x02;
        break;
      case XK_3:
        mods |= SHFT_L_MASK;
      case XK_quotedbl:
        line = 0x07; mask = 0x02;
        break;
      case XK_4:
        mods |= SHFT_L_MASK;
      case XK_apostrophe:
        line = 0x07; mask = 0x01;
        break;
      case XK_5:
        mods |= SHFT_L_MASK;
      case XK_parenleft:
        line = 0x06; mask = 0x02;
        break;
      case XK_6:
        mods |= SHFT_L_MASK;
      case XK_bracketright:
        line = 0x06; mask = 0x01;
        break;
      case XK_7:
        mods |= SHFT_L_MASK;
      case XK_egrave:
        line = 0x05; mask = 0x02;
        break;
      case XK_8:
        mods |= SHFT_L_MASK;
      case XK_exclam:
        line = 0x05; mask = 0x01;
        break;
      case XK_9:
        mods |= SHFT_L_MASK;
      case XK_ccedilla:
        line = 0x04; mask = 0x02;
        break;
      case XK_0:
        mods |= SHFT_L_MASK;
      case XK_agrave:
        line = 0x04; mask = 0x01;
        break;
      case XK_bracketleft:
        mods |= SHFT_L_MASK;
      case XK_parenright:
        line = 0x03; mask = 0x02;
        break;
      case XK_underscore:
        mods |= SHFT_L_MASK;
      case XK_minus:
        line = 0x03; mask = 0x01;
        break;
      case XK_A:
        mods |= SHFT_L_MASK;
      case XK_a:
        line = 0x08; mask = 0x08;
        break;
      case XK_Z:
        mods |= SHFT_L_MASK;
      case XK_z:
        line = 0x07; mask = 0x08;
        break;
      case XK_E:
        mods |= SHFT_L_MASK;
      case XK_e:
        line = 0x07; mask = 0x04;
        break;
      case XK_R:
        mods |= SHFT_L_MASK;
      case XK_r:
        line = 0x06; mask = 0x04;
        break;
      case XK_T:
        mods |= SHFT_L_MASK;
      case XK_t:
        line = 0x06; mask = 0x08;
        break;
      case XK_Y:
        mods |= SHFT_L_MASK;
      case XK_y:
        line = 0x05; mask = 0x08;
        break;
      case XK_U:
        mods |= SHFT_L_MASK;
      case XK_u:
        line = 0x05; mask = 0x04;
        break;
      case XK_I:
        mods |= SHFT_L_MASK;
      case XK_i:
        line = 0x04; mask = 0x08;
        break;
      case XK_O:
        mods |= SHFT_L_MASK;
      case XK_o:
        line = 0x04; mask = 0x04;
        break;
      case XK_P:
        mods |= SHFT_L_MASK;
      case XK_p:
        line = 0x03; mask = 0x08;
        break;
      case XK_bar:
        mods |= SHFT_L_MASK;
      case XK_asciicircum:
        line = 0x03; mask = 0x04;
        break;
      case XK_less:
        mods |= SHFT_L_MASK;
      case XK_asterisk:
        line = 0x02; mask = 0x02;
        break;
      case XK_Q:
        mods |= SHFT_L_MASK;
      case XK_q:
        line = 0x08; mask = 0x20;
        break;
      case XK_S:
        mods |= SHFT_L_MASK;
      case XK_s:
        line = 0x07; mask = 0x10;
        break;
      case XK_D:
        mods |= SHFT_L_MASK;
      case XK_d:
        line = 0x07; mask = 0x20;
        break;
      case XK_F:
        mods |= SHFT_L_MASK;
      case XK_f:
        line = 0x06; mask = 0x20;
        break;
      case XK_G:
        mods |= SHFT_L_MASK;
      case XK_g:
        line = 0x06; mask = 0x10;
        break;
      case XK_H:
        mods |= SHFT_L_MASK;
      case XK_h:
        line = 0x05; mask = 0x10;
        break;
      case XK_J:
        mods |= SHFT_L_MASK;
      case XK_j:
        line = 0x05; mask = 0x20;
        break;
      case XK_K:
        mods |= SHFT_L_MASK;
      case XK_k:
        line = 0x04; mask = 0x20;
        break;
      case XK_L:
        mods |= SHFT_L_MASK;
      case XK_l:
        line = 0x04; mask = 0x10;
        break;
      case XK_M:
        mods |= SHFT_L_MASK;
      case XK_m:
        line = 0x03; mask = 0x20;
        break;
      case XK_percent:
        mods |= SHFT_L_MASK;
      case XK_ugrave:
        line = 0x03; mask = 0x10;
        break;
      case XK_greater:
        mods |= SHFT_L_MASK;
      case XK_numbersign:
        line = 0x02; mask = 0x08;
        break;
      case XK_W:
        mods |= SHFT_L_MASK;
      case XK_w:
        line = 0x08; mask = 0x80;
        break;
      case XK_X:
        mods |= SHFT_L_MASK;
      case XK_x:
        line = 0x07; mask = 0x80;
        break;
      case XK_C:
        mods |= SHFT_L_MASK;
      case XK_c:
        line = 0x07; mask = 0x40;
        break;
      case XK_V:
        mods |= SHFT_L_MASK;
      case XK_v:
        line = 0x06; mask = 0x80;
        break;
      case XK_B:
        mods |= SHFT_L_MASK;
      case XK_b:
        line = 0x06; mask = 0x40;
        break;
      case XK_N:
        mods |= SHFT_L_MASK;
      case XK_n:
        line = 0x05; mask = 0x40;
        break;
      case XK_question:
        mods |= SHFT_L_MASK;
      case XK_comma:
        line = 0x04; mask = 0x40;
        break;
      case XK_period:
        mods |= SHFT_L_MASK;
      case XK_semicolon:
        line = 0x04; mask = 0x80;
        break;
      case XK_slash:
        mods |= SHFT_L_MASK;
      case XK_colon:
        line = 0x03; mask = 0x80;
        break;
      case XK_plus:
        mods |= SHFT_L_MASK;
      case XK_equal:
        line = 0x03; mask = 0x40;
        break;
      case XK_at:
        mods |= SHFT_L_MASK;
      case XK_dollar:
        line = 0x02; mask = 0x40;
        break;
    }
  }
  else if((keysym >> 8) == 0xff) {
    switch(keysym) {
      case XK_BackSpace:
        line = 0x09; mask = 0x80;
        break;
      case XK_Tab:
        line = 0x08; mask = 0x10;
        break;
      case XK_Return:
        line = 0x02; mask = 0x04;
        break;
      case XK_Escape:
        line = 0x08; mask = 0x04;
        break;
      case XK_Delete:
        line = 0x02; mask = 0x01;
        break;
      case XK_Shift_L:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  SHFT_L_MASK;
        }
        else {
          cpckbd->mods &= ~SHFT_L_MASK;
        }
        break;
      case XK_Shift_R:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  SHFT_R_MASK;
        }
        else {
          cpckbd->mods &= ~SHFT_R_MASK;
        }
        break;
      case XK_Control_L:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  CTRL_L_MASK;
        }
        else {
          cpckbd->mods &= ~CTRL_L_MASK;
        }
        break;
      case XK_Control_R:
        if(xevent->type == KeyPress) {
          cpckbd->mods |=  CTRL_R_MASK;
        }
        else {
          cpckbd->mods &= ~CTRL_R_MASK;
        }
        break;
      case XK_Alt_L:
        line = 0x01; mask = 0x02;
        break;
      case XK_Caps_Lock:
        line = 0x08; mask = 0x40;
        break;
      case XK_Up:
        line = 0x00; mask = 0x01;
        break;
      case XK_Down:
        line = 0x00; mask = 0x04;
        break;
      case XK_Left:
        line = 0x01; mask = 0x01;
        break;
      case XK_Right:
        line = 0x00; mask = 0x02;
        break;
      case XK_KP_Up:
        line = 0x09; mask = 0x01;
        break;
      case XK_KP_Down:
        line = 0x09; mask = 0x02;
        break;
      case XK_KP_Left:
        line = 0x09; mask = 0x04;
        break;
      case XK_KP_Right:
        line = 0x09; mask = 0x08;
        break;
      case XK_KP_Insert:
        line = 0x09; mask = 0x10;
        break;
      case XK_KP_Delete:
        line = 0x09; mask = 0x20;
        break;
      case XK_KP_Enter:
        line = 0x00; mask = 0x40;
        break;
      case XK_KP_Decimal:
        line = 0x00; mask = 0x80;
        break;
      case XK_KP_0:
        line = 0x01; mask = 0x80;
        break;
      case XK_KP_1:
        line = 0x01; mask = 0x20;
        break;
      case XK_KP_2:
        line = 0x01; mask = 0x40;
        break;
      case XK_KP_3:
        line = 0x00; mask = 0x20;
        break;
      case XK_KP_4:
        line = 0x02; mask = 0x10;
        break;
      case XK_KP_5:
        line = 0x01; mask = 0x10;
        break;
      case XK_KP_6:
        line = 0x00; mask = 0x10;
        break;
      case XK_KP_7:
        line = 0x01; mask = 0x04;
        break;
      case XK_KP_8:
        line = 0x01; mask = 0x08;
        break;
      case XK_KP_9:
        line = 0x00; mask = 0x08;
        break;
    }
  }
  if((cpckbd->mods & SHFT_L_MASK) != 0) {
    cpckbd->bits[0x02] &= ~0x20;
  }
  else {
    cpckbd->bits[0x02] |=  0x20;
  }
  if((cpckbd->mods & CTRL_L_MASK) != 0) {
    cpckbd->bits[0x02] &= ~0x80;
  }
  else {
    cpckbd->bits[0x02] |=  0x80;
  }
  if(xevent->type == KeyPress) {
    if((mods & SHFT_L_MASK) != 0) {
      cpckbd->bits[0x02] &= ~0x20;
    }
    if((mods & CTRL_L_MASK) != 0) {
      cpckbd->bits[0x02] &= ~0x80;
    }
    cpckbd->bits[line] &= ~mask;
  }
  else {
    cpckbd->bits[line] |=  mask;
  }
}
