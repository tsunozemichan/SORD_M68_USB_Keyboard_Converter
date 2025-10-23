// main.cpp  — pico-sdk (C++) port of m68_ps2converter_twin02.ino

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/timer.h"
#include "hardware/timer.h"
#include "hardware/resets.h"
#include "hardware/watchdog.h"
#include "hardware/uart.h"
#include "pico/multicore.h"
#include "pico/time.h"

#include "simple_pe_counter.pio.h"   // PIO: PE立下りでIRQ発火→driveAH_Direct() 呼び出し:contentReference[oaicite:2]{index=2}
#include "m68keymap.h"               // pico版：PROGMEM/pgm_read_* なし、配列は const に変更:contentReference[oaicite:3]{index=3}

// ---------- Debug pins ----------
static const uint8_t PIN_DBG_PE   = 19;
static const uint8_t PIN_DBG_NEXT = 20;

// ---------- M68 interface pins ----------
static const uint8_t PIN_PE   = 3;   // /PE（15.5µs周期、Low幅≈250ns）1
static const uint8_t PIN_CP   = 4;   // CP（≈4MHz）(未使用だが予約)

static const uint8_t PIN_A = 10;
static const uint8_t PIN_B = 11;
static const uint8_t PIN_C = 12;
static const uint8_t PIN_D = 13;
static const uint8_t PIN_E = 14;
static const uint8_t PIN_F = 15;
static const uint8_t PIN_G = 16;
static const uint8_t PIN_H = 17;

static const uint8_t IC74LVC245_B_TO_A_OE = 2;
static const uint8_t IC74LVC245_A_TO_B_OE = 18;

static const uint8_t ROW_PINS[8] = {
    PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_H
};


// ---------- UART1 (Hardware UART for keycode and modifier) ----------
#define UART_ID uart1
#define UART_BAUD_RATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9

// ====== グローバル（simple_pe_counter.pio.h が参照する extern の実体） ======
volatile uint8_t  column_scan_line = 0;          // 現在の列（0..14）
volatile bool     column_scan_line_updated = false;
volatile uint8_t  columnBits[15];                // 各列の A..H 出力ビット（1=未押下）
volatile uint8_t  bits = 0xFF;                   // 直近出力（使用箇所に合わせて維持）
volatile uint32_t last_pe_time_us = 0;           // 直近PE時刻
volatile bool     pe_signal_active = false;      // PE受信フラグ
// ===================================================

// A..H の8bitを GPIO10..17 に一括出力（100ns級）
inline void driveAH_Direct(uint8_t v) {
    uint32_t gpio_bits = (uint32_t)v << PIN_A;
    const uint32_t PIN_MASK = 0x0003FC00u; // GP10..GP17
    sio_hw->gpio_set =  (gpio_bits)     & PIN_MASK;
    sio_hw->gpio_clr = (~gpio_bits)     & PIN_MASK;
}


// キーコード --> マトリクス位置（SORD_M68_KEY_CODE_MAP を走査）:contentReference[oaicite:4]{index=4}
struct KeyPosition { int8_t col; int8_t row; };

static inline KeyPosition findKeyPosition(uint16_t keycode) {
    for (int col = 0; col < 15; col++) {
        for (int row = 0; row < 8; row++) {
            const uint16_t map_value = SORD_M68_KEY_CODE_MAP[col][row]; // 16bit対応:contentReference[oaicite:5]{index=5}
            if (map_value == keycode) return { (int8_t)col, (int8_t)row };
        }
    }
    return { -1, -1 };
}


// modifierキー
struct ModifierPosition { int8_t col; int8_t row; };

static inline ModifierPosition findModifierPosition(uint16_t modifier) {
    for (int col = 0; col < 15; col++) {
        for (int row = 0; row < 8; row++) {
            const uint16_t map_value = SORD_M68_KEY_CODE_MAP[col][row]; // 16bit対応:contentReference[oaicite:5]{index=5}
            if (map_value == modifier) return { (int8_t)col, (int8_t)row };
        }
    }
    return { -1, -1 };
}


// Mapの row(0..7: A..H) を "そのビットだけ 0" の A..H 出力へ変換
// ※バグ修正：row==0 でも 0xFE を返すのが正しい（従来コードは 0xFF を返していた）
static inline uint8_t rowToAHMask(uint8_t row /*0..7*/) {
    if (row > 7) return 0xFF;
    return (uint8_t)(0xFFu & ~(1u << row));
}

// ---------- UART1からキーコードとmodifier情報を読み取る関数 ----------
// 書式: "0x%02X:0x%02X\n" (例: "0x1C:0x00\n")
// 戻り値: true=正常読み取り, false=データなしまたはエラー
bool readKeycodeFromUART1(uint8_t &keycode, uint8_t &modifier) {
    static char buffer[16];
    static int buf_idx = 0;
    
    // UART1にデータがあるか確認
    while (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        
        // 改行を受信したら解析
        if (c == '\n' || c == '\r') {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                
                // "0x%02X:0x%02X" 形式を解析
                unsigned int key_val, mod_val;
                int parsed = sscanf(buffer, "0x%02X:0x%02X", &key_val, &mod_val);
                
                buf_idx = 0;  // バッファリセット
                
                if (parsed == 2) {
                    keycode = (uint8_t)key_val;
                    modifier = (uint8_t)mod_val;
                    return true;
                }
            }
            buf_idx = 0;
        }
        // バッファに文字を追加
        else if (buf_idx < (int)sizeof(buffer) - 1) {
            buffer[buf_idx++] = c;
        }
        else {
            // バッファオーバーフロー防止
            buf_idx = 0;
        }
    }
    
    return false;
}

// columnBitsの全要素を0xFFで埋める。
inline void clearAllColumnBits() {
    columnBits[0] = 0xFF;
    columnBits[1] = 0xFF;
    columnBits[2] = 0xFF;
    columnBits[3] = 0xFF;
    columnBits[4] = 0xFF;
    columnBits[5] = 0xFF;
    columnBits[6] = 0xFF;
    columnBits[7] = 0xFF;
    columnBits[8] = 0xFF;
    columnBits[9] = 0xFF;
    columnBits[10] = 0xFF;
    columnBits[11] = 0xFF;
    columnBits[12] = 0xFF;
    columnBits[13] = 0xFF;
    columnBits[14] = 0xFF;
}

uint8_t keycode=0;
uint8_t prev_keycode=0;
uint8_t modifier=0;
uint8_t prev_modifier=0;
int8_t last_column = -1, last_row = -1;
int8_t last_modifier_column = -1, last_modifier_row = -1;
uint8_t make_shift_key = 0; // SHIFTキーが押されたら 1 になる。
uint8_t make_ctrl_key = 0; // CTRLキーが押されたら 1 になる。
uint8_t shift_col = 1;
uint8_t shift_row = 7;
uint8_t ctrl_col = 0;
uint8_t ctr_row = 7;

// UART1から受信したキーコードとmodifierをシリアルモニタに出力
void checkAndOutputUART1Data() {
    if (readKeycodeFromUART1(keycode, modifier)) {
        if(keycode == 0){ // キーが離された時に実行。キーを押して、離したときしか入れないので、last_columnが-1になることはない。
                uint8_t rowBits = 0xFF; // ビットをデフォルトに設定。
                columnBits[last_column] = rowBits; // 前回押したキーの列に0xFFを入れて、何も押していない状態に戻す。
            return;
        }else{ // キーが押された時
            // keycode マップ検索（16bit対応）
            KeyPosition pos = findKeyPosition(keycode);
            if (pos.col >= 0) {
                // 前のキーをクリア（新しいキーと同じ列でない場合のみ）
                if (last_column >= 0 && last_column != pos.col) {
                    columnBits[last_column] = 0xFF;
                    // printf("Clear: Col=%d -> A..H=0xFF\n", last_column + 1);
                }
                
                // 新しいキーを設定
                uint8_t rowBits = rowToAHMask((uint8_t)pos.row);
                columnBits[pos.col] = rowBits;
                last_column = pos.col;
                last_row = pos.row;
                // printf("Press: Col=%d Row=%d  A..H=0x%02X\n", pos.col + 1, pos.row + 1, rowBits);
                
                // Modifierキーの実装
                if(modifier == 0x20 || modifier == 0x02 ){ // SHIFTキーが押されている。
                    uint8_t rowBits = rowToAHMask(shift_row);
                    columnBits[shift_col] = rowBits;
                    make_shift_key = 1;
                }
                if(modifier == 0x10 || modifier == 0x01 ){ // CTRLキーが押されている。
                    uint8_t rowBits = rowToAHMask(ctr_row);
                    columnBits[ctrl_col] = rowBits;
                    make_ctrl_key = 1;
                }
                if(modifier == 0 && (make_shift_key == 1 || make_ctrl_key == 1)){ // SHIFTまたはCTRLキーが離された 
                    columnBits[shift_col]= 0xFF; // HIGHに戻す
                    columnBits[ctrl_col]= 0xFF;  // HIGHに戻す。
                    make_shift_key = 0;
                    make_ctrl_key = 0;
                }
            }
       }
    }
}


int main() {
    stdio_init_all();

    // 74LVC245 /OE
    gpio_init(IC74LVC245_B_TO_A_OE); gpio_set_dir(IC74LVC245_B_TO_A_OE, GPIO_OUT);
    gpio_init(IC74LVC245_A_TO_B_OE); gpio_set_dir(IC74LVC245_A_TO_B_OE, GPIO_OUT);
    gpio_put(IC74LVC245_B_TO_A_OE, 0);  // B→A (M68→Pico) 受け 有効
    gpio_put(IC74LVC245_A_TO_B_OE, 1);  // A→B (Pico→M68) 送出 無効（準備まで）

    // ピン初期化
    gpio_init(PIN_PE);      gpio_set_dir(PIN_PE, GPIO_IN);
    gpio_init(PIN_DBG_PE);  gpio_set_dir(PIN_DBG_PE, GPIO_OUT);  gpio_put(PIN_DBG_PE, 0);
    gpio_init(PIN_DBG_NEXT);gpio_set_dir(PIN_DBG_NEXT, GPIO_OUT);gpio_put(PIN_DBG_NEXT, 0);

    // A..H はアイドル High
    for (int i = 0; i < 8; ++i) { gpio_init(ROW_PINS[i]); gpio_set_dir(ROW_PINS[i], GPIO_OUT); gpio_put(ROW_PINS[i], 1); }

    // 列データ初期化（未押下=0xFF）
    for (int col = 0; col < 15; ++col) columnBits[col] = 0xFF;

    // 送出方向を有効化（準備完了）
    gpio_put(IC74LVC245_A_TO_B_OE, 0);

    // PIO設定（/PE の立下りで IRQ→ simple_pe_handler → driveAH_Direct(columnBits[col])）:contentReference[oaicite:6]{index=6}
    setup_simple_pe_counter(PIN_PE, PIN_DBG_NEXT);
    last_pe_time_us = time_us_32();

    // UART1の初期化
    uart_init(UART_ID, UART_BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Watchdog (デバッグ中は無効化)
    if (watchdog_caused_reboot()) {
        // printf("*** M68 RESET DETECTED! ***\n");
        column_scan_line = 0;
    }
    watchdog_enable(20000 /*ms*/, 1); // 20秒

    uint16_t prev_keycode = 0;
    int8_t last_col = -1, last_row = -1;

    // ============ main loop ============
    for (;;) {
        // UART1からキーコード＆modifier受信チェック--->シリアルモニタにkeycodeとmodifierを表示するのみ。
        checkAndOutputUART1Data();

        // Watchdogキック（直近PEから6秒以内のみ）
        // uint32_t now_us = time_us_32();
        // uint32_t elapsed = now_us - last_pe_time_us;
        // if (elapsed < 19000000u) watchdog_update();

        // Watchdogキック（直近PEから6秒以内のみ）
        absolute_time_t now = get_absolute_time();                   // 現在時刻を取得
        float elapsed_sec = absolute_time_diff_us(last_pe_time_us, now) / 1e6;  // 秒単位に変換

        if (elapsed_sec < 18.0f) {
            watchdog_update();   // PEから18秒以内ならキック
        }

        tight_loop_contents();
    }
}
