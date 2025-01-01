/*
 * console.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __BASE_CONSOLE_H__
#define __BASE_CONSOLE_H__

// ---------------------------------------------------------------------------
// base::Console
// ---------------------------------------------------------------------------

namespace base {

class Console
{
public: // public interface
    Console ( std::istream& istream
            , std::ostream& ostream
            , std::ostream& estream );

    virtual ~Console() = default;

    virtual void println(const char*, ...);

    virtual void errorln(const char*, ...);

protected: // protected data
    std::istream& _istream;
    std::ostream& _ostream;
    std::ostream& _estream;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BASE_CONSOLE_H__ */
