import collections

# キュー、スタック操作の練習
data = collections.deque()
data.append(10)
data.append(50)
data.append(100)
print(data)
print(data.popleft())  # 先頭の要素をpop、キューと同じで先に入った要素が出てくる
print(data)  # popされた要素はなくなる
print(data.pop())  # デフォルトだと末尾の要素をpopする、スタックと同じで最後に入った要素が出てくる
print(data)  # popされた要素はなくなる



