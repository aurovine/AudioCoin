#ifndef GUICONSTANTS_H
#define GUICONSTANTS_H

/* Milliseconds between model updates */
static const int MODEL_UPDATE_DELAY = 250;

/* AskPassphraseDialog -- Maximum passphrase length */
static const int MAX_PASSPHRASE_SIZE = 1024;

/* BitcoinGUI -- Size of icons in status bar */
static const int STATUSBAR_ICONSIZE = 18;

/* Invalid field background style */
#define STYLE_INVALID "background:#FF8080;"

/* Base CSS for inputs */
#define INPUT_STYLE "QLineEdit { font-family: 'Open Sans'; font-size: 13px; border: none; border-bottom: 2px solid #3cb0e5; padding: 3px 5px; background: transparent; } QLineEdit:disabled { border-color: #899399; }"
#define INPUT_STYLE_INVALID "QLineEdit { font-family: 'Open Sans'; font-size: 13px; border: none; border-bottom: 2px solid red; padding: 3px 5px; background: transparent; } QLineEdit:disabled { border-color: #899399; }"

#define TOOLTIP_STYLE "QToolTip { border: none; padding: 5px; border-radius: 3px; background-color: #3cb0e5; color: white; }"

#define WT_BUTTON_STYLE "QCustomButton { font-family: 'Montserrat'; text-transform: uppercase; font-size: 10px; height: 25px; color: black; border: none; outline: none; margin: 0; padding: 5px 15px; background-color: white; } QCustomButton:hover { background-color: #3cb0e5; color: white; fill: white; } QCustomButton:disabled { color: gray; } QCustomButton:pressed { padding: 6px 15px 4px 15px; } #confirmButton, #signConfirmButton, #verifyConfirmButton, #saveButton { background-color: #8BC34A; color: white; } #confirmButton:hover, #signConfirmButton:hover, #verifyConfirmButton:hover, #saveButton:hover { background-color: #3cb0e5; }"
#define BT_BUTTON_STYLE "QCustomButton { font-family: 'Montserrat'; text-transform: uppercase; font-size: 10px; height: 25px; color: white; border: none; outline: none; margin: 0; padding: 5px 15px; background-color: black; } QCustomButton:hover { background-color: #3cb0e5; color: white; fill: white; } QCustomButton:disabled { color: gray; } QCustomButton:pressed { padding: 6px 15px 4px 15px; } #confirmButton, #signConfirmButton, #verifyConfirmButton, #saveButton { background-color: #8BC34A; color: white; } #confirmButton:hover, #signConfirmButton:hover, #verifyConfirmButton:hover, #saveButton:hover { background-color: #3cb0e5; }"

#define COLOR_POSITIVE QColor(139, 195, 74)
/* Transaction list -- unconfirmed transaction */
#define COLOR_UNCONFIRMED QColor(128, 128, 128)
/* Transaction list -- negative amount */
#define COLOR_NEGATIVE QColor(244, 67, 54)
/* Transaction list -- bare address (without label) */
#define COLOR_BAREADDRESS QColor(140, 140, 140)

/* Tooltips longer than this (in characters) are converted into rich text,
   so that they can be word-wrapped.
 */
static const int TOOLTIP_WRAP_THRESHOLD = 80;

/* Maximum allowed URI length */
static const int MAX_URI_LENGTH = 255;

/* QRCodeDialog -- size of exported QR Code image */
#define EXPORT_IMAGE_SIZE 256

#endif // GUICONSTANTS_H
