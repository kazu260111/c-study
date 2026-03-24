# if文の練習２
x = 10
if x == 5:  # 間違えてx = 5 としない(代入となり、文法エラーになる)
    print('xは5です。')
elif x > 5:
    print('xは5より大きいです。')
elif x > 3:
    print('xは3より大きく5以下です。')  # if文では最初に条件を満たしたブロックのみ実行する
else:
    print('xは3以下です。')


y = 10
if y == 5:
    print('yは５です。')
else:
    pass  # 何もしないときはpassと入れる。（Cとは違ってエラーになる)

# 複雑な条件のときのif文
a = 10
b = 20
if (a >= 5 and b > 20):  # 条件式はカッコで括れる
    print('1')
elif a >= 20 or b < 30:  # 括らなくても良い（一般的）
    print('2') 
else:
    print('3')

# 入れ子構造のif
i = 0
j = 0
if i <= 5:
    if j <= 5:
        print('i,jは5以下です。')
    else:
        print('iは5以下ですが、jは5より大きいです。')
else:
    if j <= 5:
        print('iは5より大きく、jは5以下です。')
    else:
        print('i,jは5より大きいです。')






