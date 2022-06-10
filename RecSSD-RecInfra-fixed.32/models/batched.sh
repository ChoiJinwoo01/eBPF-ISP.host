python sweep_rm1.py > host_rm1.txt
python sweep_rm1_ssd.py > ssd_rm1.txt
diff host_rm1.txt ssd_rm1.txt > diff_rm1.txt
#python sweep_rm2.py > host_rm2.txt
#python sweep_rm2_ssd.py > ssd_rm2.txt
#diff host_rm2.txt ssd_rm2.txt > diff_rm2.txt
#python sweep_rm3.py > host_rm3.txt
#python sweep_rm3_ssd.py > ssd_rm3.txt
#diff host_rm3.txt ssd_rm3.txt > diff_rm3.txt
#shutdown
