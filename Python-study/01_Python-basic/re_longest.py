import re
# 最長一致について学ぶ p262
tags = '<p><strong>WINGS</strong>サイト<a href="index.html"><img src="wings.jpg" /></a></p>'
ptn = re.compile(r'<.+>')
result = ptn.findall(tags)
for m in result:
    print(m)  # この場合すべてのタグがまとめてひとつとして出力される
print('')

ptn_short = re.compile(r'<.+?>')  # +?で最短一致、パターンが成立する範囲で最短のものを探す
result_short = ptn_short.findall(tags)
for m in result_short:
    print(m)  # タグが別々に出力される

