# tryの練習
while True:
    try:
        num = input('数字を入れてください。')  # inputは文字列として取得するので、計算のためにキャストが必要
        print('二倍すると、', float(num) * 2, 'です。')
    except ValueError as ex:
        print('エラーが発生しました。', ex)
    else:  # 例外が発生しなかったとき
        break
    finally:  # 最後に必ず実行される(エラーでもエラーでなくても必ず実行される)
        print('処理が完了しました。')

