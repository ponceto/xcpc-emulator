/*
 * psg-core.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "psg-core.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = psg::Type;
    using State     = psg::State;
    using Sound     = psg::Sound;
    using Noise     = psg::Noise;
    using Envelope  = psg::Envelope;
    using Output    = psg::Output;
    using Instance  = psg::Instance;
    using Interface = psg::Interface;

    static constexpr uint8_t ADDRESS_REGISTER      = -1;
    static constexpr uint8_t CHANNEL_A_FINE_TUNE   =  0;
    static constexpr uint8_t CHANNEL_A_COARSE_TUNE =  1;
    static constexpr uint8_t CHANNEL_B_FINE_TUNE   =  2;
    static constexpr uint8_t CHANNEL_B_COARSE_TUNE =  3;
    static constexpr uint8_t CHANNEL_C_FINE_TUNE   =  4;
    static constexpr uint8_t CHANNEL_C_COARSE_TUNE =  5;
    static constexpr uint8_t NOISE_PERIOD          =  6;
    static constexpr uint8_t MIXER_AND_IO_CONTROL  =  7;
    static constexpr uint8_t CHANNEL_A_AMPLITUDE   =  8;
    static constexpr uint8_t CHANNEL_B_AMPLITUDE   =  9;
    static constexpr uint8_t CHANNEL_C_AMPLITUDE   = 10;
    static constexpr uint8_t ENVELOPE_FINE_TUNE    = 11;
    static constexpr uint8_t ENVELOPE_COARSE_TUNE  = 12;
    static constexpr uint8_t ENVELOPE_SHAPE        = 13;
    static constexpr uint8_t IO_PORT_A             = 14;
    static constexpr uint8_t IO_PORT_B             = 15;
    static constexpr uint8_t RAMP_UP               = 0;
    static constexpr uint8_t RAMP_DOWN             = 1;
    static constexpr uint8_t HOLD_UP               = 2;
    static constexpr uint8_t HOLD_DOWN             = 3;
    static constexpr uint8_t SOUND0                = 0;
    static constexpr uint8_t SOUND1                = 1;
    static constexpr uint8_t SOUND2                = 2;
    static constexpr uint8_t NOISE0                = 0;
    static constexpr uint8_t PORT0                 = 0;
    static constexpr uint8_t PORT1                 = 1;

    static const float   ay_dac[32];
    static const float   ym_dac[32];
    static const uint8_t cycles[16][2];
};

const float BasicTraits::ay_dac[32] = {
    0.0000000f, 0.0000000f, 0.0099947f, 0.0099947f,
    0.0144503f, 0.0144503f, 0.0210575f, 0.0210575f,
    0.0307012f, 0.0307012f, 0.0455482f, 0.0455482f,
    0.0644999f, 0.0644999f, 0.1073625f, 0.1073625f,
    0.1265888f, 0.1265888f, 0.2049897f, 0.2049897f,
    0.2922103f, 0.2922103f, 0.3728389f, 0.3728389f,
    0.4925307f, 0.4925307f, 0.6353246f, 0.6353246f,
    0.8055848f, 0.8055848f, 1.0000000f, 1.0000000f
};

const float BasicTraits::ym_dac[32] = {
    0.0000000f, 0.0000000f, 0.0046540f, 0.0077211f,
    0.0109560f, 0.0139620f, 0.0169986f, 0.0200198f,
    0.0243687f, 0.0296941f, 0.0350652f, 0.0403906f,
    0.0485389f, 0.0583352f, 0.0680552f, 0.0777752f,
    0.0925154f, 0.1110857f, 0.1297475f, 0.1484855f,
    0.1766690f, 0.2115511f, 0.2463874f, 0.2811017f,
    0.3337301f, 0.4004273f, 0.4673838f, 0.5344320f,
    0.6351720f, 0.7580072f, 0.8799268f, 1.0000000f
};

const uint8_t BasicTraits::cycles[16][2] = {
    { RAMP_DOWN, HOLD_DOWN },
    { RAMP_DOWN, HOLD_DOWN },
    { RAMP_DOWN, HOLD_DOWN },
    { RAMP_DOWN, HOLD_DOWN },
    { RAMP_UP  , HOLD_DOWN },
    { RAMP_UP  , HOLD_DOWN },
    { RAMP_UP  , HOLD_DOWN },
    { RAMP_UP  , HOLD_DOWN },
    { RAMP_DOWN, RAMP_DOWN },
    { RAMP_DOWN, HOLD_DOWN },
    { RAMP_DOWN, RAMP_UP   },
    { RAMP_DOWN, HOLD_UP   },
    { RAMP_UP  , RAMP_UP   },
    { RAMP_UP  , HOLD_UP   },
    { RAMP_UP  , RAMP_DOWN },
    { RAMP_UP  , HOLD_DOWN },
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
        switch(state.type = type) {
            case Type::TYPE_AY8910:
            case Type::TYPE_AY8912:
            case Type::TYPE_AY8913:
                static_cast<void>(::memcpy(state.dac, ay_dac, sizeof(state.dac)));
                break;
            case Type::TYPE_YM2149:
                static_cast<void>(::memcpy(state.dac, ym_dac, sizeof(state.dac)));
                break;
            default:
                static_cast<void>(::memcpy(state.dac, ay_dac, sizeof(state.dac)));
                break;
        }
    }

    static inline auto destruct(State& state) -> void
    {
        state.type = Type::TYPE_INVALID;
    }

    static inline auto reset(State& state) -> void
    {
        state.ticks &= 0;
        state.index &= 0;
        for(auto& value : state.array) {
            value &= 0;
        }
        for(auto& value : state.has_sound) {
            value &= 0;
        }
        for(auto& value : state.has_noise) {
            value &= 0;
        }
        for(auto& value : state.dir_port) {
            value &= 0;
        }
    }

    static inline auto get_mixer_and_io_control(State& state, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_mixer_and_io_control(State& state, const uint8_t value) -> uint8_t
    {
        state.has_sound[SOUND0] = ((value & 0x01) == 0);
        state.has_sound[SOUND1] = ((value & 0x02) == 0);
        state.has_sound[SOUND2] = ((value & 0x04) == 0);
        state.has_noise[SOUND0] = ((value & 0x08) == 0);
        state.has_noise[SOUND1] = ((value & 0x10) == 0);
        state.has_noise[SOUND2] = ((value & 0x20) == 0);
        state.dir_port[PORT0]   = ((value & 0x40) != 0);
        state.dir_port[PORT1]   = ((value & 0x80) != 0);

        return value;
    }

    static inline auto get_port0(State& state, Instance& instance, Interface& interface, const uint8_t value) -> uint8_t
    {
        if(state.dir_port[PORT0] == 0) {
            return interface.psg_port_a_rd(instance, value);
        }
        return value;
    }

    static inline auto set_port0(State& state, Instance& instance, Interface& interface, const uint8_t value) -> uint8_t
    {
        if(state.dir_port[PORT0] != 0) {
            return interface.psg_port_a_wr(instance, value);
        }
        return value;
    }

    static inline auto get_port1(State& state, Instance& instance, Interface& interface, const uint8_t value) -> uint8_t
    {
        if(state.dir_port[PORT1] == 0) {
            return interface.psg_port_b_rd(instance, value);
        }
        return value;
    }

    static inline auto set_port1(State& state, Instance& instance, Interface& interface, const uint8_t value) -> uint8_t
    {
        if(state.dir_port[PORT1] != 0) {
            return interface.psg_port_b_wr(instance, value);
        }
        return value;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::SoundTraits
// ---------------------------------------------------------------------------

namespace {

struct SoundTraits final
    : public BasicTraits
{
    static inline auto reset(Sound& sound) -> void
    {
        sound.counter   &= 0;
        sound.period    &= 0;
        sound.phase     &= 0;
        sound.amplitude &= 0;
    }

    static inline auto clock(Sound& sound) -> void
    {
        if(sound.period == 0) {
            return;
        }
        if(++sound.counter >= sound.period) {
            sound.counter &= 0;
            sound.phase   ^= 1;
        }
    }

    static inline auto get_fine_tune(Sound& sound, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_fine_tune(Sound& sound, const uint8_t value) -> uint8_t
    {
        constexpr uint16_t mask = 0xff00;
        const     uint16_t data = static_cast<uint16_t>(value) << 0;

        sound.period = ((sound.period & mask) | data);

        return value;
    }

    static inline auto get_coarse_tune(Sound& sound, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_coarse_tune(Sound& sound, const uint8_t value) -> uint8_t
    {
        constexpr uint16_t mask = 0x00ff;
        const     uint16_t data = static_cast<uint16_t>(value) << 8;

        sound.period = ((sound.period & mask) | data);

        return value;
    }

    static inline auto get_amplitude(Sound& sound, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_amplitude(Sound& sound, const uint8_t value) -> uint8_t
    {
        const uint8_t msb = ((value << 1) & 0b00111110);
        const uint8_t lsb = ((value >> 3) & 0b00000001);

        sound.amplitude = (msb | lsb);

        return value;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::NoiseTraits
// ---------------------------------------------------------------------------

namespace {

struct NoiseTraits final
    : public BasicTraits
{
    static inline auto reset(Noise& noise) -> void
    {
        noise.counter &= 0;
        noise.period  &= 0;
        noise.shift   &= 0;
        noise.phase   &= 0;
    }

    static inline auto clock(Noise& noise) -> void
    {
        if(noise.period == 0) {
            return;
        }
        if(++noise.counter >= noise.period) {
            noise.counter &= 0;
            const uint32_t lfsr = noise.shift;
            const uint32_t bit0 = (lfsr << 16);
            const uint32_t bit3 = (lfsr << 13);
            const uint32_t msw  = (~(bit0 ^ bit3) & 0x10000);
            const uint32_t lsw  = ((lfsr >> 1) & 0x0ffff);
            noise.shift = (msw | lsw);
            noise.phase = (lfsr & 1);
        }
    }

    static inline auto get_fine_tune(Noise& sound, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_fine_tune(Noise& sound, const uint8_t value) -> uint8_t
    {
        constexpr uint16_t mask = 0xff00;
        const     uint16_t data = static_cast<uint16_t>(value) << 0;

        sound.period = ((sound.period & mask) | data);

        return value;
    }

    static inline auto get_coarse_tune(Noise& sound, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_coarse_tune(Noise& sound, const uint8_t value) -> uint8_t
    {
        constexpr uint16_t mask = 0x00ff;
        const     uint16_t data = static_cast<uint16_t>(value) << 8;

        sound.period = ((sound.period & mask) | data);

        return value;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::EnvelopeTraits
// ---------------------------------------------------------------------------

namespace {

struct EnvelopeTraits final
    : public BasicTraits
{
    static inline auto reset(Envelope& envelope) -> void
    {
        envelope.counter   &= 0;
        envelope.period    &= 0;
        envelope.shape     &= 0;
        envelope.phase     &= 0;
        envelope.amplitude &= 0;
    }

    static inline auto clock(Envelope& envelope) -> void
    {
        if(++envelope.counter >= envelope.period) {
            envelope.counter &= 0;
            switch(cycles[envelope.shape][envelope.phase]) {
                case RAMP_UP:
                    envelope.amplitude = ((envelope.amplitude + 1) & 0x1f);
                    if(envelope.amplitude == 0x1f) {
                        envelope.phase ^= 1;
                    }
                    break;
                case RAMP_DOWN:
                    envelope.amplitude = ((envelope.amplitude - 1) & 0x1f);
                    if(envelope.amplitude == 0x00) {
                        envelope.phase ^= 1;
                    }
                    break;
                case HOLD_UP:
                    envelope.amplitude = 0x1f;
                    break;
                case HOLD_DOWN:
                    envelope.amplitude = 0x00;
                    break;
                default:
                    break;
            }
        }
    }

    static inline auto get_fine_tune(Envelope& envelope, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_fine_tune(Envelope& envelope, const uint8_t value) -> uint8_t
    {
        constexpr uint16_t mask = 0xff00;
        const     uint16_t data = static_cast<uint16_t>(value) << 0;

        envelope.period = ((envelope.period & mask) | data);

        return value;
    }

    static inline auto get_coarse_tune(Envelope& envelope, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_coarse_tune(Envelope& envelope, const uint8_t value) -> uint8_t
    {
        constexpr uint16_t mask = 0x00ff;
        const     uint16_t data = static_cast<uint16_t>(value) << 8;

        envelope.period = ((envelope.period & mask) | data);

        return value;
    }

    static inline auto get_shape(Envelope& envelope, const uint8_t value) -> uint8_t
    {
        return value;
    }

    static inline auto set_shape(Envelope& envelope, const uint8_t value) -> uint8_t
    {
        envelope.shape     = value;
        envelope.phase     = 0;
        envelope.amplitude = ((envelope.shape & 0x04) == 0 ? 0x1f : 0x00);

        return value;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::OutputTraits
// ---------------------------------------------------------------------------

namespace {

struct OutputTraits final
    : public BasicTraits
{
    static inline auto reset(Output& output) -> void
    {
        output.channel0 = 0.0f;
        output.channel1 = 0.0f;
        output.channel2 = 0.0f;
    }
};

}

// ---------------------------------------------------------------------------
// psg::Instance
// ---------------------------------------------------------------------------

namespace psg {

Instance::Instance(const Type type, Interface& interface)
    : _interface(interface)
    , _state()
    , _sound()
    , _noise()
    , _envelope()
    , _output()
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
    SoundTraits::reset(_sound[BasicTraits::SOUND0]);
    SoundTraits::reset(_sound[BasicTraits::SOUND1]);
    SoundTraits::reset(_sound[BasicTraits::SOUND2]);
    NoiseTraits::reset(_noise[BasicTraits::NOISE0]);
    EnvelopeTraits::reset(_envelope);
    OutputTraits::reset(_output);
}

auto Instance::clock() -> void
{
    auto fixup = [&](Sound& lhs, Sound& rhs) -> void
    {
        if((lhs.period == rhs.period) && (lhs.counter != rhs.counter)) {
            rhs.counter = lhs.counter;
            rhs.phase   = lhs.phase;
        }
    };

    auto prepare = [&]() -> void
    {
        fixup(_sound[0], _sound[1]);
        fixup(_sound[0], _sound[2]);
        fixup(_sound[1], _sound[2]);
    };

#if 0 /* original psg output not really adapted to float [-1.0:+1.0] output */
    auto get_output = [&](const int sound_index, const int noise_index) -> float
    {
        const uint8_t has_sound = _state.has_sound[sound_index];
        const uint8_t has_noise = _state.has_noise[sound_index];
        const uint8_t sig_sound = _sound[sound_index].phase;
        const uint8_t sig_noise = _noise[noise_index].phase;
        const uint8_t amplitude = (_sound[sound_index].amplitude & 0x20 ? (_envelope.amplitude & 0x1f) : (_sound[sound_index].amplitude & 0x1f));
        const uint8_t output    = ((sig_sound & has_sound) | (sig_noise & has_noise));

        return static_cast<float>(output) * _state.dac[amplitude] * 2.0f - 1.0f;
    };
#else /* modified psg output adapted to float [-1.0:+1.0] output */
    auto get_output = [&](const int sound_index, const int noise_index) -> float
    {
        const uint8_t has_sound = _state.has_sound[sound_index];
        const uint8_t has_noise = _state.has_noise[sound_index];
        const int8_t  sig_sound = (_sound[sound_index].phase != 0 ? +1 : -1);
        const int8_t  sig_noise = (_noise[noise_index].phase != 0 ? +1 : -1);
        const uint8_t amplitude = (_sound[sound_index].amplitude & 0x20 ? (_envelope.amplitude & 0x1f) : (_sound[sound_index].amplitude & 0x1f));
        int8_t        output    = 0;

        if(has_sound != 0) {
            output |= sig_sound;
        }
        if(has_noise != 0) {
            output |= sig_noise;
        }

        return static_cast<float>(output) * _state.dac[amplitude];
    };
#endif

    auto output = [&]() -> void
    {
        _output.channel0 = get_output(BasicTraits::SOUND0, BasicTraits::NOISE0);
        _output.channel1 = get_output(BasicTraits::SOUND1, BasicTraits::NOISE0);
        _output.channel2 = get_output(BasicTraits::SOUND2, BasicTraits::NOISE0);
    };

    auto update = [&]() -> void
    {
        const auto clk_div = ((++_state.ticks) & 0x07);

        if(clk_div == 0) {
            prepare();
            SoundTraits::clock(_sound[BasicTraits::SOUND0]);
            SoundTraits::clock(_sound[BasicTraits::SOUND1]);
            SoundTraits::clock(_sound[BasicTraits::SOUND2]);
            NoiseTraits::clock(_noise[BasicTraits::NOISE0]);
            EnvelopeTraits::clock(_envelope);
            output();
        }
    };

    return update();
}

auto Instance::get_index(uint8_t index) -> uint8_t
{
    return (index = _state.index);
}

auto Instance::set_index(uint8_t index) -> uint8_t
{
    return (_state.index = index);
}

auto Instance::get_value(uint8_t value) -> uint8_t
{
    const auto index = _state.index;
    auto&      array = _state.array[index & 0x0f];

    switch(index) {
        case BasicTraits::CHANNEL_A_FINE_TUNE:
            value = SoundTraits::get_fine_tune(_sound[BasicTraits::SOUND0], (array &= 0xff));
            break;
        case BasicTraits::CHANNEL_A_COARSE_TUNE:
            value = SoundTraits::get_coarse_tune(_sound[BasicTraits::SOUND0], (array &= 0x0f));
            break;
        case BasicTraits::CHANNEL_B_FINE_TUNE:
            value = SoundTraits::get_fine_tune(_sound[BasicTraits::SOUND1], (array &= 0xff));
            break;
        case BasicTraits::CHANNEL_B_COARSE_TUNE:
            value = SoundTraits::get_coarse_tune(_sound[BasicTraits::SOUND1], (array &= 0x0f));
            break;
        case BasicTraits::CHANNEL_C_FINE_TUNE:
            value = SoundTraits::get_fine_tune(_sound[BasicTraits::SOUND2], (array &= 0xff));
            break;
        case BasicTraits::CHANNEL_C_COARSE_TUNE:
            value = SoundTraits::get_coarse_tune(_sound[BasicTraits::SOUND2], (array &= 0x0f));
            break;
        case BasicTraits::NOISE_PERIOD:
            value = NoiseTraits::get_fine_tune(_noise[BasicTraits::NOISE0], (array &= 0x1f));
            break;
        case BasicTraits::MIXER_AND_IO_CONTROL:
            value = StateTraits::get_mixer_and_io_control(_state, (array &= 0xff));
            break;
        case BasicTraits::CHANNEL_A_AMPLITUDE:
            value = SoundTraits::get_amplitude(_sound[BasicTraits::SOUND0], (array &= 0x1f));
            break;
        case BasicTraits::CHANNEL_B_AMPLITUDE:
            value = SoundTraits::get_amplitude(_sound[BasicTraits::SOUND1], (array &= 0x1f));
            break;
        case BasicTraits::CHANNEL_C_AMPLITUDE:
            value = SoundTraits::get_amplitude(_sound[BasicTraits::SOUND2], (array &= 0x1f));
            break;
        case BasicTraits::ENVELOPE_FINE_TUNE:
            value = EnvelopeTraits::get_fine_tune(_envelope, (array &= 0xff));
            break;
        case BasicTraits::ENVELOPE_COARSE_TUNE:
            value = EnvelopeTraits::get_coarse_tune(_envelope, (array &= 0xff));
            break;
        case BasicTraits::ENVELOPE_SHAPE:
            value = EnvelopeTraits::get_shape(_envelope, (array &= 0x0f));
            break;
        case BasicTraits::IO_PORT_A:
            value = StateTraits::get_port0(_state, *this, _interface, (array &= 0xff));
            break;
        case BasicTraits::IO_PORT_B:
            value = StateTraits::get_port1(_state, *this, _interface, (array &= 0xff));
            break;
        default:
            break;
    }
    return value;
}

auto Instance::set_value(uint8_t value) -> uint8_t
{
    const auto index = _state.index;
    auto&      array = _state.array[index & 0x0f];

    switch(index) {
        case BasicTraits::CHANNEL_A_FINE_TUNE:
            array = SoundTraits::set_fine_tune(_sound[BasicTraits::SOUND0], (value &= 0xff));
            break;
        case BasicTraits::CHANNEL_A_COARSE_TUNE:
            array = SoundTraits::set_coarse_tune(_sound[BasicTraits::SOUND0], (value &= 0x0f));
            break;
        case BasicTraits::CHANNEL_B_FINE_TUNE:
            array = SoundTraits::set_fine_tune(_sound[BasicTraits::SOUND1], (value &= 0xff));
            break;
        case BasicTraits::CHANNEL_B_COARSE_TUNE:
            array = SoundTraits::set_coarse_tune(_sound[BasicTraits::SOUND1], (value &= 0x0f));
            break;
        case BasicTraits::CHANNEL_C_FINE_TUNE:
            array = SoundTraits::set_fine_tune(_sound[BasicTraits::SOUND2], (value &= 0xff));
            break;
        case BasicTraits::CHANNEL_C_COARSE_TUNE:
            array = SoundTraits::set_coarse_tune(_sound[BasicTraits::SOUND2], (value &= 0x0f));
            break;
        case BasicTraits::NOISE_PERIOD:
            array = NoiseTraits::set_fine_tune(_noise[BasicTraits::NOISE0], (value &= 0x1f));
            break;
        case BasicTraits::MIXER_AND_IO_CONTROL:
            array = StateTraits::set_mixer_and_io_control(_state, (value &= 0xff));
            break;
        case BasicTraits::CHANNEL_A_AMPLITUDE:
            array = SoundTraits::set_amplitude(_sound[BasicTraits::SOUND0], (value &= 0x1f));
            break;
        case BasicTraits::CHANNEL_B_AMPLITUDE:
            array = SoundTraits::set_amplitude(_sound[BasicTraits::SOUND1], (value &= 0x1f));
            break;
        case BasicTraits::CHANNEL_C_AMPLITUDE:
            array = SoundTraits::set_amplitude(_sound[BasicTraits::SOUND2], (value &= 0x1f));
            break;
        case BasicTraits::ENVELOPE_FINE_TUNE:
            array = EnvelopeTraits::set_fine_tune(_envelope, (value &= 0xff));
            break;
        case BasicTraits::ENVELOPE_COARSE_TUNE:
            array = EnvelopeTraits::set_coarse_tune(_envelope, (value &= 0xff));
            break;
        case BasicTraits::ENVELOPE_SHAPE:
            array = EnvelopeTraits::set_shape(_envelope, (value &= 0x0f));
            break;
        case BasicTraits::IO_PORT_A:
            array = StateTraits::set_port0(_state, *this, _interface, (value &= 0xff));
            break;
        case BasicTraits::IO_PORT_B:
            array = StateTraits::set_port1(_state, *this, _interface, (value &= 0xff));
            break;
        default:
            break;
    }
    return value;
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
