# 文字列が識別子か判定するisidentifierメソッド

import keyword

id1 = 'fi'
id2 = 'if'

print(id1.isidentifier())
print(id2.isidentifier())

# 文字列が予約済みの識別子か判定する関数（keywordモジュール）
print(keyword.iskeyword(id1))
print(keyword.iskeyword(id2))


