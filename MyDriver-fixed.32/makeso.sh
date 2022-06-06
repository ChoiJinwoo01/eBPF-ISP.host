gcc -fPIC -c unvme_embed_lib.c
gcc -shared -o libflashrec.so unvme_embed_lib.o
cp libflashrec.so /home/js/eBPF-ISP/RecSSD-RecInfra-fixed.32/models
