import re
# finditerメソッドの練習 (p257)

msg = '電話番号は 123-456-789 電話番号2は 222-333-444'
ptn = re.compile(r'(\d{2,4})-(\d{2,4})-(\d{2,4})')  # ()であとから部分文字列として取り出せるようにする
result = ptn.finditer(msg)  # finditerメソッドはMatchオブジェクトを返すイテレータを返すので、forループで取り出す
for i in result:  # 読み取った番号それぞれに対して以下の処理を実行できる
    print(f'開始位置：{i.start()}')
    print(f'終了位置：{i.end()}')
    print(f'マッチング文字列：{i.group()}')
    print(f'市外局番：{i.group(1)}')
    print(f'市内局番：{i.group(2)}')
    print(f'加入者番号：{i.group(3)}')
    print('--------------------')



