# 辞書内包表記の練習 「独習Python」p247
d = {'apple':'りんご', 'orange':'みかん', 'melon':'メロン'}
result  = {value: key for key, value in d.items()}  # キーと値の入れ替え
print(result)

data = {'apple', 'orange', 'melon'}
result2 = {item[0]: item for item in data}  # 頭文字を表示
print(result2)

