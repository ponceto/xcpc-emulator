/*
 * kbd-core.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <cstdint>
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <xcpc/libxcpc-priv.h>
#include "kbd-core.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = kbd::Type;
    using State     = kbd::State;
    using Instance  = kbd::Instance;
    using Interface = kbd::Interface;

    static const unsigned int Joystick0Modifier = (AnyModifier << 1);
    static const unsigned int Joystick1Modifier = (AnyModifier << 2);

    static constexpr uint8_t ROW0 = 0x00;
    static constexpr uint8_t ROW1 = 0x01;
    static constexpr uint8_t ROW2 = 0x02;
    static constexpr uint8_t ROW3 = 0x03;
    static constexpr uint8_t ROW4 = 0x04;
    static constexpr uint8_t ROW5 = 0x05;
    static constexpr uint8_t ROW6 = 0x06;
    static constexpr uint8_t ROW7 = 0x07;
    static constexpr uint8_t ROW8 = 0x08;
    static constexpr uint8_t ROW9 = 0x09;

    static constexpr uint8_t BIT0 = 0x01;
    static constexpr uint8_t BIT1 = 0x02;
    static constexpr uint8_t BIT2 = 0x04;
    static constexpr uint8_t BIT3 = 0x08;
    static constexpr uint8_t BIT4 = 0x10;
    static constexpr uint8_t BIT5 = 0x20;
    static constexpr uint8_t BIT6 = 0x40;
    static constexpr uint8_t BIT7 = 0x80;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::StateTraits
// ---------------------------------------------------------------------------

namespace {

struct StateTraits final
    : public BasicTraits
{
    static inline auto construct(State& state, const Type type) -> void
    {
        state.type = type;
    }

    static inline auto destruct(State& state) -> void
    {
        state.type = Type::TYPE_INVALID;
    }

    static inline auto reset(State& state) -> void
    {
        state.mode &= 0;
        state.line &= 0;
        for(auto& keys : state.keys) {
            keys |= 0xff;
        }
    }

    static inline auto clock(State& state) -> void
    {
    }

    static inline auto set_type(State& state, const Type type) -> void
    {
        state.type = type;
    }

    static inline auto set_line(State& state, uint8_t line) -> uint8_t
    {
        return state.line = line;
    }

    static inline auto get_data(State& state, uint8_t data) -> uint8_t
    {
        if(state.line < 16) {
            data = state.keys[state.line];
        }
        else {
            data = 0xff;
        }
        return data;
    }

    static inline auto decode_english(State& state, const XKeyEvent& event, uint8_t& line, uint8_t& data, uint8_t& mods) -> void
    {
        char   buffer[16] = { '\0' };
        size_t buflen = sizeof(buffer);
        KeySym keysym = NoSymbol;
        KeySym keypag = NoSymbol;

        auto SET_KEY = [&](const uint8_t kbd_line, const uint8_t kbd_data) -> void
        {
            line = kbd_line;
            data = kbd_data;
            mods = ((mods & ~0x03) | 0x00);
        };

        auto SFT_KEY = [&](const uint8_t kbd_line, const uint8_t kbd_data) -> void
        {
            line = kbd_line;
            data = kbd_data;
            mods = ((mods & ~0x03) | 0x01);
        };

        auto EXT_KEY = [&](const uint8_t kbd_line, const uint8_t kbd_data) -> void
        {
            line = kbd_line;
            data = kbd_data;
            mods = ((mods & ~0x03) | 0x03);
        };

        buflen = ::XLookupString(const_cast<XKeyEvent*>(&event), buffer, buflen, &keysym, nullptr);
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
        else if(keypag == 0xfe) {
            switch(keysym) {
                case XK_dead_grave:      /* 0xfe50, U+0060 GRAVE ACCENT           */
                    SFT_KEY(ROW2, BIT6);
                    break;
                case XK_dead_circumflex: /* 0xfe52, U+005E CIRCUMFLEX ACCENT      */
                    SET_KEY(ROW3, BIT0);
                    break;
                case XK_dead_tilde:      /* 0xfe53, U+007E TILDE                  */
                    EXT_KEY(ROW8, BIT1);
                    break;
                default:
                    break;
            }
        }
        else if(keypag == 0xff) {
            switch(keysym) {
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
                    SET_KEY(ROW1, BIT0);
                    break;
                case XK_Up:
                    SET_KEY(ROW0, BIT0);
                    break;
                case XK_Right:
                    SET_KEY(ROW0, BIT1);
                    break;
                case XK_Down:
                    SET_KEY(ROW0, BIT2);
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
                    SET_KEY(ROW2, BIT7);
                    break;
                case XK_Alt_L:
                    SET_KEY(ROW1, BIT1);
                    break;
                case XK_Caps_Lock:
                    SET_KEY(ROW8, BIT6);
                    break;
                default:
                    break;
            }
        }
    }

    static inline auto decode_french(State& state, const XKeyEvent& event, uint8_t& line, uint8_t& data, uint8_t& mods) -> void
    {
        char   buffer[16] = { '\0' };
        size_t buflen = sizeof(buffer);
        KeySym keysym = NoSymbol;
        KeySym keypag = NoSymbol;

        auto SET_KEY = [&](const uint8_t kbd_line, const uint8_t kbd_data) -> void
        {
            line = kbd_line;
            data = kbd_data;
            mods = ((mods & ~0x03) | 0x00);
        };

        auto SFT_KEY = [&](const uint8_t kbd_line, const uint8_t kbd_data) -> void
        {
            line = kbd_line;
            data = kbd_data;
            mods = ((mods & ~0x03) | 0x01);
        };

        auto EXT_KEY = [&](const uint8_t kbd_line, const uint8_t kbd_data) -> void
        {
            line = kbd_line;
            data = kbd_data;
            mods = ((mods & ~0x03) | 0x03);
        };

        buflen = ::XLookupString(const_cast<XKeyEvent*>(&event), buffer, buflen, &keysym, nullptr);
        keypag = (keysym >> 8);
        if(keypag == 0x00) {
            switch(keysym) {
                case XK_space:        /* 0x0020, U+0020 SPACE                  */
                    SET_KEY(ROW5, BIT7);
                    break;
                case XK_exclam:       /* 0x0021, U+0021 EXCLAMATION MARK       */
                    SET_KEY(ROW5, BIT0);
                    break;
                case XK_quotedbl:     /* 0x0022, U+0022 QUOTATION MARK         */
                    SET_KEY(ROW7, BIT1);
                    break;
                case XK_numbersign:   /* 0x0023, U+0023 NUMBER SIGN            */
                    SET_KEY(ROW2, BIT3);
                    break;
                case XK_dollar:       /* 0x0024, U+0024 DOLLAR SIGN            */
                    SET_KEY(ROW2, BIT6);
                    break;
                case XK_percent:      /* 0x0025, U+0025 PERCENT SIGN           */
                    SFT_KEY(ROW3, BIT4);
                    break;
                case XK_ampersand:    /* 0x0026, U+0026 AMPERSAND              */
                    SET_KEY(ROW8, BIT0);
                    break;
                case XK_apostrophe:   /* 0x0027, U+0027 APOSTROPHE             */
                    SET_KEY(ROW7, BIT0);
                    break;
                case XK_parenleft:    /* 0x0028, U+0028 LEFT PARENTHESIS       */
                    SET_KEY(ROW6, BIT1);
                    break;
                case XK_parenright:   /* 0x0029, U+0029 RIGHT PARENTHESIS      */
                    SET_KEY(ROW3, BIT1);
                    break;
                case XK_asterisk:     /* 0x002a, U+002A ASTERISK               */
                    SET_KEY(ROW2, BIT1);
                    break;
                case XK_plus:         /* 0x002b, U+002B PLUS SIGN              */
                    SFT_KEY(ROW3, BIT6);
                    break;
                case XK_comma:        /* 0x002c, U+002C COMMA                  */
                    SET_KEY(ROW4, BIT6);
                    break;
                case XK_minus:        /* 0x002d, U+002D HYPHEN-MINUS           */
                    SET_KEY(ROW3, BIT0);
                    break;
                case XK_period:       /* 0x002e, U+002E FULL STOP              */
                    SFT_KEY(ROW4, BIT7);
                    break;
                case XK_slash:        /* 0x002f, U+002F SOLIDUS                */
                    SFT_KEY(ROW3, BIT7);
                    break;
                case XK_0:            /* 0x0030, U+0030 DIGIT ZERO             */
                    SFT_KEY(ROW4, BIT0);
                    break;
                case XK_1:            /* 0x0031, U+0031 DIGIT ONE              */
                    SFT_KEY(ROW8, BIT0);
                    break;
                case XK_2:            /* 0x0032, U+0032 DIGIT TWO              */
                    SFT_KEY(ROW8, BIT1);
                    break;
                case XK_3:            /* 0x0033, U+0033 DIGIT THREE            */
                    SFT_KEY(ROW7, BIT1);
                    break;
                case XK_4:            /* 0x0034, U+0034 DIGIT FOUR             */
                    SFT_KEY(ROW7, BIT0);
                    break;
                case XK_5:            /* 0x0035, U+0035 DIGIT FIVE             */
                    SFT_KEY(ROW6, BIT1);
                    break;
                case XK_6:            /* 0x0036, U+0036 DIGIT SIX              */
                    SFT_KEY(ROW6, BIT0);
                    break;
                case XK_7:            /* 0x0037, U+0037 DIGIT SEVEN            */
                    SFT_KEY(ROW5, BIT1);
                    break;
                case XK_8:            /* 0x0038, U+0038 DIGIT EIGHT            */
                    SFT_KEY(ROW5, BIT0);
                    break;
                case XK_9:            /* 0x0039, U+0039 DIGIT NINE             */
                    SFT_KEY(ROW4, BIT1);
                    break;
                case XK_colon:        /* 0x003a, U+003A COLON                  */
                    SET_KEY(ROW3, BIT7);
                    break;
                case XK_semicolon:    /* 0x003b, U+003B SEMICOLON              */
                    SET_KEY(ROW4, BIT7);
                    break;
                case XK_less:         /* 0x003c, U+003C LESS-THAN SIGN         */
                    SFT_KEY(ROW2, BIT1);
                    break;
                case XK_equal:        /* 0x003d, U+003D EQUALS SIGN            */
                    SET_KEY(ROW3, BIT6);
                    break;
                case XK_greater:      /* 0x003e, U+003E GREATER-THAN SIGN      */
                    SFT_KEY(ROW2, BIT3);
                    break;
                case XK_question:     /* 0x003f, U+003F QUESTION MARK          */
                    SFT_KEY(ROW4, BIT6);
                    break;
                case XK_at:           /* 0x0040, U+0040 COMMERCIAL AT          */
                    SFT_KEY(ROW2, BIT6);
                    break;
                case XK_A:            /* 0x0041, U+0041 LATIN CAPITAL LETTER A */
                    SFT_KEY(ROW8, BIT3);
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
                    SFT_KEY(ROW3, BIT5);
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
                    SFT_KEY(ROW8, BIT5);
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
                    SFT_KEY(ROW8, BIT7);
                    break;
                case XK_X:            /* 0x0058, U+0058 LATIN CAPITAL LETTER X */
                    SFT_KEY(ROW7, BIT7);
                    break;
                case XK_Y:            /* 0x0059, U+0059 LATIN CAPITAL LETTER Y */
                    SFT_KEY(ROW5, BIT3);
                    break;
                case XK_Z:            /* 0x005a, U+005A LATIN CAPITAL LETTER Z */
                    SFT_KEY(ROW7, BIT3);
                    break;
                case XK_bracketleft:  /* 0x005b, U+005B LEFT SQUARE BRACKET    */
                    SFT_KEY(ROW3, BIT1);
                    break;
                case XK_backslash:    /* 0x005c, U+005C REVERSE SOLIDUS        */
                    EXT_KEY(ROW2, BIT6);
                    break;
                case XK_bracketright: /* 0x005d, U+005D RIGHT SQUARE BRACKET   */
                    SET_KEY(ROW6, BIT0);
                    break;
                case XK_asciicircum:  /* 0x005e, U+005E CIRCUMFLEX ACCENT      */
                    SET_KEY(ROW3, BIT2);
                    break;
                case XK_underscore:   /* 0x005f, U+005F LOW LINE               */
                    SFT_KEY(ROW3, BIT0);
                    break;
                case XK_grave:        /* 0x0060, U+0060 GRAVE ACCENT           */
                    SET_KEY(ROW5, BIT1);
                    break;
                case XK_a:            /* 0x0061, U+0061 LATIN SMALL LETTER A   */
                    SET_KEY(ROW8, BIT3);
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
                    SET_KEY(ROW3, BIT5);
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
                    SET_KEY(ROW8, BIT5);
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
                    SET_KEY(ROW8, BIT7);
                    break;
                case XK_x:            /* 0x0078, U+0078 LATIN SMALL LETTER X   */
                    SET_KEY(ROW7, BIT7);
                    break;
                case XK_y:            /* 0x0079, U+0079 LATIN SMALL LETTER Y   */
                    SET_KEY(ROW5, BIT3);
                    break;
                case XK_z:            /* 0x007a, U+007A LATIN SMALL LETTER Z   */
                    SET_KEY(ROW7, BIT3);
                    break;
                case XK_braceleft:    /* 0x007b, U+007B LEFT CURLY BRACKET     */
                    SET_KEY(ROW8, BIT1);
                    break;
                case XK_bar:          /* 0x007c, U+007C VERTICAL LINE          */
                    SFT_KEY(ROW3, BIT2);
                    break;
                case XK_braceright:   /* 0x007d, U+007D RIGHT CURLY BRACKET    */
                    SET_KEY(ROW5, BIT1);
                    break;
                case XK_asciitilde:   /* 0x007e, U+007E TILDE                  */
                    EXT_KEY(ROW8, BIT1);
                    break;
                case XK_sterling:     /* 0x00a3, U+00A3 POUND SIGN             */
                //  SFT_KEY(ROW3, BIT0);
                    break;
                default:
                    break;
            }
        }
        else if(keypag == 0xfe) {
            switch(keysym) {
                case XK_dead_grave:      /* 0xfe50, U+0060 GRAVE ACCENT           */
                    SET_KEY(ROW5, BIT1);
                    break;
                case XK_dead_circumflex: /* 0xfe52, U+005E CIRCUMFLEX ACCENT      */
                    SET_KEY(ROW3, BIT2);
                    break;
                case XK_dead_tilde:      /* 0xfe53, U+007E TILDE                  */
                    EXT_KEY(ROW8, BIT1);
                    break;
                default:
                    break;
            }
        }
        else if(keypag == 0xff) {
            switch(keysym) {
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
                    SET_KEY(ROW1, BIT0);
                    break;
                case XK_Up:
                    SET_KEY(ROW0, BIT0);
                    break;
                case XK_Right:
                    SET_KEY(ROW0, BIT1);
                    break;
                case XK_Down:
                    SET_KEY(ROW0, BIT2);
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
                    SET_KEY(ROW2, BIT7);
                    break;
                case XK_Alt_L:
                    SET_KEY(ROW1, BIT1);
                    break;
                case XK_Caps_Lock:
                    SET_KEY(ROW8, BIT6);
                    break;
                default:
                    break;
            }
        }
    }

    static inline auto on_key_press(State& state, const XKeyEvent& event) -> void
    {
        uint8_t line = 0xff;
        uint8_t data = 0x00;
        uint8_t mods = ~(state.keys[0x0f]);

        switch(state.type) {
            case Type::TYPE_ENGLISH:
                decode_english(state, event, line, data, mods);
                break;
            case Type::TYPE_FRENCH:
                decode_french(state, event, line, data, mods);
                break;
            default:
                decode_english(state, event, line, data, mods);
                break;
        }
        if((line <= 0x0f) && (data != 0x00)) {
            if(mods & BIT0) {
                state.keys[ROW2] &= ~BIT5;
            }
            if(mods & BIT1) {
                state.keys[ROW2] &= ~BIT7;
            }
            state.keys[line] &= ~data;
            state.keys[0x0f]  = ~mods;
        }
    }

    static inline auto on_key_release(State& state, const XKeyEvent& event) -> void
    {
        uint8_t line = 0xff;
        uint8_t data = 0x00;
        uint8_t mods = ~(state.keys[0x0f]);

        switch(state.type) {
            case Type::TYPE_ENGLISH:
                decode_english(state, event, line, data, mods);
                break;
            case Type::TYPE_FRENCH:
                decode_french(state, event, line, data, mods);
                break;
            default:
                decode_english(state, event, line, data, mods);
                break;
        }
        if((line <= 0x0f) && (data != 0x00)) {
            if(mods & BIT0) {
                state.keys[ROW2] |=  BIT5;
            }
            if(mods & BIT1) {
                state.keys[ROW2] |=  BIT7;
            }
            state.keys[line] |=  data;
            state.keys[0x0f]  = ~mods;
        }
    }

    static inline auto on_button_press(State& state, const XButtonEvent& event) -> void
    {
        uint8_t line = 0xff;

        if(event.state == Joystick0Modifier) {
            line = ROW9;
        }
        if(event.state == Joystick1Modifier) {
            line = ROW6;
        }
        if(line < 16) {
            if((event.button & 0x01) == 0) {
                state.keys[line] &= ~BIT4;
            }
            else {
                state.keys[line] &= ~BIT5;
            }
        }
    }

    static inline auto on_button_release(State& state, const XButtonEvent& event) -> void
    {
        uint8_t line = 0xff;

        if(event.state == Joystick0Modifier) {
            line = ROW9;
        }
        if(event.state == Joystick1Modifier) {
            line = ROW6;
        }
        if(line < 16) {
            if((event.button & 0x01) == 0) {
                state.keys[line] |=  BIT4;
            }
            else {
                state.keys[line] |=  BIT5;
            }
        }
    }

    static inline auto on_motion_notify(State& state, const XMotionEvent& event) -> void
    {
        uint8_t line = 0xff;

        if(event.state == Joystick0Modifier) {
            line = ROW9;
        }
        if(event.state == Joystick1Modifier) {
            line = ROW6;
        }
        if(line < 16) {
            /* up */
            if(event.y <= -16384) {
                state.keys[line] &= ~BIT0;
            }
            else {
                state.keys[line] |=  BIT0;
            }
            /* down */
            if(event.y >= +16384) {
                state.keys[line] &= ~BIT1;
            }
            else {
                state.keys[line] |=  BIT1;
            }
            /* left */
            if(event.x <= -16384) {
                state.keys[line] &= ~BIT2;
            }
            else {
                state.keys[line] |=  BIT2;
            }
            /* right */
            if(event.x >= +16384) {
                state.keys[line] &= ~BIT3;
            }
            else {
                state.keys[line] |=  BIT3;
            }
        }
    }
};

}

// ---------------------------------------------------------------------------
// kbd::Instance
// ---------------------------------------------------------------------------

namespace kbd {

Instance::Instance(const Type type, Interface& interface)
    : _interface(interface)
    , _state()
{
    StateTraits::construct(_state, type);

    reset();
}

Instance::~Instance()
{
    StateTraits::destruct(_state);
}

auto Instance::reset() -> void
{
    StateTraits::reset(_state);
}

auto Instance::clock() -> void
{
    StateTraits::clock(_state);
}

auto Instance::set_type(const Type type) -> void
{
    StateTraits::set_type(_state, type);
}

auto Instance::set_line(uint8_t line) -> uint8_t
{
    return StateTraits::set_line(_state, line);
}

auto Instance::get_data(uint8_t data) -> uint8_t
{
    return StateTraits::get_data(_state, data);
}

auto Instance::key_press(const XKeyEvent& event) -> void
{
    return StateTraits::on_key_press(_state, event);
}

auto Instance::key_release(const XKeyEvent& event) -> void
{
    return StateTraits::on_key_release(_state, event);
}

auto Instance::button_press(const XButtonEvent& event) -> void
{
    return StateTraits::on_button_press(_state, event);
}

auto Instance::button_release(const XButtonEvent& event) -> void
{
    return StateTraits::on_button_release(_state, event);
}

auto Instance::motion_notify(const XMotionEvent& event) -> void
{
    return StateTraits::on_motion_notify(_state, event);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
