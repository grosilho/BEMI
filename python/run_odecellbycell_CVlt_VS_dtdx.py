import math
import os

direction = 'longitudinal'
#direction = 'trasversal'

if direction=='longitudinal':
    nx = 30
    ny = 2
    si = 3
    tend = 4
    Imax = 300
    results_subfolder = "CVl_C_dtdx"
else:
    nx = 2
    ny = 20
    si = 1.5 #1.5 or 3
    tend = 3
    Imax = 130 # 130 for si=1.5, 200 for si=3
    results_subfolder = "CVt_si_"+str(si).replace('.','p')+"_LR_dtdx"

dt = 1e-2
dG = 1e-3
cl = 1.0e-2
cw = 0.2e-2
Rl = 0.00145
Rt = 0.00145 # 10**16
se = 20
results_folder = "../results/ODECellByCellModel"

program_name = "./main"
file_name_base = results_subfolder


dt_list = [dt*pow(2,k) for k in range(-3,5)] # from -3 to 5
dG_list = [dG*pow(2,k/2) for k in range(-4,3)] # from -4 to 3

os.makedirs(results_subfolder,exist_ok=True)
os.makedirs(results_folder+"/"+results_subfolder,exist_ok=True)

with open(results_folder+"/"+results_subfolder+"/dt_list.csv", mode='w') as f:
    f.write("n, dt\n")
    for i,dt in enumerate(dt_list):
        f.write(str(i+1)+", "+str(dt)+"\n")
with open(results_folder+"/"+results_subfolder+"/dG_list.csv", mode='w') as f:
    f.write("n, dG\n")
    for j,dG in enumerate(dG_list):
        f.write(str(j+1)+", "+str(dG)+"\n")

def estimation_runtime(dt,dG):
    safety = 1.5
    initialization = 80*pow(1e-3/dG,3) #seconds
    integration = tend*100*pow(1e-2/dt,0.5)*pow(5e-4/dG,2) #seconds
    return safety*(initialization+integration)/3600 #hours

def launch_sim(dt,dG,dtstr,dGstr,runtime=0):
    output_file_name = results_subfolder+"/"+"dt_"+dtstr+"_dG_"+dGstr

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
    options.append("-vf 0")
    options.append("-halt -1")
    options.append("-tend "+str(tend))
    options.append("-Imax "+str(Imax))
    options.append("-nic 2")
    options.append("-ofile "+output_file_name)

    output_redirection = "> "+output_file_name+".txt"

    if runtime==0:
        runtime = math.ceil(estimation_runtime(dt,dG)) # in hours
    srun_command = "srun --time="+str(runtime)+":00:00"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options                                              #

    print(" ".join(run_command))
    os.system(" ".join(run_command))      

# Launch a reference solution
launch_sim(0.0005,0.000125,"ref","ref",12)

# Launch several simulations with different parameters
for i,dt in enumerate(dt_list):
    for j,dG in enumerate(dG_list):
        launch_sim(dt,dG,str(i+1),str(j+1))
