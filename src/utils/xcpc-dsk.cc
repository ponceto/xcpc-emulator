/*
 * xcpc-dsk.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include <climits>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "xcpc-dsk.h"

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

Command::Command ( base::Console&     console
                 , const std::string& program
                 , const std::string& command )
    : _console(console)
    , _arguments()
    , _program(program)
    , _command(command)
{
}

// ---------------------------------------------------------------------------
// HelpCmd
// ---------------------------------------------------------------------------

HelpCmd::HelpCmd(base::Console& console, const std::string& program)
    : Command(console, program, "help")
{
}

void HelpCmd::run()
{
    _console.println("Usage: %s <command> [OPTIONS] [FILES]...", _program.c_str());
    _console.println("");
    _console.println("available commands:");
    _console.println("");
    _console.println("    help        display this help");
    _console.println("    dump        dump the content of an existing disk image");
    _console.println("    create      create a new disk image");
    _console.println("");
}

// ---------------------------------------------------------------------------
// DumpCmd
// ---------------------------------------------------------------------------

DumpCmd::DumpCmd(base::Console& console, const std::string& program)
    : Command(console, program, "dump")
{
}

void DumpCmd::run()
{
    auto begin = [&](const std::string& filename)
    {
        _console.println("dumping '%s' ...", filename.c_str());
    };

    auto end = [&](const std::string& filename)
    {
        _console.println("dumped '%s'", filename.c_str());
    };

    auto dump = [&](const std::string& filename)
    {
        dsk::Disk disk(filename);
        disk.dump();
    };

    auto error = [&](const std::string& filename, const char* what)
    {
        _console.errorln("error while dumping '%s': %s", filename.c_str(), what);
    };

    for(auto& filename : _arguments) {
        begin(filename);
        try {
            dump(filename);
        }
        catch(const std::exception& e) {
            error(filename, e.what());
        }
        end(filename);
    }
}

// ---------------------------------------------------------------------------
// CreateCmd
// ---------------------------------------------------------------------------

CreateCmd::CreateCmd(base::Console& console, const std::string& program)
    : Command(console, program, "create")
{
}

void CreateCmd::run()
{
    auto begin = [&](const std::string& filename)
    {
        _console.println("creating '%s' ...", filename.c_str());
    };

    auto end = [&](const std::string& filename)
    {
        _console.println("created '%s'", filename.c_str());
    };

    auto create = [&](const std::string& filename)
    {
        dsk::Disk disk(filename);
        disk.create();
    };

    auto error = [&](const std::string& filename, const char* what)
    {
        _console.errorln("error while checking '%s': %s", filename.c_str(), what);
    };

    for(auto& filename : _arguments) {
        begin(filename);
        try {
            create(filename);
        }
        catch(const std::exception& e) {
            error(filename, e.what());
        }
        end(filename);
    }
}

// ---------------------------------------------------------------------------
// Program
// ---------------------------------------------------------------------------

Program::Program(base::ArgList& arglist, base::Console& console)
    : base::Program(arglist, console)
    , _program("xcpc-dsk")
    , _command()
{
}

void Program::main()
{
    auto set_program = [&](const std::string& argument) -> void
    {
        const char* c_str = argument.c_str();
        const char* slash = ::strrchr(c_str, '/');
        if(slash != nullptr) {
            c_str = slash + 1;
            _program = c_str;
        }
    };

    auto build_help_cmd = [&]() -> void
    {
        _command = std::make_unique<HelpCmd>(_console, _program);
    };

    auto build_dump_cmd = [&]() -> void
    {
        _command = std::make_unique<DumpCmd>(_console, _program);
    };

    auto build_create_cmd = [&]() -> void
    {
        _command = std::make_unique<CreateCmd>(_console, _program);
    };

    auto build_command = [&](const std::string& command) -> void
    {
        if(command == "help") {
            return build_help_cmd();
        }
        if(command == "dump") {
            return build_dump_cmd();
        }
        if(command == "create") {
            return build_create_cmd();
        }
        throw std::runtime_error(std::string() + '<' + command + '>' + ' ' + "is not a valid command");
    };

    auto add_argument = [&](const std::string& argument) -> void
    {
        if(_command) {
            _command->addArgument(argument);
        }
    };

    auto run_command = [&]() -> void
    {
        if(!_command) {
            build_help_cmd();
        }
        return _command->run();
    };

    auto parse = [&]() -> void
    {
        int argi = 0;
        for(auto& argument : _arglist) {
            if(argi == 0) {
                set_program(argument);
            }
            else if(argi == 1) {
                build_command(argument);
            }
            else if(argi > 0) {
                add_argument(argument);
            }
            ++argi;
        }
    };

    auto execute = [&]() -> void
    {
        parse();
        run_command();
    };

    return execute();
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    base::ArgList arglist ( argc
                          , argv );

    base::Console console ( std::cin
                          , std::cout
                          , std::cerr );

    try {
        Program program(arglist, console);

        program.main();
    }
    catch(const std::exception& e) {
        console.errorln("error: %s", e.what());
        return EXIT_FAILURE;
    }
    catch(...) {
        console.errorln("error: %s", "unhandled exception");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
