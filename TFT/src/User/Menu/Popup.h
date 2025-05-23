#ifndef _POPUP_H_
#define _POPUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"
#include "GUI.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

const GUI_RECT * dialogGetDoubleBtnRect(void);
const BUTTON * dialogGetBottomDoubleBtn(void);

void _setDialogTitleStr(const char * str);
void _setDialogMsgStr(const char * str);
const char * getDialogMsgStr(void);
void _setDialogOkTextStr(const char * str);
void _setDialogCancelTextStr(const char * str);

void _setDialogTitleLabel(int16_t index);
void _setDialogMsgLabel(int16_t index);
void _setDialogOkTextLabel(int16_t index);
void _setDialogCancelTextLabel(int16_t index);

#define setDialogTitle(x)      _Generic(((x + 0)), const char *: _setDialogTitleStr, \
                                                         char *: _setDialogTitleStr, \
                                                        default: _setDialogTitleLabel)(x)
#define setDialogMsg(x)        _Generic(((x + 0)), const char *: _setDialogMsgStr, \
                                                         char *: _setDialogMsgStr, \
                                                        default: _setDialogMsgLabel)(x)
#define setDialogOkText(x)     _Generic(((x + 0)), const char *: _setDialogOkTextStr, \
                                                         char *: _setDialogOkTextStr, \
                                                        default: _setDialogOkTextLabel)(x)
#define setDialogCancelText(x) _Generic(((x + 0)), const char *: _setDialogCancelTextStr, \
                                                         char *: _setDialogCancelTextStr, \
                                                        default: _setDialogCancelTextLabel)(x)

// set text from LABEL index or pointer (char *)
#define setDialogText(title, msg, oktext, canceltext) \
  {                                                   \
    setDialogTitle(title);                            \
    setDialogMsg(msg);                                \
    setDialogOkText(oktext);                          \
    setDialogCancelText(canceltext);                  \
  }

void popupDrawPage(DIALOG_TYPE type, BUTTON * btn, const char * title, const char * context, const char * yes, const char * no);
void menuDialog(void);
void showDialog(DIALOG_TYPE type, void (* ok_action)(void), void (* cancel_action)(void), void (* loop_action)(void));
void loopPopup(void);

/**
 * @brief Displays a popup for a dialog, it needs user interaction to close it
 *
 * @param type the type of the dialog (alert, question, error, etc)
 * @param title title of the message box
 * @param msg the body of the dialog
 * @param oktext the text to be displayed on the "OK" button
 * @param canceltext the text to be displayed on the "cancel" button
 * @param ok_action the action to be taken if "OK" button is pressed
 * @param cancel_action the action to be taken if "Cancel" button is pressed
 * @param loop_action the action to be taken while the dialog is active (visible/not answered)
 */
#define popupDialog(_type, _title, _msg, _oktext, _canceltext, _ok_action, _cancel_action, _loop_action) \
  {                                                                                                      \
    setDialogText(_title, _msg, _oktext, _canceltext);                                                   \
    showDialog(_type, _ok_action, _cancel_action, _loop_action);                                         \
  }

/**
 * @brief Displays a popup for a reminder, it needs user confirmation to close it
 *
 * @param type the type of the reminder (info, alert, error, etc)
 * @param title title of the message box
 * @param msg the body of the message/reminder to be displayed
 */
#define popupReminder(_type, _title, _msg)                  \
  {                                                         \
    setDialogText(_title, _msg, LABEL_CONFIRM, LABEL_NULL); \
    showDialog(_type, NULL, NULL, NULL);                    \
  }

/**
 * @brief Displays a popup with a message, the user cannot close it
 *
 * @param type the type of the message (info, alert, error, etc)
 * @param title title of the message box
 * @param msg the body of the message/reminder to be displayed
 */
#define popupSplash(_type, _title, _msg)                 \
  {                                                      \
    setDialogText(_title, _msg, LABEL_NULL, LABEL_NULL); \
    showDialog(_type, NULL, NULL, NULL);                 \
  }

#ifdef __cplusplus
}
#endif

#endif
