import re
# ignoreメソッドの練習
msg = 'アドレス1 alice@example.com アドレス2 bob@example.com'
ptn = re.compile(r'[a-z0-9.!#$%&\'*+/=?^_{|}~-]+@[a-z0-9-]+(\.[a-z0-9-]+)*', re.IGNORECASE)  # re.IGNORECASEで大文字小文字を無視
result = ptn.finditer(msg)
found = False  # 見つからなかったときのフラグ
for m in result:
    found = True
    print(m.group(0))
if not found:
    print('マッチングしませんでした')

