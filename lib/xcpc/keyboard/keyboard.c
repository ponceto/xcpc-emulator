/*
 * keyboard.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#include "keyboard-priv.h"

static void xcpc_keyboard_trace(const char* function)
{
    xcpc_log_trace("XcpcKeyboard::%s()", function);
}

XcpcKeyboard* xcpc_keyboard_alloc(void)
{
    xcpc_keyboard_trace("alloc");

    return xcpc_new(XcpcKeyboard);
}

XcpcKeyboard* xcpc_keyboard_free(XcpcKeyboard* self)
{
    xcpc_keyboard_trace("free");

    return xcpc_delete(XcpcKeyboard, self);
}

XcpcKeyboard* xcpc_keyboard_construct(XcpcKeyboard* self)
{
    xcpc_keyboard_trace("construct");

    /* clear iface */ {
        (void) memset(&self->iface, 0, sizeof(XcpcKeyboardIface));
    }
    /* clear state */ {
        (void) memset(&self->state, 0, sizeof(XcpcKeyboardState));
    }
    /* initialize iface */ {
        (void) xcpc_keyboard_set_iface(self, NULL);
    }
    return xcpc_keyboard_reset(self);
}

XcpcKeyboard* xcpc_keyboard_destruct(XcpcKeyboard* self)
{
    xcpc_keyboard_trace("destruct");

    return self;
}

XcpcKeyboard* xcpc_keyboard_new(void)
{
    xcpc_keyboard_trace("new");

    return xcpc_keyboard_construct(xcpc_keyboard_alloc());
}

XcpcKeyboard* xcpc_keyboard_delete(XcpcKeyboard* self)
{
    xcpc_keyboard_trace("delete");

    return xcpc_keyboard_free(xcpc_keyboard_destruct(self));
}

XcpcKeyboard* xcpc_keyboard_set_iface(XcpcKeyboard* self, const XcpcKeyboardIface* iface)
{
    xcpc_keyboard_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = self;
    }
    return self;
}

XcpcKeyboard* xcpc_keyboard_reset(XcpcKeyboard* self)
{
    xcpc_keyboard_trace("reset");

    /* reset state */ {
        unsigned int index = 0;
        unsigned int count = countof(self->state.keys);
        for(index = 0; index < count; ++index) {
            self->state.mode        = 0x00;
            self->state.line        = 0x00;
            self->state.keys[index] = 0xff;
        }
    }
    return self;
}

XcpcKeyboard* xcpc_keyboard_clock(XcpcKeyboard* self)
{
    return self;
}

XcpcKeyboard* xcpc_keyboard_qwerty(XcpcKeyboard* self, XKeyEvent* event)
{
    KeySym keysym = NoSymbol;
    KeySym keypag = NoSymbol;
    uint8_t line  = 0xff;
    uint8_t bits  = 0x00;
    uint8_t mods  = ~(self->state.keys[0x0f]);

    /* check key */ {
        char   buffer[16] = { '\0' };
        size_t buflen = sizeof(buffer);
        buflen = XLookupString(event, buffer, buflen, &keysym, NULL);
        keypag = (keysym >> 8);
        if(keypag == 0x00) {
            switch(keysym) {
                case XK_space:        /* 0x0020, U+0020 SPACE                  */
                    SET_KEY(ROW5, BIT7);
                    break;
                case XK_exclam:       /* 0x0021, U+0021 EXCLAMATION MARK       */
                    SFT_KEY(ROW8, BIT0);
                    break;
                case XK_quotedbl:     /* 0x0022, U+0022 QUOTATION MARK         */
                    SFT_KEY(ROW8, BIT1);
                    break;
                case XK_numbersign:   /* 0x0023, U+0023 NUMBER SIGN            */
                    SFT_KEY(ROW7, BIT1);
                    break;
                case XK_dollar:       /* 0x0024, U+0024 DOLLAR SIGN            */
                    SFT_KEY(ROW7, BIT0);
                    break;
                case XK_percent:      /* 0x0025, U+0025 PERCENT SIGN           */
                    SFT_KEY(ROW6, BIT1);
                    break;
                case XK_ampersand:    /* 0x0026, U+0026 AMPERSAND              */
                    SFT_KEY(ROW6, BIT0);
                    break;
                case XK_apostrophe:   /* 0x0027, U+0027 APOSTROPHE             */
                    SFT_KEY(ROW5, BIT1);
                    break;
                case XK_parenleft:    /* 0x0028, U+0028 LEFT PARENTHESIS       */
                    SFT_KEY(ROW5, BIT0);
                    break;
                case XK_parenright:   /* 0x0029, U+0029 RIGHT PARENTHESIS      */
                    SFT_KEY(ROW4, BIT1);
                    break;
                case XK_asterisk:     /* 0x002a, U+002A ASTERISK               */
                    SFT_KEY(ROW3, BIT5);
                    break;
                case XK_plus:         /* 0x002b, U+002B PLUS SIGN              */
                    SFT_KEY(ROW3, BIT4);
                    break;
                case XK_comma:        /* 0x002c, U+002C COMMA                  */
                    SET_KEY(ROW4, BIT7);
                    break;
                case XK_minus:        /* 0x002d, U+002D HYPHEN-MINUS           */
                    SET_KEY(ROW3, BIT1);
                    break;
                case XK_period:       /* 0x002e, U+002E FULL STOP              */
                    SET_KEY(ROW3, BIT7);
                    break;
                case XK_slash:        /* 0x002f, U+002F SOLIDUS                */
                    SET_KEY(ROW3, BIT6);
                    break;
                case XK_0:            /* 0x0030, U+0030 DIGIT ZERO             */
                    SET_KEY(ROW4, BIT0);
                    break;
                case XK_1:            /* 0x0031, U+0031 DIGIT ONE              */
                    SET_KEY(ROW8, BIT0);
                    break;
                case XK_2:            /* 0x0032, U+0032 DIGIT TWO              */
                    SET_KEY(ROW8, BIT1);
                    break;
                case XK_3:            /* 0x0033, U+0033 DIGIT THREE            */
                    SET_KEY(ROW7, BIT1);
                    break;
                case XK_4:            /* 0x0034, U+0034 DIGIT FOUR             */
                    SET_KEY(ROW7, BIT0);
                    break;
                case XK_5:            /* 0x0035, U+0035 DIGIT FIVE             */
                    SET_KEY(ROW6, BIT1);
                    break;
                case XK_6:            /* 0x0036, U+0036 DIGIT SIX              */
                    SET_KEY(ROW6, BIT0);
                    break;
                case XK_7:            /* 0x0037, U+0037 DIGIT SEVEN            */
                    SET_KEY(ROW5, BIT1);
                    break;
                case XK_8:            /* 0x0038, U+0038 DIGIT EIGHT            */
                    SET_KEY(ROW5, BIT0);
                    break;
                case XK_9:            /* 0x0039, U+0039 DIGIT NINE             */
                    SET_KEY(ROW4, BIT1);
                    break;
                case XK_colon:        /* 0x003a, U+003A COLON                  */
                    SET_KEY(ROW3, BIT5);
                    break;
                case XK_semicolon:    /* 0x003b, U+003B SEMICOLON              */
                    SET_KEY(ROW3, BIT4);
                    break;
                case XK_less:         /* 0x003c, U+003C LESS-THAN SIGN         */
                    SFT_KEY(ROW4, BIT7);
                    break;
                case XK_equal:        /* 0x003d, U+003D EQUALS SIGN            */
                    SFT_KEY(ROW3, BIT1);
                    break;
                case XK_greater:      /* 0x003e, U+003E GREATER-THAN SIGN      */
                    SFT_KEY(ROW3, BIT7);
                    break;
                case XK_question:     /* 0x003f, U+003F QUESTION MARK          */
                    SFT_KEY(ROW3, BIT6);
                    break;
                case XK_at:           /* 0x0040, U+0040 COMMERCIAL AT          */
                    SET_KEY(ROW3, BIT2);
                    break;
                case XK_A:            /* 0x0041, U+0041 LATIN CAPITAL LETTER A */
                    SFT_KEY(ROW8, BIT5);
                    break;
                case XK_B:            /* 0x0042, U+0042 LATIN CAPITAL LETTER B */
                    SFT_KEY(ROW6, BIT6);
                    break;
                case XK_C:            /* 0x0043, U+0043 LATIN CAPITAL LETTER C */
                    SFT_KEY(ROW7, BIT6);
                    break;
                case XK_D:            /* 0x0044, U+0044 LATIN CAPITAL LETTER D */
                    SFT_KEY(ROW7, BIT5);
                    break;
                case XK_E:            /* 0x0045, U+0045 LATIN CAPITAL LETTER E */
                    SFT_KEY(ROW7, BIT2);
                    break;
                case XK_F:            /* 0x0046, U+0046 LATIN CAPITAL LETTER F */
                    SFT_KEY(ROW6, BIT5);
                    break;
                case XK_G:            /* 0x0047, U+0047 LATIN CAPITAL LETTER G */
                    SFT_KEY(ROW6, BIT4);
                    break;
                case XK_H:            /* 0x0048, U+0048 LATIN CAPITAL LETTER H */
                    SFT_KEY(ROW5, BIT4);
                    break;
                case XK_I:            /* 0x0049, U+0049 LATIN CAPITAL LETTER I */
                    SFT_KEY(ROW4, BIT3);
                    break;
                case XK_J:            /* 0x004a, U+004A LATIN CAPITAL LETTER J */
                    SFT_KEY(ROW5, BIT5);
                    break;
                case XK_K:            /* 0x004b, U+004B LATIN CAPITAL LETTER K */
                    SFT_KEY(ROW4, BIT5);
                    break;
                case XK_L:            /* 0x004c, U+004C LATIN CAPITAL LETTER L */
                    SFT_KEY(ROW4, BIT4);
                    break;
                case XK_M:            /* 0x004d, U+004D LATIN CAPITAL LETTER M */
                    SFT_KEY(ROW4, BIT6);
                    break;
                case XK_N:            /* 0x004e, U+004E LATIN CAPITAL LETTER N */
                    SFT_KEY(ROW5, BIT6);
                    break;
                case XK_O:            /* 0x004f, U+004F LATIN CAPITAL LETTER O */
                    SFT_KEY(ROW4, BIT2);
                    break;
                case XK_P:            /* 0x0050, U+0050 LATIN CAPITAL LETTER P */
                    SFT_KEY(ROW3, BIT3);
                    break;
                case XK_Q:            /* 0x0051, U+0051 LATIN CAPITAL LETTER Q */
                    SFT_KEY(ROW8, BIT3);
                    break;
                case XK_R:            /* 0x0052, U+0052 LATIN CAPITAL LETTER R */
                    SFT_KEY(ROW6, BIT2);
                    break;
                case XK_S:            /* 0x0053, U+0053 LATIN CAPITAL LETTER S */
                    SFT_KEY(ROW7, BIT4);
                    break;
                case XK_T:            /* 0x0054, U+0054 LATIN CAPITAL LETTER T */
                    SFT_KEY(ROW6, BIT3);
                    break;
                case XK_U:            /* 0x0055, U+0055 LATIN CAPITAL LETTER U */
                    SFT_KEY(ROW5, BIT2);
                    break;
                case XK_V:            /* 0x0056, U+0056 LATIN CAPITAL LETTER V */
                    SFT_KEY(ROW6, BIT7);
                    break;
                case XK_W:            /* 0x0057, U+0057 LATIN CAPITAL LETTER W */
                    SFT_KEY(ROW7, BIT3);
                    break;
                case XK_X:            /* 0x0058, U+0058 LATIN CAPITAL LETTER X */
                    SFT_KEY(ROW7, BIT7);
                    break;
                case XK_Y:            /* 0x0059, U+0059 LATIN CAPITAL LETTER Y */
                    SFT_KEY(ROW5, BIT3);
                    break;
                case XK_Z:            /* 0x005a, U+005A LATIN CAPITAL LETTER Z */
                    SFT_KEY(ROW8, BIT7);
                    break;
                case XK_bracketleft:  /* 0x005b, U+005B LEFT SQUARE BRACKET    */
                    SET_KEY(ROW2, BIT1);
                    break;
                case XK_backslash:    /* 0x005c, U+005C REVERSE SOLIDUS        */
                    SET_KEY(ROW2, BIT6);
                    break;
                case XK_bracketright: /* 0x005d, U+005D RIGHT SQUARE BRACKET   */
                    SET_KEY(ROW2, BIT3);
                    break;
                case XK_asciicircum:  /* 0x005e, U+005E CIRCUMFLEX ACCENT      */
                    SET_KEY(ROW3, BIT0);
                    break;
                case XK_underscore:   /* 0x005f, U+005F LOW LINE               */
                    SFT_KEY(ROW4, BIT0);
                    break;
                case XK_grave:        /* 0x0060, U+0060 GRAVE ACCENT           */
                    SFT_KEY(ROW2, BIT6);
                    break;
                case XK_a:            /* 0x0061, U+0061 LATIN SMALL LETTER A   */
                    SET_KEY(ROW8, BIT5);
                    break;
                case XK_b:            /* 0x0062, U+0062 LATIN SMALL LETTER B   */
                    SET_KEY(ROW6, BIT6);
                    break;
                case XK_c:            /* 0x0063, U+0063 LATIN SMALL LETTER C   */
                    SET_KEY(ROW7, BIT6);
                    break;
                case XK_d:            /* 0x0064, U+0064 LATIN SMALL LETTER D   */
                    SET_KEY(ROW7, BIT5);
                    break;
                case XK_e:            /* 0x0065, U+0065 LATIN SMALL LETTER E   */
                    SET_KEY(ROW7, BIT2);
                    break;
                case XK_f:            /* 0x0066, U+0066 LATIN SMALL LETTER F   */
                    SET_KEY(ROW6, BIT5);
                    break;
                case XK_g:            /* 0x0067, U+0067 LATIN SMALL LETTER G   */
                    SET_KEY(ROW6, BIT4);
                    break;
                case XK_h:            /* 0x0068, U+0068 LATIN SMALL LETTER H   */
                    SET_KEY(ROW5, BIT4);
                    break;
                case XK_i:            /* 0x0069, U+0069 LATIN SMALL LETTER I   */
                    SET_KEY(ROW4, BIT3);
                    break;
                case XK_j:            /* 0x006a, U+006A LATIN SMALL LETTER J   */
                    SET_KEY(ROW5, BIT5);
                    break;
                case XK_k:            /* 0x006b, U+006B LATIN SMALL LETTER K   */
                    SET_KEY(ROW4, BIT5);
                    break;
                case XK_l:            /* 0x006c, U+006C LATIN SMALL LETTER L   */
                    SET_KEY(ROW4, BIT4);
                    break;
                case XK_m:            /* 0x006d, U+006D LATIN SMALL LETTER M   */
                    SET_KEY(ROW4, BIT6);
                    break;
                case XK_n:            /* 0x006e, U+006E LATIN SMALL LETTER N   */
                    SET_KEY(ROW5, BIT6);
                    break;
                case XK_o:            /* 0x006f, U+006F LATIN SMALL LETTER O   */
                    SET_KEY(ROW4, BIT2);
                    break;
                case XK_p:            /* 0x0070, U+0070 LATIN SMALL LETTER P   */
                    SET_KEY(ROW3, BIT3);
                    break;
                case XK_q:            /* 0x0071, U+0071 LATIN SMALL LETTER Q   */
                    SET_KEY(ROW8, BIT3);
                    break;
                case XK_r:            /* 0x0072, U+0072 LATIN SMALL LETTER R   */
                    SET_KEY(ROW6, BIT2);
                    break;
                case XK_s:            /* 0x0073, U+0073 LATIN SMALL LETTER S   */
                    SET_KEY(ROW7, BIT4);
                    break;
                case XK_t:            /* 0x0074, U+0074 LATIN SMALL LETTER T   */
                    SET_KEY(ROW6, BIT3);
                    break;
                case XK_u:            /* 0x0075, U+0075 LATIN SMALL LETTER U   */
                    SET_KEY(ROW5, BIT2);
                    break;
                case XK_v:            /* 0x0076, U+0076 LATIN SMALL LETTER V   */
                    SET_KEY(ROW6, BIT7);
                    break;
                case XK_w:            /* 0x0077, U+0077 LATIN SMALL LETTER W   */
                    SET_KEY(ROW7, BIT3);
                    break;
                case XK_x:            /* 0x0078, U+0078 LATIN SMALL LETTER X   */
                    SET_KEY(ROW7, BIT7);
                    break;
                case XK_y:            /* 0x0079, U+0079 LATIN SMALL LETTER Y   */
                    SET_KEY(ROW5, BIT3);
                    break;
                case XK_z:            /* 0x007a, U+007A LATIN SMALL LETTER Z   */
                    SET_KEY(ROW8, BIT7);
                    break;
                case XK_braceleft:    /* 0x007b, U+007B LEFT CURLY BRACKET     */
                    SFT_KEY(ROW2, BIT1);
                    break;
                case XK_bar:          /* 0x007c, U+007C VERTICAL LINE          */
                    SFT_KEY(ROW3, BIT2);
                    break;
                case XK_braceright:   /* 0x007d, U+007D RIGHT CURLY BRACKET    */
                    SFT_KEY(ROW2, BIT3);
                    break;
                case XK_asciitilde:   /* 0x007e, U+007E TILDE                  */
                    EXT_KEY(ROW8, BIT1);
                    break;
                case XK_sterling:     /* 0x00a3, U+00A3 POUND SIGN             */
                    SFT_KEY(ROW3, BIT0);
                    break;
                default:
                    break;
            }
        }
        else if(keypag == 0xff) {
            switch(keysym) {
                case XK_End:
                    if(event->type == KeyPress) {
                        if(self->state.mode == MODE_STANDARD) {
                            self->state.mode = MODE_JOYSTICK;
                        }
                        else {
                            self->state.mode = MODE_STANDARD;
                        }
                    }
                    break;
                case XK_BackSpace:
                    SET_KEY(ROW9, BIT7);
                    break;
                case XK_Tab:
                    SET_KEY(ROW8, BIT4);
                    break;
                case XK_Return:
                    SET_KEY(ROW2, BIT2);
                    break;
                case XK_Escape:
                    SET_KEY(ROW8, BIT2);
                    break;
                case XK_Delete:
                    SET_KEY(ROW2, BIT0);
                    break;
                case XK_Left:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT2);
                    }
                    else {
                        SET_KEY(ROW1, BIT0);
                    }
                    break;
                case XK_Up:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT0);
                    }
                    else {
                        SET_KEY(ROW0, BIT0);
                    }
                    break;
                case XK_Right:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT3);
                    }
                    else {
                        SET_KEY(ROW0, BIT1);
                    }
                    break;
                case XK_Down:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT1);
                    }
                    else {
                        SET_KEY(ROW0, BIT2);
                    }
                    break;
                case XK_KP_Space:
                    SET_KEY(ROW5, BIT7);
                    break;
                case XK_KP_Tab:
                    SET_KEY(ROW8, BIT4);
                    break;
                case XK_KP_Enter:
                    SET_KEY(ROW0, BIT6);
                    break;
                case XK_KP_Left:
                    SET_KEY(ROW9, BIT2);
                    break;
                case XK_KP_Up:
                    SET_KEY(ROW9, BIT0);
                    break;
                case XK_KP_Right:
                    SET_KEY(ROW9, BIT3);
                    break;
                case XK_KP_Down:
                    SET_KEY(ROW9, BIT1);
                    break;
                case XK_KP_Insert:
                    SET_KEY(ROW9, BIT5);
                    break;
                case XK_KP_Delete:
                    SET_KEY(ROW9, BIT4);
                    break;
                case XK_KP_Decimal:
                    SET_KEY(ROW0, BIT7);
                    break;
                case XK_KP_0:
                    SET_KEY(ROW1, BIT7);
                    break;
                case XK_KP_1:
                    SET_KEY(ROW1, BIT5);
                    break;
                case XK_KP_2:
                    SET_KEY(ROW1, BIT6);
                    break;
                case XK_KP_3:
                    SET_KEY(ROW0, BIT5);
                    break;
                case XK_KP_4:
                    SET_KEY(ROW2, BIT4);
                    break;
                case XK_KP_5:
                    SET_KEY(ROW1, BIT4);
                    break;
                case XK_KP_6:
                    SET_KEY(ROW0, BIT4);
                    break;
                case XK_KP_7:
                    SET_KEY(ROW1, BIT2);
                    break;
                case XK_KP_8:
                    SET_KEY(ROW1, BIT3);
                    break;
                case XK_KP_9:
                    SET_KEY(ROW0, BIT3);
                    break;
                case XK_Shift_L:
                    SET_KEY(ROW2, BIT5);
                    break;
                case XK_Control_L:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT4);
                    }
                    else {
                        SET_KEY(ROW2, BIT7);
                    }
                    break;
                case XK_Alt_L:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT5);
                    }
                    else {
                        SET_KEY(ROW1, BIT1);
                    }
                    break;
                case XK_Caps_Lock:
                    SET_KEY(ROW8, BIT6);
                    break;
                default:
                    break;
            }
        }
    }
    /* update state */ {
        if((line <= 0x0f) && (bits != 0x00)) {
            if(event->type == KeyPress) {
                if(mods & BIT0) {
                    self->state.keys[ROW2] &= ~BIT5;
                }
                if(mods & BIT1) {
                    self->state.keys[ROW2] &= ~BIT7;
                }
                self->state.keys[line] &= ~bits;
                self->state.keys[0x0f]  = ~mods;
            }
            if(event->type == KeyRelease) {
                if(mods & BIT0) {
                    self->state.keys[ROW2] |=  BIT5;
                }
                if(mods & BIT1) {
                    self->state.keys[ROW2] |=  BIT7;
                }
                self->state.keys[line] |=  bits;
                self->state.keys[0x0f]  = ~mods;
            }
        }
    }
    return self;
}

XcpcKeyboard* xcpc_keyboard_azerty(XcpcKeyboard* self, XKeyEvent* event)
{
    KeySym keysym = NoSymbol;
    KeySym keypag = NoSymbol;
    uint8_t line  = 0xff;
    uint8_t bits  = 0x00;
    uint8_t mods  = ~(self->state.keys[0x0f]);

    /* check key */ {
        char   buffer[16] = { '\0' };
        size_t buflen = sizeof(buffer);
        buflen = XLookupString(event, buffer, buflen, &keysym, NULL);
        keypag = (keysym >> 8);
        if(keypag == 0x00) {
            switch(keysym) {
                case XK_space:        /* 0x0020, U+0020 SPACE                  */
                    SET_KEY(ROW5, BIT7);
                    break;
                case XK_exclam:       /* 0x0021, U+0021 EXCLAMATION MARK       */
                    SFT_KEY(ROW8, BIT0);
                    break;
                case XK_quotedbl:     /* 0x0022, U+0022 QUOTATION MARK         */
                    SFT_KEY(ROW8, BIT1);
                    break;
                case XK_numbersign:   /* 0x0023, U+0023 NUMBER SIGN            */
                    SFT_KEY(ROW7, BIT1);
                    break;
                case XK_dollar:       /* 0x0024, U+0024 DOLLAR SIGN            */
                    SFT_KEY(ROW7, BIT0);
                    break;
                case XK_percent:      /* 0x0025, U+0025 PERCENT SIGN           */
                    SFT_KEY(ROW6, BIT1);
                    break;
                case XK_ampersand:    /* 0x0026, U+0026 AMPERSAND              */
                    SFT_KEY(ROW6, BIT0);
                    break;
                case XK_apostrophe:   /* 0x0027, U+0027 APOSTROPHE             */
                    SFT_KEY(ROW5, BIT1);
                    break;
                case XK_parenleft:    /* 0x0028, U+0028 LEFT PARENTHESIS       */
                    SFT_KEY(ROW5, BIT0);
                    break;
                case XK_parenright:   /* 0x0029, U+0029 RIGHT PARENTHESIS      */
                    SFT_KEY(ROW4, BIT1);
                    break;
                case XK_asterisk:     /* 0x002a, U+002A ASTERISK               */
                    SFT_KEY(ROW3, BIT5);
                    break;
                case XK_plus:         /* 0x002b, U+002B PLUS SIGN              */
                    SFT_KEY(ROW3, BIT4);
                    break;
                case XK_comma:        /* 0x002c, U+002C COMMA                  */
                    SET_KEY(ROW4, BIT7);
                    break;
                case XK_minus:        /* 0x002d, U+002D HYPHEN-MINUS           */
                    SET_KEY(ROW3, BIT1);
                    break;
                case XK_period:       /* 0x002e, U+002E FULL STOP              */
                    SET_KEY(ROW3, BIT7);
                    break;
                case XK_slash:        /* 0x002f, U+002F SOLIDUS                */
                    SET_KEY(ROW3, BIT6);
                    break;
                case XK_0:            /* 0x0030, U+0030 DIGIT ZERO             */
                    SET_KEY(ROW4, BIT0);
                    break;
                case XK_1:            /* 0x0031, U+0031 DIGIT ONE              */
                    SET_KEY(ROW8, BIT0);
                    break;
                case XK_2:            /* 0x0032, U+0032 DIGIT TWO              */
                    SET_KEY(ROW8, BIT1);
                    break;
                case XK_3:            /* 0x0033, U+0033 DIGIT THREE            */
                    SET_KEY(ROW7, BIT1);
                    break;
                case XK_4:            /* 0x0034, U+0034 DIGIT FOUR             */
                    SET_KEY(ROW7, BIT0);
                    break;
                case XK_5:            /* 0x0035, U+0035 DIGIT FIVE             */
                    SET_KEY(ROW6, BIT1);
                    break;
                case XK_6:            /* 0x0036, U+0036 DIGIT SIX              */
                    SET_KEY(ROW6, BIT0);
                    break;
                case XK_7:            /* 0x0037, U+0037 DIGIT SEVEN            */
                    SET_KEY(ROW5, BIT1);
                    break;
                case XK_8:            /* 0x0038, U+0038 DIGIT EIGHT            */
                    SET_KEY(ROW5, BIT0);
                    break;
                case XK_9:            /* 0x0039, U+0039 DIGIT NINE             */
                    SET_KEY(ROW4, BIT1);
                    break;
                case XK_colon:        /* 0x003a, U+003A COLON                  */
                    SET_KEY(ROW3, BIT5);
                    break;
                case XK_semicolon:    /* 0x003b, U+003B SEMICOLON              */
                    SET_KEY(ROW3, BIT4);
                    break;
                case XK_less:         /* 0x003c, U+003C LESS-THAN SIGN         */
                    SFT_KEY(ROW4, BIT7);
                    break;
                case XK_equal:        /* 0x003d, U+003D EQUALS SIGN            */
                    SFT_KEY(ROW3, BIT1);
                    break;
                case XK_greater:      /* 0x003e, U+003E GREATER-THAN SIGN      */
                    SFT_KEY(ROW3, BIT7);
                    break;
                case XK_question:     /* 0x003f, U+003F QUESTION MARK          */
                    SFT_KEY(ROW3, BIT6);
                    break;
                case XK_at:           /* 0x0040, U+0040 COMMERCIAL AT          */
                    SET_KEY(ROW3, BIT2);
                    break;
                case XK_A:            /* 0x0041, U+0041 LATIN CAPITAL LETTER A */
                    SFT_KEY(ROW8, BIT5);
                    break;
                case XK_B:            /* 0x0042, U+0042 LATIN CAPITAL LETTER B */
                    SFT_KEY(ROW6, BIT6);
                    break;
                case XK_C:            /* 0x0043, U+0043 LATIN CAPITAL LETTER C */
                    SFT_KEY(ROW7, BIT6);
                    break;
                case XK_D:            /* 0x0044, U+0044 LATIN CAPITAL LETTER D */
                    SFT_KEY(ROW7, BIT5);
                    break;
                case XK_E:            /* 0x0045, U+0045 LATIN CAPITAL LETTER E */
                    SFT_KEY(ROW7, BIT2);
                    break;
                case XK_F:            /* 0x0046, U+0046 LATIN CAPITAL LETTER F */
                    SFT_KEY(ROW6, BIT5);
                    break;
                case XK_G:            /* 0x0047, U+0047 LATIN CAPITAL LETTER G */
                    SFT_KEY(ROW6, BIT4);
                    break;
                case XK_H:            /* 0x0048, U+0048 LATIN CAPITAL LETTER H */
                    SFT_KEY(ROW5, BIT4);
                    break;
                case XK_I:            /* 0x0049, U+0049 LATIN CAPITAL LETTER I */
                    SFT_KEY(ROW4, BIT3);
                    break;
                case XK_J:            /* 0x004a, U+004A LATIN CAPITAL LETTER J */
                    SFT_KEY(ROW5, BIT5);
                    break;
                case XK_K:            /* 0x004b, U+004B LATIN CAPITAL LETTER K */
                    SFT_KEY(ROW4, BIT5);
                    break;
                case XK_L:            /* 0x004c, U+004C LATIN CAPITAL LETTER L */
                    SFT_KEY(ROW4, BIT4);
                    break;
                case XK_M:            /* 0x004d, U+004D LATIN CAPITAL LETTER M */
                    SFT_KEY(ROW4, BIT6);
                    break;
                case XK_N:            /* 0x004e, U+004E LATIN CAPITAL LETTER N */
                    SFT_KEY(ROW5, BIT6);
                    break;
                case XK_O:            /* 0x004f, U+004F LATIN CAPITAL LETTER O */
                    SFT_KEY(ROW4, BIT2);
                    break;
                case XK_P:            /* 0x0050, U+0050 LATIN CAPITAL LETTER P */
                    SFT_KEY(ROW3, BIT3);
                    break;
                case XK_Q:            /* 0x0051, U+0051 LATIN CAPITAL LETTER Q */
                    SFT_KEY(ROW8, BIT3);
                    break;
                case XK_R:            /* 0x0052, U+0052 LATIN CAPITAL LETTER R */
                    SFT_KEY(ROW6, BIT2);
                    break;
                case XK_S:            /* 0x0053, U+0053 LATIN CAPITAL LETTER S */
                    SFT_KEY(ROW7, BIT4);
                    break;
                case XK_T:            /* 0x0054, U+0054 LATIN CAPITAL LETTER T */
                    SFT_KEY(ROW6, BIT3);
                    break;
                case XK_U:            /* 0x0055, U+0055 LATIN CAPITAL LETTER U */
                    SFT_KEY(ROW5, BIT2);
                    break;
                case XK_V:            /* 0x0056, U+0056 LATIN CAPITAL LETTER V */
                    SFT_KEY(ROW6, BIT7);
                    break;
                case XK_W:            /* 0x0057, U+0057 LATIN CAPITAL LETTER W */
                    SFT_KEY(ROW7, BIT3);
                    break;
                case XK_X:            /* 0x0058, U+0058 LATIN CAPITAL LETTER X */
                    SFT_KEY(ROW7, BIT7);
                    break;
                case XK_Y:            /* 0x0059, U+0059 LATIN CAPITAL LETTER Y */
                    SFT_KEY(ROW5, BIT3);
                    break;
                case XK_Z:            /* 0x005a, U+005A LATIN CAPITAL LETTER Z */
                    SFT_KEY(ROW8, BIT7);
                    break;
                case XK_bracketleft:  /* 0x005b, U+005B LEFT SQUARE BRACKET    */
                    SET_KEY(ROW2, BIT1);
                    break;
                case XK_backslash:    /* 0x005c, U+005C REVERSE SOLIDUS        */
                    SET_KEY(ROW2, BIT6);
                    break;
                case XK_bracketright: /* 0x005d, U+005D RIGHT SQUARE BRACKET   */
                    SET_KEY(ROW2, BIT3);
                    break;
                case XK_asciicircum:  /* 0x005e, U+005E CIRCUMFLEX ACCENT      */
                    SET_KEY(ROW3, BIT0);
                    break;
                case XK_underscore:   /* 0x005f, U+005F LOW LINE               */
                    SFT_KEY(ROW4, BIT0);
                    break;
                case XK_grave:        /* 0x0060, U+0060 GRAVE ACCENT           */
                    SFT_KEY(ROW2, BIT6);
                    break;
                case XK_a:            /* 0x0061, U+0061 LATIN SMALL LETTER A   */
                    SET_KEY(ROW8, BIT5);
                    break;
                case XK_b:            /* 0x0062, U+0062 LATIN SMALL LETTER B   */
                    SET_KEY(ROW6, BIT6);
                    break;
                case XK_c:            /* 0x0063, U+0063 LATIN SMALL LETTER C   */
                    SET_KEY(ROW7, BIT6);
                    break;
                case XK_d:            /* 0x0064, U+0064 LATIN SMALL LETTER D   */
                    SET_KEY(ROW7, BIT5);
                    break;
                case XK_e:            /* 0x0065, U+0065 LATIN SMALL LETTER E   */
                    SET_KEY(ROW7, BIT2);
                    break;
                case XK_f:            /* 0x0066, U+0066 LATIN SMALL LETTER F   */
                    SET_KEY(ROW6, BIT5);
                    break;
                case XK_g:            /* 0x0067, U+0067 LATIN SMALL LETTER G   */
                    SET_KEY(ROW6, BIT4);
                    break;
                case XK_h:            /* 0x0068, U+0068 LATIN SMALL LETTER H   */
                    SET_KEY(ROW5, BIT4);
                    break;
                case XK_i:            /* 0x0069, U+0069 LATIN SMALL LETTER I   */
                    SET_KEY(ROW4, BIT3);
                    break;
                case XK_j:            /* 0x006a, U+006A LATIN SMALL LETTER J   */
                    SET_KEY(ROW5, BIT5);
                    break;
                case XK_k:            /* 0x006b, U+006B LATIN SMALL LETTER K   */
                    SET_KEY(ROW4, BIT5);
                    break;
                case XK_l:            /* 0x006c, U+006C LATIN SMALL LETTER L   */
                    SET_KEY(ROW4, BIT4);
                    break;
                case XK_m:            /* 0x006d, U+006D LATIN SMALL LETTER M   */
                    SET_KEY(ROW4, BIT6);
                    break;
                case XK_n:            /* 0x006e, U+006E LATIN SMALL LETTER N   */
                    SET_KEY(ROW5, BIT6);
                    break;
                case XK_o:            /* 0x006f, U+006F LATIN SMALL LETTER O   */
                    SET_KEY(ROW4, BIT2);
                    break;
                case XK_p:            /* 0x0070, U+0070 LATIN SMALL LETTER P   */
                    SET_KEY(ROW3, BIT3);
                    break;
                case XK_q:            /* 0x0071, U+0071 LATIN SMALL LETTER Q   */
                    SET_KEY(ROW8, BIT3);
                    break;
                case XK_r:            /* 0x0072, U+0072 LATIN SMALL LETTER R   */
                    SET_KEY(ROW6, BIT2);
                    break;
                case XK_s:            /* 0x0073, U+0073 LATIN SMALL LETTER S   */
                    SET_KEY(ROW7, BIT4);
                    break;
                case XK_t:            /* 0x0074, U+0074 LATIN SMALL LETTER T   */
                    SET_KEY(ROW6, BIT3);
                    break;
                case XK_u:            /* 0x0075, U+0075 LATIN SMALL LETTER U   */
                    SET_KEY(ROW5, BIT2);
                    break;
                case XK_v:            /* 0x0076, U+0076 LATIN SMALL LETTER V   */
                    SET_KEY(ROW6, BIT7);
                    break;
                case XK_w:            /* 0x0077, U+0077 LATIN SMALL LETTER W   */
                    SET_KEY(ROW7, BIT3);
                    break;
                case XK_x:            /* 0x0078, U+0078 LATIN SMALL LETTER X   */
                    SET_KEY(ROW7, BIT7);
                    break;
                case XK_y:            /* 0x0079, U+0079 LATIN SMALL LETTER Y   */
                    SET_KEY(ROW5, BIT3);
                    break;
                case XK_z:            /* 0x007a, U+007A LATIN SMALL LETTER Z   */
                    SET_KEY(ROW8, BIT7);
                    break;
                case XK_braceleft:    /* 0x007b, U+007B LEFT CURLY BRACKET     */
                    SFT_KEY(ROW2, BIT1);
                    break;
                case XK_bar:          /* 0x007c, U+007C VERTICAL LINE          */
                    SFT_KEY(ROW3, BIT2);
                    break;
                case XK_braceright:   /* 0x007d, U+007D RIGHT CURLY BRACKET    */
                    SFT_KEY(ROW2, BIT3);
                    break;
                case XK_asciitilde:   /* 0x007e, U+007E TILDE                  */
                    EXT_KEY(ROW8, BIT1);
                    break;
                case XK_sterling:     /* 0x00a3, U+00A3 POUND SIGN             */
                    SFT_KEY(ROW3, BIT0);
                    break;
                default:
                    break;
            }
        }
        else if(keypag == 0xff) {
            switch(keysym) {
                case XK_End:
                    if(event->type == KeyPress) {
                        if(self->state.mode == MODE_STANDARD) {
                            self->state.mode = MODE_JOYSTICK;
                        }
                        else {
                            self->state.mode = MODE_STANDARD;
                        }
                    }
                    break;
                case XK_BackSpace:
                    SET_KEY(ROW9, BIT7);
                    break;
                case XK_Tab:
                    SET_KEY(ROW8, BIT4);
                    break;
                case XK_Return:
                    SET_KEY(ROW2, BIT2);
                    break;
                case XK_Escape:
                    SET_KEY(ROW8, BIT2);
                    break;
                case XK_Delete:
                    SET_KEY(ROW2, BIT0);
                    break;
                case XK_Left:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT2);
                    }
                    else {
                        SET_KEY(ROW1, BIT0);
                    }
                    break;
                case XK_Up:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT0);
                    }
                    else {
                        SET_KEY(ROW0, BIT0);
                    }
                    break;
                case XK_Right:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT3);
                    }
                    else {
                        SET_KEY(ROW0, BIT1);
                    }
                    break;
                case XK_Down:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT1);
                    }
                    else {
                        SET_KEY(ROW0, BIT2);
                    }
                    break;
                case XK_KP_Space:
                    SET_KEY(ROW5, BIT7);
                    break;
                case XK_KP_Tab:
                    SET_KEY(ROW8, BIT4);
                    break;
                case XK_KP_Enter:
                    SET_KEY(ROW0, BIT6);
                    break;
                case XK_KP_Left:
                    SET_KEY(ROW9, BIT2);
                    break;
                case XK_KP_Up:
                    SET_KEY(ROW9, BIT0);
                    break;
                case XK_KP_Right:
                    SET_KEY(ROW9, BIT3);
                    break;
                case XK_KP_Down:
                    SET_KEY(ROW9, BIT1);
                    break;
                case XK_KP_Insert:
                    SET_KEY(ROW9, BIT5);
                    break;
                case XK_KP_Delete:
                    SET_KEY(ROW9, BIT4);
                    break;
                case XK_KP_Decimal:
                    SET_KEY(ROW0, BIT7);
                    break;
                case XK_KP_0:
                    SET_KEY(ROW1, BIT7);
                    break;
                case XK_KP_1:
                    SET_KEY(ROW1, BIT5);
                    break;
                case XK_KP_2:
                    SET_KEY(ROW1, BIT6);
                    break;
                case XK_KP_3:
                    SET_KEY(ROW0, BIT5);
                    break;
                case XK_KP_4:
                    SET_KEY(ROW2, BIT4);
                    break;
                case XK_KP_5:
                    SET_KEY(ROW1, BIT4);
                    break;
                case XK_KP_6:
                    SET_KEY(ROW0, BIT4);
                    break;
                case XK_KP_7:
                    SET_KEY(ROW1, BIT2);
                    break;
                case XK_KP_8:
                    SET_KEY(ROW1, BIT3);
                    break;
                case XK_KP_9:
                    SET_KEY(ROW0, BIT3);
                    break;
                case XK_Shift_L:
                    SET_KEY(ROW2, BIT5);
                    break;
                case XK_Control_L:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT4);
                    }
                    else {
                        SET_KEY(ROW2, BIT7);
                    }
                    break;
                case XK_Alt_L:
                    if(self->state.mode == MODE_JOYSTICK) {
                        SET_KEY(ROW9, BIT5);
                    }
                    else {
                        SET_KEY(ROW1, BIT1);
                    }
                    break;
                case XK_Caps_Lock:
                    SET_KEY(ROW8, BIT6);
                    break;
                default:
                    break;
            }
        }
    }
    /* update state */ {
        if((line <= 0x0f) && (bits != 0x00)) {
            if(event->type == KeyPress) {
                if(mods & BIT0) {
                    self->state.keys[ROW2] &= ~BIT5;
                }
                if(mods & BIT1) {
                    self->state.keys[ROW2] &= ~BIT7;
                }
                self->state.keys[line] &= ~bits;
                self->state.keys[0x0f]  = ~mods;
            }
            if(event->type == KeyRelease) {
                if(mods & BIT0) {
                    self->state.keys[ROW2] |=  BIT5;
                }
                if(mods & BIT1) {
                    self->state.keys[ROW2] |=  BIT7;
                }
                self->state.keys[line] |=  bits;
                self->state.keys[0x0f]  = ~mods;
            }
        }
    }
    return self;
}
