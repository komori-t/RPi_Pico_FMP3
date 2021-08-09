/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PICO_H_
#define PICO_H_

/** \file pico.h
*  \defgroup pico_base pico_base
*
* Core types and macros for the Raspberry Pi Pico SDK. This header is intended to be included by all source code
*/

/*
 * ライブラリ内で static_assert が使用されているので，
 * assert.h のインクルードは必須．ところが，標準 C
 * ライブラリにおける assert() の実装を利用すると，
 * _read, _write, _sbrk 等のシステムコールが必要と
 * なり面倒である．そこで，assert.h をインクルードした
 * 直後に t_stddef.h をインクルードすることで TOPPERS
 * による assert() マクロの実装に上書きする．
 */
#include <assert.h>
#include <t_stddef.h>
#include <t_syslog.h>

#include "pico/types.h"
#include "pico/version.h"
#include "pico/config.h"
#include "pico/platform.h"
#include "pico/error.h"

#endif
