import math
import os
import os.path
from os import path
import csv

results_subfolder = "CVl_C_Rl_cl_50_si_10"
results_folder = "../results/ODECellByCellModel"
program_name = "./main"

nx = 30
ny = 2
si = 10
se = 20
Rt = 0.00145 # 10**16
cl = 50e-4
cw = 10e-4
tend = 30
# For si=3:  Imax=200 for cl=100e-4 and Imax=400 for cl=50e-4. 
# For si=12: Imax=400 for cl=100e-4 and Imax=800 for cl=50e-4
Imax = 800 
dt = 2e-2
dG = 5e-4

Rl_list = [pow(2,k) for k in range(-7,15)]

try:
    os.mkdir(results_subfolder)
except OSError as error:
    print(error)
try:
    os.mkdir(results_folder+"/"+results_subfolder)
except OSError as error:
    print(error)

if path.isfile(results_folder+"/"+results_subfolder+"/Rl_list.csv"):
    print("file exists")
    with open(results_folder+"/"+results_subfolder+"/Rl_list.csv", mode='r') as csvfile:
        csv_reader=csv.reader(csvfile, delimiter=',')
        csv_reader=list(csv_reader)
        last_n = int(csv_reader[-1][0])
else:
    last_n=0

with open(results_folder+"/"+results_subfolder+"/Rl_list.csv", mode='a') as f:
    if last_n==0:
        f.write("n, Rl\n")
    for i,Rl in enumerate(Rl_list):
        f.write(str(last_n+i+1)+", "+str(Rl)+"\n")

def estimation_runtime(dt,dG):
    safety = 1.5
    initialization = 160*pow(1e-3/dG,3) #seconds
    integration = tend*200*pow(1e-2/dt,0.5)*pow(5e-4/dG,2) #seconds
    return safety*(initialization+integration)/3600 #hours

def launch_sim(Rl,Rlstr,runtime=0):
    output_file_name = results_subfolder+"/"+"Rl_"+Rlstr

    options = ["-test 20 -ofreq 1e0 -rk mRKC -rfreq 5 -spec true"]
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

    if runtime==0:
        runtime = math.ceil(estimation_runtime(dt,dG)) # in hours
    srun_command = "srun --time="+str(runtime)+":00:00"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options                                              #

    print(" ".join(run_command))
    os.system(" ".join(run_command))      


for i,Rl in enumerate(Rl_list):
    launch_sim(Rl,str(last_n+i+1))
