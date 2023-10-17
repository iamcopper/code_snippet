# 生成pro工程文件（helloqt.pro）
qmake -project

# 在helloworld.pro末尾增加如下内容，详见问题1
echo 'greaterThan(QT_MAJOR_VERSION, 4): QT += widgets' >> helloqt.pro

# 生成Makefile文件
qmake helloqt.pro

# 编译
make
