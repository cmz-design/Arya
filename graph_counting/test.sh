#!/bin/bash

# 使用 find 命令查找当前路径下的所有文件夹
# -type d 表示只查找目录
# -maxdepth 1 表示只在当前目录下查找，不递归进入子目录
# -exec 后面是要执行的命令，{} 代表匹配到的文件或目录
# cd ./src
# find . -maxdepth 1 -type d -exec sh -c '
#     # 在这里执行你想要的操作，比如进入目录并执行其他命令
#     echo "Entering directory: {}" >> output.log
#     cd "{}" || exit 1  # 切换目录，如果失败则退出脚本
#     # 在这里可以执行其他命令
#     make clean
#     make
#     ./GraphCounting.out ../../graphs/mico/mico.undigraph ../../patterns/triangle 40000000 32 >> output.log
#     ' \;home/cmz/Arya-main/graph_counting/patterns

cd ./src/multi_thread_single_machine
make clean
make
for pattern in {triangle,5_cycle_5_star}
do
    ./GraphCounting.out ../../graphs/LiveJournal1 ../../patterns/$pattern 40000000 32
done

# cd ../reuse_sampler_single_machine
# perf record -o perf.data ./GraphCounting.out ../../graphs/friendster ../../patterns/triangle 40000000 32
# perf report -i perf.data

# cd ../caching_decomposed_pattern_single_machine
# perf record -o perf.data ./GraphCounting.out ../../graphs/friendster ../../patterns/triangle 40000000 32
# perf report -i perf.data
