/*
 * gtk3-application.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "gtk3-application.h"

// ---------------------------------------------------------------------------
// gtk3::ApplicationTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ApplicationTraits
    : BasicTraits
{
    static GtkApplication* create_application(const std::string& app_id)
    {
#if 1
        constexpr auto flags = G_APPLICATION_HANDLES_OPEN;
#else
#if GLIB_CHECK_VERSION(2, 74, 0)
        constexpr auto flags = G_APPLICATION_DEFAULT_FLAGS;
#else
        constexpr auto flags = G_APPLICATION_FLAGS_NONE;
#endif
#endif
        return ::gtk_application_new(app_id.c_str(), flags);
    }

    static auto initialize(Application& application, GtkApplication*& instance) -> void
    {
        if(instance != nullptr) {
            signal_connect(instance, sig_open    , G_CALLBACK(&on_open    ), &application);
            signal_connect(instance, sig_startup , G_CALLBACK(&on_startup ), &application);
            signal_connect(instance, sig_shutdown, G_CALLBACK(&on_shutdown), &application);
            signal_connect(instance, sig_activate, G_CALLBACK(&on_activate), &application);
        }
    }

    static auto finalize(Application& application, GtkApplication*& instance) -> void
    {
        if(instance != nullptr) {
            instance = (::g_object_unref(instance), nullptr);
        }
    }

    static auto run(Application& application, int argc, char* argv[]) -> int
    {
        if(application) {
            return ::g_application_run(application, argc, argv);
        }
        return EXIT_FAILURE;
    }

    static auto on_open(GApplication* object, GFile** files, int num_files, char* hint, Application* application) -> void
    {
        ApplicationListener& listener(application->listener());

        return listener.on_open(files, num_files);
    }

    static auto on_startup(GApplication* object, Application* application) -> void
    {
        ApplicationListener& listener(application->listener());

        return listener.on_startup();
    }

    static auto on_shutdown(GApplication* object, Application* application) -> void
    {
        ApplicationListener& listener(application->listener());

        return listener.on_shutdown();
    }

    static auto on_activate(GApplication* object, Application* application) -> void
    {
        ApplicationListener& listener(application->listener());

        return listener.on_activate();
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ApplicationTraits;

}

// ---------------------------------------------------------------------------
// gtk3::ApplicationListener
// ---------------------------------------------------------------------------

namespace gtk3 {

auto ApplicationListener::on_open(GFile** files, int num_files) -> void
{
}

auto ApplicationListener::on_startup() -> void
{
}

auto ApplicationListener::on_shutdown() -> void
{
}

auto ApplicationListener::on_activate() -> void
{
}

}

// ---------------------------------------------------------------------------
// gtk3::Application
// ---------------------------------------------------------------------------

namespace gtk3 {

Application::Application(const std::string& app_id)
    : Application(traits::create_application(app_id), *this)
{
}

Application::Application(ApplicationListener& listener, const std::string& app_id)
    : Application(traits::create_application(app_id), listener)
{
}

Application::Application(GtkApplication* instance)
    : Application(instance, *this)
{
}

Application::Application(GtkApplication* instance, ApplicationListener& listener)
    : ApplicationListener()
    , _instance(instance)
    , _listener(listener)
{
    traits::initialize(*this, _instance);
}

Application::~Application()
{
    traits::finalize(*this, _instance);
}

auto Application::create_application(const std::string& app_id) -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_application(app_id);
        traits::initialize(*this, _instance);
    }
}

auto Application::run(int argc, char* argv[]) -> int
{
    return traits::run(*this, argc, argv);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
