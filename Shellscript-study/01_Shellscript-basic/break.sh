#!/bin/bash
#
# breakの練習
# 現在のディレクトリに指定したファイル名のファイルがあるか判定する。

found=0

read -p "現在のディレクトリを調べます。ファイル名を入力してください。" file_name

for file in *  # 現在のディレクトリにあるすべてのファイルに対して実行する
do
	if [ "$file" = "$file_name" ] ; then    
		found=1
		break  # ファイルが見つかったらその時点でforループを終了させる
	fi
done

if [ "$found" -eq 1 ]; then
	echo "現在のディレクトリにファイルが見つかりました。"
else
	echo "現在のディレクトリにファイルが見つかりませんでした。"
fi



