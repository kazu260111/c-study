# 学習メモ
- 「ゼロからのTCP/IPプロトコルスタック自作自作入門」を読みながら学習したことをまとめる。
- [このリポジトリ](https://github.com/kazu260111/microps_fork_TCP-IP)にて学習中。

## 第5回  2026-04-27
### 今回やったこと(概要)
- Appendix 2 (書籍 p587 ~ )
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

### つまづいたこと・難しかったこと(引っかかったところや疑問に思ったこと)


### 解決した方法(自力での試行錯誤や調べた内容)


### 感想


#### メモ(本筋から外れた、あるいは重要度の低い内容)


## 第4回  2026-04-25,26
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

## 第3回  2026-04-22,23
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



## 第2回  2026-04-21
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


## 第1回  2026-04-20
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

