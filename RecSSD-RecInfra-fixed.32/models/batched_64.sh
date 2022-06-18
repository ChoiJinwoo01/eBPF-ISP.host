for k in 1 2
do
	python sweep_rm2_host_k${k}.py > host_rm2_k${k}.txt
	python sweep_rm2_ssd_k${k}.py > ssd_rm2_k${k}.txt
	diff host_rm2_k${k}.txt ssd_rm2_k${k}.txt > diff_rm2_k${k}.txt
done

python sweep_rm2_ssd_k0.py > ssd_rm2_k0.txt
diff host_rm2_k0.txt ssd_rm2_k0.txt > diff_rm2_k0.txt
#shutdown
