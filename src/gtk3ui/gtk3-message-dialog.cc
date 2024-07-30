/*
 * gtk3-message-dialog.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include "gtk3-message-dialog.h"

// ---------------------------------------------------------------------------
// gtk3::MessageDialogTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct MessageDialogTraits
    : BasicTraits
{
    static GtkWidget* create_message_dialog()
    {
        return nullptr;
    }

    static GtkWidget* create_message_info_dialog()
    {
        return ::gtk_message_dialog_new_with_markup ( nullptr
                                                    , GTK_DIALOG_MODAL
                                                    , GTK_MESSAGE_INFO
                                                    , GTK_BUTTONS_OK
                                                    , nullptr );
    }

    static GtkWidget* create_message_warning_dialog()
    {
        return ::gtk_message_dialog_new_with_markup ( nullptr
                                                    , GTK_DIALOG_MODAL
                                                    , GTK_MESSAGE_WARNING
                                                    , GTK_BUTTONS_OK
                                                    , nullptr );
    }

    static GtkWidget* create_message_question_dialog()
    {
        return ::gtk_message_dialog_new_with_markup ( nullptr
                                                    , GTK_DIALOG_MODAL
                                                    , GTK_MESSAGE_QUESTION
                                                    , GTK_BUTTONS_YES_NO
                                                    , nullptr );
    }

    static GtkWidget* create_message_error_dialog()
    {
        return ::gtk_message_dialog_new_with_markup ( nullptr
                                                    , GTK_DIALOG_MODAL
                                                    , GTK_MESSAGE_ERROR
                                                    , GTK_BUTTONS_OK
                                                    , nullptr );
    }

    static GtkWidget* create_message_other_dialog()
    {
        return ::gtk_message_dialog_new_with_markup ( nullptr
                                                    , GTK_DIALOG_MODAL
                                                    , GTK_MESSAGE_OTHER
                                                    , GTK_BUTTONS_CLOSE
                                                    , nullptr );
    }

    static void set_primary_markup(MessageDialog& message_dialog, const std::string& text)
    {
        if(message_dialog) {
            ::gtk_message_dialog_set_markup(message_dialog, text.c_str());
        }
    }

    static void set_secondary_markup(MessageDialog& message_dialog, const std::string& text)
    {
        if(message_dialog) {
            ::gtk_message_dialog_format_secondary_markup(message_dialog, "%s", text.c_str());
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::MessageDialogTraits;

}

// ---------------------------------------------------------------------------
// gtk3::MessageDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

MessageDialog::MessageDialog()
    : MessageDialog(traits::create_message_dialog())
{
}

MessageDialog::MessageDialog(GtkWidget* instance)
    : Dialog(instance)
{
}

void MessageDialog::set_primary_markup(const std::string& text)
{
    return traits::set_primary_markup(*this, text);
}

void MessageDialog::set_secondary_markup(const std::string& text)
{
    return traits::set_secondary_markup(*this, text);
}

}

// ---------------------------------------------------------------------------
// gtk3::MessageInfoDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

MessageInfoDialog::MessageInfoDialog()
    : MessageInfoDialog(traits::create_message_info_dialog())
{
}

MessageInfoDialog::MessageInfoDialog(GtkWidget* instance)
    : MessageDialog(instance)
{
}

void MessageInfoDialog::create_message_info_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_message_info_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::MessageWarningDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

MessageWarningDialog::MessageWarningDialog()
    : MessageWarningDialog(traits::create_message_warning_dialog())
{
}

MessageWarningDialog::MessageWarningDialog(GtkWidget* instance)
    : MessageDialog(instance)
{
}

void MessageWarningDialog::create_message_warning_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_message_warning_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::MessageQuestionDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

MessageQuestionDialog::MessageQuestionDialog()
    : MessageQuestionDialog(traits::create_message_question_dialog())
{
}

MessageQuestionDialog::MessageQuestionDialog(GtkWidget* instance)
    : MessageDialog(instance)
{
}

void MessageQuestionDialog::create_message_question_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_message_question_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::MessageErrorDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

MessageErrorDialog::MessageErrorDialog()
    : MessageErrorDialog(traits::create_message_error_dialog())
{
}

MessageErrorDialog::MessageErrorDialog(GtkWidget* instance)
    : MessageDialog(instance)
{
}

void MessageErrorDialog::create_message_error_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_message_error_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::MessageOtherDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

MessageOtherDialog::MessageOtherDialog()
    : MessageOtherDialog(traits::create_message_other_dialog())
{
}

MessageOtherDialog::MessageOtherDialog(GtkWidget* instance)
    : MessageDialog(instance)
{
}

void MessageOtherDialog::create_message_other_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_message_other_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
