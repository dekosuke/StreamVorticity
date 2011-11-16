rm a.out
rm data/*
g++ main.cpp -O2
cp hoge.txt hoge.bak
time ./a.out > hoge.txt
./draw.sh
