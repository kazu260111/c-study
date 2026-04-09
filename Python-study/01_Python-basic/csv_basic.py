import csv
# tsv(csv)ファイルの書き込みと読み込みの練習

# 使用するファイル
file_path = './tsv_basic.tsv'

# 書き込むデータ
data = [
    [1, 'Alice', 15, 'りんご'],
    [2, 'Bob', 12, '野球'],
    [3, 'Chris', 10, 'チョコレート']
    ]

# tsvファイルの書き込み
try:
    with open(file_path, 'w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file, delimiter='\t', quoting=csv.QUOTE_ALL)  # writerオブジェクトを作る
        writer.writerows(data)

except PermissionError as e:
    print(f'エラー：権限がありません。 {e}')

except Exception as e:
    print(f'エラー：予期しないエラーが発生しました。 {e}')


# tsvファイルの読み込み
try:
    with open(file_path, encoding='utf-8') as file:
        for row in csv.reader(file, delimiter='\t'):  # reader関数でreaderオブジェクトを作り、forループでリストを取り出す
            for cell in row:  # リストを取り出してprintで順に出力する
                print(cell)
            print('--------')  # 区切り線を表示

except FileNotFoundError as e:
    print(f'エラー：ファイルが見つかりません。 {e}')

except Exception as e:
    print(f'エラー：予期しないエラーが発生しました。 {e}')
