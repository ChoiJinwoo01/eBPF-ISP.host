#define _GNU_SOURCE

/**
 * @file
 * @brief UNVMe RL embedding table lookup library for use from python.
 *
 * Modified by Mark Wilkening
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <time.h>
#include <err.h>
#include <sys/mman.h>
#include "rdtsc.h"

#include <linux/types.h>
#include <stdint.h>

#include <fcntl.h>
#include <linux/nvme_ioctl.h>
#include <sys/ioctl.h>

#define PGSIZE	4096

/// Print fatal error and exit
#define FATAL(fmt, arg...)  do { ERROR(fmt, ##arg); abort(); } while (0)

/// macro to print an io related error message
#define IOERROR(s, p)   errx(1, "ERROR: " s " lba=%#lx", (p)->lba)



/// page structure
/*typedef struct {
  void*           buf;        ///< IO buffer
  u64             lba;        ///< lba
  unvme_iod_t     iod;        ///< returned IO descriptor
  u64             tsc;        ///< tsc time
  } bw_page_t;*/


struct nvme_passthru_cmd cmd_stats = { 
	.opcode = 0x86,
	.flags = 0,
	.rsvd1=0,
	.nsid=1,
	.cdw2=0,	//param0
	.cdw3=0,	//param1
	.metadata=0,
	.addr       = 0,
	.metadata_len=0,
	.data_len=0,
	.cdw10=0,        //slba
	.cdw11=0,       //slba(upper part of 64bit)
	.cdw12=0,     //nlb
	.cdw13=0,
	.cdw14=0,
	.cdw15=0,
	.timeout_ms=0,
	.result=-1,
};

struct nvme_passthru_cmd cmd_stats_clear = { 
	.opcode = 0x87,
	.flags = 0,
	.rsvd1=0,
	.nsid=1,
	.cdw2=0,	//param0
	.cdw3=0,	//param1
	.metadata=0,
	.addr       = 0,
	.metadata_len=0,
	.data_len=0,
	.cdw10=0,        //slba
	.cdw11=0,       //slba(upper part of 64bit)
	.cdw12=0,     //nlb
	.cdw13=0,
	.cdw14=0,
	.cdw15=0,
	.timeout_ms=0,
	.result=-1,
};



typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef struct {
	u32 attribute_size;
	u32 embedding_length;
	u32 result_embeddings;
	u32 input_embeddings;
	u32 table_id;

	u32 embedding_id_list[];
} embed_config_t;

// Global variables
//static const unvme_ns_t* ns;           ///< unvme namespace pointer
static int qcount = 8;                 ///< queue count
static int qsize = 256;                ///< queue size
static u64 timeout = 0;                ///< tsc elapsed timeout
static char* pciname = "01:00.0";      ///< PCIe identifier for OpenSSD
static int slba = 1024;                ///< Block address to store embedding table
// Lets put tables 1GB apart -- stride is in logical blocks (4KB)
static int table_stride = 250000;

static int fd;

static void* fromPageAlloc;

/**
 * Submit an io and record the submission latency time.
 */
/*static void io_submit(int q, int rw, bw_page_t* p)
  {
  p->tsc = rdtsc();
  if (rw) {
  p->iod = unvme_awrite(ns, q, p->buf, p->lba, ns->nbpp);
  if (!p->iod) IOERROR("awrite", p);
  } else {
  p->iod = unvme_aread(ns, q, p->buf, p->lba, ns->nbpp);
  if (!p->iod) IOERROR("aread", p);
  }
  }*/

/**
 * Reads/writes a region of data.
 */
static void rw_region(void *buf, u64 slba, u64 nlb,
		int rw, int q)
{
	/*timeout = 30 * rdtsc_second();

	  u64 lba = slba;
	  int qdepth = qsize - 1;

	  bw_page_t* pages = calloc(qdepth, sizeof(bw_page_t));
	  void* buf_p = buf;
	  bw_page_t* p = pages;
	  int i;
	  for (i = 0; i < qdepth; i++) {
	  p->buf = buf_p;
	  p->lba = lba;
	  buf_p += ns->pagesize;
	  lba += ns->nbpp;
	  p++;
	  }

	  for (i = 0; i < qdepth; i++) io_submit(q, rw, pages + i);

	  i = 0;
	  int pending = qdepth;
	  do {
	  p = pages + i;
	  if (p->iod) {
	  if (unvme_apoll(p->iod, 0) == 0) {
	  if (lba < slba + nlb) {
	  p->buf = buf_p;
	  p->lba = lba;
	  buf_p += ns->pagesize;
	  lba += ns->nbpp;
	  io_submit(q, rw, p);
	  } else {
	  p->iod = 0;
	  pending--;
	  }
	  } else if ((rdtsc_elapse(p->tsc)) > timeout) {
	  IOERROR("apoll timeout", p);
	  }
	  }
	  if (++i == qdepth) i = 0;
	  } while (pending > 0);

	  free(pages);*/
}

void open_unvme()
{
	fd = open("/dev/nvme0n1", O_RDWR);
	fromPageAlloc = (void *)malloc(4096);
	printf("Hello, world!\n");
	/*if (!(ns = unvme_open(pciname))) exit(1);
	  fromPageAlloc = unvme_alloc(ns, 4096);*/
}

void close_unvme()
{
	close(fd);
	printf("Bye, world...\n");
	/*unvme_free(ns, fromPageAlloc);
	  unvme_close(ns);*/
}

void flush_unvme()
{
	/* todo: request flush command via ioctl && deal with SSD */
	//unvme_flush(ns, 0);
}

void isp_stats_unvme()
{
	ioctl(fd, NVME_IOCTL_IO_CMD, &cmd_stats);
}

void isp_stats_clear_unvme()
{
	ioctl(fd, NVME_IOCTL_IO_CMD, &cmd_stats_clear);
}

void unvme_write_table(int* table, int vector_length,
		int table_length, int table_id)
{
	/* todo: alloc page(s) and issue write() */
	void* buf = (void *)malloc(sizeof(int) * vector_length * table_length+sizeof(int));
	void* page_buf = (void *)malloc(PGSIZE);
	void* buf_ptr = buf;

	int* bufp = (int*)buf;
	int* tablep = table;
	int i;
	u32 write_num;
	for (i = 0; i < vector_length * table_length; i++)
	{
		*bufp = *tablep;
		bufp++;
		tablep++;
	}

	printf("vector_length: %d / table_length: %d\n", vector_length, table_length);
	for(i=0;i<(4*vector_length*table_length) /PGSIZE;i++){    
		memcpy(page_buf, buf_ptr, PGSIZE);
		buf_ptr += PGSIZE;
		struct nvme_user_io io = {
			.opcode     = 0x01,
			.flags      = 0,
			.control    = 0,
			.nblocks    = 0, // read a page
			.rsvd       = 0,
			.metadata   = 0,
			.addr       = (__u64)(uintptr_t) page_buf,
			.slba       = slba + (table_id * table_stride) + i,
			.dsmgmt     = 0,
			.reftag     = 0,
			.appmask    = 0,
			.apptag     = 0,
		};
		int err = ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
		if(err != 0){
			printf("write_table %d err : %d\n", table_id, err);
		}
	}
	free(buf);
	free(page_buf);
	/*void* buf = unvme_alloc(ns, 4 * vector_length * table_length);

	  float* bufp = (float*)buf;
	  float* tablep = table;
	  int i;
	  for (i = 0; i < vector_length * table_length; i++)
	  {
	 *bufp = *tablep;
	 bufp++;
	 tablep++;
	 }

	 rw_region(buf, slba + (table_id * table_stride),
	 (4 * vector_length * table_length) / ns->blocksize, 1, 0);*/
}

float* unvme_sparse_length_sum(
		int* flatInd, int vector_length, int batchsize, int embed_per_result,
		int table_id, int qid, int input_embeddings)
{
	/* todo: make map page and submit it by ISP command */
	int attribute_size = 4;
	void* map_page = (void *)malloc(PGSIZE);
	void* result_ptr;
	int buffersize = (4*vector_length*batchsize > (4*2*input_embeddings + 20)) ?
		4*vector_length*batchsize:(4*2*input_embeddings + 20);
	float* convert_float = (float *)malloc(buffersize + sizeof(float));
	u64 *ptr = map_page;
	u64 base = 1<<31;

	u64 tstart = rdtsc();
	for(int i=0;i<input_embeddings;i++){
		u32 slba_is = slba + (table_id * table_stride) + ((attribute_size * vector_length * flatInd[i*2+1]) / 4096);
		struct nvme_passthru_cmd cmd = { //submit ISP requests
			.opcode = 0x82,
			.flags = 0,
			.rsvd1=0,
			.nsid=1,
			.cdw2=flatInd[i*2],	//param0, result vector
			.cdw3=flatInd[i*2+1],	//param1, vector length
			.metadata=0,
			.addr       = 0,
			.metadata_len=0,
			.data_len=4096,
			.cdw10=slba_is,        //slba
			.cdw11=0,       //slba(upper part of 64bit)
			.cdw12=0,     //nlb part(zero based)
			.cdw13=0,
			.cdw14=0,
			.cdw15=qid,
			.timeout_ms=0,
			.result=-1,
		};
		int err = ioctl(fd, NVME_IOCTL_IO_CMD, &cmd);
		if(err != 0){
			printf("ISP request err : %d\n", err);
		}
		if(flatInd[i*2] >= 4096/(vector_length*attribute_size)){
			printf("WARNING: access beyond range: %d\n", flatInd[i*2]);
		}
		//printf("ISP request to slba %d\n", slba_is);
	}

	struct nvme_passthru_cmd cmd_page_read = { 
		.opcode = 0x84,
		.flags = 0,
		.rsvd1=0,
		.nsid=1,
		.cdw2=0,	//param0
		.cdw3=0,	//param1
		.metadata=0,
		.addr       = (__u64)(uintptr_t) map_page,
		.metadata_len=0,
		.data_len=4096,
		.cdw10=0,        //slba
		.cdw11=0,       //slba(upper part of 64bit)
		.cdw12=0,     //nlb
		.cdw13=0,
		.cdw14=0,
		.cdw15=qid,
		.timeout_ms=0,
		.result=-1,
	};
	int err = ioctl(fd, NVME_IOCTL_IO_CMD, &cmd_page_read);
	if(err != 0){
		printf("Page read err : %d\n", err);
	}

	struct nvme_passthru_cmd cmd_clear = { 
		.opcode = 0x85,
		.flags = 0,
		.rsvd1=0,
		.nsid=1,
		.cdw2=0,	//param0
		.cdw3=0,	//param1
		.metadata=0,
		.addr       = 0,
		.metadata_len=0,
		.data_len=0,
		.cdw10=0,        //slba
		.cdw11=0,       //slba(upper part of 64bit)
		.cdw12=0,     //nlb
		.cdw13=0,
		.cdw14=0,
		.cdw15=qid,
		.timeout_ms=0,
		.result=-1,
	};

	err = ioctl(fd, NVME_IOCTL_IO_CMD, &cmd_clear);
	if(err != 0){
		printf("Clear err : %d\n", err);
	}
	//printf("Clear map page issued\n");

	u64 telapse = rdtsc_elapse(tstart);


	//float* time_ptr = ((float*)result_ptr) + (vector_length*batchsize);
	//*time_ptr = ((float)telapse / (float)rdtsc_second());


	result_ptr = convert_float;
	free(map_page);
	return (float*)result_ptr;
}

int* unvme_read_embedding(int embedidx, int vector_length, int table_id, int qid)
{
	/* todo: issue normal read request */
	int attribute_size = 4;
	void* fromPage = fromPageAlloc;
	int err = 0;

	void* embedding = fromPage +
		((embedidx % (4096 / (attribute_size * vector_length))) * (attribute_size * vector_length));

	struct nvme_user_io io = {
		.opcode     = 0x02,
		.flags      = 0,
		.control    = 0,
		.nblocks    = 0, // read a page
		.rsvd       = 0,
		.metadata   = 0,
		.addr       = (__u64)(uintptr_t) fromPage,
		.slba       = slba + (table_id * table_stride) + ((attribute_size * vector_length * embedidx) / 4096),
		.dsmgmt     = 0,
		.reftag     = 0,
		.appmask    = 0,
		.apptag     = 0,
	};

	u64 tstart = rdtsc();
	//pread(fd, fromPage, 4096, (slba + (table_id * table_stride) + ((attribute_size * vector_length * embedidx) / 4096))*4096);
	err = ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
	if(err != 0){
		printf("read_embedding err : %d\n", err);
	}
	u64 telapse = rdtsc_elapse(tstart);

	float* time_ptr = ((float*)embedding) + (vector_length);
	*time_ptr = ((float)telapse / (float)rdtsc_second());

	/*u64 tstart = rdtsc();
	  unvme_read(ns, qid, fromPage,
	  slba + (table_id * table_stride) + ((attribute_size * vector_length * embedidx) / 4096),
	  1);
	  u64 telapse = rdtsc_elapse(tstart);

	  float* time_ptr = ((float*)embedding) + (vector_length);
	 *time_ptr = ((float)telapse / (float)rdtsc_second());*/

	return (int*)embedding;
}
