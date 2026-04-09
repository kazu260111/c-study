import datetime
# ファイルに書き込む練習 p272

file = open('./openfile.log', 'a', encoding='UTF-8')  # aは追記モード
file.write(f'{datetime.datetime.now()}\n')
file.close()
print('現在時刻を保存しました。')

