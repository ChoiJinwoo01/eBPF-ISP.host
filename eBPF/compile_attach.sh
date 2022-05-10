export C_INCLUDE_PATH=/usr/include/aarch64-linux-gnu/

#clang -I${LIBBPF_DIR}/root/usr/include/ -L${LIBBPF_DIR} attach.c -lbpf
clang attach.c -lbpf
