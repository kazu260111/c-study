# 引用元
- このファイルのソースコードは書籍「ゼロからのTCP/IPプロトコルスタック自作入門」または[こちらのリポジトリ](https://github.com/pandax381/microps)から引用、改変したものです。
- ソースコードのライセンスはMITライセンスです。詳しくはLICENSEをご確認ください。

## 書籍情報
**書籍名**: ゼロからのTCP/IPプロトコルスタック自作自作入門
**著者**: 山本雅也
**出版社**: マイナビ出版

# 学習メモ
- 「ゼロからのTCP/IPプロトコルスタック自作自作入門」を読みながら学習したことをまとめる
- 引用元リポジトリに自分のコメントをつけることで理解を深める
- 書籍を読みながらTCP/IPプロトコルスタックを完成させる
- プログラム本体は[このリポジトリ](https://github.com/kazu260111/microps_fork_TCP-IP)にて作成中。


## step 3 2026-05-01
### 今回やったこと(概要)
- step 3 (書籍 p79~)
  - プロトコルの管理

### 学べたこと(具体的な内容)
- プロトコル種別
  - IP (Internet Protocol)
    - 0x0800
  - ARP (Address Resolution Protocol)
    - 0x0806
  - RARP (Reverse ARP)
    - 0x8035
  - IPv6 (Internet Protocol version 6)
    - 0x86dd

- プロトコル処理をモジュール化して、後から新しいプロトコルのモジュールを追加できるようにする
- 対応していないプロトコルのパケットは破棄する

- プロトコル構造体 (p83)

```c

struct net_protocol {
    struct net_protocol *next;  /* プロトコルのリストを走査する */
    uint16_t type;  /* プロトコル種別(定数) */
    net_protocol_handler_t handler;  /* プロトコルパケットを処理する入力ハンドラの関数ポインタ */
};

```

### つまづいたこと・難しかったこと(引っかかったところや疑問に思ったこと)


### 解決した方法(自力での試行錯誤や調べた内容)


### 感想


#### メモ(本筋から外れた、あるいは重要度の低い内容)
## step 2 2026-04-30
### 今回やったこと(概要)
- step 2 (書籍 p63~)
  - デバイスドライバ
    - ネットワークデバイスのドライバ
    - ループバックデバイスのドライバ

### 学べたこと(具体的な内容)
- デバイスドライバ
  - ネットワークデバイスの管理
    - ネットワークデバイスによってデバイス操作の方法はバラバラ
      - デバイス依存の処理はデバイスドライバに実装し、デバイス制御の共通インターフェースが
        デバイスドライバを呼び出すようにする
        -> OSは個々のデバイスの違いを気にしなくて良くなる
  - システム起動時など、デバイスを検出したタイミングでロード、初期化ルーチンを呼び出す

- 初期化ルーチン
  - デバイスを管理するためのオブジェクト生成
  - デバイスドライバの制御のルーチンを紐付けてシステムに登録
 
- struct net_device_ops
  - デバイス固有の処理を行う関数のアドレスを格納する構造体
    ポリモーフィズムを実現する(共通のインターフェースで操作できるようにする)
  - ３つの関数ポインタ(open, close, output)をもつ

- 制御ルーチンの呼び出し(p69)
  - ネットワークデバイスの起動
```c

struct net_device {
    struct net_device *next;
    unsigned int index;
    char name[IFNAMSIZ];
    uint16_t type;
    uint16_t mtu;
    uint16_t flags;
    uint16_t hlen;
    uint16_t alen;
    uint8_t addr[NET_DEVICE_ADDR_LEN];
    uint8_t broadcast[NET_DEVICE_ADDR_LEN];
    struct net_device_ops *ops;  /* デバイス固有の処理を行う関数のアドレスを格納する構造体のポインタ */
    void *priv;  /* 任意のデータを指すポインタ、どのようなデータでも紐付けられるようにvoid *型 */
};

static int
net_device_open(struct net_device *dev)
{
    /* 省略 */
    /* 制御ルーチンの呼び出しをする */
	if (dev->ops->open) {
		if (dev->ops->open(dev) == -1) {
			errorf("failure, dev=%s", dev->name);
			return -1;
		}
    /* 省略 */
	return 0;
}

```

- ネットワークデバイスの停止 (p70)

```c

static int
net_device_close(struct net_device *dev)
{
    /* 省略 */
    /* 停止ルーチンの起動、もし停止ルーチンがNULLなら停止ルーチンがないのでスキップ */
	if (dev->ops->close) {
		if (dev->ops->close(dev) == -1) {
			errorf("failure, dev=%s", dev->name);
			return -1;
		}
	}
    /* 省略 */
	return 0;
}

```

- ネットワークデバイスへの出力 (p70)

```c

int
net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst)
{
    /* 省略 */
    /* 出力ルーチンがNULLだったら(アドレスが設定されていないなら)エラーにする */
	if (!dev->ops->output) {
		errorf("output callback function is not set, dev=%s", dev->name);
		return -1;
	}
    /* 出力ルーチンを実行 */
    /* devはデバイスのパラメータで、type, data, len, dstは送信データのパラメータ */
	if (dev->ops->output(dev, type, data, len, dst) == -1) {
		errorf("failure, dev=%s, len=%zu", dev->name, len);
		return -1;
	}
	return 0;
}

```

- ネットワークデバイスからの入力 (p601)

```c

int
net_input(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev)
{
    /* この段階ではデバック情報を表示しておくだけ。%04xで4桁を16進数で表示 */
	debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
	debugdump(data, len);
	return 0;
}

```
- ループバックデバイスの実装
  - 自分自身と通信するための特殊なネットワークデバイス
    - ループバックデバイスのoutputがそのままループバックデバイスからのinputになる
    - デバイスドライバのみで完結
    
- ループバックデバイスの初期化ルーチン (p72)

```c

struct net_device *
loopback_init(void)
{
	struct net_device *dev;
    /* メモリ確保 */
	dev = net_device_alloc();
	if (!dev) {
		errorf("net_device_alloc() failure");
		return NULL;
	}
    /* net_device構造体の*devに値を入れる */
	dev->type = NET_DEVICE_TYPE_LOOPBACK;
	dev->mtu = LOOPBACK_MTU;
	dev->flags = NET_DEVICE_FLAG_LOOPBACK;
	dev->hlen = 0; /* non header */
	dev->alen = 0; /* non address */
    /* 制御ルーチンのポインタ(出力ルーチンのみ設定されている) */
	dev->ops = &loopback_ops;
	if (net_device_register(dev) == -1) {
		errorf("net_device_register() failure");
		return NULL;
	}
	infof("success, dev=%s", dev->name);
	return dev;
}

```

- ループバックデバイスの出力ルーチン (p74)

```c

static int
loopback_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst)
{
	debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
	debugdump(data, len);
    /* ループバックデバイスは引数で渡されたパケットをそのまま入力パケットとして渡す */
	return net_input(type, data, len, dev);
}

```

- 実行結果

```bash
$ ./test/test.exe
17:38:53.480 [I] setup: setup protocol stack... (test/test.c:35)
17:38:53.480 [I] net_init: initialize... (net.c:116)
17:38:53.480 [I] net_init: success (net.c:121)
17:38:53.480 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:41)
17:38:53.480 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
17:38:53.480 [I] net_run: startup... (net.c:130)
17:38:53.480 [I] net_device_open: dev=net0 (net.c:48)
17:38:53.480 [I] net_run: success (net.c:138)
17:38:53.480 [D] app_main: press Ctrl+C to terminate (test/test.c:66)
17:38:53.480 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:84)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
17:38:53.480 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
17:38:53.480 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:108)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
^C17:38:53.859 [D] app_main: terminate (test/test.c:74)
17:38:53.860 [I] cleanup: cleanup protocol stack... (test/test.c:55)
17:38:53.860 [I] net_shutdown: shutting down... (net.c:147)
17:38:53.860 [I] net_device_close: dev=net0 (net.c:66)
17:38:53.860 [I] net_shutdown: success (net.c:154)

```

### 感想
- 特に難しいところはなかった。
- ループバックデバイスの実装でパケットの送受信処理が機能していることを確認できた。

## step 1 2026-04-29
### 今回やったこと(概要)
- step 1 (書籍 p45~)

### 学べたこと(具体的な内容)
- ネットワークデバイス構造体(p50) (net.c)

```c

struct net_device {
    /* 次のネットワークデバイスのポインタ
     * 連結リストでネットワークデバイスを管理していて、先頭からリストを走査する
     * NULLならリストの最後という意味
     */
    struct net_device *next;
    /* ネットワークデバイスを一意に識別する番号、デバイス登録時自動生成 */
    unsigned int index;
    /* ネットワークデバイスの名前、デバイス登録時自動生成 */
    char name[IFNAMSIZ];
    /* ネットワークデバイスの種別で、NET_DEVICE_TYPE_DUMMY, LOOPBACK, ETHERNETの3つの定数から設定 */
    uint16_t type;
    /* ネットワークデバイスが一度に送信できるデータの最大サイズ */
    uint16_t mtu;
    /* ネットワークデバイスの特性と状態を表す定数NET_DEVICE_FLAG_UP, LOOPBACK, BROADCAST, P2P, NEED_ARPから設定する */
    uint16_t flags;
    /* データリンクのヘッダ長、データリンクのプロトコルにヘッダがないなら0 */
    uint16_t hlen;
    /* データリンクのアドレス長、データリンクのプロトコルにアドレスがないなら0 */
    uint16_t alen;
    /* ネットワークデバイスのアドレス、定数で16と設定されている。実際のアドレスの長さはalenにある */
    uint8_t addr[NET_DEVICE_ADDR_LEN];
    /* データリンクの ブロードキャストアドレス */
    uint8_t broadcast[NET_DEVICE_ADDR_LEN];
};

```

 
- ネットワークデバイスオブジェクトの割当 (p52)

```c

/* 戻り値はnet_device構造体のポインタ */
struct net_device *
net_device_alloc(void)
{
	struct net_device *dev;
    /* net_device構造体の大きさのメモリを確保 */
	dev = memory_alloc(sizeof(*dev));
    /* 失敗したらエラー */
	if (!dev) {
		errorf("memory_alloc() failure");
		return NULL;
	}
    /* 確保したメモリのアドレスを返す :/
	return dev;
}

```

- ネットワークデバイスの登録 (p53)

```c

int
net_device_register(struct net_device *dev)
{
	static unsigned int index = 0;
    
    /* インデックス番号を設定したらインデックスをインクリメント */
	dev->index = index++;
    /* デバイス名の設定 (net0など) */
	snprintf(dev->name, sizeof(dev->name), "net%d", dev->index);
    /* デバイスをリストの先頭に追加、devicesはリストの先頭の意味 */
	dev->next = devices;
    /* リストの先頭を更新 */
	devices = dev;
	infof("success, dev=%s, type=0x%04x", dev->name, dev->type);
	return 0;		
}

```

- ネットワークデバイスの起動 (p54)

```c

static int
net_device_open(struct net_device *dev)
{
	infof("dev=%s", dev->name);
    /* すでに稼働フラグがセットされているかチェック */
    /* NET_DEVICE_IS_UP()は論理積を使ってフラグを調べるマクロ */
	if (NET_DEVICE_IS_UP(dev)) {
		errorf("already opened, dev=%s", dev->name);
		return -1;
	}
    /* 論理和を使ったフラグ操作。NET_DEVICE_FLAG_UP は0x0001 */
	dev->flags |= NET_DEVICE_FLAG_UP;
	return 0;
}

```

- ネットワークデバイスの停止 (p55)

```c

static int
net_device_close(struct net_device *dev)
{
	infof("dev=%s", dev->name);
    /* 稼働フラグが設定されていないならエラー */
	if (!NET_DEVICE_IS_UP(dev)) {
		errorf("not opened, dev=%s", dev->name);
		return -1;
	}
    /* 論理積を使って稼働フラグを外す */
	dev->flags &= ~NET_DEVICE_FLAG_UP;
	return 0;
}

```

- プロトコルスタックの動作との連動 (p56)

```c

int
net_run(void)
{
	struct net_device *dev;

	infof( "startup...");
	if (platform_run() == -1) {
		errorf( "platform_run() failure");
		return -1;
	}
    /* リストを先頭から走査してnet_device_openを実行 */
	for (dev = devices; dev; dev = dev->next) {
		net_device_open(dev);
	}
	infof( "success");
	return 0;
}

int
net_shutdown(void)
{
	struct net_device *dev;

	infof( "shutting down...");
	if (platform_shutdown() == -1) {
		warnf( "platform_shutdown() failure");
	}
    /* リストを先頭から走査してnet_device_closeを実行 */
	for (dev = devices; dev; dev = dev->next) {
		net_device_close(dev);
	}
	infof( "success");
	    return 0;
}
```

- ネットワークデバイスへの出力 (p57)

```c

/* ネットワークデバイスからデータを送信する関数 */
/* type: プロトコル種別
 * len: 送信データの長さ
 * data: 送信データのバイト列
 * dst: データリンク上の宛先アドレス
 * mtu: 一度に送信できるデータの最大サイズ
 */

int
net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst)
{
	debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
	debugdump(data, len);
    /* ネットワークデバイスが稼働していないならエラー */
	if (!NET_DEVICE_IS_UP(dev)) {
		errorf("not opened, dev=%s", dev->name);
		return -1;
	}
    /* mtuよりも送信データの長さが長いならエラー */
	if (dev->mtu < len) {
		errorf("too long, dev=%s, mtu=%u, len=%zu", dev->name, dev->mtu, len);
		return -1;
	}
	return 0;
}

```

- テスト用ダミーデバイスの登録 (p58)

```c

struct net_device *
dummy_init(void)
{
	struct net_device *dev;
	dev = net_device_alloc();
	if (!dev) {
		errorf("net_device_alloc() failure");
		return NULL;
	}
    /* ダミーデータの登録 */
	dev->type = NET_DEVICE_TYPE_DUMMY;
	dev->mtu = 128;
	dev->hlen = 0; /* no header */
	dev->alen = 0; /* no address */
	if (net_device_register(dev) == -1) {
		errorf("net_device_register() failure");
		return NULL;
	}
	infof("success, dev=%s", dev->name);
    /* net_device構造体のポインタを返す */
	return dev;
}

```

- setup()を修正したが省略 (p59)
  -  dev = dummy_init(); の追加、エラーチェックを追加

- app_mainのwhile内にnet_device_output()を追加 (p60)
  - 以下はapp_main内の修正部分

```c

	while (!terminate) {
        /* テストデータを送信する。宛先アドレスはNULL */
		if (net_device_output(dev, 0x0800, test_data, sizeof(test_data), NULL) == -1) {
			errorf("net_device_output() failure");
			break;
		}
		sleep(1);
	}

```

- 実行結果
```bash
$ ./test.exe
20:26:53.068 [I] setup: setup protocol stack... (test/test.c:54)
20:26:53.068 [I] net_init: initialize... (net.c:88)
20:26:53.068 [I] net_init: success (net.c:93)
20:26:53.068 [I] net_device_register: success, dev=net0, type=0x0000 (net.c:41)
20:26:53.068 [I] dummy_init: success, dev=net0 (test/test.c:33)
20:26:53.068 [I] net_run: startup... (net.c:102)
20:26:53.068 [I] net_device_open: dev=net0 (net.c:48)
20:26:53.068 [I] net_run: success (net.c:110)
20:26:53.068 [D] app_main: press Ctrl+C to terminate (test/test.c:85)
20:26:53.068 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:72)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
20:26:54.069 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:72)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
^C20:26:54.664 [D] app_main: terminate (test/test.c:93)
20:26:54.664 [I] cleanup: cleanup protocol stack... (test/test.c:74)
20:26:54.664 [I] net_shutdown: shutting down... (net.c:119)
20:26:54.664 [I] net_device_close: dev=net0 (net.c:60)
20:26:54.664 [I] net_shutdown: success (net.c:126)

```

### 感想
- 定数と論理和を使ったフラグ操作は自分で使ったことがないので取り入れたい。
- 全ての修正コードを書いていくと長すぎるので、次回から重要なロジックや新しく学んだ概念などに絞ってここに書いていくことにする。

## Appendix 3 2026-04-28
### 今回やったこと(概要)
- Appendix 3 (書籍 p595~)
  - タスク管理

### 学べたこと(具体的な内容)
- タスク構造体 (p595) 

```c
/* 排他制御に使うlockを定義 */
static lock_t lock = LOCK_INITIALIZER;
static struct sched_task *tasks; /* sleep tasks */

struct sched_task {
    struct sched_task *next;  /* 次の要素を指すポインタ */
    pthread_cond_t cond;  /* pthreadライブラリの条件変数。条件を満たすまでスレッドを待機させられる */
    int interrupted;  /* スケジュールされたタスクが中断されたときは1、通常時0 */
    /* 休止タスクのスレッド数のカウント。スレッドが待機しているときはsched_task_destroyで条件変数を破棄しないようにする */
    int wc; /* wait count */
};

```

- タスク構造体の初期化 (p596)

```c

int
sched_task_init(struct sched_task *task)
{
    /* 構造体に各値を入れる */
    /* 次の要素をNULLに */
    task->next = NULL;
    /* 条件変数condはこの関数で初期化 */
    pthread_cond_init(&task->cond, NULL);
    /* タスクが中断されたを示すフラグを0に初期化 */
    task->interrupted = 0;
    /* 待機スレッド数のカウンタを0に初期化 */
    task->wc = 0;
    return 0;
}

```

- タスク構造体のリソース解放 (p596)

```c

int
sched_task_destroy(struct sched_task *task)
{   
    /* 休止状態のタスクがあるならキャンセル */
    if (task->wc) {
        return -1;
    }
    /* 条件変数condを破棄してリソースを解放する */
    return pthread_cond_destroy(&task->cond);
}

```

- タスクの休止 (p597)

```c

/* 引数：タスク構造体のポインタ、ロック構造体のポインタ、タイムアウト時間 */
int
sched_task_sleep(struct sched_task *task, lock_t *lock, const struct timespec *abstime)
{
    int ret;
    /* 割り込みが来ていたら休止をやめてエラーを返す */
    if (task->interrupted) {
        errno = EINTR;
        return -1;
    }
    /* 休止スレッド数を1増やす */
    task->wc++;
    /* 休止タスクのリストに追加 */
    tasks_add(task);
    /* タイムアウト時間が0でないなら、pthread_cond_timedwaitを使用 */
    if (abstime) {
        ret = pthread_cond_timedwait(&task->cond, lock, abstime);
    } else {
        /* タイムアウト時間が0(NULL)ならpthread_cond_waitを使用 */
        ret = pthread_cond_wait(&task->cond, lock);
    }
    /* 休止から復帰する */
    tasks_del(task);
    /* 休止スレッド数を1減らす */
    task->wc--;
    /* 割り込みが来ていたら */
    if (task->interrupted) {
        /* もし他に休止スレッドがないならフラグを0にする */
        /* 他のスレッドがいる場合はそのスレッドが割り込まれたことを認識させるためにフラグを1のままにしておく */
        if (!task->wc) {
            task->interrupted = 0;
        }
        errno = EINTR;
        return -1;
    }
    return ret;
}

```

- 休止タスクのリストへの追加 (p599)
  - 書籍のコードとGithubのコードでsleep tasksの名前が変わっていた

```c

static lock_t lock = LOCK_INITIALIZER;
static struct sched_task *tasks; /* sleep tasks */

static void
tasks_add(struct sched_task *task)
{
    /* 排他制御(鍵をかける) */
    lock_acquire(&lock);
    /* taskをリストの最初に入れる */
    task->next = tasks;
    tasks = task;
    /* 鍵を開ける */
    lock_release(&lock);
}

```

- 休止タスクのリストから削除 (p599)
  - 関数や変数の名前が書籍と変わっている

```c

static void
tasks_del(struct sched_task *task)
{
    struct sched_task *entry;
    /* 排他制御 */
    lock_acquire(&lock);
    /* 削除するタスクが先頭のとき */
    if (tasks == task) {
        /* tasksのアドレスを削除するタスクが指している次のタスクのアドレスに設定 */
        tasks = task->next;
        /* リストから消したタスクが他のタスクを指さないようにする */
        task->next = NULL;
        /* 排他制御終了 */
        lock_release(&lock);
        return;
    }
    /* 削除するタスクを先頭から走査して探す */
    for (entry = tasks; entry; entry = entry->next) {
        /* 削除するタスクが見つかったら */
        if (entry->next == task) {
            /* 削除するタスクをリストから外すようにnextを変更 */
            entry->next = task->next;
            /* リストから消したタスクが他のタスクを指さないようにする */    
            task->next = NULL;
            break;
        }
    }
    /* 排他制御終了 */
    lock_release(&lock);
}

```

- タスクの起床 (p600)

```c

int
sched_task_wakeup(struct sched_task *task)
{
    /* 休止タスクを全て起こす */
    return pthread_cond_broadcast(&task->cond);
}

```

- タスク管理機構の初期化 (p601)

```c

int
sched_init(void)
{
    /* INTR_IRQ_USERが来たときの割り込みハンドラを設定 */
    return intr_register(INTR_IRQ_USER, sched_irq_handler, 0, NULL);
}

```

- 割り込みハンドラ (p601)

```c

/* INTR_IRQ_USERに対して呼び出される割り込みハンドラ */
static void
sched_irq_handler(unsigned int irq, void *arg)
{
    struct sched_task *task;

    /* 使わない引数 */
    (void)irq;
    (void)arg;
    /* 排他制御 */
    lock_acquire(&lock);
    /* タスクを先頭から走査 */
    for (task = tasks; task; task = task->next) {
        /* 割り込みが発生したとき */
        if (!task->interrupted) {
            task->interrupted = 1;
            /* 全スレッドを起こす */
            pthread_cond_broadcast(&task->cond);
        }
    }
    /* 排他制御終了 */
    lock_release(&lock);
}

```


- タスク管理機構の起動 (p602)

```c

/* 何もしないが恐らくわかりやすさのためにあるダミーの関数 */
int
sched_run(void)
{
    /* do nothing */
    return 0;
}

```

- タスク管理機構の停止 (p602)

```c

/* ダミーの関数 */
int
sched_shutdown(void)
{
    /* do nothing */
    return 0;
}

```
### 感想
- 読んで大体理解できたと思う。排他制御が出てきたが、うっかり鍵をかけたままにせず解放するまでを忘れないようにしたい。
- 排他制御は他にもデッドロックや優先度逆転など気をつけることが多いらしいので、注意して使うようにしたい。

## Appendix 2  2026-04-27
### 今回やったこと(概要)
- Appendix 2 (書籍 p587~)
  - タイマー処理

### 学べたこと(具体的な内容)
- 以下は platform/linux/timer.c の一部

```c
/* タイマー構造体 */
struct timer {
    struct timer *next;  /* 次のタイマーを指すポインタ */
    struct timeval interval;  /* タイマーの発火間隔(タイマー割り込みの間隔) */
    struct timeval last;  /* タイマーが最後に発火した時刻 */
    void (*handler)(void);  /* タイマーが発火したときの関数のポインタ */
};

static timer_t timerid;

/* タイマーが起動したら、新しいタイマーを登録できない(排他制御の仕組みが必要)
 * もしタイマー起動中に新しく登録しようとすると、タイマーリストの走査中に登録のためにリストを書き換えようとして予想外のアドレスを読み込んでしまいセグメンテーションフォルトが起こったりする
 * (必ずしもCPUはソースコード順にプログラムを実行するわけではない)
 */
/*
 * NOTE: if you want to add/delete the entries after timer_run(),
 *       you need to protect these lists with a mutex.
 */
static struct timer *timers;

/* タイマーの登録関数 */
int
timer_register(struct timeval interval, void (*handler)(void))
{
    struct timer *timer;

    /* タイマーの構造体のメモリを確保 */
    timer = memory_alloc(sizeof(*timer));
    /* timerがNULL(メモリを確保できなかった)なら */
    if (!timer) {
        errorf("memory_alloc() failure");
        return -1;
    }
    
    /* タイマー変数に各値を入れる */
    timer->interval = interval;
    gettimeofday(&timer->last, NULL);  /* 現在時刻を入れる */
    timer->handler = handler;
    /* リストの先頭に追加 */
    timer->next = timers;
    timers = timer;
    infof("success, interval={%d, %d}", interval.tv_sec, interval.tv_usec);
    return 0;
}

```

- タイマー機構の初期化(p590~)

```c
int
timer_init(void)
{
    struct sigevent sev;
    
    /* シグナルをプロセスに送るように設定 */
    sev.sigev_notify = SIGEV_SIGNAL;
    /* INTR_IRQ_TIMERで設定したシグナルを送るよう設定 */
    sev.sigev_signo = INTR_IRQ_TIMER;
    /* ポインタを渡すように設定 */
    sev.sigev_value.sival_ptr = &timerid;
    /* タイマー作成 */
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        errorf("timer_create: %s", strerror(errno));
        return -1;
    }
    return intr_register(INTR_IRQ_TIMER, timer_irq_handler, 0, NULL);
}

```
- タイマー割り込みの捕捉(p592~)

```c

static void
timer_irq_handler(unsigned int irq, void *arg)
{
    struct timer *timer;
    struct timeval now, diff;
    /* 使わない引数 */
    (void)irq;
    (void)arg;
    /* 現在時刻の取得 */
    gettimeofday(&now, NULL);
    /* タイマーリストを走査 */
    for (timer = timers; timer; timer = timer->next) {
        /* nowからtimer->lastを引いて、差をdiffに格納 */
        timersub(&now, &timer->last, &diff);
        /* diffがtimer->intervalより大きかったら発火 */
        if (timercmp(&timer->interval, &diff, <) != 0) { /* true (!0) or false (0) */
            /* 発火したタイマーの関数を実行 */
            /* メモ：この関数の実行に時間がかかると、他のタイマーの走査が遅れて正確な時間が測れない可能性がある */
            timer->handler();
            /* timer->lastの更新 */
            timer->last = now;
        }
    }
}

```
- タイマー機構の起動(p592~)

```c

int
timer_run(void)
{
    /* 1msをtsに設定 */
    const struct timespec ts = {0, 1000000}; /* 1ms */
    /* タイマーの間隔を設定 */
    struct itimerspec interval = {ts, ts};
    
    /* タイマーの起動 */
    if (timer_settime(timerid, 0, &interval, NULL) == -1) {
        errorf("timer_settime: %s", strerror(errno));
        return -1;
    }
    /* タイマーの情報を表示 */
    infof("interval={%d, %d}, initial={%d, %d}",
        interval.it_interval.tv_sec, interval.it_interval.tv_nsec,
        interval.it_value.tv_sec, interval.it_value.tv_nsec);
    return 0;
}

```
- タイマー機構の停止(p593~)

```c

int
timer_shutdown(void)
{
    /* タイマーを削除する */
    if (timer_delete(timerid) == -1) {
        errorf("timer_delete: %s", strerror(errno));
        return -1;
    }
    return 0;
}

```

### 感想
- POSIXのタイマーの扱い方を学んだが、APIの使い方は自作プログラムでやりながら覚えるのが良いと思うので復習したい。

## Appendix 1  2026-04-25,26
### 今回やったこと(概要)
- Appendix 1  A1.4 ~ A1.6  (書籍 p578 ~ p585)

### 学べたこと(具体的な内容)
- 以下は学習のためにplatform/linux/intr.cの一部にコメントをつけたもの(p577~)

```c
/*>>> 割り込み管理の構造体 <<<*/
/* どの割り込みが発生したらどの処理が実行されるかの情報をもっている */ 
struct irq_entry {
    struct irq_entry *next;  /* 次の要素を指すポインタ */
    unsigned int irq;  /* 割り込みを識別する値, I/Oイベントの通知で使用するシグナル番号 */
    intr_isr_t isr;  /* 割り込みハンドラの関数ポインタ。割り込みが起きたときに実行する関数のポインタを置く。 */
    int flags;  /* INTR_IRQ_SHAREDで使用されるフラグ。フラグ設定時一つの割り込み番号を複数デバイスで共有できる。 */
    void *arg;
};

/*
 * NOTE: if you want to add/delete the entries after intr_run(),
 *       you need to protect these lists with a mutex.
 */
/* 割り込みリストの先頭部分のポインタとしてグローバル変数irqsを定義する */
/* 割り込みが起きたとき、irqsから順番にリストを走査する */
static struct irq_entry *irqs;

static pthread_t tid;
static pthread_barrier_t barrier;
static sigset_t sigmask;

/*>>> 割り込み登録関数 <<<*/
int
intr_register(unsigned int irq, intr_isr_t isr, int flags, void *arg)
{
    /* 登録したい割り込み設定 */
    struct irq_entry *entry;

    /* irqsから始めて、nextがNULLになるまでリストを走査する */
    for (entry = irqs; entry; entry = entry->next) {
        /* もし登録したい割り込み番号が登録予定の割り込み番号と一致した場合 */
        if (entry->irq == irq) {
            /* INTR_IRQ_SHARED は別ファイルで0x0001と定義されているので、entry->flagsかflagsのどちらかが0だった場合は{}ブロックが実行される
            -> 同じ番号を共有できない設定と判定されてエラーとなる  */
            if (entry->flags ^ INTR_IRQ_SHARED || flags ^ INTR_IRQ_SHARED) {
                errorf("conflicts with already registered IRQs, irq=%u", irq);
                return -1;
            }
        }
    }
    /* entry用のメモリ確保 */
    entry = memory_alloc(sizeof(*entry));
    /* NULLなら(メモリが確保できなかったら)エラーを出す */
    if (!entry) {
        errorf("memory_alloc() failure");
        return -1;
    }
    /* entryに割り込み設定を入れていく */
    entry->irq = irq;
    entry->isr = isr;
    entry->flags = flags;
    entry->arg = arg;
    /* リストの後ろではなく、先頭に追加する */
    entry->next = irqs;
    /* irqsが新しい割り込み設定のアドレスを指すようにする */
    irqs = entry;
    /* sigmask(グローバル変数)に含まれるシグナルを待ち受ける
     * -> sigaddsetでsigmaskにirq(シグナル番号)を登録
     * (共有フラグが設定されていないなら、一つのシグナルに対して一つの割り込みが対応している) 
     */
    sigaddset(&sigmask, irq);
    infof("success, irq=%u", irq);
    return 0;
}

```
- 割り込み機構の実装(p580) /platform/linux/intr.cの一部

```c
static pthread_t tid;  /* スレッドID */
static pthread_barrier_t barrier;  /* バリア変数(スレッドの同期をとる)
static sigset_t sigmask;  /* シグナルの集合 */

int
intr_init(void)
{
    tid = pthread_self();  /* スレッドの初期化 */
    pthread_barrier_init(&barrier, NULL, 2);  /* バリア変数に同期をとるスレッドを設定 */
    sigemptyset(&sigmask);  /* シグナル集合の初期化(空にする) */
    sigaddset(&sigmask, SIGHUP);  /* SIGHUPを設定 */
    return 0;
}

int
intr_run(void)
{
    int err;
    /* sigmaskに登録してあるシグナルにメインスレッドが反応しないようにする */
    err = pthread_sigmask(SIG_BLOCK, &sigmask, NULL);
    if (err) {
        errorf("pthread_sigmask() %s", strerror(err));
        return -1;
    }
    /* intr_mainを実行する子スレッドを作成。さっき設定したシグナルブロックの設定を引き継ぐ。 */
    /* 起動したスレッドのスレッドIDをグローバル変数tidに入れる。 */
    err = pthread_create(&tid, NULL, intr_main, NULL);
    if (err) {
        errorf("pthread_create() %s", strerror(err));
        return -1;
    }
    /* intr_mainとintr_run(ここ)にあるこの関数が実行されない限り進まない。
     * ここで同期を取っている。
     */
    pthread_barrier_wait(&barrier);
    return 0;
}

int
intr_shutdown(void)
{
    /* tidとpthread_self()の値を比べて、これらが等しいならスレッドは作られていないことになる(tidで intr_mainが実行されていないことになる) */
    if (pthread_equal(tid, pthread_self()) != 0) {
        /* Thread not created. */
        return -1;
    }
    /* intr_initで設定したSIGHUPのシグナルをシグナル処理スレッド(intr_main)に送る */
    pthread_kill(tid, SIGHUP);
    /* シグナル処理スレッドが終わるのを待ってスレッドのリソースを回収する(スタック領域などを回収する) */
    pthread_join(tid, NULL);
    return 0;
}

/*>>> シグナル処理スレッド <<<*/
static void *
intr_main(void *arg)
{
    int terminate = 0, sig, err;
    struct irq_entry *entry;

    infof("start...");
    /* intr_runと同期を取っている */
    pthread_barrier_wait(&barrier);
    /* SIGHUPシグナルを受け取るまでループ */
    while (!terminate) {
        /* sigmaskに設定したシグナルが来るまで待機 */
        /* intr_runのシグナル配送禁止の設定はこのスレッドに引き継がれているが、sigwaitで待つことはできる */
        err = sigwait(&sigmask, &sig);
        if (err) {
            errorf("sigwait() %s", strerror(err));
            break;
        }
        switch (sig) {
        /* シグナルがSIGHUPだった場合 */
        case SIGHUP:
            /* ループを終わらせるためのフラグを立てる */
            terminate = 1;
            break;
        default:
            if (sig != INTR_IRQ_TIMER) {
                debugf("IRQ <%d> occurred", sig);
            }
            /* irqs(リストの先頭)から走査して、シグナルと割り込み番号が一致すれば割り込みハンドラを呼び出す。 */
            for (entry = irqs; entry; entry = entry->next) {
                if (entry->irq == (unsigned int)sig) {
                    entry->isr(entry->irq, entry->arg);
                    /* もしentry->flag が1なら、走査を続行する(0ならここでbreak) */
                    if (entry->flags ^ INTR_IRQ_SHARED) {
                        break;
                    }
                }
            }
            break;
        }
    }
    infof("terminated");
    return NULL;
}

```


### つまづいたこと・難しかったこと(引っかかったところや疑問に思ったこと)
- シグナルに関する関数をよく知らなかったのでsignal.hを見に行ってみたが、時間がかかるのでとりあえず使い方を理解するようにしたい。(調べたsigset_tは1024ビットでシグナルのオンオフを表す配列のようだった)

### 感想
- 書籍のコードに自分でコメントを書くとかなり理解しやすくなるので、難しいところはコメントをつけていきたい。
  - 一つ一つの関数は短いので読みやすいと思った。今後のプログラミングの参考にしたい。
- 安全にシグナルを受け取るには、親プロセスの段階でSIG_BLOCKを設定して子プロセスでsigwaitを実行するのが重要だとわかった。

## Appendix 1  2026-04-22,23
### 今回やったこと(概要)
- Appendix 1  A1.1 ~ A1.3  (書籍 p573 ~ p578)
  - 割り込み処理

### 学べたこと(具体的な内容)
#### 割り込み処理
- タスクを実行しているCPUがイベントの発生を通知される-> CPUが割り込みに対応-> 割り込みタスクが完了したら元のタスクに戻る
  - ハードウェア割り込みとソフトウェア割り込みがある
    - ハードウェア割り込み：キーボードの入力やストレージへの書き込みがこちらに分類される。ネットワークデバイスに到着したデータを読みだす処理もこれ。
    - ソフトウェア割り込み：ソフトウェアが発生元であること以外はハードウェア割り込みと同じ仕組み。プロトコルスタックの処理において使う。
- 今回学ぶプログラムではユーザ空間でハードウェア割り込みを再現するためにシグナルを使用する。
  - シグナルを受け取ったプログラムは設定しておいたシグナルハンドラを実行し、その後元の処理に戻る。
    - これは割り込みの処理と似ているのでシグナルを割り込みの代替として活用する

#### 調べたこと
- fcntl()
  - 開いたファイルの設定を変えるシステムコール
  - 書籍のコードの引数にある val | O_ASYNC はvalの元々の設定にO_ASYNCフラグを追加
- ファイルディスクリプタ
  - プログラムがファイルを操作するときに使う(割り当てられる)番号
- O_ASYNCフラグ
  - シグナル駆動I/Oで使用する。open()システムコールの引数にしてもLinuxだとフラグが設定できない(p575)ので、ファイルディスクリプタをfdに設定した後にfcntlでO_ASYNCフラグを有効にする必要がある。
  - これを有効にすると、データが来たとき(fdが入出力可能な状態になるとき)にSIGIOを送ってくれるようになる
    - データの送り先はあらかじめ決める必要がある(F_SETOWNを使用する)
    - 以下のコードで現在のプロセスにSIGIOを送れるようになる
    ```c
    fcntl(fd, F_SETOWN, getpid());
    ``` 
- F_SETOWNフラグ
  - fcntl()で使う。シグナルの送り先を設定するのに使う
- F_SETFLフラグ
  - fcntl()で使う。フラグの設定を上書きするのに使う
    - 現在の設定(val)に追加でO_ASYNCフラグを追加する例
    ```c
    val = fcntl(fd, F_GETFL, 0);  /* 現在の設定をvalに入れる */
    fcntl(fd, F_SETFL, val | O_ASYNC);
    ```
    - val | の部分がないと、設定がO_ASYNCのみ有効になってしまう
    - 実際にコードを使用するときは、ifで戻り値をチェックして-1ならエラーにする

### つまづいたこと・難しかったこと(引っかかったところや疑問に思ったこと)
- ファイルディスクリプタに関してうろ覚えだった(以前「試して理解 Linuxのしくみ」で読んだ記憶が少しあっただけ)のでもう一度調べ直した。
- openやfcntlのフラグは知らなかったので調べた。よく使うものを優先的に覚えていきたい。
- 関数ポインタは使ったことがなかったため調べた。構造体のメンバとして関数ポインタを登録すれば、あとから変数ごとに別の関数を登録できる。
  - このため、例えば「割り込みハンドラ」の関数ポインタをメンバと決めておいて、変数ごとに違う関数を設定して呼び出すこともできる。
    - いちいちif文で分岐したりすることがなくて済む。

### 解決した方法(自力での試行錯誤や調べた内容)
- ネット検索やLLMで調べられたので特に問題なかった

### 感想
- 関数ポインタは便利だと思うので今度Cプログラムを作るときに構造体に入れるようにしたい。

#### メモ(本筋から外れた、あるいは重要度の低い内容)
- 毎回作業開始前にディレクトリを変えるのが面倒なのでtmuxinatorで設定を記録しておいた。(使用時は tmuxinator t)



## step 0  2026-04-21
### 今回やったこと(概要)
- step 0 はじめに (p15 ~ p43)
- clangdのインストール
  - vimプラグインのインストール(clangdをvimで使用するため)
    - vim-lsp
    - vim-lsp-settings
- Linuxのユーザ空間でカーネル内に近い処理をするためのプログラム(platform/linux)の確認

### 学べたこと(具体的な内容)
- 書籍が用意したマクロの使い方の確認
  - errorf(), warnf(), infof(), debugf()はLinuxカーネルでのカーネルログ(printk)みたいなものだと理解した。
- sigaction構造体
  - sigaction関数と一緒に使用して、シグナルが来たときに何を実行するかを指定するために使う
- 関数ポインタ
  - 関数を指すポインタは初めて見たので、調べたら　void (\*op) (int) のように戻り値と引数を指定すれば関数を指すポインタが作れるらしい
    - op = func; op(1); のようにすれば普通の関数のように使える
  - 今回ではstruct sigactionのメンバが関数ポインタだった。(自分の環境では /usr/include/x86_64-linux-gnu/bits/sigaction.h にあった)

### つまづいたこと・難しかったこと(引っかかったところや疑問に思ったこと)
- マクロを調べるために定義元をたどっていったが、定義元を読んでもすぐには理解できなかった。(sigaction構造体など)
  - 書籍を読み進めながらこういった基礎の部分の理解を深めていきたい。

### 感想
- 書籍が用意したマクロを調べていたら結構時間がかかってしまった。まだ全部理解できていないので、次回から少しずつ調べていきたい。
- とりあえずstep0のビルドと実行は成功した(書籍に書いてあるとおりにやるだけなので簡単だった)ので、次回からstep1に進む。

#### メモ(本筋から外れた、あるいは重要度の低い内容)
- clangdとvimプラグインは使ったことがなかったので、LLMで概要を調べてインストールした。
  - インストール前に一応Google検索をしたりGithubの制作者のリポジトリを見に行ってみた。star数も多かったので大丈夫そうだと考えてインストールした。
  - インストールしてもなぜかvimから使えず、色々調べていたら急に使えるようになった。どうやら裏でインストールやインデックスを作っていて反応しなかったらしい。
- bearコマンドでclangd用のjsonファイルを作れるらしいのでインストールした。
  - bear -- make でmakeを実行しつつjsonファイルを出力することで、コンパイルオプションを記録して正確なコード編集支援をしてくれるらしい。
    - 先にmake clean を実行しておく


## step 0  2026-04-20
### 今回やったこと(概要)
- 開発の流れの確認 (書籍 p15 ~ p43)
- 使用する初期ファイルの概要把握
  - 急な予定が入って途中で中断したので次回続きをする

### 学んだこと(具体的な内容)
- 開発の進行方法の確認
  - 開発はステップ1~30に小分けされているので、順を追って進める。
    - 各ステップの雛形としてタグが用意されているので、どこかで詰まってもタグからやり直すことができる。
- リポジトリに用意された関数の定義や構造体、定数などを記述したコードを取り込んで確認した。(途中まで)

### つまづいたこと・難しかったこと(引っかかったところや疑問に思ったこと)
- 難しいことは特になかった。非常に学習しやすい親切な設計なので根気よくやれば詰まることはなさそうだと思った。

### 解決した方法(自力での試行錯誤や調べた内容)
- わからなかった用語  
  - IDE : 統合開発環境。ソフトウェア開発のために必要なツール(コード編集やコンパイルなど)を一つにまとめたもの。
  - Language Server(clangd) : コンパイルエラーや定義元ジャンプ、名前のオートコンプリートなどをしてくれる。
    - 前回のプログラムではVimの標準機能だけで作業していたので、これを使えば作業が楽になりそうで期待している

### 感想
- TCP/IPプロトコルスタックを自作するといういかにも難しそうな内容だが、前回C言語のプログラムを作った経験から意外とコードを読むだけである程度の意味がわかった。
  - 自分が作るときもこれくらいわかりやすく書けるよう意識したい。
- 基本的に解説に従ってコードを書き、詰まったとしてもタグからやり直せるのでかなり親切な設計だと思った。
- サンプルコードにグローバル変数が多いのが気になったので、LLMで調べてみたら学習用のコードなのでわかりやすさ重視でこうなっているという出力になった。
  - ポインタだけで操作していこうとすると、共有するデータの種類が多いので関数の引数が多くなりすぎて理解しにくくなるともあった。
  - 一般的にはグローバル変数を使うときは原則staticとつけることで他ファイルから勝手に変更されないようにしているらしい。
    - 大規模なプログラムでどのファイルが何を操作しているかを把握するのは大変だし、バグが発生したときに全てのファイルを確認して原因究明するとなったら作業量が膨大になりそうなので、今後もグローバル変数は注意して使うようにしたい。

#### メモ(本筋から外れた、あるいは重要度の低い内容)
- vimでファイル編集中にdateコマンドを実行して日付を入力しようとしたら、%が現在のファイル名に置換されてしまった。
  - \でbashと同じように入力できる。 :r !date "+\%Y-\%m-\%d"
- 学習メモ用のテンプレートはtレジスタに入れておいたので次回からはこれを使う。
- git cloneした初見のリポジトリはとりあえずtreeコマンドを使うと全体がわかりやすくてよい。
- git checkoutで過去にコミットしていたバージョンに戻れることを初めて知った。
  - 前回作ったプログラムでは一日の終わりにコミットしていたが、トラブルがあった時に戻れるのは便利なので、今後はこまめにコミットしておきたい。
- プログラムを異なるプラットフォームでも動作するように設計する(プラットフォーム依存のコードを本体から分離する)というのは将来的な作業量を減らせるし、汎用的なコードは便利なので意識できそうならしていきたいと思った。(今はまだLinuxで手一杯なので、今後自作OSに挑戦するときにプラットフォームについても学びたいと思った)
- 前回のプログラムではヘッダファイルで関数を定義するときに一行で書いていたが、戻り値と関数名は別の行にしたほうが見やすいと気づいた。(特にenum型の戻り値だと読みづらかった)
- Makefileが前回作ったものよりかなり長かったが、書籍によれば基本的に手を加えなくても開発は進められるので、今は先に進んでそのうち調べようと思った。Makefileの書き方の詳細については慣れながら覚えていきたい。

