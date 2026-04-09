import re
# シングルラインモードの練習
msg = "初めまして。\n私はAliceです。"
ptn = re.compile(r'^.+')  # .はデフォルトだと改行を含まない
result = ptn.findall(msg)
for m in result:
    print(m)

print('')

ptn_single = re.compile(r'^.+', re.DOTALL)  # 改行も.に含まれるようになった
result_single = ptn_single.findall(msg)
for m in result_single:
    print(m)
