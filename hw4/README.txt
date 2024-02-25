CONTRIBUTIONS

TODO: write a brief summary of how each team member contributed to
the project.
1. Maverick Espinosa (mespin11)
2. Rupasri Chalavadi (rchalav1)

Maverick: implemented parallelism for mergesort
Rupasri: worked on error handling/ experiment

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.

Test run with threshold 2097152

real    0m0.372s

Test run with threshold 1048576

real    0m0.188s

Test run with threshold 524288

real    0m0.127s

Test run with threshold 262144

real    0m0.091s

Test run with threshold 131072

real    0m0.103s

Test run with threshold 65536

real    0m0.100s

Test run with threshold 32768

real    0m0.106s

Test run with threshold 16384

real    0m0.110s


States a reasonable explanation for why you saw the times you did:

From the running the experiments shell script, we noticed as the threshold
decreases, the real time it took for the program to start and stop also decreased.
This relationship, however only took effect once the threshold decreased till
65500. Decreasing the threshold further resulted in the real time increasing.

This change in relationship could be due to the fact that when we have a high threshold (eg 2097152)
the CPU isnt maximally being used since we are doing sequential sort and not utilizing recursion for sorting(O(n) vs O(lg(n))). 
After decreasing the threshold, at some point, the size > threshold and we will utilize more cores of the CPU through
forking, thus the time it takes to execute the recursive calls will be shorter since mergesorting 
the left and right sections of the input array will occur simultaneously instead of after the 
the left section is divided up into singletons. 

At some point where size >> threshold, the number
of processes being run utilizes the maximum core units, thus instead of actually increasing efficiency,
the cpu will now have to figure out how to balance sharing the use of its cores and thus result in
longer program execution times. In short, the overhead of creating and managing multiple processes outweighs the benefits 
of parallelism. The cost of creating and managing processes becomes more significant as the size of the input 
data increases, which can lead to longer execution times.
