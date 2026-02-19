/*
 * gtk3-dialog.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-dialog.h"

// ---------------------------------------------------------------------------
// gtk3::DialogTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct DialogTraits
    : BasicTraits
{
    static auto create_dialog() -> GtkWidget*
    {
        return nullptr;
    }

    static auto dispatch(DialogListener& listener, const int response) -> int
    {
        switch(response) {
            case GTK_RESPONSE_NONE:
                listener.on_response_none();
                break;
            case GTK_RESPONSE_REJECT:
                listener.on_response_reject();
                break;
            case GTK_RESPONSE_ACCEPT:
                listener.on_response_accept();
                break;
            case GTK_RESPONSE_DELETE_EVENT:
                listener.on_response_delete_event();
                break;
            case GTK_RESPONSE_OK:
                listener.on_response_ok();
                break;
            case GTK_RESPONSE_CANCEL:
                listener.on_response_cancel();
                break;
            case GTK_RESPONSE_CLOSE:
                listener.on_response_close();
                break;
            case GTK_RESPONSE_YES:
                listener.on_response_yes();
                break;
            case GTK_RESPONSE_NO:
                listener.on_response_no();
                break;
            case GTK_RESPONSE_APPLY:
                listener.on_response_apply();
                break;
            case GTK_RESPONSE_HELP:
                listener.on_response_help();
                break;
            default:
                listener.on_response(response);
                break;
        }
        return response;
    }

    static auto run(Dialog& dialog) -> int
    {
        if(dialog) {
            return ::gtk_dialog_run(dialog);
        }
        return 0;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::DialogTraits;

}

// ---------------------------------------------------------------------------
// gtk3::DialogListener
// ---------------------------------------------------------------------------

namespace gtk3 {

auto DialogListener::on_response(const int response) -> void
{
}

auto DialogListener::on_response_none() -> void
{
    on_response(GTK_RESPONSE_NONE);
}

auto DialogListener::on_response_reject() -> void
{
    on_response(GTK_RESPONSE_REJECT);
}

auto DialogListener::on_response_accept() -> void
{
    on_response(GTK_RESPONSE_ACCEPT);
}

auto DialogListener::on_response_delete_event() -> void
{
    on_response(GTK_RESPONSE_DELETE_EVENT);
}

auto DialogListener::on_response_ok() -> void
{
    on_response(GTK_RESPONSE_OK);
}

auto DialogListener::on_response_cancel() -> void
{
    on_response(GTK_RESPONSE_CANCEL);
}

auto DialogListener::on_response_close() -> void
{
    on_response(GTK_RESPONSE_CLOSE);
}

auto DialogListener::on_response_yes() -> void
{
    on_response(GTK_RESPONSE_YES);
}

auto DialogListener::on_response_no() -> void
{
    on_response(GTK_RESPONSE_NO);
}

auto DialogListener::on_response_apply() -> void
{
    on_response(GTK_RESPONSE_APPLY);
}

auto DialogListener::on_response_help() -> void
{
    on_response(GTK_RESPONSE_HELP);
}

}

// ---------------------------------------------------------------------------
// gtk3::Dialog
// ---------------------------------------------------------------------------

namespace gtk3 {

Dialog::Dialog()
    : Dialog(traits::create_dialog())
{
}

Dialog::Dialog(GtkWidget* instance)
    : Window(instance)
    , DialogListener()
    , _dialog_listener(*this)
{
}

auto Dialog::run() -> int
{
    return traits::dispatch(_dialog_listener, traits::run(*this));
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
