import requests
# jsonデータの取得の練習 p301

res = requests.get('https://wings.msn.to/tmp/books.json')  # requests.request('get', 'URL') と同じ意味
bs = res.json()  # dict型を戻り値にする
print(bs['books'][0]['title'])
