#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// 函数指针类型定义
typedef int (*printf_func)(const char *format, ...);
typedef int (*vprintf_func)(const char *format, va_list ap);
typedef int (*puts_func)(const char *str);

// 保存原始函数
static printf_func real_printf = NULL;
static vprintf_func real_vprintf = NULL;
static puts_func real_puts = NULL;

// 获取原始函数
static void ensure_real_functions() {
	if (!real_printf) {
		real_printf = (printf_func)dlsym(RTLD_NEXT, "printf");
	}
	if (!real_vprintf) {
		real_vprintf = (vprintf_func)dlsym(RTLD_NEXT, "vprintf");
	}
	if (!real_puts) {
		real_puts = (puts_func)dlsym(RTLD_NEXT, "puts");
	}
}

// 初始化
void __attribute__((constructor)) init() {
	ensure_real_functions();
	if (real_printf) {
		real_printf("[HOOK INIT] printf hijack loaded\n");
	}
}

// 劫持 printf
int printf(const char *format, ...) {
	ensure_real_functions();

	if (!real_printf || !real_vprintf) {
		// 后备方案
		write(STDOUT_FILENO, "[FALLBACK] ", 11);
		va_list args;
		va_start(args, format);
		int result = vprintf(format, args);
		va_end(args);
		return result;
	}

	// 输出劫持标记
	int prefix_len = real_printf("[HOOKED] ");

	// 处理格式化字符串
	va_list args;
	va_start(args, format);
	int result = real_vprintf(format, args);
	va_end(args);

	return prefix_len + result;
}

// 劫持 puts（因为 printf("string\n") 可能被编译器优化为 puts）
int puts(const char *str) {
	ensure_real_functions();

	if (!real_puts) {
		// 后备方案
		write(STDOUT_FILENO, "[HOOKED] ", 9);
		write(STDOUT_FILENO, str, strlen(str));
		write(STDOUT_FILENO, "\n", 1);
		return 9 + strlen(str) + 1;
	}

	// 构建新的字符串
	int len = strlen(str);
	char *hooked_str = malloc(len + 10);  // [HOOKED] + 字符串 + \0
	if (!hooked_str) {
		return real_puts(str);
	}

	snprintf(hooked_str, len + 10, "[HOOKED] %s", str);
	int result = real_puts(hooked_str);
	free(hooked_str);

	return result;
}
