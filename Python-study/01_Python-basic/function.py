# 関数の練習
def get_name():
    name = input('あなたの名前は？')
    return name

def func_hello():
    print('Hello,', name, 'さん', sep = '' )

#　ここからメインの処理 
name = get_name()
func_hello()



