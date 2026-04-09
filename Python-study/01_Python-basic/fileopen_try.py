import datetime
# try, exceptによる例外処理付きのファイル操作
data = 'Aliceはりんごを持っています。\nBobはボールを持っています。\n'
file_path = './fileopen_try.txt'

# >>> ファイルへの書き込み <<<
try:
    with open(file_path, 'a', encoding='utf-8') as file:
        file.write(f'{datetime.datetime.now()}\n')  # 現在時刻
        file.write(data)  # 文字列なのでwriteメソッドを使う
except PermissionError as e:
    print(f'エラー：ファイルに書き込む権限がありません。 {e}')

except Exception as e:  # Exceptionで多くのエラーを捕捉できる
    print(f'エラー：予期せぬエラーが発生しました。 {e}')

# >>> ファイルの読み出し <<<
try:
    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            print(line, end='')

except FileNotFoundError as e:
    print(f'エラー：ファイルが見つかりません。 {e}')

except Exception as e:
    print(f'エラー：予期せぬエラーが発生しました。 {e}')

