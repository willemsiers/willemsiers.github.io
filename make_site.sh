echo "removing files _*.exe"
rm _*.exe
echo "removing files _*.o"
rm _*.o
echo "removing files in ./out/*"
rm ./out/*

set -e

echo "building..."
g++ blog.cpp -c -Wall -std=c++17 -o _blog.o
g++ -o _blog.exe _blog.o ../willlibcrypto/_libcrypto.o

echo "running..."
./_blog.exe blog_entry_0001
./_blog.exe blog_entry_0002
./_blog.exe blog_entry_0003
./_blog.exe blog_entry_0004
./_blog.exe blog_entry_0005
./_blog.exe blog_entry_0006
