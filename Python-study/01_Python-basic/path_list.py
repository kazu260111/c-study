import datetime
import os

# フォルダー配下のファイル情報を取得 p288

PATH = './checked_folder'
for f in os.listdir(PATH):  # ファイルやフォルダの名前をリストとして得る
    p = os.path.join(PATH, f)  # PATHとファイルの名前をつなげて、完全なパスをpに入れる

    print(p)
    print('フォルダー' if os.path.isdir(p) else 'ファイル')
    print(datetime.datetime.fromtimestamp(os.path.getatime(p)))  # getatimeで得た時刻はUnixタイムスタンプ(経過秒)なので、frmotimestampでdatetimeで使えるようにする
    print(os.path.getsize(p), 'byte')
    print('------')
