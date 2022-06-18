CC=gcc
CFLAGS= -O2 -Wall
TARGET=host_noaxis ISP_noaxis host_axis0 ISP_axis0 host_axis1 ISP_axis1 ISP_noaxis_mt host_noaxis_mt

all : $(TARGET)

host_noaxis: host_noaxis.c
	$(CC) $(CFLAGS) -o host_noaxis $^

ISP_noaxis: ISP_noaxis.c
	$(CC) $(CFLAGS) -o ISP_noaxis $^

host_axis0: host_axis0.c
	$(CC) $(CFLAGS) -o host_axis0 $^

ISP_axis0: ISP_axis0.c
	$(CC) $(CFLAGS) -o ISP_axis0 $^

host_axis1: host_axis1.c
	$(CC) $(CFLAGS) -o host_axis1 $^

ISP_axis1: ISP_axis1.c
	$(CC) $(CFLAGS) -o ISP_axis1 $^

host_noaxis_mt: host_noaxis_mt.c
	$(CC) $(CFLAGS) -o host_noaxis_mt $^

ISP_noaxis_mt: ISP_noaxis_mt.c
	$(CC) $(CFLAGS) -o ISP_noaxis_mt $^

clean:
	rm $(TARGET)
