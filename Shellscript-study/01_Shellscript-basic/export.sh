#!/bin/bash
#
# export,環境変数についての学習
#

# TEXTというシェル変数を作成し、helloを代入
echo "TEXT=helloを実行します。"
TEXT=hello
echo "現在のシェル変数TEXTは${TEXT}"

# TEXTは環境変数でないことを確認
echo "printenv TEXTを実行します。"
printenv TEXT

# exportでTEXTを環境変数にする
echo "export TEXTを実行します。"
export TEXT
printf "環境変数TEXTは"
printenv TEXT
echo

# シェル変数TEXTにbyeを代入すると、環境変数TEXTも変化することを確認
echo "TEXT=byeを実行します。"
TEXT=bye
echo "シェル変数TEXTは${TEXT}"
printf "環境変数TEXTは"
printenv TEXT
echo

