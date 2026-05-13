# C言語・Linuxカーネル学習用リポジトリ
このリポジトリではC言語やLinuxカーネルなどを基礎から学ぶために、自作のプログラムを書きながら学習内容をまとめていく。

## 目的
- C言語とLinuxカーネルの学習から低レイヤーの動作を理解する
- bashやpythonなどの実用的な言語についても学び、実践的に使えるようにする

## 開発環境
### OS等
- OS: Ubuntu 24.04.4 LTS
- カーネル: Linux 6.17.0-14-generic
- アーキテクチャ: x86_64 

### コンパイラ等
- GCC: 13.3.0
- Shell: GNU bash 5.2.21 
- Python: 3.12.3

## 各ディレクトリの概要
### [Linux-kernel](./Linux-kernel)
Linuxカーネルについて学んだことをまとめる。
学んだことを自作のプログラムで実際に確かめる。

- 参考書籍
  - **[試して理解] Linuxのしくみ　増補改訂版**
    - 著者：武内　覚
    - 出版社：技術評論社
  - **Linuxカーネルプログラミング　第２版**
    - 著者：Kaiwan N.Billimoria
    - 訳者：武内　覚、大岩　尚宏
    - 出版社：オライリー・ジャパン

### [c-study](./c-study)
C言語について学んだことをまとめる。
学んだ関数などを自作のプログラムで実際に使用して理解を深める。

- 作成した主なプログラム
  - [会員情報管理プログラム](https://github.com/kazu260111/c-study/tree/main/c-study/02_c-practice/member_management)

- 参考書籍
  - **C言語　ゼロからはじめるプログラミング　新版**
    - 著者：三谷　純
    - 出版社：翔泳社
  - **C実践プログラミング　3rd Edition**
    - 著者：Steve Oualline
    - 訳者：谷口　功
    - 監訳：望月　康司
    - 出版社：オライリー・ジャパン

### [Shellscript-study](./Shellscript-study)
bashについて学んだことをまとめる。
シェルスクリプトを自作して実用的なbashの活用方法を学ぶ。

- 参考書籍
  - **シェルスクリプト基本リファレンス [改訂第４版]**
    - 著者：山森　丈範
    - 出版社：技術評論社
  - **新しいLinuxの教科書　第２版**
    - 著者：三宅　英明、大角　祐介
    - 出版社：SB Creative

### [Python-study](./Python-study)
Pythonについて学んだことをまとめる。
学んだことからプログラムを自作して実用的なPythonの使い方を学ぶ。
「Fluent Python」はPythonの基礎が終わったら読み始める予定。

- 参考書籍
  - **独習Python**
    - 著者：山田　祥寛
    - 出版社：翔泳社
  - **Fluent Python 第２版**
    - 著者：Luciano Ramalho
    - 訳者：牧野　聡
    - 出版社：オライリー・ジャパン

### [TCP-IP](./TCP-IP)
- **ゼロからのTCP/IPプロトコルスタック自作入門**
  - 著者：山本　雄也
  - 出版社：マイナビ出版
  - 備考：[forkした別リポジトリ](https://github.com/kazu260111/microps_fork_TCP-IP)にて学習中
  - [学習メモ](./TCP-IP/microps_study_log.md)

## 追加予定のディレクトリ
### Rust-study
Rustについて学んだことをまとめる。
C言語とLinuxカーネルの学習がある程度進んだら追加予定。

- 参考書籍
  - **プログラミングRust　第２版**
    - 著者：Jim Blandy, Jason Orendorff, Leonora F.S. Tindall
    - 訳者：中田　秀基
    - 出版社：オライリー・ジャパン
