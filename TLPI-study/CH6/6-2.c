/* 6-2 longjmp()でリターンした関数に戻るとどうなるかを学ぶ */

#include <setjmp.h>
#include <stdio.h>
static jmp_buf env;

int set_jump(void) {
	if (!setjmp(env)) {
		printf("setjmp()完了\n");
		return 0;
	}
	printf("longjmp()でset_jump()に戻ってきた状態(ここはかろうじて実行されるが、次で壊れる)\n");
	return 0;
}

int main(void) {
	printf("set_jump()を起動します\n");
	set_jump();
	printf("longjmp()を実行してすでにリターンした関数に移動します\n");
	longjmp(env, 1);
	printf("ここには到達しないはず\n");
	return 0;
}

/*
 * 実行結果
 * $ ./6-2.out
 * set_jump()を起動します
 * setjmp()完了
 * longjmp()を実行してすでにリターンした関数に移動します
 * longjmp()でset_jump()に戻ってきた状態(ここはかろうじて実行されるが、次で壊れる)
 * Segmentation fault         (コアダンプ) ./6-2.out
 * 
 * set_jump()から戻ってきた後のprintf()でset_jump()の破棄されたスタックフレームが上書きされる。
 * その後longjmp()で上書きされたスタックフレームに戻ると、printf()の命令自体はテキスト領域(書き込み不可)を使うので
 * 実行できるが、returnしようとすると破棄されたスタックフレームにあった値が上書きされているので異常なアドレスに
 * ジャンプしようとしてセグメンテーションフォルトが起きた。
 * gdbでセグメンテーションフォルト直前のret命令を確認すると戻りアドレスがmainと_finiの間の何もないエリアを
 * 指していた。(アドレスはobjdumpで確認していた)
 * 
 * (gdb) x/gx $rsp
 * 0x7fffffffdc08: 0x00000000004004e6  # 何もない場所を指している
 * (gdb) si
 * 0x00000000004004e6 in ?? ()
 * (gdb) x/gx $rip
 * 0x4004e6:	0x8348fa1e0ff30000
 * (gdb) x/gx $rax
 * 0x0:	❌️ Cannot access memory at address 0x0
 *
 * このときのアセンブリは以下のようになっていたため、raxにアクセスしようとしてセグフォで終了してしまった。
 *
 *  0x4004e1 <main+39>      call   0x400390 <longjmp@plt>                                            * >0x4004e6                add    %al,(%rax)
 *
 */
