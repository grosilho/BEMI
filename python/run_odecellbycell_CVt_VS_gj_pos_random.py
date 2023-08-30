import math
import os
import os.path
from os import path
import csv

segments = ["flat", "wave", "squaredwave", "squaredwave_op"]

subfolders = ["CVt_C_"+segment+"_random_gj" for segment in segments]
results_folder = "../results/ODECellByCellModel"
program_name = "./bemi"

dry_run = False

nx = 1
ny = 30
si = 3
se = 20
Rl = 0.00145
Rt = 0.00145
cl = 100e-4
cw = 10e-4
tend = 2
Imax = 200
dt = 5e-3
dG = 2e-4

N = 500
last_N = 0

segment_defs = {"flat":"-hs true -op false -hl 0.5e-4 -ha 0", 
                "wave":"-hs true -op false -hl 2.5e-4 -ha 1e-4", 
                "squaredwave":"-hs false -op false -hl 2.5e-4 -ha 1e-4", 
                "squaredwave_op":"-hs false -op true -hl 2.5e-4 -ha 1e-4"}

if not dry_run:
    for subfolder in subfolders:
        os.makedirs(subfolder,exist_ok=True)
        os.makedirs(results_folder+"/"+subfolder,exist_ok=True)

        if path.isfile(results_folder+"/"+subfolder+"/N.csv"):
            with open(results_folder+"/"+subfolder+"/N.csv", mode='r') as csvfile:
                csv_reader=csv.reader(csvfile, delimiter=',')
                csv_reader=list(csv_reader)
                last_N = int(csv_reader[-1][0])
        with open(results_folder+"/"+subfolder+"/N.csv", mode='w') as f:
            f.write("N\n")
            f.write(str(last_N+N)+"\n")

def launch_sim(subfolder,seg_opt,nstr):
    output_file_name = subfolder+"/"+"n_"+nstr

    options = ["-ndom 6 -ofreq 1e0 -rfreq 5 -spec true"]
    options.append("-dt "+str(dt))
    options.append("-dG "+str(dG))
    options.append("-cw "+str(cw))
    options.append("-cl "+str(cl))
    options.append("-nx "+str(nx))
    options.append("-ny "+str(ny))
    #options.append("-Rl "+str(Rl))
    #options.append("-Rt "+str(Rt))
    #options.append("-si "+str(si))
    #options.append("-se "+str(se))
    options.append("-tend "+str(tend))
    options.append("-Imax "+str(Imax))
    options.append("-vf 0")
    options.append(seg_opt)
    options.append("-halt 2")
    options.append("-ofile "+output_file_name)

    output_redirection = "> "+output_file_name+".txt"

    #srun_command = "srun --time=00:10:00"
    srun_command = "srun --mem=3000 --oversubscribe"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options                                              #

    print(" ".join(run_command))
    if not dry_run:
        os.system(" ".join(run_command))      

for subfolder,segment in zip(subfolders,segments):
    for i in range(last_N+1,last_N+N+1):
        launch_sim(subfolder,segment_defs[segment],str(i))
