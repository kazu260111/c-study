#!/bin/bash
#
#evalの練習
#
day0=Sunday
day1=Monday
day2=Tuesday

today=day0
tommorow=day1

printf "今日の曜日:"
eval echo \"\$"$today"\"  # 一回目のevalによる展開で"$Sunday"が残る
printf "明日の曜日:"
eval echo \"\$"$tommorow"\"
