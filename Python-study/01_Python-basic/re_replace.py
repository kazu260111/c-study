import re
# subメソッドの練習 p268
msg = 'サポートサイトはhttps://www.wings.msn.to/です。'
ptn = re.compile(r'http(s)?://([\w-]+\.)+[\w-]+(/[\w./?%&=-]*)?', re.IGNORECASE | re.ASCII) # 日本語が入らないようre.ASCIIを使用した
print(ptn.sub(r'<a href="\g<0>">\g<0></a>', msg))  # g<0>はマッチした文字列全体


