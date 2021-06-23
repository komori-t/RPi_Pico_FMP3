/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2009-2020 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN  
 *
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
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
 *  @(#) $Id: pcb.h 207 2020-01-30 09:31:28Z ertl-honda $
 */

/*
 *		プロセッサ管理ブロックの定義
 */

#ifndef TOPPERS_PCB_H
#define TOPPERS_PCB_H

#ifndef TOPPERS_MACRO_ONLY
/*
 *  PCBの前方参照（ターゲット依存部用）
 */
typedef struct processor_control_block PCB;

#endif /* TOPPERS_MACRO_ONLY */

#include "target_pcb.h"

#ifndef TOPPERS_MACRO_ONLY

#include <queue.h>

/*
 *  PCB定義のための前方参照
 */
typedef struct task_control_block TCB;
typedef struct spin_lock_initialization_block SPNINIB;
typedef struct time_event_control_block TEVTCB;

/*
 *  プロセッサ管理ブロック（PCB）の定義
 */
struct processor_control_block {
#ifndef OMIT_TARGET_PCB
	/*
	 *  プロセッサ管理ブロックのターゲット依存部
	 */
	TPCB	target_pcb;
#endif /* OMIT_TARGET_PCB */

	/*
	 *  プロセッサID
	 */
	ID		 prcid;

	/*
	 *  実行状態のタスク
	 *
	 *  実行状態のタスク（＝プロセッサがコンテキストを持っているタスク）
	 *  のTCBを指すポインタ．実行状態のタスクがない場合はNULLにする．
	 *
	 *  サービスコールの処理中で，自タスク（サービスコールを呼び出した
	 *  タスク）に関する情報を参照する場合はp_runtskを使う．p_runtskを
	 *  書き換えるのは，ディスパッチャ（と初期化処理）のみである．
	 */
	TCB		*p_runtsk;

	/*
	 *  実行すべきタスク
	 *
	 *  実行すべきタスクのTCBを指すポインタ．実行できるタスクがない場
	 *  合はNULLにする．
	 *
	 *  p_runtskは，通常はp_schedtskと一致しているが，非タスクコンテキ
	 *  スト実行中は，一致しているとは限らない．また，他のプロセッサで
	 *  p_schedtskが変更された後，要求されたプロセッサ間割込みを受け付
	 *  けてディスパッチするまでの間も，p_runtskとp_schedtskは一致しな
	 *  くなる．
	 *
	 *  割込み優先度マスク全解除でない状態の間とディスパッチ禁止状態の
	 *  間（すなわち，dspflgがfalseである間）は，p_schedtskを更新しな
	 *  い．
	 */
	TCB		*p_schedtsk;

	/*
	 *  ディスパッチ許可状態
	 *
	 *  ディスパッチ許可状態であることを示すフラグ．
	 */
	bool_t	enadsp;

	/*
	 *  タスクディスパッチ可能状態
	 *
	 *  割込み優先度マスク全解除状態であり，ディスパッチ許可状態である
	 *  （ディスパッチ禁止状態でない）ことを示すフラグ．ディスパッチ保
	 *  留状態でないことは，タスクコンテキスト実行中で，CPUロック状態
	 *  でなく，dspflgがtrueであることで判別することができる．
	 */
	bool_t	dspflg;

	/*
	 *  レディキュー
	 *
	 *  レディキューは，実行できる状態のタスクを管理するためのキューである．
	 *  実行状態のタスクも管理しているため，レディ（実行可能）キューという
	 *  名称は正確ではないが，レディキューという名称が定着しているため，こ
	 *  の名称で呼ぶことにする．
	 *
	 *  レディキューは，優先度ごとのタスクキューで構成されている．タスクの
	 *  TCBは，該当する優先度のキューに登録される．
	 */
	QUEUE	ready_queue[TNUM_TPRI];

	/*
	 *  レディキューサーチのためのビットマップ
	 *
	 *  レディキューのサーチを効率よく行うために，優先度ごとのタスクキュー
	 *  にタスクが入っているかどうかを示すビットマップを用意している．ビッ
	 *  トマップを使うことで，メモリアクセスの回数を減らすことができるが，
	 *  ビット操作命令が充実していないプロセッサで，優先度の段階数が少ない
	 *  場合には，ビットマップ操作のオーバーヘッドのために，逆に効率が落ち
	 *  る可能性もある．
	 *
	 *  優先度が16段階であることを仮定しているため，uint16_t型としている．
	 */
	uint16_t	ready_primap;

	/*
	 *  取得しているスピンロック
	 *
	 *  取得しているスピンロックのスピンロック初期化ブロックを指すポイ
	 *  ンタ．取得しているスピンロックがない場合はNULLにする．このフィー
	 *  ルドにアクセスするのは，自プロセッサのみであるため，ジャイアン
	 *  トロックを取らずにアクセスして良い．
	 */
	const SPNINIB	*p_locspn;

	/*
	 *  タイムイベントコントロールブロックへのポインタ
	 */ 
	TEVTCB   *p_tevtcb;

	/*
	 *  カーネル動作状態フラグ
	 *
	 *  スタートアップルーチンで，false（＝0）に初期化されることを期待
	 *  している．
	 */
	bool_t	 kerflg;
};

/*
 *  PCBへのアクセステーブル（kernel_cfg.c）
 */
extern PCB *const	p_pcb_table[];

/*
 *  プロセッサIDからPCBを取り出すためのマクロ
 */
#define INDEX_PRC(prcid)	((uint_t)((prcid) - TMIN_PRCID))
#define get_pcb(prcid)		(p_pcb_table[INDEX_PRC(prcid)])

/*
 *  自プロセッサのPCBの取得
 *
 *  get_my_pcbは，kernel_cfg.c中に生成される割込みハンドラから呼び出さ
 *  れるため，参照する変数に"_kernel_"を付ける必要がある．
 */
#ifndef get_my_pcb
#define get_my_pcb()		(_kernel_p_pcb_table[get_my_prcidx()])
#endif /* get_my_pcb */

/*
 *  マスタプロセッサの判定
 */
#ifndef is_mprc
#define is_mprc(p_my_pcb)	(p_my_pcb->prcid == TOPPERS_MASTER_PRCID)
#endif /* is_mprc */

/*
 *  プロセッサのインデックス値からプロセッサIDを取り出すためのマクロ
 */
#define ID_PRC(prcidx)		((ID)((prcidx) + TMIN_PRCID))

/*
 *  タイムマスタプロセッサのPCBの取得
 */
#define P_TM_PCB	p_pcb_table[TOPPERS_TMASTER_PRCID - 1]

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_PCB_H */