import requests
# requestsモジュールの練習 p298
res = requests.request('get', 'https://codezine.jp/')  # Responseオブジェクトを生成
print(res.text)  # テキストを取得

# iter_linesメソッドでも読み込める
for line in res.iter_lines():  # iter_linesメソッドはbytes型を戻り値とする
    print(line.decode('utf-8'))  # decodeメソッドで文字列に変換
