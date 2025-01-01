/*
 * gtk3-dialog.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
    static GtkWidget* create_dialog()
    {
        return nullptr;
    }

    static int dispatch(DialogListener& listener, const int response)
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

    static int run(Dialog& dialog)
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

void DialogListener::on_response(const int response)
{
}

void DialogListener::on_response_none()
{
    on_response(GTK_RESPONSE_NONE);
}

void DialogListener::on_response_reject()
{
    on_response(GTK_RESPONSE_REJECT);
}

void DialogListener::on_response_accept()
{
    on_response(GTK_RESPONSE_ACCEPT);
}

void DialogListener::on_response_delete_event()
{
    on_response(GTK_RESPONSE_DELETE_EVENT);
}

void DialogListener::on_response_ok()
{
    on_response(GTK_RESPONSE_OK);
}

void DialogListener::on_response_cancel()
{
    on_response(GTK_RESPONSE_CANCEL);
}

void DialogListener::on_response_close()
{
    on_response(GTK_RESPONSE_CLOSE);
}

void DialogListener::on_response_yes()
{
    on_response(GTK_RESPONSE_YES);
}

void DialogListener::on_response_no()
{
    on_response(GTK_RESPONSE_NO);
}

void DialogListener::on_response_apply()
{
    on_response(GTK_RESPONSE_APPLY);
}

void DialogListener::on_response_help()
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

int Dialog::run()
{
    return traits::dispatch(_dialog_listener, traits::run(*this));
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
