# eBPF-ISP.host
eBPF-ISP is the In-Storage Processing framework using eBPF architecture in SSD. To offload program to the SSD, follow these steps.

# Insert In-Storage Program
To insert In-Storage Program, you should submit a request with opcode 0x83(nvme_isp_insert). 
In development...

# Run In-Storage Program
To invoke ISP, you should submit a request with opcode 0x82(nvme_isp_read). 
To submit a request with arbitrary OPcode, use ioctl.
After ISP, the struct nvme_passthru_cmd's member "result"(32bit integer) will indicate the eBPF program's result.

# Large size return
Some return values like vector, array, ... is bigger than 32bit int.
To get these results, you should use map page.
To get map page, you should submit a request with opcode 0x84(nvme_isp_page_read).
