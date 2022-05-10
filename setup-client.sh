sudo nvme disconnect-all
sudo modprobe mlx4_core
lsmod | grep mlx
sudo modprobe nvme-rdma
lsmod | grep nvme
echo
sudo nvme discover -t tcp -a 169.254.126.124 -s 4420
echo
sudo nvme connect -t tcp -n nvmetcp -a 169.254.126.124 -s 4420
sleep 5
lsblk
