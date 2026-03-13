#/bin/bash
#
# getoptsの練習
#

flag_a=0
flag_b=0
flag_c=0
flag_d=0

while getopts abc:d option  #a,b,dは普通のオプション、cは引数をシェル変数OPTARGに代入する
# ここでオプションを読めた場合は終了ステータスとして０を返すのでループが続く
# オプションを読めた場合はシェル変数OPTINDに次に解釈するオプションの番号（位置パラメータ）が代入される
# オプションを読めなかった場合は、０以外の終了ステータスを返してループが終了

do 
	case "$option" in
		a)
		       	echo "aが選択されました。"
			flag_a=1;;
		b)
			echo "bが選択されました。"
			flag_b=1;;
		c)
			echo "cが選択されました。引数は${OPTARG}です。"
			flag_c=1
			arg_c=${OPTARG}
			;;

		d)
			echo "dが選択されました。"
			flag_d=1;;

		\?)  #任意の一文字ではなく、?の文字を受け取るので\が必要
			echo "エラーが発生しました。"
			exit 1;;

	esac
done


# ここから選択したオプションを配列に入れる
selected_opts=()  # 選択したオプションの配列
arguments=()  # 選択した引数の配列
if [ "$flag_a" -eq 1 ]; then
	selected_opts+=("a")
fi

if [ "$flag_b" -eq 1 ]; then
	selected_opts+=("b")
fi

if [ "$flag_c" -eq 1 ]; then
	selected_opts+=("c")
	arguments+=("${arg_c}")
fi

if [ "$flag_d" -eq 1 ]; then
	selected_opts+=("d")
fi

# ここから配列の文字を,区切りで出力
if [ ${#selected_opts[@]} -gt 0 ]; then

	# IFSを変更するのでサブシェルで処理をする
	(
		IFS=", "
		echo "選択されたオプションは${selected_opts[*]}です。"
		if [ ${#arguments[@]} -gt 0 ]; then
			echo "引数は${arguments[*]}です。"
		fi
	)
else
	echo "選択されたオプションはありません。"
fi


	
