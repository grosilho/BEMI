import math
import os
import os.path
from os import path
import csv

# segments = ["flat", "wave", "squaredwave", "squaredwave_op"]
segments = ["flat"] # , "wave", "squaredwave", "squaredwave_op"]
sim_cell = "middle"

nx = 20
ny = 20

subfolders = ["CVltd_C_"+segment+"_randomly_deactivated_gj_nx_"+str(nx)+"_ny_"+str(ny)+"_stim_cell_"+sim_cell for segment in segments]
results_folder = "../results/ODECellByCellModel"
program_name = "./main"

dry_run = True

cl = 100e-4
cw = 10e-4
tend = 3
Imax = 1500
dt = 5e-3
dG = 10e-4

N = 10
last_N = 0

segment_defs = {"flat":"-hs true -op false -hl 0.5e-4 -ha 0", 
                "wave":"-hs true -op false -hl 2.5e-4 -ha 1e-4", 
                "squaredwave":"-hs false -op false -hl 2.5e-4 -ha 1e-4", 
                "squaredwave_op":"-hs false -op true -hl 2.5e-4 -ha 1e-4"}

horprob = [10*i for i in range(0,11)]
subsubfolders = ["p_"+str(prob) for prob in horprob]

if not dry_run:
    for subfolder in subfolders:
        os.makedirs(subfolder,exist_ok=True)
        os.makedirs(results_folder+"/"+subfolder,exist_ok=True)

        for subsubfolder,prob in zip(subsubfolders,horprob):
            os.makedirs(subfolder+"/"+subsubfolder,exist_ok=True)
            os.makedirs(results_folder+"/"+subfolder+"/"+subsubfolder,exist_ok=True)
            if path.isfile(results_folder+"/"+subfolder+"/"+subsubfolder+"/N.csv"):
                with open(results_folder+"/"+subfolder+"/"+subsubfolder+"/N.csv", mode='r') as csvfile:
                    csv_reader=csv.reader(csvfile, delimiter=',')
                    csv_reader=list(csv_reader)
                    last_N = int(csv_reader[-1][0])
            with open(results_folder+"/"+subfolder+"/"+subsubfolder+"/N.csv", mode='w') as f:
                f.write("N\n")
                f.write(str(last_N+N)+"\n")

def launch_sim(subfolder,subsubfolder,seg_opt,pstr,nstr):
    output_file_name = subfolder+"/"+subsubfolder+"/"+"n_"+nstr

    options = ["-test 20 -ofreq 1e0 -rk mRKC -rfreq 5 -spec true"]
    options.append("-dt "+str(dt))
    options.append("-dG "+str(dG))
    options.append("-cw "+str(cw))
    options.append("-cl "+str(cl))
    options.append("-nx "+str(nx))
    options.append("-ny "+str(ny))
    options.append("-tend "+str(tend))
    options.append("-Imax "+str(Imax))
    options.append("-vf 0")
    options.append(seg_opt)
    options.append("-halt 2")
    options.append("-hprob "+pstr)
    options.append("-ofile "+output_file_name)

    output_redirection = "> "+output_file_name+".txt"

    srun_command = "srun --time=00:30:00"
    #srun_command = "srun --mem=3000 --oversubscribe"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options                                              #

    print(" ".join(run_command))
    if not dry_run:
        os.system(" ".join(run_command))      

for subfolder,segment in zip(subfolders,segments):
    for subsubfolder,prob in zip(subsubfolders,horprob):
        for i in range(last_N+1,last_N+N+1):
            launch_sim(subfolder,subsubfolder,segment_defs[segment],str(prob/100),str(i))
