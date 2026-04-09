import datetime
# withの練習
data = ['Aliceはりんごを持っています。\nBobはボールを持っています。\n']

with open('./with_open_basic.txt', 'a', encoding='UTF-8') as file:
    file.write(f'{datetime.datetime.now()}\n')
    file.writelines(data)  # リストを書くにはwritelinesを使用する
with open('./with_open_basic.txt', 'r', encoding='UTF-8') as file:
    data_read = file.read()
    print(data_read)

    file.seek(0)  # seek位置をリセットする
    
    data_line = file.readlines()
    for line in data_line:
        print(line, end='')  # end='' でprintの改行をなくす(元の文章の改行と重複するため)
