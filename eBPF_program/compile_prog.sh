export C_INCLUDE_PATH=/usr/include/aarch64-linux-gnu/

clang -target bpf -Wall -O2 -c $1c -o $1o
