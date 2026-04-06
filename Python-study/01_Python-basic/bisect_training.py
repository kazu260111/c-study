import bisect
# bisectモジュールの練習

data = [10, 40, 100, 50, 30, 20]
print(data)
data.sort()  # insort仕様前にソートする必要がある
print(data)
bisect.insort(data, 35)  # 対象と入れるデータを引数にする
print(data)  # 順番に並ぶように新しい要素が入る
print()

# enumerate関数の練習
# enumerate自体はイテレータ(オブジェクトの内容を列挙する仕組みを備えたオブジェクト)で実行時にすべて処理するのではなく、forループなどで必要になったとき初めて処理を実行しているので、printなどを使ってもインデックス番号と値は表示されない

# 以下はイテレータの使い方の練習
data2 = ['りんご', 'みかん', 'いちご']
itr = iter(data2)
while True:
    try:
        print(next(itr))
    except StopIteration:
        break
print()

# enumerateの練習
for index, value in enumerate(data2):
    print(index, ':', value)  # 0番から表示する
print()

for index, value in enumerate(data2):
    print(index + 1, ':', value)  # +1して1番から表示する
print()
for index, value in enumerate(data2, start=1):  # start=1の方が上の例よりも処理が軽い
    print(index, ':', value)  



