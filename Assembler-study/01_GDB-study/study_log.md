# 学習内容
- GDBの基本的な使い方を学ぶ。
- セグメンテーションフォルトの起こる簡単なC言語のプログラムを用意し、
  GDBでどこで異常が起きているのかを調査する。
  - 実践的なデバッグスキルを身につける練習とする。

# 参考書籍
**独習アセンブラ 新版**
著者：大崎　博之
出版社：翔泳社

# 学習記録
## NULLポインタ参照
- 以下のようなコードを用意した。
```c
#include <stdio.h>
/* 注意：このプログラムはクラッシュします */
int null_pointer_func(void){
	int *p = NULL;
	/* ここでNULLポインタ参照が起きる */
	*p = 1;
	return 0;
}
int main(void) {
	printf("プログラムが実行されました\n");
	null_pointer_func();
	printf("この文章は表示されません\n");
	return 0;
}
```
- このコードをコンパイルし、GDBで調査する。
```bash
# -g : 実行ファイルにデバッグ情報を入れる
$ gcc -g null_pointer.c -o null_pointer
# -q : gdbの起動メッセージを表示しない
$ gdb -q null_pointer
Reading symbols from null_pointer...
(gdb) 
```
- 次にログを記録する設定をする。
```text
(gdb) set logging enabled on
Copying output to gdb.txt.
Copying debug output to gdb.txt.

# set logging on だと記録はできるが警告が出るので、こちらのコマンドを使った。
# set logging enabled off で記録終了
# set logging file (ファイル名)  でファイル名を指定できる
```
- runでプログラムを動作させる
```text
(gdb) run
Starting program: /home/ka2601/git-study/c-study/Assembler-study/01_GDB-study/null_pointer 

This GDB supports auto-downloading debuginfo from the following URLs:
  <ima:enforcing>
  <https://debuginfod.fedoraproject.org/>
  <ima:ignore>
Enable debuginfod for this session? (y or [n]) n  # デバッグ情報の自動ダウンロード(今回は不要)
Debuginfod has been disabled.
To make this setting permanent, add 'set debuginfod enabled off' to .gdbinit.
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".
プログラムが実行されました

Program received signal SIGSEGV, Segmentation fault.  # セグメンテーションフォルト
0x0000000000400476 in null_pointer_func () at null_pointer.c:6
6               *p = 1;  # ここでクラッシュ
Missing rpms, try: dnf --enablerepo='*debug*' install glibc-debuginfo-2.43-5.fc44.x86_64
(gdb) bt
#0  0x0000000000400476 in null_pointer_func () at null_pointer.c:6
#1  0x0000000000400496 in main () at null_pointer.c:11
(gdb) 
```
- null_pointer()でセグメンテーションフォルトが起きているので、さらに調べる
```text
(gdb) p p  # pに何が入っているか調べる
$1 = (int *) 0x0  # NULLポインタを操作しようとしていた
(gdb) frame 1  # #1に移動(関数の呼び出し元)
#1  0x0000000000400496 in main () at null_pointer.c:11
11              null_pointer_func();
(gdb) list  # 現在地の周辺のコードを調べる
6               *p = 1;
7               return 0;
8       }
9       int main(void) {
10              printf("プログラムが実行されました\n");
11              null_pointer_func();  # 何も引数を渡していない
12              printf("この文章は表示されません\n");
13              return 0;
14      }
15      
(gdb) 
```
- ここまででNULLポインタを操作していたことが原因だとわかった。
- 次はアセンブリからコードを観察してみる。
```text
(gdb) break *main  # runのときmain関数の手前で止まるようにする
Breakpoint 1 at 0x400483: file null_pointer.c, line 9.
(gdb) run
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/ka2601/git-study/c-study/Assembler-study/01_GDB-study/null_pointer 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".

Breakpoint 1, main () at null_pointer.c:9
9       int main(void) {
(gdb) disas  # アセンブリを表示
Dump of assembler code for function main:
=> 0x0000000000400483 <+0>:     push   %rbp  # 矢印のここが現在地
   0x0000000000400484 <+1>:     mov    %rsp,%rbp
   0x0000000000400487 <+4>:     mov    $0x401178,%edi
   0x000000000040048c <+9>:     call   0x400370 <puts@plt>
   0x0000000000400491 <+14>:    call   0x400466 <null_pointer_func>
   0x0000000000400496 <+19>:    mov    $0x4011a0,%edi
   0x000000000040049b <+24>:    call   0x400370 <puts@plt>
   0x00000000004004a0 <+29>:    mov    $0x0,%eax
   0x00000000004004a5 <+34>:    pop    %rbp
   0x00000000004004a6 <+35>:    ret
End of assembler dump.
```
- このまま進めてもいいが、TUIモードを有効にするとレジスタの値などが見やすくなる。
  - set logging enabled on で記録モードにしていた場合、TUIモードだとうまく記録されないようなので注意
```text
$ layout regs  # 3つのウィンドウになる
# Ctrl + x を押したあとにoを押すとアクティブウィンドウを切り替えられる
# layout asm や layout src、layout splitなどで必要なウィンドウだけ出せる
```
- プログラムを進めていくときは、niまたはsiを使って一行ずつ進める。
  - niは関数があっても入らずそのまま進み、siは関数の中の場所まで進む。
  - siで標準ライブラリの関数に入ってしまったら、finishで抜ける。

- セグメンテーションフォルトが起こったあたりまで進めた。
```text
>0x40046a <null_pointer_func+4>  movq   $0x0,-0x8(%rbp)  # ここでpにNULLを入れている 
 0x400472 <null_pointer_func+12> mov    -0x8(%rbp),%rax  # raxレジスタにpの中身NULL(0x0)を入れる
 0x400476 <null_pointer_func+16> movl   $0x1,(%rax)  # 1をraxレジスタが示す場所(0x0)に代入しようとするが、アクセスできずセグフォ
```
- ここでコマンド画面でもセグメンテーションフォルトと表示されるので、この場所で問題が起きたことがわかる。

## メモリの解放後に解放したメモリを使用する(Use-After-Free)
-  以下のようなプログラムを準備した。
```c
/* 注意：このプログラムは実験用プログラムです。意図的なバグが含まれています。 */
#include <stdio.h>
#include <stdlib.h>

int main(void){
	int *ptr = malloc(sizeof(int));  /* intサイズのメモリを確保 */
	*ptr = 1;  /* 確保したメモリに1を書き込む */
	free(ptr);  /* 確保したメモリを解放 */
	*ptr = 2;  /*>>> 解放したメモリに2を書き込む(危険) <<<*/
	printf("%d\n", *ptr);  /* 解放したあと2を書き込んだはずのメモリにアクセスする */
	return 0;
}
```
- このプログラムをコンパイルして実行してみる。
```bash
$ gcc -g use_memory_after_free.c -o use_memory_after_free
$ ./use_memory_after_free 
2  # 正しい答えが出力される
```
- 正しい答えが出力された。次に、プログラムに何行か付け加えてみる。
```c
/* 注意：このプログラムは実験用プログラムです。意図的なバグが含まれています。 */
#include <stdio.h>
#include <stdlib.h>

int main(void){
	int *ptr = malloc(sizeof(int));  /* intサイズのメモリを確保 */
	*ptr = 1;  /* 確保したメモリに1を書き込む */
	free(ptr);  /* 確保したメモリを解放 */
	*ptr = 2;  /*>>> 解放したメモリに2を書き込む(危険) <<<*/
	printf("%d\n", *ptr);  /* 解放したあと2を書き込んだはずのメモリにアクセスする */

	/* ここから追加のプログラム */
	int *ptr_2 = calloc(1, sizeof(int));  /* 新しくメモリを確保(ゼロ埋め) */
	printf("%d\n", *ptr);  /* もう一度2が書き込まれているはずのメモリにアクセスする */
	free(ptr_2);
	return 0;
}
```
- callocで新しくメモリを確保したあと、さっき2を書き込んだはずのメモリにもう一度アクセスする。
```bash
$ gcc -g use_memory_after_free.c -o use_memory_after_free
$ ./use_memory_after_free 
2  # 正しい答え
0  # 0に変わってしまった
```
- このように、一度解放されたメモリはしばらくはアクセスしても解放前の値が入っているが、
  mallocなどで新しくメモリが確保されたときに書き換えられてしまう。
  - 解放してすぐは正常な値が入っているので、場合によってはバグがあっても気づかず
    プログラムが正常に動いてしまう可能性がある。
  - mallocなどを使ったあとは必ずfreeで解放するのを忘れないことが重要。

- GDBでも調べてみる。disasでアセンブリを確認できる。
```text
=> 0x0000000000400496 <+0>:     push   %rbp
   0x0000000000400497 <+1>:     mov    %rsp,%rbp
   0x000000000040049a <+4>:     sub    $0x10,%rsp
   0x000000000040049e <+8>:     mov    $0x4,%edi
   0x00000000004004a3 <+13>:    call   0x4003a0 <malloc@plt>  # mallocでメモリを確保
   0x00000000004004a8 <+18>:    mov    %rax,-0x8(%rbp)  # mallocで確保したアドレスをptrに入れている
   0x00000000004004ac <+22>:    mov    -0x8(%rbp),%rax
   0x00000000004004b0 <+26>:    movl   $0x1,(%rax)
   0x00000000004004b6 <+32>:    mov    -0x8(%rbp),%rax
   0x00000000004004ba <+36>:    mov    %rax,%rdi
   0x00000000004004bd <+39>:    call   0x400370 <free@plt>
   0x00000000004004c2 <+44>:    mov    -0x8(%rbp),%rax
   0x00000000004004c6 <+48>:    movl   $0x2,(%rax)
   0x00000000004004cc <+54>:    mov    -0x8(%rbp),%rax
   0x00000000004004d0 <+58>:    mov    (%rax),%eax
   0x00000000004004d2 <+60>:    mov    %eax,%esi
   0x00000000004004d4 <+62>:    mov    $0x401228,%edi
   0x00000000004004d9 <+67>:    mov    $0x0,%eax
   0x00000000004004de <+72>:    call   0x400380 <printf@plt>
   0x00000000004004e3 <+77>:    mov    $0x4,%esi
   0x00000000004004e8 <+82>:    mov    $0x1,%edi
   0x00000000004004ed <+87>:    call   0x400390 <calloc@plt>  # callocでメモリを確保して、0で埋める
   0x00000000004004f2 <+92>:    mov    %rax,-0x10(%rbp)  # ここでおそらく最初に確保したのと同じアドレスを渡している
   0x00000000004004f6 <+96>:    mov    -0x8(%rbp),%rax
   0x00000000004004fa <+100>:   mov    (%rax),%eax
   0x00000000004004fc <+102>:   mov    %eax,%esi
   0x00000000004004fe <+104>:   mov    $0x401228,%edi
   0x0000000000400503 <+109>:   mov    $0x0,%eax
   0x0000000000400508 <+114>:   call   0x400380 <printf@plt>
   0x000000000040050d <+119>:   mov    -0x10(%rbp),%rax
   0x0000000000400511 <+123>:   mov    %rax,%rdi
   0x0000000000400514 <+126>:   call   0x400370 <free@plt>
   0x0000000000400519 <+131>:   mov    $0x0,%eax
   0x000000000040051e <+136>:   leave
   0x000000000040051f <+137>:   ret
```
- niでプログラムを進めていき、以下の場所でmallocが確保したメモリアドレスが入っているraxの値を確認する
```text
     0x00000000004004a3 <+13>:    call   0x4003a0 <malloc@plt>
  => 0x00000000004004a8 <+18>:    mov    %rax,-0x8(%rbp) # mallocで得たアドレスがraxに入っている
     0x00000000004004ac <+22>:    mov    -0x8(%rbp),%rax
```
- x $raxでraxレジスタに入っている値を確認できる
```text
(gdb) x $rax
0x404010:       0x00000000
```
- さらにプログラムを進めていき、callocを使ったあとに確保したメモリアドレスが入っているraxの値を確認する
```text
   0x00000000004004ed <+87>:    call   0x400390 <calloc@plt>
=> 0x00000000004004f2 <+92>:    mov    %rax,-0x10(%rbp)  # callocで得たアドレスがraxに入っている
   0x00000000004004f6 <+96>:    mov    -0x8(%rbp),%rax
```
- もう一度 x $raxでraxレジスタに入っている値を確認する
```text
(gdb) x $rax
0x404010:       0x00000000  # 最初のmallocと同じアドレスをcallocは確保している
```
- mallocとcallocが同じアドレスを確保しているため、callocがポインタが指していた2を0で埋めてしまったことがわかる。
