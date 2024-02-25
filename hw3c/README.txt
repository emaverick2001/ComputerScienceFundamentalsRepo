Student names:
    1. Maverick Espinosa (mespin11)
    2. Rupasri Chalavadi (rchalav1)

    Maverick: worked on command line arguments validation, created framework (mapped out datastructures)
    Rupasri: worked on makefile, wrote up logic code

    Both worked together to discuss logic of the stores and loads, and worked together to debug (took forever lol) the functions.

Part b (Experiment):

To test the most efficient configuration of cache we will change one parameter and hold all other parameters constant
The command we will use to execute our test is "./csim 256 4 16 write-allocate write-through  lru < gcc.trace" and this will be used as the refernce.
We are using gcc.trace since it will allow us to test with many different variations

Here is the run with no parameters changed:

Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9347882

Changing the number of sets:
Decreasing: "./csim 128 4 16 write-allocate write-through  lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 314007
Load misses: 4190
Store hits: 188015
Store misses: 9471
Total cycles: 10075873

Increasing: "./csim 512 4 16 write-allocate write-through  lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 315211
Load misses: 2986
Store hits: 188468
Store misses: 9018
Total cycles: 8610669

We can see that by increasing the number of sets, the total number of cycles produced is less.
Next we will change the number of blocks.

Decreasing: "./csim 256 1 16 write-allocate write-through  lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 298863
Load misses: 19334
Store hits: 185202
Store misses: 12284
Total cycles: 32930817

Increasing: "./csim 256 8 16 write-allocate write-through  lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 315261
Load misses: 2936
Store hits: 188513
Store misses: 8973
Total cycles: 25030819

We see that increasing the number of blocks takes up less amount of cycles.
Next we will change the byte size of the blocks.

Decreasing: "./csim 256 4 8 write-allocate write-through  lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 312586
Load misses: 5611
Store hits: 179374
Store misses: 18112
Total cycles: 25014494

Increasing: "./csim 256 4 32 write-allocate write-through  lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 316126
Load misses: 2071
Store hits: 192823
Store misses: 4663
Total cycles: 25653554

We can see that decreasing the block size in bytes produces a smaller amount of total cycles.
More Offset bits with the same amount of index bits results in less tag bits
less tag bits means more hits since higher chance for tag of address to match up (since there are less values being compared);
Now we will change the write hit policy.

Write Through: 
"./csim 256 4 16 write-allocate write-through lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 25321682

Write Back:
"./csim 256 4 16 write-allocate write-back lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9347882

We can see that write back is more efficient since it takes less total cycles.
Intuitively, this makes sense because each time a memory address is found using write back it doesnt write to memory until evicted. Thus
we will write to memory less (based on how many times we need to evict a slot) and so it takes less cycles.
Now we will change write miss policy.

Write Allocate:
"./csim 256 4 16 write-allocate write-through lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 25321682

No Write Allocate:
"./csim 256 4 16 no-write-allocate write-through lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 22871800

We can see that no write allocate is more efficient since it takes less total cycles and with this policy we access main memory less than write allocate.
In cases where we are writing to the same block of memory over and over it would be better to have wrtie-allocate since
the block will be called multiple times and the cache will have this available, decreasing the number of cycles it takes to
access the memory address.
Next we will change the eviction policy.

lru:
"./csim 256 4 16 no-write-allocate write-through lru < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 22871800

fifo:
"./csim 256 4 16 no-write-allocate write-through fifo < gcc.trace"

Total loads: 318197
Total stores: 197486
Load hits: 311017
Load misses: 7180
Store hits: 163705
Store misses: 33781
Total cycles: 23109682

We can see that lru is more efficient since it takes less total cycles and causes less store/load misses. If a given slot is called multiple times and if it is removed by fifo due to the set the slot is in being full, 
then we will get more load/store misses causing us to access main memory more often.


Based on our experiment, we can conclude that the best cache will be one with the
highest number of sets and blocks, lowest number of bytes per block. The write-hit policy will be
no-write allocate (assuming the same block isn't called as frequently). The write-miss policy will be 
write-back and the eviciton policy will be lru.