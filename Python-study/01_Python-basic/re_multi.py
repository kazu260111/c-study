import re
# マルチラインモードの練習

msg = '3個のリンゴをAliceは持っている。\n1個のリンゴをBobが食べた。'
ptn = re.compile(r'^\d+')  # ^は文字列の先頭を意味する
result = ptn.findall(msg)
for m in result:
    print(m)

print("")

ptn2 = re.compile(r'^\d+', re.MULTILINE)  # ^が行頭を意味するようになる
result2 = ptn2.findall(msg)
for m in result2:
    print(m)
