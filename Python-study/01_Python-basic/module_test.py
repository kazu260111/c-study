# モジュールの練習
# 文字数をカウントするプログラム
import unicodedata

data = 'Taro Tanaka 18'
count = 0

for ch in data:  # 文字列の各文字に対してループ
    if unicodedata.east_asian_width(ch) in 'FWA':  # 全角英数、漢字や全角カナ、特殊文字のとき
        count += 2  # 二文字分として扱う
    else:
        count += 1  # 一文字分として扱う
# ここまでをすべての文字に対してループ

print(count)  
