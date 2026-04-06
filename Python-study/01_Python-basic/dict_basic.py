# 辞書型(dict)の練習
d1 = {'赤':'red', '白':'white', '黄色':'yellow'}
print(d1)


d2 = {}
print(d2)  # 空の{}は辞書とみなされる（セットを作るときはsetを使う）

# 内部的にはハッシュ表を使って要素を保存している。（キーからハッシュ値を求める）
# キーはハッシュ可能な型である必要がある (int, str, bytes, tuple, frozensetなど)
# list, set, dictなどの型はハッシュ不可なので辞書のキーにならない

d = {'赤':'red', '白':'white', '黄色':'yellow'}
d['青'] = 'blue'
d['赤'] = 'aka'
print(d)

# ハッシュ値の確認（マイナスになることもあり、プログラムを実行するたびに変化する）
print(hash('赤'))
print(hash('白'))
print(hash('黄色'))

