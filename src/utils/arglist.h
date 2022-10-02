/*
 * arglist.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __BASE_ARGLIST_H__
#define __BASE_ARGLIST_H__

// ---------------------------------------------------------------------------
// base::ArgList
// ---------------------------------------------------------------------------

namespace base {

class ArgList
{
public: // public interface
    ArgList();

    ArgList(int argc, char* argv[]);

    virtual ~ArgList() = default;

    auto args() const -> const std::vector<std::string>&
    {
        return _arglist;
    }

    auto add(const std::string& arg) -> void
    {
        return _arglist.push_back(arg);
    }

    auto begin() const -> auto
    {
        return _arglist.begin();
    }

    auto end() const -> auto
    {
        return _arglist.end();
    }

protected: // protected data
    std::vector<std::string> _arglist;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BASE_ARGLIST_H__ */
