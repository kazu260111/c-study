# 代入演算子の練習
# 以下は間違い（代入式は値を返さないのでエラーになる）
# count = 10
# print(count += 5)
# print(count *= 4)
# print(count /= 6)
# print(count %= 3)

count = 10
# 代入式と表示は二回に分ける
count += 5
print(count)

count *= 4
print(count)

count /= 6
print(count)

count %= 3
print(count)


# ビット演算子
x = 10  # 二進数で1010
x >>= 2  # 1010を2ビット右に動かすのでxは二進数で0010になる
print(x)  # 十進数だと2

x = 10
x &= 2  # 2(0010)とビット論理積を計算するとxは二進数だと0010になる
print(x)  # 十進数だと2

x = 10
x ^= 2  # 2(0010)とビット排他論理和を計算、xは二進数だと1000になる
print(x)  # 十進数だと8






