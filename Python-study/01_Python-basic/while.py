# whileの練習
# 九九を表示するプログラム

i = 1
j = 1
while i <= 9:  
    while j <= 9:
        print(i * j, ' ', end = '')  # i*1 i*2 … というように出力
        j += 1
    print('')  # 改行
    j = 1  # 9になったjを1に初期化
    i += 1  # iを1加算して次の段に移行


