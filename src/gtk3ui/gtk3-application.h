/*
 * gtk3-application.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_APPLICATION_H__
#define __GTK3_CXX_APPLICATION_H__

#include <gtk3ui/gtk3-base.h>

// ---------------------------------------------------------------------------
// gtk3::ApplicationListener
// ---------------------------------------------------------------------------

namespace gtk3 {

class ApplicationListener
{
public: // public interface
    ApplicationListener() = default;

    ApplicationListener(const ApplicationListener&) = delete;

    ApplicationListener& operator=(const ApplicationListener&) = delete;

    virtual ~ApplicationListener() = default;

    virtual auto on_open(GFile** files, int num_files) -> void;

    virtual auto on_startup() -> void;

    virtual auto on_shutdown() -> void;

    virtual auto on_activate() -> void;
};

}

// ---------------------------------------------------------------------------
// gtk3::Application
// ---------------------------------------------------------------------------

namespace gtk3 {

class Application
    : public ApplicationListener
{
public: // public interface
    Application(const std::string& app_id);

    Application(ApplicationListener&, const std::string& app_id);

    Application(GtkApplication*);

    Application(GtkApplication*, ApplicationListener&);

    virtual ~Application();

    operator bool() const
    {
        return _instance != nullptr;
    }

    operator GApplication*() const
    {
        return G_APPLICATION(_instance);
    }

    operator GtkApplication*() const
    {
        return GTK_APPLICATION(_instance);
    }

    GtkApplication* operator*() const
    {
        return _instance;
    }

    ApplicationListener& listener() const
    {
        return _listener;
    }

    auto create_application(const std::string& app_id) -> void;

    virtual auto run(int argc, char* argv[]) -> int;

protected: // protected data
    GtkApplication*      _instance;
    ApplicationListener& _listener;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_APPLICATION_H__ */
