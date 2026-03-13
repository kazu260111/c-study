#!/bin/bash

#yesかnoを選択して、分岐するシェルスクリプト。大文字に対応した。

read -p "選択してください。" answer
if [[ "$answer" == y || "$answer" == Y ]]; then
	echo "yesが選択されました。"
elif [[ "$answer" == n || "$answer" == N ]]; then
	echo "noが選択されました。"
else
	echo "エラーが発生しました。"
fi

echo "すべての処理が完了しました。"
