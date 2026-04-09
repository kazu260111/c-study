# このファイルはserialize_basic.pyで使う
class Book:
    def __init__(self, isbn, title, price):  # selfは必ず書く
        self.isbn = isbn
        self.title = title
        self.price = price
