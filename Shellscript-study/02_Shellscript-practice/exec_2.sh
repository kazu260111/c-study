#!/bin/bash
#
# システムチェックをするシェルスクリプト
#

log_file=my_systemlog_$(date "+%Y-%m-%d").log  #日付付きのログファイルの設定
exec 3>&1 # 1番のバックアップとして3番を用意する
exec >> "$log_file"  # 1番の出力をログファイルへ（追記する）

# ここからシステムチェック関数
run_cmd() {
	echo "===== [ $@ ] ====="  # コマンドとその引数を表示する
	"$@"  # コマンドの実行
	echo  # 改行を入れる
}


# 以下はシステムチェックに使いたいコマンド
run_cmd date
run_cmd uname -a
run_cmd uptime
run_cmd df -h
run_cmd free -m

# 終了処理
exec 1>&3  # バックアップしておいた３番を使って１番の出力をもとに戻す
exec 3>&-  # ３番を閉じる

# 以下は手動実行する場合にログをすぐ確認するためのスクリプト
# 自動実行させる場合は無効にする
if true; then  # 無効にする場合はこの行のtrueをfalseに変更
	read -p "出力したログをlessコマンドで確認しますか？[y|n]" log_check

	if [[ "$log_check" == [yY] ]]; then
		less "$log_file"

	elif [[ "$log_check" == [nN] ]]; then
		:
	
	else
		echo "不適切な入力です。"
	fi
fi



