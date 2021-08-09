/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2020 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id: chip_serial.c 210 2020-02-07 10:55:38Z ertl-komori $
 */

/*
 * シリアルインタフェースドライバのチップ依存部（RP2040用）
 * （非TECS版専用）
 */

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/resets.h"
#include "hardware/structs/padsbank0.h"
#include "tusb.h"
#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "target_serial.h"
#include "kernel/kernel_impl.h"

struct sio_port_control_block {
    intptr_t exinf;
    bool_t tx_int_enabled;
    bool_t rx_int_enabled;
};

static struct sio_port_control_block siopcb;

extern void _retrieve_unique_id_on_boot(void);
extern void __aeabi_mem_init(void);

/*
 * SIOポートのオープン
 * 本来この関数は logtask_main() または main_task()
 * -> serial_opn_por() -> sio_opn_por() のように
 * 呼び出される．また，serial_opn_por() はディスパッチ
 * 保留状態で呼び出すことができない．
 * ところが，カーネル起動メッセージ (banner) はカーネル
 * 起動前に target_fput_log() を呼び出す．そのためこれ
 * より前に SIO ポートを準備しておく必要があるから，
 * software_init_hook() から sio_opn_por() を呼び
 * 出している．さらにカーネル起動後シリアルドライバから
 * 通常通りもう一度 sio_opn_por() が呼び出されるが，
 * このときにハードウェアを再設定してはならない．しかし，
 * シリアルドライバ側が必要とする exinf は保存する必要
 *　がある．
 */
SIOPCB *sio_opn_por(ID siopid, intptr_t exinf)
{
    /*
     * 一度目，二度目の呼び出しどちらでもこれらコードは実行する．
     */
    siopcb.exinf = exinf;
    siopcb.tx_int_enabled = false;
    siopcb.rx_int_enabled = false;
    if (tud_cdc_connected()) {
        /* 二度目の呼び出しであったらリターン */
        return &siopcb;
    }

    /* USB の PLL とクロックの設定 */
    pll_init(pll_usb, 1, 480 * MHZ, 5, 2);
    clock_configure(clk_usb,
                    0, // No GLMUX
                    CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);

    /*
     * tinyusb は memset() や memcpy() を利用するが，
     * 標準 C ライブラリの実装では動作しない．代わりに
     * ROM 上にあるサブルーチンを呼び出す．ラッパーの実装
     * は mem_ops_aeabi.S にあるが，その初期化を行う
     * 必要がある．
     */
    __aeabi_mem_init();
    /*
     * GPIO26 から 29 はリセット時入力設定になっているが，
     * これらポートの入力設定を無効にしないとなぜか USB が
     * 動作しない．
     */
    hw_clear_alias(padsbank0_hw)->io[26] = hw_clear_alias(padsbank0_hw)->io[27] =
    hw_clear_alias(padsbank0_hw)->io[28] = hw_clear_alias(padsbank0_hw)->io[29] = PADS_BANK0_GPIO0_IE_BITS;
    /*
     * unique_id.c にある初期化処理
     */
    _retrieve_unique_id_on_boot();

    tusb_init();

    while (! tud_cdc_connected()) {
        /*
         * ここで接続を待たなかった場合，target_fput_log()
         * が無限ループすることになる．target_fput_log() は
         * 全割り込み禁止状態で呼び出されるから USB の接続処理
         * も行うことができず，プログラムがスタックしてしまう．
         */
        tud_task();
        /*
         * この時点では全ての割り込みが禁止されているので，
         * 割り込みフラグを調べてハンドラを呼び出すことで
         * 割り込みをエミュレートする．
         */
        if (probe_int(USART_INTNO)) {
            dcd_rp2040_irq();
        }
    }
    return &siopcb;
}

/*
 * SIOポートのクローズ
 */
void sio_cls_por(SIOPCB *p_siopcb)
{
}

/*
 * SIOポートへの文字送信
 */
bool_t sio_snd_chr(SIOPCB *p_siopcb, char c)
{
    if (tud_cdc_write_available()) {
        tud_cdc_write_char(c);
        tud_cdc_write_flush();
        return true;
    }
    return false;
}

/*
 * SIOポートからの文字受信
 */
int_t sio_rcv_chr(SIOPCB *p_siopcb)
{
    if (tud_cdc_available()) {
        return tud_cdc_read_char();
    }
    return -1;
}

/*
 * SIOポートからのコールバックの許可
 */
void sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            p_siopcb->tx_int_enabled = true;
            break;
        case SIO_RDY_RCV:
            p_siopcb->rx_int_enabled = true;
            break;
    }
}

/*
 * SIOポートからのコールバックの禁止
 */
void sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            p_siopcb->tx_int_enabled = false;
            break;
        case SIO_RDY_RCV:
            p_siopcb->rx_int_enabled = false;
            break;
    }
}

/*
 * SIOポートへの文字出力
 */
void target_fput_log(char c)
{
    if (c == '\n') {
        while (! sio_snd_chr(&siopcb, '\r')) ;
    }
    while (! sio_snd_chr(&siopcb, c)) ;
}

void tud_cdc_tx_complete_cb(uint8_t itf)
{
    if (siopcb.tx_int_enabled) {
        /*
         *  送信可能コールバックルーチンを呼び出す．
         */
        sio_irdy_snd(siopcb.exinf);
    }
}

void tud_cdc_rx_cb(uint8_t itf)
{
    if (siopcb.rx_int_enabled) {
        /*
         *  受信通知コールバックルーチンを呼び出す．
         */
        sio_irdy_rcv(siopcb.exinf);
    }
}

/*
 * USBデバイスタスク
 */
void sio_usbd_task(intptr_t exinf)
{
    while (1) {
        tud_task();
        dly_tsk(10000);
    }
}
