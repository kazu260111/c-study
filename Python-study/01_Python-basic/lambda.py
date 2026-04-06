# lambda, mapの練習(これを使うよりリスト内包表記で書いたほうが処理的によい）
data1 = [1, 2, 3]
result1 = map(lambda v: v * v, data1)  # map関数は処理とリストを引数にする、vは仮変数
print(list(result1))  # map関数はオブジェクトを戻り値とするのでlistで変換する必要がある

data2 = [3, 2, 1]
result2 = map(lambda v1, v2: v1 + v2, data1, data2)
print(list(result2))

