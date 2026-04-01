#include <stdio.h>

int main() {
	// 测试1: 简单字符串
	printf("Hello, World!\n");

	// 测试2: 带格式化字符串
	printf("Number: %d, Float: %.2f, String: %s\n", 42, 3.14159, "test");

	// 测试3: 多个参数
	printf("A=%d, B=%d, Sum=%d\n", 10, 20, 10+20);

	// 测试4: 只有格式字符串
	printf("%s\n", "This is a string");

	return 0;
}
