rm trans
/opt/opencilk/bin/clang++ -std=c++17 -o trans trans.cpp

numactl --cpubind=0 --membind=0 ./trans /home/wuyiyang/dataset/com-friendster.ungraph.txt friendster
