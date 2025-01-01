/*
 * xcpc-dsk.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_DSK_H__
#define __XCPC_DSK_H__

#include <xcpc/formats/dsk/dsk-format.h>
#include "arglist.h"
#include "console.h"
#include "program.h"

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

class Command
{
public: // public interface
    Command ( base::Console&     console
            , const std::string& program
            , const std::string& command );

    Command(const Command&) = delete;

    Command& operator=(const Command&) = delete;

    virtual ~Command() = default;

    virtual void run() = 0;

    void addArgument(const std::string& argument)
    {
        _arguments.add(argument);
    }

protected: // protected data
    base::Console&    _console;
    base::ArgList     _arguments;
    const std::string _program;
    const std::string _command;
};

// ---------------------------------------------------------------------------
// HelpCmd
// ---------------------------------------------------------------------------

class HelpCmd final
    : public Command
{
public: // public interface
    HelpCmd ( base::Console&     console
            , const std::string& program );

    virtual ~HelpCmd() = default;

    virtual void run() override final;
};

// ---------------------------------------------------------------------------
// DumpCmd
// ---------------------------------------------------------------------------

class DumpCmd final
    : public Command
{
public: // public interface
    DumpCmd ( base::Console&     console
            , const std::string& program );

    virtual ~DumpCmd() = default;

    virtual void run() override final;
};

// ---------------------------------------------------------------------------
// CreateCmd
// ---------------------------------------------------------------------------

class CreateCmd final
    : public Command
{
public: // public interface
    CreateCmd ( base::Console&     console
              , const std::string& program );

    virtual ~CreateCmd() = default;

    virtual void run() override final;
};

// ---------------------------------------------------------------------------
// Program
// ---------------------------------------------------------------------------

class Program final
    : public base::Program
{
public: // public interface
    Program ( base::ArgList& arglist
            , base::Console& console );

    virtual ~Program() = default;

    virtual void main() override final;

protected: // protected data
    std::string              _program;
    std::unique_ptr<Command> _command;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_DSK_H__ */
