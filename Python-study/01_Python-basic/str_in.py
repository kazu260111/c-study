# in演算子の練習
msg = 'Alice has an apple. Bob has a ball.'
print('as' in msg)
print('as' not in msg)
print('Alice' in msg[1:])  # ２文字目からAliが含まれているか調べる、結果はFalse
print('alice' in msg.casefold())  # casefoldで大文字小文字の区別がなくなるのでTrue

# startswith, endswithメソッドの練習
print(msg.startswith('A'))
print(msg.endswith('A'))


