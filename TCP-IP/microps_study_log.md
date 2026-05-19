# 引用元
- このファイルのソースコードは書籍「ゼロからのTCP/IPプロトコルスタック自作入門」または[こちらのリポジトリ](https://github.com/pandax381/microps)から引用、改変したものです。
- ソースコードのライセンスはMITライセンスです。詳しくはLICENSEをご確認ください。

## 書籍情報
**書籍名**: ゼロからのTCP/IPプロトコルスタック自作入門
**著者**: 山本雅也
**出版社**: マイナビ出版

# 学習メモ
- 「ゼロからのTCP/IPプロトコルスタック自作入門」を読みながら学習したことをまとめる
- 引用したソースコードに自分のコメントをつけることで理解を深める
- 書籍を読みながらTCP/IPプロトコルスタックを完成させる
- プログラム本体は[このリポジトリ](https://github.com/kazu260111/microps_fork_TCP-IP)にて作成中。


## step 0 2026-05-19
### 今回やったこと(概要)
- step 10 (書籍 p187~)
  - Ethernetフレーム入力の処理

### 学べたこと(具体的な内容)
- Ethernetの基本
  - IEEE 802.3として標準化されている
  - 様々なプロトコルのデータを運ぶことができる

- Ethernetフレームの構造
  - プリアンブル
    Ethernetフレームの境界を識別する信号。
    ハードウェアで処理されるのでこのプロジェクトでは気にしなくてよい。
  - ヘッダ
    - 宛先MACアドレス(6byte)
      ブロードキャスト通信のときはFF:FF:FF:FF:FF:FFと設定。
    - 送信元MACアドレス(6byte)
    - 種別(2byte)
      ペイロードに格納されているデータのプロトコルを示す。(EtherType)
      IPの場合は0x0800。
  - ペイロード(可変長)
    MTUは一般的に1500なので、この場合Ethernetフレームの最大サイズは1518byteとなる。
    フレームの最小サイズは64byteと定められているので、ペイロードは最低46byte必要。
  - トレーラ(4byte)
    FCS(Frame Check Sequence)が格納されている。
    CRC(巡回冗長検査)でエラーを検出する。
    FCSはハードウェアで処理されることが多いので、このプロジェクトでは気にしない。

- Ethernetフレームの詳細を出力する (p195)

```c
/* Ehernetヘッダ構造体 */
struct ether_hdr {
    uint8_t dst[ETHER_ADDR_LEN];  /* 宛先MACアドレス(6byte) */
    uint8_t src[ETHER_ADDR_LEN];  /* 送信元MACアドレス(6byte) */
    uint16_t type;  /* 種別(2byte) */
};

void
ether_print(const uint8_t *frame, size_t flen)
{
	struct ether_hdr *hdr;
	char addr[ETHER_ADDR_STR_LEN];

    /* エラー出力がかぶらないようにする */
	flockfile(stderr);
    /*
     * EthernetフレームをEthernetヘッダの構造体でキャストして
     * ヘッダの各フィールドににアクセスできるようにする 
     */
	hdr = (struct ether_hdr *)frame;
    /* ether_addr_ntopはバイナリから文字列に変換する関数 */
	fprintf(stderr, "	src: %s\n", ether_addr_ntop(hdr->src, addr, sizeof(addr)));
	fprintf(stderr, "	dst: %s\n", ether_addr_ntop(hdr->dst, addr, sizeof(addr)));
	fprintf(stderr, "	type: 0x%04x\n", ntoh16(hdr->type));
#ifdef HEXDUMP
	hexdump(stderr, frame, flen);
#endif
    /* ロック解除 */
	funlockfile(stderr);
}

```
    
- MACアドレスのバイナリを文字列に変換する関数 (p196)

```c

char *
ether_addr_ntop(const uint8_t *n, char *p, size_t size)
{
    /* nかpがNULLならエラー */
    if (!n || !p) {
        return NULL;
    }
    /* MACアドレスの形式で文字列をpに格納 */
    snprintf(p, size, "%02x:%02x:%02x:%02x:%02x:%02x", n[0], n[1], n[2], n[3], n[4], n[5]);
    return p;
}

```

- MACアドレスを文字列からバイナリに変換する関数 (p197)

```c

int
ether_addr_pton(const char *p, uint8_t *n)
{
    int index;
    char *ep;
    long val;
    
    /* 変換前の文字列か、バイナリの格納先がNULLならエラー */
    if (!p || !n) {
        return -1;
    }
    /* MACアドレスの長さ分(6回)ループする */
    for (index = 0; index < ETHER_ADDR_LEN; index++) {
        /* pを16進数の数字として読み取り、数字以外を読み取ったところで止まる(epに保存) */
        val = strtol(p, &ep, 16);
        /* 数字が読み取れないか、数字の値が異常か、最後の数字を読んだらループを抜ける */
        if (ep == p || val < 0 || val > 0xff || (index < ETHER_ADDR_LEN - 1 && *ep != ':')) {
            break;
        }
        /* バイナリに格納 */
        n[index] = (uint8_t)val;
        /* 次の読み取り開始位置を保存 */
        p = ep + 1;
    }
    /* 読み取った数字の長さが異常か、最後に\0を読み取れなかったらエラー */
    if (index != ETHER_ADDR_LEN || *ep != '\0') {
        return -1;
    }
    return 0;
}

```

- Ethernetフレームの送受信
  このプロジェクトでは、EthernetフレームをOSの機能を使ってユーザ空間で直接送受信する。
  以下でその方法についてまとめている。
  - パケットソケット(socket())
    よくパケットキャプチャで使われるシステムコール。
    カーネル空間でOSのプロトコルスタックが受け取るフレームの複製を受け取ることができる。
    (このプロジェクトでは使用しない)
    - 使い方
    ```c
    /* 
     * PF_PACKET: プロトコルファミリ(パケット)
     * SOCK_RAW: ソケットタイプ(RAWデータ)
     * ETH_P_ALL: 受信するプロトコルタイプ(すべて)
     */
    soc = socket(PF_PACKET, SOCK_RAW, ETH_P_ALL); 
    ```
  - TAPデバイス
    - 仮想的なEthernetデバイス。アプリケーションから自由に読み書きできる。
      OSからEthernetデバイスとして認識され、TAPデバイスへの入力はOSのプロトコルスタック
      の入力として扱われる。また、OSのプロトコルスタックからTAPデバイスへの入力は
      TAPデバイスを使用しているアプリケーション(自作プロトコルスタック)への入力となる。
  
  - TAPデバイスの作成(再起動時にリセットされるので注意)
    ```bash
    # tap0という名前でTAPデバイスを作成(userに権限を渡してsudoを使わなくていいようにしておく)
    $ sudo ip tuntap add mode tap user $USER name tap0
    # TAPデバイスのアドレスを設定する
    $ sudo ip addr add 192.0.2.1/24 dev tap0
    # IPv6機能を無効化しておく
    $ sudo sysctl -q net.ipv6.conf.tap0.disable_ipv6=1
    # TAPデバイスの起動
    $ sudo ip link set tap0 up
    ```
  - TAPデバイスにIPアドレスが設定されたか確認
    ```bash
    $ ip addr show dev tap0
    ```
  - TAPデバイスが所属するサブネットワークの確認
    ```bash
    $ ip route show 192.0.2.0/24
    ```
  - 作成したTAPデバイスへのping
    ```bash
    # OSのプロトコルスタックが応答する
    $ ping -c 3 192.0.2.1
    ```
  - 作成したTAPデバイスの所属するサブネットワークの他のIPアドレスへのping
    ```bash
    # tap0をオープンしているアプリケーションがまだないので、Destination Unreachableになる
    $ ping -c 1 192.0.2.2
    ```
  - プログラムからTAPデバイスをオープンして制御する(概念的なコード) (p201)
    ```c
    int fd, err;
    struct ifreq ifr;
    ssize_t n;
    uint8_t buf[2048];
    /* デバイスファイルを読み書き可能で開く */
    fd = open("/dev/net/tun", O_RDWR);
    if (fd == -1) {
        return -1;
    }
    /* ifr(インターフェースリクエスト構造体)を0で埋める */
    memset(&ifr, 0, sizeof(ifr));
    /* tap0の名前を登録 */
    strcpy(ifr.ifr_name, "tap0");
    /*
     * flags: IFF_TUN    - TUN device (no Ethernet headers)
     *        IFF_TAP    - TAP device
     *        IFF_NO_PI  - Do not provide packet information
     */
    /* フラグの設定 */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    /* ファイルディスクリプタ(open()で開けた場所)にTAPデバイスを接続 */
    err = ioctl(fd, TUNSETIFF, &ifr);
    if (err) {
        return -1;
    }
    /* ファイルディスクリプタから実際に読み込む */
    n = read(fd, buf, sizeof(buf);

```

- テストプログラムの作成(/test/tap.c) (p203)
  - 省略(上述のTAPデバイスをオープンするコードを使用する)



### 感想
- Ethernetフレームの構造を学んだ。IPパケットでこういったデータの扱いに慣れていたので
  特に苦戦しなかった。
- socket()について調べるときにLinuxプログラムインターフェースを読んでみたが、
  p1243~からTCP/IPについても説明されていた。
  今やっている内容の理解の助けになりそうなので読んでおこうと思う。
  - 分厚い書籍なので読むのが大変だが、定期的に読み進めてコードを書いていきたい。
  - (メモ) p1262にリトル・ビックエンディアンについて
  
#### メモ(本筋から外れた、あるいは重要度の低い内容)
## step 9 2026-05-17
### 今回やったこと(概要)
- step 9 (書籍 p175~)
  - ICMPメッセージの送信
    - ICMPの照会メッセージ(Echoメッセージ)を受信したとき、Echo Replyを返せるようにする
    - 受け取ったIPパケットに対応するプロトコルが登録されていないとき、IPパケットを破棄して
      送信元にDestination Unreachableメッセージを送る(ICMPモジュールを経由する)

- ICMPモジュール送信関数 (p178)

```c

int
icmp_output(uint8_t type, uint8_t code, uint32_t val, const uint8_t *data, size_t len, ip_addr_t src, ip_addr_t dst)
{
    /* ICMPメッセージを入れる箱(サイズはIPパケットのペイロードの最大サイズと同じ値 */
	uint8_t buf[ICMP_BUFSIZ];
	struct icmp_hdr *hdr;
	size_t msg_len;
	char addr1[IP_ADDR_STR_LEN];
	char addr2[IP_ADDR_STR_LEN]; 

    /* 受け取ったデータが長すぎたらエラー(オーバーフローしてしまう) */
	if (sizeof(buf) < sizeof(*hdr) + len) {
		errorf("too large");
		return -1;
	}
    /* キャストしてicmp_hdr型のようにbufのメモリを扱えるようにする */
	hdr = (struct icmp_hdr *)buf;
    /* 各フィールドに値を入れる */
	hdr->icmp_type = type;
	hdr->icmp_code = code;
	hdr->icmp_sum = 0;
	hdr->dep = val;
    /* データ部分はicmp_hdrではアクセスできないので、memcpyを使用して直接メモリを指定する */
	memcpy(hdr + 1, data, len);
    /* メッセージサイズを保存 */
	msg_len = sizeof(*hdr) + len;
    /* チェックサムの計算(ヘッダだけでなくメッセージ全体を計算することに注意) */
	hdr->icmp_sum = cksum16((uint16_t *)hdr, msg_len, 0);
	debugf("%s => %s, len=%zu",
	    ip_addr_ntop(src, addr1, sizeof(addr1)),
	    ip_addr_ntop(dst, addr2, sizeof(addr2)), msg_len);
	icmp_print(buf, msg_len);
    /* 構築したICMPメッセージをIPモジュール送信関数に渡す */
	return ip_output(IP_PROTOCOL_ICMP, buf, msg_len, src, dst);
}

```

- Echo Replyメッセージの送信 (p180)
  - Echoメッセージを受信したらEcho Replyメッセージを送信するようにする

```c

static void
icmp_input(const struct ip_hdr *iphdr, const uint8_t *data, size_t len, struct ip_iface *iface)
{
    /* 省略 */
	hdr = (struct icmp_hdr *)data;
    /* ICMP種別がEchoのときのみ応答する */
	switch (hdr->icmp_type) {
		case ICMP_TYPE_ECHO:
			/*
             * Echo Replyを送るので、受信したEchoメッセージの内容をそのまま送り返す。
             * 送信元と宛先アドレスは入れ替えるが、送信元アドレスは
             * 受信インターフェースのユニキャストアドレスにする。
             * (元のEchoメッセージがブロードキャストアドレス宛の可能性があるため)
             * 
			 */
			icmp_output(ICMP_TYPE_ECHO_REPLY, hdr->icmp_code, hdr->dep,
			    (uint8_t *)(hdr + 1), len - sizeof(*hdr), iface->unicast, iphdr->src);
			break;
        /* 他の種別のメッセージは応答しない */
		default:
			/* ignore */
			break;
	}
}

```

- Destination Unreachableメッセージの送信 (p181)
  - 受け取ったIPパケットのプロトコル未登録だった場合、IPパケットは破棄される。
    この場合にDestination Unreachableメッセージを送信する処理を追加する。

```c

static void
ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
    
    /* 省略 */
    /*
     * Destination UnreachableメッセージではIPヘッダとペイロードの先頭8バイト
     * を送ることになっている。このプロジェクトではペイロードの長さが8バイト以上
     * のときだけDestination Unreachableメッセージを送る。
     */
     /* IPパケットのペイロードが8バイト以上のとき(totalはIPパケット全体の長さ) */
	if (hlen + 8 <= total) {
        /* コードはprotocol unreachableとし、送るデータは受け取ったデータの先頭8バイトとする */
		icmp_output(ICMP_TYPE_DEST_UNREACH, ICMP_CODE_PROTO_UNREACH, 0, data, hlen + 8, iface->unicast, hdr->src);
	}
}

```

- 実行結果

```bash
$ ./test/test.exe 2>&1 | tee -i step9.txt
23:11:18.827 [I] setup: setup protocol stack... (test/test.c:36)
23:11:18.827 [I] net_init: initialize... (net.c:196)
23:11:18.827 [I] net_protocol_register: success, type=0x0800 (net.c:172)
23:11:18.827 [I] ip_protocol_register: success, protocol=1 (ip.c:155)
23:11:18.827 [I] net_init: success (net.c:209)
23:11:18.827 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:50)
23:11:18.827 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
23:11:18.827 [I] ip_iface_register: dev=net0, 127.0.0.1, 255.0.0.0, 127.255.255.255 (ip.c:106)
23:11:18.827 [I] net_device_add_iface: success, dev=net0 (net.c:133)
23:11:18.827 [I] net_run: startup... (net.c:218)
23:11:18.827 [I] net_device_open: dev=net0 (net.c:57)
23:11:18.827 [I] net_run: success (net.c:226)
23:11:18.827 [D] app_main: press Ctrl+C to terminate (test/test.c:84)
23:11:18.827 [D] icmp_output: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:155)
	type: 8 (Echo)
	code: 0
	 sum: 0x39ed
	id: 5752
	seq: 1
23:11:18.827 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=12 (ip.c:321)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 40343
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x2043
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:18.827 [D] ip_output_device: dev=net0, len=32, target=127.0.0.1 (ip.c:267)
23:11:18.827 [D] net_device_output: dev=net0, type=0x0800, len=32 (net.c:93)
23:11:18.827 [D] loopback_output: dev=net0, type=0x0800, len=32 (driver/loopback.c:13)
23:11:18.827 [D] net_input: dev=net0, type=0x0800, len=32 (net.c:181)
23:11:18.827 [D] ip_input: dev=net0, len=32 (ip.c:201)
23:11:18.827 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 40343
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x2043
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:18.827 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:115)
	type: 8 (Echo)
	code: 0
	 sum: 0x39ed
	id: 5752
	seq: 1
23:11:18.827 [D] icmp_output: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:155)
	type: 0 (EchoReply)
	code: 0
	 sum: 0x41ed
	id: 5752
	seq: 1
23:11:18.827 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=12 (ip.c:321)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 14851
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x83d7
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:18.827 [D] ip_output_device: dev=net0, len=32, target=127.0.0.1 (ip.c:267)
23:11:18.827 [D] net_device_output: dev=net0, type=0x0800, len=32 (net.c:93)
23:11:18.827 [D] loopback_output: dev=net0, type=0x0800, len=32 (driver/loopback.c:13)
23:11:18.827 [D] net_input: dev=net0, type=0x0800, len=32 (net.c:181)
23:11:18.827 [D] ip_input: dev=net0, len=32 (ip.c:201)
23:11:18.827 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 14851
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x83d7
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:18.827 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:115)
	type: 0 (EchoReply)
	code: 0
	 sum: 0x41ed
	id: 5752
	seq: 1
23:11:19.828 [D] icmp_output: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:155)
	type: 8 (Echo)
	code: 0
	 sum: 0x39ec
	id: 5752
	seq: 2
23:11:19.828 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=12 (ip.c:321)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 44766
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x0efc
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:19.828 [D] ip_output_device: dev=net0, len=32, target=127.0.0.1 (ip.c:267)
23:11:19.828 [D] net_device_output: dev=net0, type=0x0800, len=32 (net.c:93)
23:11:19.828 [D] loopback_output: dev=net0, type=0x0800, len=32 (driver/loopback.c:13)
23:11:19.828 [D] net_input: dev=net0, type=0x0800, len=32 (net.c:181)
23:11:19.828 [D] ip_input: dev=net0, len=32 (ip.c:201)
23:11:19.828 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 44766
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x0efc
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:19.828 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:115)
	type: 8 (Echo)
	code: 0
	 sum: 0x39ec
	id: 5752
	seq: 2
23:11:19.828 [D] icmp_output: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:155)
	type: 0 (EchoReply)
	code: 0
	 sum: 0x41ec
	id: 5752
	seq: 2
23:11:19.828 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=12 (ip.c:321)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 6606
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xa40c
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:19.828 [D] ip_output_device: dev=net0, len=32, target=127.0.0.1 (ip.c:267)
23:11:19.828 [D] net_device_output: dev=net0, type=0x0800, len=32 (net.c:93)
23:11:19.828 [D] loopback_output: dev=net0, type=0x0800, len=32 (driver/loopback.c:13)
23:11:19.828 [D] net_input: dev=net0, type=0x0800, len=32 (net.c:181)
23:11:19.828 [D] ip_input: dev=net0, len=32 (ip.c:201)
23:11:19.828 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 32 (payload: 12)
	   id: 6606
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xa40c
	  src: 127.0.0.1
	  dst: 127.0.0.1
23:11:19.828 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=12 (icmp.c:115)
	type: 0 (EchoReply)
	code: 0
	 sum: 0x41ec
	id: 5752
	seq: 2
23:11:20.534 [D] app_main: terminate (test/test.c:93)
23:11:20.534 [I] cleanup: cleanup protocol stack... (test/test.c:65)
23:11:20.534 [I] net_shutdown: shutting down... (net.c:235)
23:11:20.534 [I] net_device_close: dev=net0 (net.c:75)
23:11:20.534 [I] net_shutdown: success (net.c:242)

```
### 感想
- 特に難しいところはなかった。
  - 前回と今回でICMPの仕様を直接プログラムで学べたのが面白かった。
    - テキストで読むだけよりも解像度が上がるので記憶に定着しやすいと思った。

## step 8 2026-05-17
### 今回やったこと(概要)
- step 8 (書籍 p161~)
 - ICMPメッセージの入力と検証
   - ICMPメッセージの入力処理

### 学べたこと(具体的な内容)
- ICMPの基本
  - ICMPメッセージはエラー通知のために使用される
    - エラーを検出したルータまたは宛先からICMPメッセージが送られてくる
  - ICMPメッセージ自体はIPパケットと同じように送信される
    - IPヘッダのプロトコルフィールドにICMPのプロトコル番号が設定される
    - ICMPメッセージ自体はIPパケットのペイロード部分に格納される
  - ICMPメッセージの種類の区分には「エラー」と「照会」がある
    - 「エラー」はIPパケットの送信過程で起きた問題を送信元に通知する
      - 「Destination Unreachable」 は「3」
    - 「照会」はネットワークやノードの状態を調べるために送信されたメッセージや、
      それに対する応答
      - 「Echo」は「8」、「Echo Reply」は「0」
  - ICMPの基本仕様はRFC792で定められている
  
- ICMPメッセージの構造
  - 種別(1byte)
    - ICMPのメッセージ種別
  - コード(1byte)
    - メッセージをさらに細かく分類する値(メッセージごとに決まっているか、0が設定される)
  - チェックサム(2byte)
    - チェックサムの範囲はICMPメッセージ全体(可変長のデータも含む)であることに注意
  - メッセージ固有フィールド(4byte)
    - メッセージごとに違うものが書き込まれるフィールド
  - データ(可変長)
    - ICMPメッセージのデータ部分

- Echo / Echo Replyメッセージ(種別:8/0)
  - コード
    - 未使用(常に0)
  - メッセージ固有フィールド
    識別子(2byte)とシーケンス番号(2byte)として扱われる
    - 識別子
      Echoメッセージを送信したプロセスのプロセスIDが設定される。
      この値はEcho Replyが返されるときも変わらないので、Echo側はどのプロセスがEchoを
      送信したか、Echo Replyの識別子を見て判断できる。
    - シーケンス番号
      同じプロセスが複数回Echoメッセージを送ったとき、何番目に送信されたメッセージか
      判断するために使用する(初期値は0)。

- Destination Unreachableメッセージ(種別:3)
  - コード
    - エラーの理由を示す番号が格納される(0:network unreachable など)
  - メッセージ固有フィールド
    - 未使用(常に0)
  - データ:
    - 原因となったIPパケットのIPヘッダとペイロードの先頭8バイトが格納される
    
- ICMPで使う構造体 (p167)

```c

/* ICMPヘッダの共通部分を扱う構造体 */
struct icmp_common {
   uint8_t type;  /* 種別 */
    uint8_t code;  /* コード */
    uint16_t sum;  /* チェックサム */
};

/* ICMPヘッダの構造体(メッセージ内容に依存しない) */
struct icmp_hdr
{
    /* ICMPヘッダの共通部分の構造体 */
    struct icmp_common com;
    /* ICMPのメッセージ内容に依存するフィールド */
    uint32_t dep;
};

/* Echoメッセージ用の構造体 */
struct icmp_echo {
    /* ICMPヘッダの共通部分の構造体 */
    struct icmp_common com;
    /* メッセージ固有フィールド */
    uint16_t id;  /* 識別子 */ 
    uint16_t seq;  /* シーケンス番号 */
};

/* Destination Unreachable用の構造体 */
struct icmp_dest_unreach {
    /* ICMPヘッダの共通部分の構造体 */
    struct icmp_common com;
    /* メッセージ固有フィールド */
    uint32_t unused; /* このメッセージでは未使用の領域(常に0) */
};

```
- IPパケットの詳細表示 (p170)

```c

static void
icmp_print(const uint8_t *data, size_t len)
{
	struct icmp_hdr *hdr;
	struct icmp_echo *echo;
	struct icmp_dest_unreach *unreach;

    /* エラー出力が他とかぶらないようにロック */
	flockfile(stderr);
    /* ICMPメッセージをメッセージ内容に依存しないICMPヘッダ構造体でキャスト */
	hdr = (struct icmp_hdr *)data;
    /* icmp_type_ntoaはメッセージを番号から文字列に変える */
	fprintf(stderr, "	type: %u (%s)\n", hdr->icmp_type, icmp_type_ntoa(hdr->icmp_type));
	fprintf(stderr, "	code: %u\n", hdr->icmp_code);
	fprintf(stderr, "        sum: 0x%04x\n", ntoh16(hdr->icmp_sum));
    /* メッセージの種類で分岐 */
	switch (hdr->icmp_type) {
		case ICMP_TYPE_ECHO_REPLY:
		case ICMP_TYPE_ECHO:
            /* Echo/Echo Reply用の構造体にキャスト */
			echo = (struct icmp_echo *)hdr;
			fprintf(stderr, "      	    id: %u\n", ntoh16(echo->id));
			fprintf(stderr, "	   seq: %u\n", ntoh16(echo->seq));
			break;
		case ICMP_TYPE_DEST_UNREACH:
            /* DestinationUnreachable用の構造体にキャスト */
			unreach = (struct icmp_dest_unreach *)hdr;
			fprintf(stderr, "	unused: %u\n", ntoh32(unreach->unused));
			break;
        /* その他のメッセージはこのプロジェクトでは対応しない */
		default:
			fprintf(stderr, "	   dep: 0x%08x\n", ntoh32(hdr->dep));
			break;
	}
#ifdef HEXDUMP
	hexdump(stderr, data, len);
#endif
    /* ロックの解除 */
	funlockfile(stderr);
}

```

- メッセージタイプを文字列に変換(p171)
  - ICMPメッセージタイプの値を引数として、ICMPメッセージタイプの文字列を返す(コードは省略)

- ICMPメッセージの検証 (p172)
  - ICMPメッセージの長さ、チェックサムを検証する(コードは省略)

- 実行結果
```bash
$ ./test/test.exe 2>&1 | tee -i step8.txt
18:34:30.238 [I] setup: setup protocol stack... (test/test.c:35)
18:34:30.238 [I] net_init: initialize... (net.c:196)
18:34:30.238 [I] net_protocol_register: success, type=0x0800 (net.c:172)
18:34:30.238 [I] ip_protocol_register: success, protocol=1 (ip.c:155)
18:34:30.238 [I] net_init: success (net.c:209)
18:34:30.238 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:50)
18:34:30.238 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
18:34:30.238 [I] ip_iface_register: dev=net0, 127.0.0.1, 255.0.0.0, 127.255.255.255 (ip.c:106)
18:34:30.238 [I] net_device_add_iface: success, dev=net0 (net.c:133)
18:34:30.238 [I] net_run: startup... (net.c:218)
18:34:30.238 [I] net_device_open: dev=net0 (net.c:57)
18:34:30.238 [I] net_run: success (net.c:226)
18:34:30.238 [D] app_main: press Ctrl+C to terminate (test/test.c:80)
18:34:30.238 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=28 (ip.c:314)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 24383
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x5e8b
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:34:30.238 [D] ip_output_device: dev=net0, len=48, target=127.0.0.1 (ip.c:260)
18:34:30.238 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:93)
18:34:30.238 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
18:34:30.238 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:181)
18:34:30.238 [D] ip_input: dev=net0, len=48 (ip.c:201)
18:34:30.238 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 24383
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x5e8b
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:34:30.238 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=28 (icmp.c:112)
	type: 8 (Echo)
	code: 0
	 sum: 0x3564
	id: 128
	seq: 1
18:34:31.240 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=28 (ip.c:314)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 14599
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x84c3
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:34:31.240 [D] ip_output_device: dev=net0, len=48, target=127.0.0.1 (ip.c:260)
18:34:31.240 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:93)
18:34:31.241 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
18:34:31.241 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:181)
18:34:31.241 [D] ip_input: dev=net0, len=48 (ip.c:201)
18:34:31.241 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 14599
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x84c3
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:34:31.241 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=28 (icmp.c:112)
	type: 8 (Echo)
	code: 0
	 sum: 0x3564
	id: 128
	seq: 1
18:34:31.649 [D] app_main: terminate (test/test.c:88)
18:34:31.649 [I] cleanup: cleanup protocol stack... (test/test.c:64)
18:34:31.649 [I] net_shutdown: shutting down... (net.c:235)
18:34:31.649 [I] net_device_close: dev=net0 (net.c:75)
18:34:31.649 [I] net_shutdown: success (net.c:242)
```

### 感想
- 特に難しいところはなかった。
  - 前回までから引き続き、必要なときにキャストで構造体の見方を変える手法が出てきた。
    - この方法には大分慣れたので自作のプログラムでも応用してみたい。
    - パディングのようなコンパイラの動作には注意するように気を付けたい。

## step 7 2026-05-15
### 今回やったこと
- step 7 (書籍 p147)
  - 上位プロトコルの管理

### 学べたこと(具体的な内容)

- プロトコルの登録 (p152)

```c

int
ip_protocol_register(uint8_t protocol, ip_protocol_handler_t handler)
{
	struct ip_protocol *entry;
    /* プロトコルのリストを先頭から走査して、登録するプロトコルがすでにあるならエラー */
	for (entry = protocols; entry; entry = entry->next) {
		if (entry->protocol == protocol) {
			errorf("already exists, protocol=%u", protocol);
			return -1;
		}
	}
    /* メモリ確保 */
	entry = memory_alloc(sizeof(*entry));
	if (!entry) {
		errorf("memory_alloc() failure");
		return -1;
	}
    /* 各値を入れる */
	entry->protocol = protocol;
	entry->handler = handler;
    /* 登録したプロトコルをリストの先頭に追加 */
	entry->next = protocols;
	protocols = entry;
	infof("success, protocol=%u", protocol);
	return 0;
}

```

- パケットの振り分け (p153)
  - プロトコル登録リストを走査して、IPヘッダのprotocolフィールドと一致するプロトコルを探す。
  - プロトコルが見つかったらプロトコルの入力ハンドラを起動する。

```c

static void
ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
	struct ip_hdr *hdr;
	uint8_t v;
	uint16_t hlen, total, offset;
	struct ip_iface *iface;
	char addr[IP_ADDR_STR_LEN];
	struct ip_protocol *proto;

	debugf("dev=%s, len=%zu", dev->name, len);
    /* IPヘッダの長さをチェック */
	if (len < IP_HDR_SIZE_MIN) {
		errorf("too short");
		return;
	}
	hdr = (struct ip_hdr *) data;
    /* ビット操作してIPのバージョンを取り出す */
	v = hdr->vhl >> 4;
    /* IPv4でないならエラー */
	if (v != IP_VERSION_IPV4) {
		errorf("ip version error: v=%u", v);
		return;
	}
    /* ヘッダ長を得るために、下位4ビットを取り出したあとに4倍(4バイト単位なので) */
	hlen = (hdr->vhl & 0x0f) << 2;
	if (len < hlen) {
		errorf("header length error: len=%zu < hlen=%u", len, hlen);
		return;
	}
    /* IPヘッダチェックサムの計算 */
	if (cksum16((uint16_t *)hdr, hlen, 0) != 0) {
		errorf("checksum error");
		return;
	}
    /* バイトオーダー変換 */
	total = ntoh16(hdr->total);
	if (len < total) {
		errorf("total length error: len=%zu < total=%u", len, total);
		return;
	}
	offset = ntoh16(hdr->offset);
    /* フラグメントに未対応なので、MFフラグが1かフラグメントオフセットが0でないときエラー */
	if (offset & IP_HDR_FLAG_MF || offset & IP_HDR_OFFSET_MASK) {
		errorf("fragments do not support");
		return;
	}
    /* デバイスに紐づくインターフェースを探し、それをIPインターフェース構造体にダウンキャスト */
	iface = (struct ip_iface *)net_device_get_iface(dev, NET_IFACE_FAMILY_IP);
	if (!iface) {
		/* ignore */
		return;
	}
    /* 自分宛てのパケットでないなら無視 */
	if (hdr->dst != iface->unicast) {
		if (hdr->dst != iface->broadcast && hdr->dst != IP_ADDR_BROADCAST) {
			/* ignore: for other host */
			return;
		}
	}
	debugf("permit, dev=%s, iface=%s", dev->name, ip_addr_ntop(iface->unicast, addr, sizeof(addr)));
	ip_print(data, total);
    /* プロトコルのリストを走査して、一致したらハンドラを起動 */
	for (proto = protocols; proto; proto = proto->next) {
		if (proto->protocol == hdr->protocol) {
			proto->handler(hdr, data + hlen, total - hlen, iface);
			return;
		}
	}
	/* unsupported protocol */
}

```

- ICMPモジュールの初期化関数(p154)
  - ip_protocol_register()を呼ぶ(省略)

- ICMPモジュールの入力ハンドラ(p155)

```c 

static void
icmp_input(const struct ip_hdr *iphdr, const uint8_t *data, size_t len, struct ip_iface *iface)
{
	char addr1[IP_ADDR_STR_LEN];
	char addr2[IP_ADDR_STR_LEN];
	
	debugf("%s => %s, len=%zu",
        /* iphdrはICMPデータを持ってきたIPパケットのヘッダの先頭 */
	    ip_addr_ntop(iphdr->src, addr1, sizeof(addr1)),
	    ip_addr_ntop(iphdr->dst, addr2, sizeof(addr2)), len);
	debugdump(data, len);
}

```

- net_init()にicmp_initの呼び出しを追記(省略) (p156)

- テストプログラムでのIPモジュールの送信関数の呼び出しで、ICMPのプロトコル番号を使うよう変更(省略)

- 実行結果

```bash
$ ./test/test.exe 2>&1 | tee -i step_7.txt
18:32:06.985 [I] setup: setup protocol stack... (test/test.c:35)
18:32:06.985 [I] net_init: initialize... (net.c:196)
18:32:06.985 [I] net_protocol_register: success, type=0x0800 (net.c:172)
18:32:06.985 [I] ip_protocol_register: success, protocol=1 (ip.c:155)
18:32:06.985 [I] net_init: success (net.c:209)
18:32:06.985 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:50)
18:32:06.985 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
18:32:06.985 [I] ip_iface_register: dev=net0, 127.0.0.1, 255.0.0.0, 127.255.255.255 (ip.c:106)
18:32:06.985 [I] net_device_add_iface: success, dev=net0 (net.c:133)
18:32:06.985 [I] net_run: startup... (net.c:218)
18:32:06.985 [I] net_device_open: dev=net0 (net.c:57)
18:32:06.985 [I] net_run: success (net.c:226)
18:32:06.985 [D] app_main: press Ctrl+C to terminate (test/test.c:80)
18:32:06.985 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=28 (ip.c:314)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 64564
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xc195
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:32:06.985 [D] ip_output_device: dev=net0, len=48, target=127.0.0.1 (ip.c:260)
18:32:06.985 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:93)
18:32:06.985 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
18:32:06.985 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:181)
18:32:06.985 [D] ip_input: dev=net0, len=48 (ip.c:201)
18:32:06.985 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 64564
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xc195
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:32:06.985 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=28 (icmp.c:14)
18:32:07.987 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=28 (ip.c:314)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 64672
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xc129
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:32:07.987 [D] ip_output_device: dev=net0, len=48, target=127.0.0.1 (ip.c:260)
18:32:07.987 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:93)
18:32:07.987 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
18:32:07.987 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:181)
18:32:07.987 [D] ip_input: dev=net0, len=48 (ip.c:201)
18:32:07.987 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:242)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 64672
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xc129
	  src: 127.0.0.1
	  dst: 127.0.0.1
18:32:07.987 [D] icmp_input: 127.0.0.1 => 127.0.0.1, len=28 (icmp.c:14)
18:32:08.322 [D] app_main: terminate (test/test.c:88)
18:32:08.322 [I] cleanup: cleanup protocol stack... (test/test.c:64)
18:32:08.322 [I] net_shutdown: shutting down... (net.c:235)
18:32:08.322 [I] net_device_close: dev=net0 (net.c:75)
18:32:08.322 [I] net_shutdown: success (net.c:242)

```
### 感想
- 特に難しいところはなかった。
  - 次回からICMPモジュールの実装なので、「マスタリング TCP/IP」(p198~)を読んで予習しておこうと思う。

#### メモ(本筋から外れた、あるいは重要度の低い内容)
## step 6 2026-05-13
### 今回やったこと(概要)
- step 6 (書籍 p133~)
  - IPパケットの送信
    - ルーティングやアドレス解決はまだ実装しない

### 学べたこと(具体的な内容)
- IPモジュールの送信 (p136)
  - 上位のプロトコルから送信データを受ける関数

```c

ssize_t
ip_output(uint8_t protocol, const uint8_t *data, size_t len, ip_addr_t src, ip_addr_t dst)
{
	char addr1[IP_ADDR_STR_LEN];
	char addr2[IP_ADDR_STR_LEN];
	struct ip_iface *iface;
	uint16_t id;
    /* パケットの長さ */
	ssize_t plen;
    /* ip_build_packet()で使う、IPパケットを入れる入れ物 */
	uint8_t buf[IP_TOTAL_SIZE_MAX];
	
    /* バイナリから文字列へ変換 */
	ip_addr_ntop(src, addr1, sizeof(addr1));
	ip_addr_ntop(dst, addr2, sizeof(addr2));
	debugf("%s => %s, protocol=%d, len=%zu", addr1, addr2, protocol, len);
    /* ルーティングはまだ未実装(エラー) */
    /* IP_ADDR_ANYはIPアドレスが指定されていない状態 */
	if (src == IP_ADDR_ANY) {
		errorf("ip routing does not implement");
		return -1;
	}
    /* srcをもつインターフェースの検索 */
	iface = ip_iface_select(src);
	if (!iface) {
		errorf("iface not found, src=%s", addr1);
		return -1;
	}
    /*
     * 現段階ではIPパケットは同じネットワーク内に送る必要があるので、
     * サブネットマスクを利用して宛先アドレスと送信元アドレスのネットワーク部を
     * 比較し、一致しなければ届かないのでエラー。(ブロードキャスト通信ならOK)
     */
	if ((dst & iface->netmask) != (iface->unicast & iface->netmask) && dst != IP_ADDR_BROADCAST) {
		errorf("not reached, dst=%s", addr2);
		return -1;
	}
    /* MTU(最大転送データサイズ)とIPパケットのサイズを比較(IPオプションは使わないのでヘッダサイズは常に最小) */ 
    /* このプロジェクトではフラグメントは行わない */
	if (NET_IFACE(iface)->dev->mtu < IP_HDR_SIZE_MIN + len) {
		errorf("too long, dev=%s, mtu=%u < %zu", 
		    NET_IFACE(iface)->dev->name, NET_IFACE(iface)->dev->mtu, IP_HDR_SIZE_MIN + len);
		return -1;
	}
    /* IDをランダムに設定 */
	id = random16();
    /* IPパケットを構築し、戻り値としてIPパケットの長さを返す */
	plen = ip_build_packet(protocol, data, len, id, 0, iface->unicast, dst, buf, sizeof(buf));
	if (plen == -1) {
		errorf("ip_build_packet() failure");
		return -1;
	}
    /* IPパケットの送信 */
	if (ip_output_device(iface, buf, plen, dst) == -1) {
		errorf("ip_output_device() failure");
		return -1;
	}
	return plen;
}

```

- IPパケットの組み立て (p139)

```c

static ssize_t
ip_build_packet(uint8_t protocol, const uint8_t *data, size_t len, uint16_t id,
                uint16_t offset, ip_addr_t src, ip_addr_t dst, uint8_t *buf, size_t size)
{
	uint16_t hlen, total;
	struct ip_hdr *hdr;

	hlen = IP_HDR_SIZE_MIN;
	total = hlen + len;
	if (size < total) {
		return -1;
	}
    /* 
     * bufをip_hdrのポインタにキャストすることで、構造体として直感的に各値を入れられる。
     * ip_hdrのメンバはコンパイラがパディングを勝手に入れないようにバイト数を調整されて
     * いるので、キャストしてもぴったりbufに入るらしい。構造体をキャストするときはパディング
     * に注意(もしくはコンパイラに指示してパディングを入れないように)して、メモリの状態を
     * 完全に把握する必要がある。
     */
	hdr = (struct ip_hdr *)buf;
    /* ビット操作して各値を取り出す */
    /* hlenは4バイト単位なので4で割る(2ビット右シフト) */
	hdr->vhl = (IP_VERSION_IPV4 << 4) | (hlen >> 2);
	hdr->tos = 0;
    /* ネットワークバイトオーダーに変換 */
	hdr->total = hton16(total);
	hdr->id = hton16(id);
	hdr->offset = hton16(offset);
	hdr->ttl = 0xff;
	hdr->protocol = protocol;
    /* ここでIPヘッダチェックサムに0に入れておかないとチェックサムの計算が変わるので注意 */
	hdr->sum = 0;
	hdr->src = src;
	hdr->dst = dst;
    /* IPヘッダのチェックサム計算 */
	hdr->sum = cksum16((uint16_t *)hdr, hlen, 0); /* don't convert byteorder */
    /* ペイロード部分からデータを書き込む */
	memcpy(buf + hlen, data, len);
	ip_print(buf, total);
	return (ssize_t)total;
}

```

- ネットワークデバイスからの送信 (p141)

```c

static int
ip_output_device(struct ip_iface *iface, const uint8_t *data, size_t len, ip_addr_t target)
{
	char addr[IP_ADDR_STR_LEN];
    /* ハードウェアアドレス */
	uint8_t hwaddr[NET_DEVICE_ADDR_LEN] = {};

    /* バイナリから文字列へ変換 */
	ip_addr_ntop(target, addr, sizeof(addr));
	debugf("dev=%s, len=%zu, target=%s", NET_IFACE(iface)->dev->name, len, addr);
    /* アドレス解決が必要かチェック */
	if (NET_IFACE(iface)->dev->flags & NET_DEVICE_FLAG_NEED_ARP) {
        /* ブロードキャストだった場合はブロードキャストアドレスをコピーしてくる */
		if (target == iface->broadcast || target == IP_ADDR_BROADCAST) {
			memcpy(hwaddr, NET_IFACE(iface)->dev->broadcast, NET_IFACE(iface)->dev->alen);
		}
        /* ユニキャストアドレスの解決(ARP)はまだ未実装なのでエラー */
		else {
			errorf("ARP does not implement");
			return -1;
		}
	}
    /* ネットワークデバイスからIPパケットを出力 */
	return net_device_output(NET_IFACE(iface)->dev, NET_PROTOCOL_TYPE_IP, data, len, hwaddr);
}

```

- テストプログラムの変更(p142)
  - ネットワークデバイスの送信関数をIPモジュールの送信関数に変更(省略)

- 実行結果

```bash
$ ./test/test.exe 2>&1 | tee -i step6_test.txt
22:42:44.929 [I] setup: setup protocol stack... (test/test.c:35)
22:42:44.930 [I] net_init: initialize... (net.c:195)
22:42:44.930 [I] net_protocol_register: success, type=0x0800 (net.c:171)
22:42:44.930 [I] net_init: success (net.c:204)
22:42:44.930 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:49)
22:42:44.930 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
22:42:44.930 [I] ip_iface_register: dev=net0, 127.0.0.1, 255.0.0.0, 127.255.255.255 (ip.c:99)
22:42:44.930 [I] net_device_add_iface: success, dev=net0 (net.c:132)
22:42:44.930 [I] net_run: startup... (net.c:213)
22:42:44.930 [I] net_device_open: dev=net0 (net.c:56)
22:42:44.930 [I] net_run: success (net.c:221)
22:42:44.930 [D] app_main: press Ctrl+C to terminate (test/test.c:80)
22:42:44.930 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=28 (ip.c:272)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 14790
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x8404
	  src: 127.0.0.1
	  dst: 127.0.0.1
22:42:44.930 [D] ip_output_device: dev=net0, len=48, target=127.0.0.1 (ip.c:218)
22:42:44.930 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
22:42:44.930 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
22:42:44.930 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:180)
22:42:44.930 [D] ip_input: dev=net0, len=48 (ip.c:166)
22:42:44.930 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:207)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 14790
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x8404
	  src: 127.0.0.1
	  dst: 127.0.0.1
22:42:45.930 [D] ip_output: 127.0.0.1 => 127.0.0.1, protocol=1, len=28 (ip.c:272)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 13448
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x8942
	  src: 127.0.0.1
	  dst: 127.0.0.1
22:42:45.931 [D] ip_output_device: dev=net0, len=48, target=127.0.0.1 (ip.c:218)
22:42:45.931 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
22:42:45.931 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
22:42:45.931 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:180)
22:42:45.931 [D] ip_input: dev=net0, len=48 (ip.c:166)
22:42:45.931 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:207)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 13448
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0x8942
	  src: 127.0.0.1
	  dst: 127.0.0.1
22:42:46.838 [D] app_main: terminate (test/test.c:88)
22:42:46.838 [I] cleanup: cleanup protocol stack... (test/test.c:64)
22:42:46.838 [I] net_shutdown: shutting down... (net.c:230)
22:42:46.838 [I] net_device_close: dev=net0 (net.c:74)
22:42:46.838 [I] net_shutdown: success (net.c:237)

```

### 感想
- 予め確保した配列をキャストして構造体とみなして扱う手法は面白かった。
  これによるメリットは複数あるらしい。
  - 可変長のデータ(ペイロード)を扱うときヘッダとペイロードを一つのbufに入れて、
    隙間を詰めるようにデータを入れることができる(今回はmemcpyを使った)
    - 隙間を詰めることでデータ送信時にそのまま送れば良くなるので無駄がない
  - 同じbufを何度も使いまわすことができる

## step 5 2026-05-06
### 今回やったこと(概要)
- step 5 (書籍 p113~)
  - 論理インターフェース

### 学べたこと(具体的な内容)
- 論理インターフェース
  - IPアドレスのような論理アドレスをもつ
     - ネットワークデバイスに紐付ける(複数可能にする)

- インターフェース構造体 (p118)

```c

struct net_iface {
    /* インターフェースをつなげてリストとして管理 */
    struct net_iface *next;
    /* 紐付いたネットワークデバイスへのポインタ */
    struct net_device *dev; /* back pointer to parent */
    /* 
     * familyはIPv4かIPv6なのかといった、どのプロトコルのためのインターフェースかを示す定数
     * ファミリ(プロトコルの種類)に依存したアドレス等のデータは別の構造体で定義する 
     * (あくまでnet_iface構造体は汎用的なものにする)
     */
    int family;
    /* depends on implementation of protocols. */
};

```

- ネットワークデバイスとインターフェースを紐付ける(p120)

```c

int
net_device_add_iface(struct net_device *dev, struct net_iface *iface)
{
	struct net_iface *entry;
    /* インターフェースのリストを走査する */
	for (entry = dev->ifaces; entry; entry = entry->next) {
		if (entry->family == iface->family) {
			/*
			 * NOTE: For simplicity, only one iface can be added per family.
			 */
			return -1;
		}
	}
    /* リストの先頭に追加 */
	iface->next = dev->ifaces;
    /* 各値を入れる */
	iface->dev = dev;
	dev->ifaces = iface;
	infof("success, dev=%s", dev->name);
	return 0;
}

```

- ネットワークデバイスに紐付けられたインターフェースを取得する(p112)

```c

struct net_iface *
net_device_get_iface(struct net_device *dev, int family)
{
	struct net_iface *entry;
	/* インターフェースのリストを先頭から走査する */
	for (entry = dev->ifaces; entry; entry = entry->next) {
		if (entry->family == family) {
			break;
		}
	}
	return entry;
}

```

- IPインターフェース構造体(p123)

```c

struct ip_iface {
    /*
     * 先頭にnet_iface構造体を配置することで、(struct net_iface *)型に
     * キャストしたとき、net_ifaceの部分だけがピックアップされる。
     * 直接net_ifaceが扱えるようになるので、汎用的なインターフェース構造体
     * として使えるようになる。
     */
    struct net_iface iface;
    struct ip_iface *next;
    ip_addr_t unicast;
    ip_addr_t netmask;
    ip_addr_t broadcast;
};

```

- IPインターフェースの割当 (p124)

```c

struct ip_iface *
ip_iface_alloc(const char *unicast, const char *netmask)
{
	struct ip_iface *iface;
    /* メモリ確保 */
	iface = memory_alloc(sizeof(*iface));
	if (!iface) {
		errorf("memory_alloc() failure");
		return NULL;
	}
    /* 
     * NET_IFACEはマクロで、(struct net_iface *)でキャストしている。
     * これによってip_iface構造体の最初のフィールド(struct net_iface iface)
     * だけがピックアップされる形になる。
     */
	NET_IFACE(iface)->family = NET_IFACE_FAMILY_IP;
    /* 文字列からネットワークバイトオーダーのバイナリに変換 */
	if (ip_addr_pton(unicast, &iface->unicast) == -1) {
		errorf("ip_addr_pton() failure, addr=%s", unicast);
		memory_free(iface);
		return NULL;
	}
	if (ip_addr_pton(netmask, &iface->netmask) == -1) {
		errorf("ip_addr_pton() failure, addr=%s", netmask);
		memory_free(iface);
		return NULL;
	}
    /*
     * ブロードキャストアドレスの計算をする。
     * ユニキャストアドレスとサブネットマスクの論理積を計算すると、
     * ネットワーク部だけが残る。これとサブネットマスクのビット反転
     * との論理和を計算すると、ちょうどホスト部がすべて1になって
     * ブロードキャストアドレスになる。
     */
	iface->broadcast = (iface->unicast & iface->netmask) | ~iface->netmask;
	return iface;
}

```

- IPインターフェースを登録する (p126)

```c

/*
 * NOTE: must not be call after net_run()
 */
int
ip_iface_register(struct net_device *dev, struct ip_iface *iface)
{
	char addr1[IP_ADDR_STR_LEN];
	char addr2[IP_ADDR_STR_LEN];
	char addr3[IP_ADDR_STR_LEN];
    
    /* ユニキャストIP、サブネットマスク、ブロードキャストIPをバイナリから文字列に変換 */ 
	infof("dev=%s, %s, %s, %s", dev->name,      
        ip_addr_ntop(iface->unicast, addr1, sizeof(addr1)),
	    ip_addr_ntop(iface->netmask, addr2, sizeof(addr2)),
	    ip_addr_ntop(iface->broadcast, addr3, sizeof(addr3)));
    /* 第二引数はNET_IFACEでキャストすることで汎用的なインターフェース構造体になっている */
	if (net_device_add_iface(dev, NET_IFACE(iface)) == -1) {
		errorf("net_device_add_iface() failure");
		return -1;
	    }
    /* リストの先頭に追加 */
	iface->next = ifaces;
	ifaces = iface;
	return 0;
}

```

- struct ip_iface * 
  ip_iface_select(ip_addr_t addr)    (p127)
   - 登録しているIPインターフェースを操作して引数 のIPと一致する
     ユニキャストIPアドレスが設定されたIPインターフェースを見つけて返す。(省略)


- IPパケットのフィルタリングをする (p128)
  - 自分宛てのパケットやブロードキャストのパケットを区別する

```c

static void
ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
    /* 省略 */
	struct ip_iface *iface;
	char addr[IP_ADDR_STR_LEN];
    /* 省略 */
    /* 
     * net_device_get_ifaceの戻り値はstruct net_ifaceという汎用的なインターフェース構造体
     * だが、これはもともとstruct ip_ifaceからキャストして来た一部なので、もう一度
     * 元のstruct ip_ifaceに戻してip_iface固有の情報にアクセスできる。(ダウンキャスト)
     */
	iface = (struct ip_iface *)net_device_get_iface(dev, NET_IFACE_FAMILY_IP);
	if (!iface) {
		/* ignore */
		return;
	}
    /* ユニキャストアドレスかブロードキャストアドレスに一致するか確認 */
	if (hdr->dst != iface->unicast) {
		if (hdr->dst != iface->broadcast && hdr->dst != IP_ADDR_BROADCAST) {
			/* ignore: for other host */
			return;
		}
	}
	debugf("permit, dev=%s, iface=%s", dev->name, ip_addr_ntop(iface->unicast, addr, sizeof(addr)));
	ip_print(data, total);
}

```

- テストプログラムを変更してビルドした。(p129)
  - 省略

- 実験結果

```bash
$ ./test/test.exe 2>&1 | tee -i step5_test.txt
19:02:02.802 [I] setup: setup protocol stack... (test/test.c:37)
19:02:02.803 [I] net_init: initialize... (net.c:195)
19:02:02.803 [I] net_protocol_register: success, type=0x0800 (net.c:171)
19:02:02.803 [I] net_init: success (net.c:204)
19:02:02.803 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:49)
19:02:02.803 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
19:02:02.803 [I] ip_iface_register: dev=net0, 127.0.0.1, 255.0.0.0, 127.255.255.255 (ip.c:97)
19:02:02.803 [I] net_device_add_iface: success, dev=net0 (net.c:132)
19:02:02.803 [I] net_run: startup... (net.c:213)
19:02:02.803 [I] net_device_open: dev=net0 (net.c:56)
19:02:02.803 [I] net_run: success (net.c:221)
19:02:02.803 [D] app_main: press Ctrl+C to terminate (test/test.c:77)
19:02:02.803 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
19:02:02.803 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
19:02:02.803 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:180)
19:02:02.803 [D] ip_input: dev=net0, len=48 (ip.c:164)
19:02:02.803 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:205)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 128
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xbd4a
	  src: 127.0.0.1
	  dst: 127.0.0.1
19:02:03.804 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
19:02:03.804 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
19:02:03.804 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:180)
19:02:03.804 [D] ip_input: dev=net0, len=48 (ip.c:164)
19:02:03.804 [D] ip_input: permit, dev=net0, iface=127.0.0.1 (ip.c:205)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 128
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xbd4a
	  src: 127.0.0.1
	  dst: 127.0.0.1
19:02:04.148 [D] app_main: terminate (test/test.c:85)
19:02:04.148 [I] cleanup: cleanup protocol stack... (test/test.c:66)
19:02:04.148 [I] net_shutdown: shutting down... (net.c:230)
19:02:04.148 [I] net_device_close: dev=net0 (net.c:74)
19:02:04.148 [I] net_shutdown: success (net.c:237)
```

### 感想
- IPアドレスの仕様について詳しく知らなかったので、今回「マスタリングTCP/IP 入門編」を読みながら
  IPアドレスの基本を学んだ。サブネットマスクが生まれた経緯も書いてあったので(同書p155)、
  仕組みを覚えるのに役立った。
- ダウンキャストの部分が少し難しかったが、何度か読んだりLLMを使ったりして理解できた。
  - 他の言語でもある概念らしいのでここで理解できてよかった。

## step 4 2026-05-05
### 今回やったこと(概要)
- step 4 (書籍 p91~)
  - IPパケットの入力と検証

### 学べたこと(具体的な内容)
#### IPパケット
- データリンクはそれぞれ違った仕様をもつが、IPはデータリンクの特性の違いを吸収して抽象化できる
- 論理アドレス
  - IPアドレスはデータリンクに依存しないので、TCP/IP上の全てのノードを一意に識別できる
- パケット分割
  - IPはデータリンクによるMTU（ネットワークデバイスから一度に送信できるデータの最大サイズ)の違いを吸収するために、IPパケットを分割する仕組みをもつ
  - 分割されたパケットはパケットの受け取りノードでIPヘッダの情報から復元される
    
#### IPパケットの構造 
- IPパケットはヘッダとペイロード(データ部分)に分かれている。

- ヘッダ
  - バージョン(4bit)
    - IPのバージョン。IPv4なら「4」
  - ヘッダ長(4bit)
    - IPヘッダの長さ。4バイト単位(実際の値を4で割ったもの)で設定される。(IPヘッダの長さは必ず4の倍数)
  - サービス種別(8bit)
    - IPパケットの優先度などを指定する。(あまり使われないので今回のプロジェクトでは0)
  - データグラム全長(16bit)
    - IPパケット全体の長さ。1バイト単位で値を設定。
  - 識別子(16bit)
    - IPパケットを識別するためのID。送信元がIPパケットごとに異なる値を設定。分割されたパケットは同じIDを共有。識別子から元のパケットを復元する。
  - フラグ(3bit)
    - IPパケットが分割できるかを表す。1ビット目は常に0。2ビット目は1なら分割できないパケット。3ビット目は1ならこれは分割されたパケットで、続きのパケットがあることを表す。
  - フラグメント・オフセット(13bit)
    - 分割されたIPパケットのどの部分にあたるかを表す。(8バイト単位)
    - 分割されてないパケットまたは最初のパケット断片なら0に設定。
  - 生存時間(8bit)
    - TTL。実際は中継可能回数を表し、IPパケットがルータなどにより中継される度にデクリメントされ、0になるとパケットが破棄される。
  - プロトコル番号(8bit)
    - ペイロードのデータのプロトコルを示す番号で、IANAによって番号が管理されている。/etc/protocolsでプロトコルと番号の対応表が見られる。
  - ヘッダ・チェックサム(16bit)
    - IPヘッダのチェックサムであることに注意。(IP全体ではない)
  - 送信元アドレス(32bit)
  - 宛先アドレス(32bit)
  - オプション(可変長)
    - IPヘッダにオプションをつけることができる。(実際の通信ではあまり使われない) 
    - IPヘッダの長さは4の倍数である必要がある(ヘッダ長を設定するフィールドは4バイト単位)ので、必要ならパディング(詰めもの)を入れてバイト数を調整する。
  
- バイトオーダー (p96)
  - ネットワークに複数バイトの数値データを送るときは原則ビッグエンディアンにするというルールがある(このあたりは「独習アセンブラ」の書籍で読んだことがあるので省略した)
- チェックサムの計算(p97)
   - IPヘッダの16ビットごとの値を足して、桁あふれ分はすべて下位のビットに足す。(数学のmodのイメージ)
   - 計算した値をビット反転し、チェックサムに入れて送信。
   - 受信側は同様にIPヘッダの値とチェックサムの値を足し合わせた後、桁あふれ分を下位ビットに足す。それをビット反転して0になったら検証成功。
  
#### コード
- IPヘッダ構造体 (p103) (ip.h)
  - 一部のヘッダのフィールド(ビット数が少ないもの)はまとめられている

```c

struct ip_hdr {
    uint8_t vhl;  /* IPバージョン(4bit)とヘッダ長(4bit) */
    uint8_t tos;  /* サービス種別(8bit)、0が入る */
    uint16_t total;  /* データグラム全長(16bit) */
    uint16_t id;  /* 識別子(16bit) */
    uint16_t offset;  /* フラグ(3bit)とフラグメントオフセット(13bit) */
    uint8_t ttl;  /* 生存時間(8bit) */
    uint8_t protocol;  /* プロトコル番号(8bit) */
    uint16_t sum;  /* ヘッダチェックサム(16bit) */
    ip_addr_t src;  /* 送信元アドレス(32bit) */
    ip_addr_t dst;  /* 宛先アドレス(32bit) */
};

```

- 文字列からバイナリへ変換する関数 (p104)
  - IPアドレスをバイナリとしてネットワークバイトオーダー(ビックエンディアン)で格納する
```c

/* 引数: *p - IPアドレスの文字列  *n - 変換した後のバイナリを入れるバッファ */
int
ip_addr_pton(const char *p, ip_addr_t *n)
{
    char *sp, *ep;
    /* オクテットの番号(0~3) */
    int idx;
    /* 変換したIPアドレスのオクテットを一時的に受け取る */
    long ret;

    sp = (char *)p;
    for (idx = 0; idx < 4; idx++) {
        /* IPアドレスの文字列をlongに変換し、読み取れないドット(.)をepに入れる */
        ret = strtol(sp, &ep, 10);
        /* 読み取る数字はは0~255の範囲のはずなので、違うならエラー */
        if (ret < 0 || ret > 255) {
            return -1;
        }
        /* 数字が読み取れないならエラー */
        if (ep == sp) {
            return -1;
        }
        /* ドットまたは終端文字が適切でないならエラー */
        if ((idx == 3 && *ep != '\0') || (idx != 3 && *ep != '.')) {
            return -1;
        }
        /* nをip_addr_t(これはuint32_tと同じ)型からuint8_t型にキャストする。
         * これによりnをuint32_t型の変数1個ではなくuint8_t型の配列のポインタとみなすことで、
         * 1バイトずつret内のオクテットを書き込むことができる。
         * (n[0]は第一オクテット、n[1]は第二オクテットという風に扱える。)
         */
        ((uint8_t *)n)[idx] = ret;
        sp = ep + 1;  /* ドットの次にspを移動させ、次のオクテットを読めるようにする */
    }
    return 0;
}

```

- ip_print関数 (p106)

```c

static void
ip_print(const uint8_t *data, size_t len)
{
    /* IPヘッダのポインタ */
	struct ip_hdr *hdr;
    /* IPヘッダの各フィールドの変数を定義 */
	uint8_t v, hl, hlen;
	uint16_t total, offset;
	char addr[IP_ADDR_STR_LEN];
    
    /* 途中で他の出力が混ざらないようにする */
	flockfile(stderr);
    /* 受け取ったデータをstruct ip_hdr構造体として扱う */
	hdr = (struct ip_hdr *)data;
    /* バージョンはvhlを右に4ビット移動させれば良い(構造体の定義を参照) */
	v = hdr->vhl >> 4;
    /* 0x0fとの論理積で上位ビットを取り除き、ヘッダ長を得る */
	hl = hdr->vhl & 0x0f;
    /* 4バイト単位なので実際の数値を得るために4倍 = 2^2 = 2ビット左に移動すれば良い */
	hlen = hl << 2;
	fprintf(stderr, "	  vhl: 0x%02x [v: %u, hl: %u (%u)]\n", hdr->vhl, v, hl, hlen);
	fprintf(stderr, "	  tos: 0x%02x\n", hdr->tos);
    /* ntoh16()で多バイト長の数値のバイトオーダーの変換をする */
	total = ntoh16(hdr->total);
	fprintf(stderr, "	total: %u (payload: %u)\n", total, total - hlen);
	fprintf(stderr, "	   id: %u\n", ntoh16(hdr->id));
	offset = ntoh16(hdr->offset);
    /* オフセットの上位3ビットと下位13ビットをそれぞれ取り出す */
	fprintf(stderr, "      offset: 0x%04x [flags=%x, offset=%u]\n",
		offset, offset >> 13, offset & IP_HDR_OFFSET_MASK);
	fprintf(stderr, "	  ttl: %u\n", hdr->ttl);
	fprintf(stderr, "    protocol: %u\n", hdr->protocol);
	fprintf(stderr, "	  sum: 0x%04x\n", ntoh16(hdr->sum));
    /* IPアドレスのバイナリ(ネットワークバイトオーダー)を文字列に変換 */
	fprintf(stderr, "	  src: %s\n", ip_addr_ntop(hdr->src, addr, sizeof(addr)));
	fprintf(stderr, "	  dst: %s\n", ip_addr_ntop(hdr->dst, addr, sizeof(addr)));
/* デバック用 */
#ifdef HEXDUMP
	hexdump(stderr, data, len);
#endif
    /* flockfileの解除 */
	funlockfile(stderr);
}
```

- IPヘッダの検証 (p108)

```c

static void
ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
	/* IPヘッダのポインタ */
	struct ip_hdr *hdr;
    /* バージョン */
	uint8_t v;
    /* ヘッダの長さ、合計サイズ、オフセット */
	uint16_t hlen, total, offset;

	debugf("dev=%s, len=%zu", dev->name, len);
    /* IPヘッダの最小サイズは20バイトなので、これを下回るならエラー */
	if (len < IP_HDR_SIZE_MIN) {
		errorf("too short");
		return;
	}
    /* データを型変換して構造体として扱う */
	hdr = (struct ip_hdr *) data;
    /* ビット操作してバージョンの情報を取り出す */
	v = hdr->vhl >> 4;
    /* IPv4か確認 */
	if (v != IP_VERSION_IPV4) {
		errorf("ip version error: v=%u", v);
		return;
	}
    /* 上位ビットを取り出したあと、4倍する */
	hlen = (hdr->vhl & 0x0f) << 2;
	if (len < hlen) {
		errorf("header length error: len=%zu < hlen=%u", len, hlen);
		return;
	}
    /* チェックサムの計算(IPヘッダだけで計算するのでペイロードのデータは不要) */
	if (cksum16((uint16_t *)hdr, hlen, 0) != 0) {
		errorf("checksum error");
		return;
	}
	total = ntoh16(hdr->total);
	if (len < total) {
		errorf("total length error: len=%zu < total=%u", len, total);
		return;
	}
	offset = ntoh16(hdr->offset);
    /* IPパケットが分割されていたらエラー(このプロジェクトではサポートしない) */
    /* IP_HDR_FLAG_MF(0x2000)との論理積を計算する。MFフラグが設定(後続のフラグメントがある)なら条件式の左側は0ではなくなる。
     * さらに、右側でIP_HDR_OFFSET_MASK(0x1FFF)で下位13ビット(フラグメントオフセット)を取り出し、それが0でないか確認する。
     */
	if (offset & IP_HDR_FLAG_MF || offset & IP_HDR_OFFSET_MASK) {
		errorf("fragments do not support");
		return;
	}
	ip_print(data, total);
}

```

- 実行結果
```bash

$ ./test/test.exe 2>&1 | tee -i step4_test.txt
19:14:59.516 [I] setup: setup protocol stack... (test/test.c:35)
19:14:59.516 [I] net_init: initialize... (net.c:159)
19:14:59.516 [I] net_protocol_register: success, type=0x0800 (net.c:135)
19:14:59.516 [I] net_init: success (net.c:168)
19:14:59.516 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:49)
19:14:59.516 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
19:14:59.516 [I] net_run: startup... (net.c:177)
19:14:59.516 [I] net_device_open: dev=net0 (net.c:56)
19:14:59.516 [I] net_run: success (net.c:185)
19:14:59.516 [D] app_main: press Ctrl+C to terminate (test/test.c:66)
19:14:59.516 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
19:14:59.516 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
19:14:59.516 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:144)
19:14:59.516 [D] ip_input: dev=net0, len=48 (ip.c:93)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 128
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xbd4a
	  src: 127.0.0.1
	  dst: 127.0.0.1
19:15:00.516 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
19:15:00.516 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
19:15:00.516 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:144)
19:15:00.516 [D] ip_input: dev=net0, len=48 (ip.c:93)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 128
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xbd4a
	  src: 127.0.0.1
	  dst: 127.0.0.1
19:15:01.519 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
19:15:01.519 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
19:15:01.519 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:144)
19:15:01.519 [D] ip_input: dev=net0, len=48 (ip.c:93)
	  vhl: 0x45 [v: 4, hl: 5 (20)]
	  tos: 0x00
	total: 48 (payload: 28)
	   id: 128
      offset: 0x0000 [flags=0, offset=0]
	  ttl: 255
    protocol: 1
	  sum: 0xbd4a
	  src: 127.0.0.1
	  dst: 127.0.0.1
19:15:01.908 [D] app_main: terminate (test/test.c:74)
19:15:01.908 [I] cleanup: cleanup protocol stack... (test/test.c:55)
19:15:01.908 [I] net_shutdown: shutting down... (net.c:194)
19:15:01.908 [I] net_device_close: dev=net0 (net.c:74)
19:15:01.908 [I] net_shutdown: success (net.c:201)
```
### 感想
- IPパケットの仕様はテキストを読んで多少知っていたが、実際に自分で直接扱うと解像度が上がって勉強になった。
  - ヘッダ長やフラグメントオフセットが4バイトまたは8バイト単位なのはそのうち忘れそうなので気をつけるようにしたい。
- ビット演算を使って特定の情報を取り出す方法は動作が軽く便利なので、パフォーマンスを求めるときは積極的に使うようにしたい。

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

- プロトコルの登録 (p84)

```c

int
net_protocol_register(uint16_t type, net_protocol_handler_t handler)
{
	struct net_protocol *proto;
    /* 同じプロトコルがすでに登録されていないかチェック */
	for (proto = protocols; proto; proto = proto->next) {
		if (type == proto->type) {
			errorf("already registered, type0x%04x", proto->type);
			return -1;
		}
	}
    /* メモリ確保 */
	proto = memory_alloc(sizeof(*proto));
       	if (!proto) {
		errorf("memory_alloc() failure");
		return -1;
	}
    /* 各フィールド(メンバ)に代入 */
	proto->type = type;
	proto->handler = handler;
    /* プロトコルのリストの先頭に新しいプロトコルを追加 */
	proto->next = protocols;
	protocols = proto;
	infof("success, type=0x%04x", type);
	return 0;
}

```

- 受け取ったパケットの振り分け (p86)

```c

int
net_input(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev)
{
	struct net_protocol *proto;
    /* 省略 */
    /* 一致するプロトコルをプロトコルのリストから探す */
	for (proto = protocols; proto; proto = proto->next) {
        /* 見つかったらハンドラを起動 */
		if (proto->type == type) {
			proto->handler(data, len, dev);
			return 0;
		}
	}
	/* unsupported protocol */
	return 0;
}

```
- IPモジュールの実装(p86)(ip.c)
  - 今の段階ではプロトコルの登録と入力ハンドラの呼び出しの確認用

- 初期化関数(p86)
```c

int
ip_init(void)
{
    /* プロトコルスタックにプロトコルの種類と入力ハンドラを登録する */
	if (net_protocol_register(NET_PROTOCOL_TYPE_IP, ip_input) == -1) {
		errorf("net_protocol_register() failure");
		return -1;
	}
	return 0;

```

- 入力ハンドラの準備 (p87)

```c

static void
ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
	debugf("dev=%s, len=%zu", dev->name, len);
	debugdump(data, len);
}

```

- 初期化関数の呼び出し (p87)

```c

int
net_init(void)
{
    /* 省略 */
    /* ip_initを起動する */
	if (ip_init() == -1) {
		errorf("ip_init() failure");
		return -1;
	}
    /* 省略 */
}

```

- テストプログラムを微修正(プロトコル種別の定数を設定)して実行した結果

```bash
$ ./test/test.exe 2>&1 | tee -i step3_test.txt
22:12:31.363 [I] setup: setup protocol stack... (test/test.c:35)
22:12:31.363 [I] net_init: initialize... (net.c:159)
22:12:31.363 [I] net_protocol_register: success, type=0x0800 (net.c:135)
22:12:31.363 [I] net_init: success (net.c:168)
22:12:31.363 [I] net_device_register: success, dev=net0, type=0x0001 (net.c:49)
22:12:31.363 [I] loopback_init: success, dev=net0 (driver/loopback.c:42)
22:12:31.363 [I] net_run: startup... (net.c:177)
22:12:31.363 [I] net_device_open: dev=net0 (net.c:56)
22:12:31.363 [I] net_run: success (net.c:185)
22:12:31.363 [D] app_main: press Ctrl+C to terminate (test/test.c:66)
22:12:31.363 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:31.363 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:31.363 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:144)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:31.363 [D] ip_input: dev=net0, len=48 (ip.c:11)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:32.364 [D] net_device_output: dev=net0, type=0x0800, len=48 (net.c:92)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:32.364 [D] loopback_output: dev=net0, type=0x0800, len=48 (driver/loopback.c:13)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:32.364 [D] net_input: dev=net0, type=0x0800, len=48 (net.c:144)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
22:12:32.364 [D] ip_input: dev=net0, len=48 (ip.c:11)
+------+-------------------------------------------------+------------------+
| 0000 | 45 00 00 30 00 80 00 00 ff 01 bd 4a 7f 00 00 01 | E..0.......J.... |
| 0010 | 7f 00 00 01 08 00 35 64 00 80 00 01 31 32 33 34 | ......5d....1234 |
| 0020 | 35 36 37 38 39 30 21 40 23 24 25 5e 26 2a 28 29 | 567890!@#$%^&*() |
+------+-------------------------------------------------+------------------+
^C22:12:33.182 [D] app_main: terminate (test/test.c:74)
22:12:33.182 [I] cleanup: cleanup protocol stack... (test/test.c:55)
22:12:33.182 [I] net_shutdown: shutting down... (net.c:194)
22:12:33.182 [I] net_device_close: dev=net0 (net.c:74)
22:12:33.182 [I] net_shutdown: success (net.c:201)
```

### 感想
- コード自体は特に難しくなかった。
- いろいろなプロトコルに対応できるよう、将来性を考えた仕様にしておくとよいとわかった。

#### メモ(本筋から外れた、あるいは重要度の低い内容)
- teeコマンドを使うと、ctrl+cを押したら最後の終了ログが出なくなってしまった。
  - -iオプションを付けることでteeでログを保存しつつ終了ログが出るようになった。

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
    共通のインターフェースで操作できるようにする
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

- タスクの再開 (p600)

```c

int
sched_task_wakeup(struct sched_task *task)
{
    /* 休止タスクを全て再開する */
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
- 割り込み機構の初期化関数(p580) /platform/linux/intr.cの一部

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

