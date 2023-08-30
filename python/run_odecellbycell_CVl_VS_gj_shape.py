import math
import os

priority = 'freq'
wave = 'smooth'

results_subfolder = "CVl_C_"+priority+"_"+wave
results_folder = "../results/ODECellByCellModel"
program_name = "./bemi"

nx = 30
ny = 2
si = 3
se = 20
Rl = 0.00145
Rt = 0.00145 # 10**16
cl = 100e-4
cw = 10e-4
tend = 10
Imax = 200
dt = 1e-2
dG = 1.5e-4

if priority=="amp":
    freq_list = [3]
    amp_list = [i*0.25e-4 for i in range(0,11)]
else:
    freq_list = range(0,8)
    amp_list = [5e-5]

if wave=="smooth":
    wave_opt = "true"
else:
    wave_opt = "false"

os.makedirs(results_subfolder,exist_ok=True)
os.makedirs(results_folder+"/"+results_subfolder,exist_ok=True)

with open(results_folder+"/"+results_subfolder+"/freq_list.csv", mode='w') as f:
    f.write("n, freq\n")
    for i,freq in enumerate(freq_list):
        f.write(str(i+1)+", "+str(freq)+"\n")
with open(results_folder+"/"+results_subfolder+"/amp_list.csv", mode='w') as f:
    f.write("n, amp\n")
    for i,amp in enumerate(amp_list):
        f.write(str(i+1)+", "+str(amp)+"\n")

def estimation_runtime(dt,dG):
    safety = 1.
    initialization = 200*pow(1e-3/dG,3) #seconds
    integration = tend*100*pow(1e-2/dt,0.5)*pow(5e-4/dG,2) #seconds
    return safety*(initialization+integration)/3600 #hours

def launch_sim(freq,freqstr,amp,ampstr,runtime=0):
    output_file_name = results_subfolder+"/"+"freq_"+freqstr+"_amp_"+ampstr

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
    options.append("-tend "+str(tend))
    options.append("-Imax "+str(Imax))
    options.append("-nic 2")
    options.append("-vf "+str(freq))
    options.append("-va "+str(amp))
    options.append("-vs "+wave_opt)
    options.append("-halt -1")
    options.append("-ofile "+output_file_name)

    output_redirection = "> "+output_file_name+".txt"

    if runtime==0:
        runtime = math.ceil(estimation_runtime(dt,dG)) # in hours
    srun_command = "srun --time="+str(runtime)+":00:00"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options                                              #

    print(" ".join(run_command))
    os.system(" ".join(run_command))      

# Launch several simulations with different parameters
for i,freq in enumerate(freq_list):
    for j,amp in enumerate(amp_list):
        launch_sim(freq,str(i+1),amp,str(j+1))
