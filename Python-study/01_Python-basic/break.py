# breakの練習
# 1から順に足していき、合計が1000を超えたところで終了する
sum = 0
for i in range(1, 101):
    sum += i
    if sum >= 1000:  # 合計が1000を超えたらbreak
        print(i)
        print(sum)
        break

# continueの練習
# 1から100までの3の倍数を足し合わせる
sum2 = 0
for i in range(1, 101):
    if i % 3 != 0:  # 3で割り切れないとき
        continue
    sum2 += i    
print('1から100までの3の倍数を足し合わせると', sum2)


