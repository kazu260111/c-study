import pickle
import book  # 同じディレクトリにbook.pyを準備する
# オブジェクトのシリアライズの練習　p285

b = book.Book('978-4-7981-5382-7', '独習C# 新版', 3600)
file_path = './serialize_output.bin'

# ファイルの書き込み（binファイルの出力、シリアライズ）
try:
    with open(file_path, 'wb') as file:
        pickle.dump(b, file)

except PermissionError as e:
    print(f'エラー：権限がありません。 {e}')

except Exception as e:
    print(f'エラー：予期せぬエラーが発生しました。 {e}')

# ファイルの読み込み(binファイルの読み込み、デシリアライズ)
try:
    with open(file_path, 'rb') as file:
        loaded_book =  pickle.load(file)
        print(loaded_book.title)

except FileNotFoundError as e:
    print(f'エラー：ファイルが見つかりません。 {e}')

except Exception as e:
    print(f'エラー：予期せぬエラーが発生しました。 {e}')
        

# 注意：pickleでデシリアライズするとそこに含まれている命令がそのまま実行されるので、外部のデータはpickleを使ってデシリアライズしないようにする(jsonなどを使う)
