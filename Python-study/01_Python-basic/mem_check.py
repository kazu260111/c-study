# メモリチェック用のプログラム
# /proc/meminfoからMemTotal, MemFree, MemAvailableの情報を抜き出して表示
# 以下は改善前のバージョン
if False:  # 改善前のプログラムを実行する場合はここをTrueにする
    def check_mem():
        try:  # エラー時に理由を表示したいのでtryをつけておく
            with open('/proc/meminfo', 'r') as file:
                for line in file:  # 一行ずつ読み込めるので、抜き出したい列を探す
                    if line.startswith('MemTotal'):
                        print(line)
                    elif line.startswith('MemFree'):
                        print(line)
                    elif line.startswith('MemAvailable'):
                        print(line)
        except FileNotFoundError:
            print('エラーが発生しました。/proc/meminfoが見つかりませんでした。')
    
    if __name__ == '__main__':  # このプログラムが直接実行されたとき（直接実行されたときだけ__main__になる）
        check_mem()

# プログラム動作結果
#
# MemTotal:        8130688 kB
# 
# MemFree:         3639816 kB
# 
# MemAvailable:    6640256 kB
#
#
# 問題点：各項目に余計な改行がある。stripメソッドで削除する必要がある。

# 改善後のバージョン
if True:  # 改善前のプログラムを実行する場合はここをFalseにする
    def check_mem():
        try:  # エラー時に理由を表示したいのでtryをつけておく
            with open('/proc/meminfo', 'r') as file:
                for line in file:  # 一行ずつ読み込めるので、抜き出したい列を探す
                    if line.startswith('MemTotal'):
                        print(line.rstrip())  # ここで右端の改行を消す
                    elif line.startswith('MemFree'):
                        print(line.rstrip())
                    elif line.startswith('MemAvailable'):
                        print(line.rstrip())
        except FileNotFoundError:
            print('エラーが発生しました。/proc/meminfoが見つかりませんでした。')
    
    if __name__ == '__main__':  # このプログラムが直接実行されたとき（直接実行されたときだけ__main__になる）
        check_mem()

# 改善後の動作結果
# MemTotal:        8130688 kB
# MemFree:         3633884 kB
# MemAvailable:    6635172 kB

