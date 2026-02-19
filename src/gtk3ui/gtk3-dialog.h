/*
 * gtk3-dialog.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_DIALOG_H__
#define __GTK3_CXX_DIALOG_H__

#include <gtk3ui/gtk3-window.h>

// ---------------------------------------------------------------------------
// gtk3::DialogListener
// ---------------------------------------------------------------------------

namespace gtk3 {

class DialogListener
{
public: // public interface
    DialogListener() = default;

    DialogListener(const DialogListener&) = delete;

    DialogListener& operator=(const DialogListener&) = delete;

    virtual ~DialogListener() = default;

    virtual void on_response(const int response);

    virtual void on_response_none();

    virtual void on_response_reject();

    virtual void on_response_accept();

    virtual void on_response_delete_event();

    virtual void on_response_ok();

    virtual void on_response_cancel();

    virtual void on_response_close();

    virtual void on_response_yes();

    virtual void on_response_no();

    virtual void on_response_apply();

    virtual void on_response_help();
};

}

// ---------------------------------------------------------------------------
// gtk3::Dialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class Dialog
    : public Window
    , public DialogListener
{
public: // public interface
    Dialog();

    Dialog(GtkWidget*);

    virtual ~Dialog() = default;

    operator GtkDialog*() const
    {
        return GTK_DIALOG(_instance);
    }

    DialogListener& dialog_listener() const
    {
        return _dialog_listener;
    }

    virtual int run();

protected: // protected data
    DialogListener& _dialog_listener;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_DIALOG_H__ */
