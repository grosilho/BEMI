import math
import os
import os.path
from os import path
import csv

results_subfolder = "CVl_C_si_cl_50"
results_folder = "../results/ODECellByCellModel"
program_name = "./bemi"

dry_run = False

nx = 30
ny = 2
se = 20
Rl = 0.00145 # 10**16
Rt = 0.00145 # 10**16
cl = 50e-4
cw = 10e-4
tend = 30
Imax = 200
dt = 2e-2
dG = 5e-4

# Imax_list = [80*pow(2,k/2.5) for k in range(0,8)] # for cl=100e-4
Imax_list = [120*pow(2,k/2.5) for k in range(0,8)] # for cl=50e-4
si_list = [3*pow(2,k) for k in range(-4,4)] 

if not dry_run:
    os.makedirs(results_subfolder,exist_ok=True)
    os.makedirs(results_folder+"/"+results_subfolder,exist_ok=True)

    with open(results_folder+"/"+results_subfolder+"/si_list.csv", mode='w') as f:
        f.write("n, si\n")
        for i,si in enumerate(si_list):
            f.write(str(i+1)+", "+str(si)+"\n")
    with open(results_folder+"/"+results_subfolder+"/Imax_list.csv", mode='w') as f:
        f.write("n, Imax\n")
        for i,Imax in enumerate(Imax_list):
            f.write(str(i+1)+", "+str(Imax)+"\n")

def estimation_runtime(dt,dG):
    safety = 1.5
    initialization = 160*pow(1e-3/dG,3) #seconds
    integration = tend*200*pow(1e-2/dt,0.5)*pow(5e-4/dG,2) #seconds
    return safety*(initialization+integration)/3600 #hours

def launch_sim(si,Imax,istr,runtime=0):
    output_file_name = results_subfolder+"/"+"si_Imax_"+istr

    options = ["-ndom 6 -ofreq 1e0 -rfreq 5 -spec true"]
    options.append("-dt "+str(dt))
    options.append("-dG "+str(dG))
    options.append("-cw "+str(cw))
    options.append("-cl "+str(cl))
    options.append("-nx "+str(nx))
    options.append("-ny "+str(ny))
    options.append("-Rl "+str(Rl))
    options.append("-Rt "+str(Rt))
    options.append("-si "+str(si))
    options.append("-se "+str(se))
    options.append("-nic 2")
    options.append("-tend "+str(tend))
    options.append("-Imax "+str(Imax))
    options.append("-ofile "+output_file_name)

    output_redirection = "> "+output_file_name+".txt"

    #if runtime==0:
    #    runtime = math.ceil(estimation_runtime(dt,dG)) # in hours
    #srun_command = "srun --time="+str(runtime)+":00:00"
    srun_command = "srun --mem=1000 --oversubscribe"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options                                              #

    print(" ".join(run_command))
    if not dry_run:
        os.system(" ".join(run_command))      


for i,(si,Imax) in enumerate(zip(si_list,Imax_list)):
    launch_sim(si,Imax,str(i+1))
