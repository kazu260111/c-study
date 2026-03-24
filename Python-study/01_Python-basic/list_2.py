# リスト内包表記
data = [10, 20, 30]
data2 = [ i * 2 for i in data]  # dataの各要素を二倍にした配列
print(data)
print(data2)

data3 = []  # 空のリスト
for i in data:
    data3.append(i * 2)
print(data3)

# dataの各要素を文字列に
data4 = []
for i in data:
    data4.append(str(i))
print(data4)

# 短く書く
data5 = [str(i) for i in data] 
print(data5)

# 特定の値だけを取り出す
data6 = [ i for i in data if i <= 20]
print(data6)


