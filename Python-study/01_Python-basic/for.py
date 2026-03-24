# for文の練習
# 九九の表示
for i in range(1, 10):
    for j in range(1, 10):
        print(i * j, ' ', end = '')
    print()  # これだけで改行になる

# forでリストの各要素を表示
data = ['apple', 'cherry', 'banana']
for fruit in data:
    print(fruit)

