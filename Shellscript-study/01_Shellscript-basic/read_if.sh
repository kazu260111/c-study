#!/bin/bash

read -p "選択してください。[y/n]" answer
if [ "$answer" = "y" ]
then
	echo "yesを選択しました。"
elif [ "$answer" = "n" ] 
then
	echo "noを選択しました。"
else
	echo "エラーが発生しました。小文字で入力してください。"
fi

echo "すべての処理が完了しました。"
