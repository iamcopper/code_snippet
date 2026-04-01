# 1. 编译修正版劫持库
gcc -shared -fPIC -o libhook_printf.so hook_printf.c -ldl

# 2. 编译测试程序
gcc -o test test.c

# 3. 运行测试（不劫持）
echo "=== 正常运行（不劫持）==="
./test

# 4. 运行测试（劫持）
echo -e "\n=== 劫持运行 ==="
LD_PRELOAD=./libhook_printf.so ./test

rm libhook_printf.so test
