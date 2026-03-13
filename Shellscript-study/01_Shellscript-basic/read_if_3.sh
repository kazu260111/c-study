#!/bin/bash

#ifの条件分岐をパターンマッチにして短くした。

read -p "選択してください。" answer
if [[ "$answer" == [yY] ]] ; then
	echo "yesが選択されました。"
elif [[ "$answer" == [nN] ]]; then
	echo "noが選択されました。"
else
	echo "エラーが発生しました。"
fi

echo "すべての処理が完了しました。"

