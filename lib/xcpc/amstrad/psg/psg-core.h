/*
 * psg-core.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_PSG_CORE_H__
#define __XCPC_PSG_CORE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace psg {

class State;
class Instance;
class Interface;

}

// ---------------------------------------------------------------------------
// psg::Type
// ---------------------------------------------------------------------------

namespace psg {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
    TYPE_AY8910  =  1,
    TYPE_AY8912  =  2,
    TYPE_AY8913  =  3,
    TYPE_YM2149  =  4,
};

}

// ---------------------------------------------------------------------------
// psg::State
// ---------------------------------------------------------------------------

namespace psg {

struct State
{
    uint8_t  type;
    uint32_t ticks;
    uint8_t  index;
    uint8_t  array[16];
    uint8_t  has_sound[3];
    uint8_t  has_noise[3];
    uint8_t  dir_port[2];
    float    dac[32];
};

}

// ---------------------------------------------------------------------------
// psg::Sound
// ---------------------------------------------------------------------------

namespace psg {

struct Sound
{
    uint16_t counter;
    uint16_t period;
    uint8_t  phase;
    uint8_t  amplitude;
};

}

// ---------------------------------------------------------------------------
// psg::Noise
// ---------------------------------------------------------------------------

namespace psg {

struct Noise
{
    uint16_t counter;
    uint16_t period;
    uint32_t shift;
    uint8_t  phase;
};

}

// ---------------------------------------------------------------------------
// psg::Envelope
// ---------------------------------------------------------------------------

namespace psg {

struct Envelope
{
    uint16_t counter;
    uint16_t period;
    uint8_t  shape;
    uint8_t  phase;
    uint8_t  amplitude;
};

}

// ---------------------------------------------------------------------------
// psg::Output
// ---------------------------------------------------------------------------

namespace psg {

struct Output
{
    float channel0;
    float channel1;
    float channel2;
};

}

// ---------------------------------------------------------------------------
// psg::Instance
// ---------------------------------------------------------------------------

namespace psg {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto get_index(uint8_t index) -> uint8_t;

    auto set_index(uint8_t index) -> uint8_t;

    auto get_value(uint8_t value) -> uint8_t;

    auto set_value(uint8_t value) -> uint8_t;

    auto operator->() -> State*
    {
        return &_state;
    }

    auto get_output() const -> const Output&
    {
        return _output;
    }

protected: // protected data
    Interface& _interface;
    State      _state;
    Sound      _sound[3];
    Noise      _noise[1];
    Envelope   _envelope;
    Output     _output;
};

}

// ---------------------------------------------------------------------------
// psg::Interface
// ---------------------------------------------------------------------------

namespace psg {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;

    virtual auto psg_port_a_rd(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto psg_port_a_wr(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto psg_port_b_rd(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto psg_port_b_wr(Instance& instance, uint8_t data) -> uint8_t = 0;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_PSG_CORE_H__ */
