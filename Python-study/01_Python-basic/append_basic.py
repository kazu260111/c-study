# appendメソッドの練習
data = ['yamada', 'tanaka', 'uehara']
data.append('oohara')
print(data)  # 末尾にooharaが追加
data.insert(0, 'ogawa')
data.insert(-1, 'mizuhara')
print(data)  # 先頭にogawa,末尾から2番めにmizuharaが追加
print(data.pop(0))  # popメソッドは選択した値を戻り値として、元リストから削除する
print(data.pop())  # デフォルトだと末尾の要素を削除（pop）
print(data)  # popされた値は消えている

