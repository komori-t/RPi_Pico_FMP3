/*
 * シリアルインタフェースドライバのターゲット依存部（非TECS版専用）
 *
 * $Id: target_serial.h 210 2020-02-07 10:55:38Z ertl-komori $
 */

#ifndef TOPPERS_TARGET_SERIAL_H
#define TOPPERS_TARGET_SERIAL_H

#include "rpi_pico.h"

/*
 * USART関連の定義
 */
#define USART_INTNO  (RP2040_UART0_IRQn + 16)
#define USART_INTPRI (TMAX_INTPRI - 1)
#define USART_ISRPRI 1

/*
 * ボーレート
 */
#define BPS_SETTING  (115200)

/*
 * チップで共通な定義
 */
#include "chip_serial.h"

#endif /* TOPPERS_TARGET_SERIAL_H */
