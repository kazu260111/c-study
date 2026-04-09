import re
# findallメソッドの練習(マッチした文字列のリストを返す)
msg = '電話番号1 333-444-555 電話番号2 123-456-789'
ptn = re.compile(r'\d{2,4}-\d{2,4}-\d{2,4}')  # バックスラッシュを多く使うのでraw文字列を使うと良い {}内に半角スペースを入れないよう注意
result = ptn.findall(msg)
for i in result:
    print(i)
