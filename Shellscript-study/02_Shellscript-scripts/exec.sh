#!/bin/bash
#
# execの練習
# オリジナルのシステムチェック結果をlogに記録する。
#

log_file=my_systemlog_$(date "+%Y-%m-%d").log  # 日付付きのログファイルの設定
exec 3>&1  # 3番を出力先のバックアップとして残す
exec >> "$log_file"  # 標準出力（1番）をlogファイルへ
echo "========================================"  # この行からのコマンドはlogファイルに記録される
echo "システムチェック　$(date "+%Y-%m-%d %H:%M:%S")"
echo "========================================" 
date  
echo ""  # 出力ログが見づらいので区切りのために改行（echoの繰り返しはシェルスクリプトが読みづらくなる原因となる。次回作成時ではコマンド実行用関数を別に作成し、繰り返しを減らす予定）
uname -a  
echo ""
uptime
echo ""
df -h
echo ""
free -m  # この行までlogファイルに記録される
exec 1>&3  
# 1番の出力先をもとに戻す
# 参考： exec > /dev/tty とすると、cronで呼び出されたときなどの直接ターミナルで操作しないときに動作しない
exec 3>&-  # 3番を閉じておく

# ここからは自動実行だと不要
if true; then  # 出力確認を無効にする場合はこの行のtrueをfalseにする
	read -p "出力したログをlessコマンドで確認しますか？[y/n]" log_check

	if [[ "$log_check" == [yY] ]]; then
       		less "$log_file"

	elif [[ "$log_check" == [nN] ]]; then
		:
	else
		echo "不適切な入力です。"
	fi
fi



