for k in 0 1 2
do
	python sweep_rm1_host_k${k}.py > host_rm1_k${k}.txt
	python sweep_rm1_ssd_k${k}.py > ssd_rm1_k${k}.txt
	diff host_rm1_k${k}.txt ssd_rm1_k${k}.txt > diff_rm1_k${k}.txt
done

for k in 0 1 2
do
	python sweep_rm3_host_k${k}.py > host_rm3_k${k}.txt
	python sweep_rm3_ssd_k${k}.py > ssd_rm3_k${k}.txt
	diff host_rm3_k${k}.txt ssd_rm3_k${k}.txt > diff_rm3_k${k}.txt
done
#shutdown
