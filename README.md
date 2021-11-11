# exchange_simulator
task in file task_c_dev.pdf

make : compile program ./prog
make test : create 2 files my_output.txt where can we view the result of program and difference.txt w    here are all
differences between "output.txt" and "my_output.txt"
 
./prog input.txt - create/rewrite my_output.txt with trades and out in console created structure of d    ata

Logic of program :
 Create array of orders (named hashmap) for fast access to each order, id of order equal hashmap[id]. Two red-black trees are created to store buy and sell orders. All elements are arranged in ascending order of price (if the price is the same, then the elements are arranged in ascending order of id). This data structure was chosen because operations of search, delete and add are performed in O(log N).
