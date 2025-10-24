#ifndef M68KEYMAP_H
#define M68KEYMAP_H

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// キーマップ定義
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// USB HID Keycode (標準USB HID Usage ID for Keyboard/Keypad Page)
// -----------------------------------------------------------------------------

// 特殊キー
#define USB_KEY_ESC         0x29 // ESCキー
#define USB_KEY_TAB         0x2B // TABキー
#define USB_KEY_SPACE       0x2C // スペースキー
#define USB_KEY_RETURN      0x28 // リターンキー(Enter)
#define USB_KEY_DELETE      0x4C // デリートキー
#define USB_KEY_BACKSPACE   0x2A // バックスペースキー

// Modifierキー
#define USB_KEY_LEFT_CTRL   0xE0 // 左コントロールキー
#define USB_KEY_LEFT_SHIFT  0xE1 // 左シフトキー
#define USB_KEY_LEFT_ALT    0xE2 // 左ALTキー
#define USB_KEY_LEFT_GUI    0xE3 // 左GUIキー(Windows/Command)
#define USB_KEY_RIGHT_CTRL  0xE4 // 右コントロールキー
#define USB_KEY_RIGHT_SHIFT 0xE5 // 右シフトキー
#define USB_KEY_RIGHT_ALT   0xE6 // 右ALTキー
#define USB_KEY_RIGHT_GUI   0xE7 // 右GUIキー

// ファンクションキー
#define USB_KEY_F1          0x3A // F1キー
#define USB_KEY_F2          0x3B // F2キー
#define USB_KEY_F3          0x3C // F3キー
#define USB_KEY_F4          0x3D // F4キー
#define USB_KEY_F5          0x3E // F5キー
#define USB_KEY_F6          0x3F // F6キー
#define USB_KEY_F7          0x40 // F7キー
#define USB_KEY_F8          0x41 // F8キー
#define USB_KEY_F9          0x42 // F9キー
#define USB_KEY_F10         0x43 // F10キー
#define USB_KEY_F11         0x44 // F11キー
#define USB_KEY_F12         0x45 // F12キー

// 矢印キー
#define USB_KEY_RIGHT       0x4F // 右矢印キー
#define USB_KEY_LEFT        0x50 // 左矢印キー
#define USB_KEY_DOWN        0x51 // 下矢印キー
#define USB_KEY_UP          0x52 // 上矢印キー

// 数字キー(メインキーボード)
#define USB_KEY_1           0x1E // 1キー
#define USB_KEY_2           0x1F // 2キー
#define USB_KEY_3           0x20 // 3キー
#define USB_KEY_4           0x21 // 4キー
#define USB_KEY_5           0x22 // 5キー
#define USB_KEY_6           0x23 // 6キー
#define USB_KEY_7           0x24 // 7キー
#define USB_KEY_8           0x25 // 8キー
#define USB_KEY_9           0x26 // 9キー
#define USB_KEY_0           0x27 // 0キー

// アルファベットキー
#define USB_KEY_a           0x04 // aキー
#define USB_KEY_b           0x05 // bキー
#define USB_KEY_c           0x06 // cキー
#define USB_KEY_d           0x07 // dキー
#define USB_KEY_e           0x08 // eキー
#define USB_KEY_f           0x09 // fキー
#define USB_KEY_g           0x0A // gキー
#define USB_KEY_h           0x0B // hキー
#define USB_KEY_i           0x0C // iキー
#define USB_KEY_j           0x0D // jキー
#define USB_KEY_k           0x0E // kキー
#define USB_KEY_l           0x0F // lキー
#define USB_KEY_m           0x10 // mキー
#define USB_KEY_n           0x11 // nキー
#define USB_KEY_o           0x12 // oキー
#define USB_KEY_p           0x13 // pキー
#define USB_KEY_q           0x14 // qキー
#define USB_KEY_r           0x15 // rキー
#define USB_KEY_s           0x16 // sキー
#define USB_KEY_t           0x17 // tキー
#define USB_KEY_u           0x18 // uキー
#define USB_KEY_v           0x19 // vキー
#define USB_KEY_w           0x1A // wキー
#define USB_KEY_x           0x1B // xキー
#define USB_KEY_y           0x1C // yキー
#define USB_KEY_z           0x1D // zキー

// 記号キー (JIS)
#define USB_KEY_MINUS       0x2D // -/=キー(マイナス/アンダースコア)
#define USB_KEY_CARET       0x2E // ^/~キー
#define USB_KEY_ATMARK      0x2F // @/'キー
#define USB_KEY_LEFT_BRACE  0x30 // [/{キー
#define USB_KEY_RIGHT_BRACE 0x32 // ]/}キー
#define USB_KEY_SEMICOLON   0x33 // ;/+キー(セミコロン)
#define USB_KEY_COLON       0x34 // :/*キー(コロン)
#define USB_KEY_KANA        0x35 // Hankaku/Zenkaku
#define USB_KEY_COMMA       0x36 // ,/<キー(カンマ/小なり)
#define USB_KEY_PERIOD      0x37 // ./>キー(ピリオド/大なり)
#define USB_KEY_SLASH       0x38 // //?キー(スラッシュ/クエスチョン)
#define USB_KEY_CAPS        0x39 // CAPSキー(スラッシュ/クエスチョン)
#define USB_KEY_BACKSLASH   0x87 // \/_キー(バックスラッシュ/アンダーバー)


// テンキー
#define USB_KEY_NUM0            0x62 // テンキー0
#define USB_KEY_NUM1            0x59 // テンキー1
#define USB_KEY_NUM2            0x5A // テンキー2
#define USB_KEY_NUM3            0x5B // テンキー3
#define USB_KEY_NUM4            0x5C // テンキー4
#define USB_KEY_NUM5            0x5D // テンキー5
#define USB_KEY_NUM6            0x5E // テンキー6
#define USB_KEY_NUM7            0x5F // テンキー7
#define USB_KEY_NUM8            0x60 // テンキー8
#define USB_KEY_NUM9            0x61 // テンキー9
#define USB_KEY_NUMPAD_DOT      0x63 // テンキー.
#define USB_KEY_NUMPAD_SLASH    0x54 // テンキー/
#define USB_KEY_NUMPAD_ASTERISK 0x55 // テンキー*
#define USB_KEY_NUMPAD_MINUS    0x56 // テンキー-
#define USB_KEY_NUMPAD_PLUS     0x57 // テンキー+
#define USB_KEY_NUMPAD_ENTER    0x58 // テンキーEnter

#define USB_KEY_S1              0x46 // PrintScreen
#define USB_KEY_S2              0x47 // ScreenLock
#define USB_KEY_RESET           0x48 // Pause
#define USB_KEY_LF              0x4A // Home
#define USB_KEY_SMALL           0x4D // End

#define USB_KEY_NUM_E           0xFF
#define USB_KEY_NONE            0xFF


// -----------------------------------------------------------------------------
// キーマトリクス定義
// -----------------------------------------------------------------------------

static const uint16_t SORD_M68_KEY_CODE_MAP[15][8] = {
    // A,                      B,             C,              D,                 E,              F,              G,            H                      // 74LS166にパラレル入力されるデータ。8ビット。A..Hはデータシートで各ピンについている名前。
    { USB_KEY_NONE,            USB_KEY_NONE,  USB_KEY_LEFT_SHIFT, USB_KEY_NONE,       USB_KEY_TAB,        USB_KEY_ESC,       USB_KEY_NONE, USB_KEY_LEFT_CTRL   }, // Col 01 // M68キーボードにおける列。全部で15列。16列は未実装なので、スキャン対象から除外されている。
    { USB_KEY_NONE,            USB_KEY_NONE,  USB_KEY_z,          USB_KEY_a,          USB_KEY_q,          USB_KEY_1,         USB_KEY_NONE, USB_KEY_LEFT_SHIFT  }, // Col 02
    { USB_KEY_DOWN,            USB_KEY_LEFT,  USB_KEY_x,          USB_KEY_s,          USB_KEY_w,          USB_KEY_2,         USB_KEY_NONE, USB_KEY_SMALL       }, // Col 03
    { USB_KEY_UP,              USB_KEY_RIGHT, USB_KEY_c,          USB_KEY_d,          USB_KEY_e,          USB_KEY_3,         USB_KEY_NONE, USB_KEY_RESET       }, // Col 04
    { USB_KEY_NONE,            USB_KEY_NONE,  USB_KEY_v,          USB_KEY_f,          USB_KEY_r,          USB_KEY_4,         USB_KEY_NONE, USB_KEY_S1          }, // Col 05
    { USB_KEY_NUMPAD_ASTERISK, USB_KEY_NONE,  USB_KEY_b,          USB_KEY_g,          USB_KEY_t,          USB_KEY_5,         USB_KEY_NONE, USB_KEY_S2          }, // Col 06
    { USB_KEY_NONE,            USB_KEY_NUM7,  USB_KEY_SPACE,      USB_KEY_h,          USB_KEY_y,          USB_KEY_6,         USB_KEY_NONE, USB_KEY_NONE        }, // Col 07
    { USB_KEY_NUM9,            USB_KEY_NUM8,  USB_KEY_n,          USB_KEY_j,          USB_KEY_u,          USB_KEY_7,         USB_KEY_NONE, USB_KEY_NONE        }, // Col 08
    { USB_KEY_NUM_E,           USB_KEY_NUM4,  USB_KEY_m,          USB_KEY_k,          USB_KEY_i,          USB_KEY_8,         USB_KEY_F1,   USB_KEY_NONE        }, // Col 09
    { USB_KEY_NUM6,            USB_KEY_NUM5,  USB_KEY_COMMA,      USB_KEY_l,          USB_KEY_o,          USB_KEY_9,         USB_KEY_F2,   USB_KEY_NONE        }, // Col 10
    { USB_KEY_NUM3,            USB_KEY_NUM1,  USB_KEY_PERIOD,     USB_KEY_SEMICOLON,  USB_KEY_p,          USB_KEY_0,         USB_KEY_F3,   USB_KEY_NONE        }, // Col 11
    { USB_KEY_NUMPAD_DOT,      USB_KEY_NUM0,  USB_KEY_SLASH,      USB_KEY_COLON,      USB_KEY_ATMARK,     USB_KEY_MINUS,     USB_KEY_F4,   USB_KEY_KANA        }, // Col 12
    { USB_KEY_NONE,            USB_KEY_NONE,  USB_KEY_BACKSLASH,  USB_KEY_RIGHT_BRACE,USB_KEY_LEFT_BRACE, USB_KEY_CARET,     USB_KEY_F5,   USB_KEY_NONE        }, // Col 13
    { USB_KEY_NONE,            USB_KEY_NONE,  USB_KEY_NONE,       USB_KEY_NONE,       USB_KEY_NONE,       USB_KEY_BACKSLASH, USB_KEY_F6,   USB_KEY_NONE        }, // Col 14
    { USB_KEY_NONE,            USB_KEY_NONE,  USB_KEY_NONE,       USB_KEY_LF,         USB_KEY_RETURN,     USB_KEY_BACKSPACE, USB_KEY_F7,   USB_KEY_NONE        }  // Col 15
};

#endif
