project 5 for programming for performance cs377p


run using `g++ -std=c++11 -o main_xyz main_xyz.cpp -lpthread && ./main_xyz` where xyz is the version of main you want to run described below

For example, `g++ -std=c++11 -o main_7 main_7.cpp -lpthread && ./main_7` for the version that eliminates true and false sharing using multithreading. 

Be sure to adjust the number of threads for each compilation e.g. 1, 2, 4, 8, 16



main_2. 
1% approximation calculation for pi using semicircle

main_3. 
do the computation using pthreads; num threads = [1, 2, 3, 8, 16]

main_4.
add a mutex lock around each read/write

main_5.
make the read/write an atomic instruction

main_6.
eliminate true sharing, placing partial sum in arr_sum[]

main_7.
eliminate true and false sharing, storing partial sum in local variable before adding to arr_sum[]