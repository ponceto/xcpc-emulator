/*
 * gtk3-message-dialog.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_MESSAGE_DIALOG_H__
#define __GTK3_CXX_MESSAGE_DIALOG_H__

#include <gtk3ui/gtk3-dialog.h>

// ---------------------------------------------------------------------------
// gtk3::MessageDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class MessageDialog
    : public Dialog
{
public: // public interface
    MessageDialog();

    MessageDialog(GtkWidget*);

    MessageDialog(const MessageDialog&) = delete;

    MessageDialog& operator=(const MessageDialog&) = delete;

    virtual ~MessageDialog() = default;

    operator GtkMessageDialog*() const
    {
        return GTK_MESSAGE_DIALOG(_instance);
    }

    void set_primary_markup(const std::string& text);

    void set_secondary_markup(const std::string& text);
};

}

// ---------------------------------------------------------------------------
// gtk3::MessageInfoDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class MessageInfoDialog
    : public MessageDialog
{
public: // public interface
    MessageInfoDialog();

    MessageInfoDialog(GtkWidget*);

    virtual ~MessageInfoDialog() = default;

    void create_message_info_dialog();
};

}

// ---------------------------------------------------------------------------
// gtk3::MessageWarningDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class MessageWarningDialog
    : public MessageDialog
{
public: // public interface
    MessageWarningDialog();

    MessageWarningDialog(GtkWidget*);

    virtual ~MessageWarningDialog() = default;

    void create_message_warning_dialog();
};

}

// ---------------------------------------------------------------------------
// gtk3::MessageQuestionDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class MessageQuestionDialog
    : public MessageDialog
{
public: // public interface
    MessageQuestionDialog();

    MessageQuestionDialog(GtkWidget*);

    virtual ~MessageQuestionDialog() = default;

    void create_message_question_dialog();
};

}

// ---------------------------------------------------------------------------
// gtk3::MessageErrorDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class MessageErrorDialog
    : public MessageDialog
{
public: // public interface
    MessageErrorDialog();

    MessageErrorDialog(GtkWidget*);

    virtual ~MessageErrorDialog() = default;

    void create_message_error_dialog();
};

}

// ---------------------------------------------------------------------------
// gtk3::MessageOtherDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class MessageOtherDialog
    : public MessageDialog
{
public: // public interface
    MessageOtherDialog();

    MessageOtherDialog(GtkWidget*);

    virtual ~MessageOtherDialog() = default;

    void create_message_other_dialog();
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_MESSAGE_DIALOG_H__ */
