import math
import os
import os.path
from os import path
import csv

results_subfolder1 = "CVl_VS_cells_size"
results_folder = "../results/ODECellByCellModel"
program_name = "./main"

dry_run = False

nx = 30
ny = 2
si = 3
se = 20
Rl = 0.00145
Rt = 0.00145
cl = 100e-4
cw = 10e-4
tend = 10
Imax = 400
dt = 1e-2
dG = 2e-4

results_subfolder2_list = ["cl","cw","clcw"]

cl_list = [cl*pow(2,k/2) for k in range(-8,3)] # 2 in piu in lunghezza
cw_list = [cw*pow(2,k/2) for k in range(-4,5)]
clcw_list = [(cl*pow(2,k/2),cw*pow(2,k/2)) for k in range(-4,3)]

cl_Imax_list = [235*pow(2,-k/2.5) for k in range(-len(cl_list)+1,3)]# /3 was a bit too much
cw_Imax_list = [390*pow(2,k/5) for k in range(-len(cw_list)+1,1)] # 375 seems ok, Imax has to decreas faster than/6, try /5 
clcw_Imax_list = [230*pow(2,-k/3.5) for k in range(-len(clcw_list)+1,3)] #before 250 a bit too high, 200 a bit too low and /4 seems almost ok

def write_list(results_subfolder2,data_name,data_list):
    with open(results_folder+"/"+results_subfolder1+"/"+results_subfolder2+"/"+data_name+"_list.csv", mode='w') as f:
        f.write("n, "+data_name+"\n")  
        for i,data in enumerate(data_list):
            f.write(str(i+1)+", "+str(data)+"\n")

def estimation_runtime(dt,dG):
    safety = 1.5
    initialization = 160*pow(1e-3/dG,3) #seconds
    integration = tend*200*pow(1e-2/dt,0.5)*pow(5e-4/dG,2) #seconds
    return safety*(initialization+integration)/3600 #hours

if not dry_run:
    for results_subfolder2 in results_subfolder2_list:
        os.makedirs(results_subfolder1+"/"+results_subfolder2,exist_ok=True)
        os.makedirs(results_folder+"/"+results_subfolder1+"/"+results_subfolder2,exist_ok=True)
    
    write_list("cl","cl",cl_list)
    write_list("cl","cw",len(cl_list)*[cw])
    
    write_list("cw","cl",len(cw_list)*[cl])
    write_list("cw","cw",cw_list)
    
    write_list("clcw","cl",[cl for (cl,cw) in clcw_list])
    write_list("clcw","cw",[cw for (cl,cw) in clcw_list])

def launch_sim(str1,str2,cl,cw,Imax,runtime=0):
    output_file_name = results_subfolder1+"/"+str1+"/"+str1+"_"+str2

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

    #if runtime==0:
    #    runtime = math.ceil(estimation_runtime(dt,dG)) # in hours
    #srun_command = "srun --time="+str(runtime)+":00:00"
    srun_command = "srun --time=02:00:00"
    #srun_command = "srun --mem=1000 --oversubscribe"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  
    #run_command = [program_name] + options                                              

    print(" ".join(run_command))
    if not dry_run:
        os.system(" ".join(run_command))      


for i,(cl,Imax) in enumerate(zip(cl_list,cl_Imax_list)):
    launch_sim("cl",str(i+1),cl,cw,Imax)
for i,(cw,Imax) in enumerate(zip(cw_list,cw_Imax_list)):
    launch_sim("cw",str(i+1),cl,cw,Imax)
for i,((cl,cw),Imax) in enumerate(zip(clcw_list,clcw_Imax_list)):
    launch_sim("clcw",str(i+1),cl,cw,Imax)    
