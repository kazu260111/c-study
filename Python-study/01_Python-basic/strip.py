# stripメソッドの練習
# stripはすべての空白、lstripは左の空白、rstripは右の空白を消す
# タブ文字や全角空白、改行文字も消すことに注意
msg = ' Tanaka Taro \n\t'
print(msg.strip())
print(msg.lstrip())
print(msg.rstrip())

# 空白以外も消せる
# 以下は失敗例
print(msg.strip('Ta'))  # strip関数はこの場合だと両端からTとaがあるか調べて消すので、この場合は消えない（スペースなどがあるため）

# 左側のTaだけ消す例
print(msg.strip(' Ta'))  # 半角スペースも消去対象にした。

# ほとんど消す例
print(msg.strip(' Taor\n\t'))  # タブ文字や改行も入れた。結果はnak



