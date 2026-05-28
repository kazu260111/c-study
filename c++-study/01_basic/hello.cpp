#include <iostream>

int
hello_func(){
	/* std::coutで標準出力に出力し、endlで改行を出して終了 */
	std::cout << "hello" << std::endl;
	std::cout << "world" << '\n';
	return 0;
}
int
add_func(int a, int b){
	int sum = 0;
	sum = a + b;
	return sum;
}
	       
int main() {
	hello_func();
	int a = 1;
	const int b = 2;  /* constで変数の中身が変わらないようにできる */
	int sum = add_func(a, b);
	std::cout << sum  << std::endl;
	std::cout << "++sum: " << ++sum << std::endl;
	int i = 0;
	std::cout << "i: " <<  i << '\n';
	std::cout << "後置インクリメントだと、出力への反映は+される前にされる\n";
	std::cout << "i++: " << i++ << '\n';
	std::cout << "現在のiは " << i << '\n';
	return 0;
}

