#!/bin/bash
#
# continueの練習
# 10までの数のうち5の倍数以外を表示

i=0
while
	i=$((i+1))
	[ "$i" -le 10 ]
do
	if [ $((i%5)) -eq 0 ]; then
		continue
	fi
	echo "$i"
done



