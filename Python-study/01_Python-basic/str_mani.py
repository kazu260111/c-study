# 文字列を操作する
data = 'Taro Tanaka'

print(data.lower())  # 小文字に
print(data.upper())  # 大文字に
print(data.swapcase())

# 文字列の一部を取得

print(data[2])  # 3文字目を出力（配列と同じで0からカウント）
print(data[2:6])  # 3文字目から6文字目を出力(7文字目までではない)
print(data[2:])  # 3文字目から最後まで出力
print(data[:6])  # 6文字目まで出力
print(data[::-1])  # 逆順で出力

# 文字の種類の判定
print(data.isalnum())
print(data.isalpha())  # 空白があるのでFalse



