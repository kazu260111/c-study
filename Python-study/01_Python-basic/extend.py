import copy  # deepcopyに必要

# extendメソッドの練習
data = ['りんご', 'みかん', 'いちご']
print(data)
data.extend(['モモ', 'バナナ'])  # 要素を追加
print(data)
data[0:3] = ['リンゴ', 'ミカン', 'イチゴ']  # 要素の置き換え(この場合最初の要素から三番目の要素まで置き換えになる。四番目は置き換わらない)
print(data)
data.remove('リンゴ')  # 要素の削除
print(data)
print()  # カッコを忘れないように注意

data2 = data.copy()  # dataの複製 (メソッドを使うときはカッコを忘れないように注意)
print(data)
print(data2)
print(data == data2)  # 中身が同じなのでTrue
print(data is data2)  # 実体は違うのでFalse
print()

data3 = data  # 参照先のコピー
print(data)
print(data3)
print(data == data3)  # 中身が同じなのでTrue
print(data is data3)  # 参照先（実体）が同じなのでTrue
print()

# deepcopyの練習
# copyメソッドによるコピーはシャローコピーと呼ばれ、リストの配下にあるリストについては参照先のコピーしかしていない。
# 深さのあるリストの場合はディープコピーが必要。

data4 = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
data5 = data4.copy()
data6 = copy.deepcopy(data4)

data4[0][0] = '1000'
print(data4)
print(data5)
print(data6)


