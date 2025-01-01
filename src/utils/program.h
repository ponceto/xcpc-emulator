/*
 * program.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __BASE_PROGRAM_H__
#define __BASE_PROGRAM_H__

#include "arglist.h"
#include "console.h"

// ---------------------------------------------------------------------------
// base::Program
// ---------------------------------------------------------------------------

namespace base {

class Program
{
public: // public interface
    Program(ArgList& arglist, Console& console);

    virtual ~Program() = default;

    virtual void main() = 0;

protected: // protected data
    ArgList& _arglist;
    Console& _console;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BASE_PROGRAM_H__ */
