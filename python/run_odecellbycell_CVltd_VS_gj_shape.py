import math
import os

nx = 30
ny = 1

results_subfolder = "CVtld_C_nx_"+str(nx)+"_ny_"+str(ny)
results_folder = "../results/ODECellByCellModel"
program_name = "./main"

dry_run = False

#si = 3 #uncomment also in launch_sim() if you want to change these
#se = 20
#Rl = 0.00145
#Rt = 0.00145 # 10**16
cl = 100e-4
cw = 10e-4
tend = 2
Imax = 200
dt = 1e-2
dG = 2e-4

segments = { "flat":"-halt 1 -hp 0.05 -hs true -op false -hl 1.0e-4 -ha 0", 
            "wave":"-halt 1 -hp 0.2 -hs true -op false -hl 1.7e-4 -ha 1e-4", # for p=0.2 put hl=1.7e-4
            "squaredwave":"-halt 1 -hp 0.2 -hs false -op false -hl 2.5e-4 -ha 1e-4", 
            "squaredwave_op":"-halt 1 -hp 0.2 -hs false -op true -hl 2.5e-4 -ha 1e-4" }

if not dry_run:
    os.makedirs(results_subfolder,exist_ok=True)
    os.makedirs(results_folder+"/"+results_subfolder,exist_ok=True)

def launch_sim(segment_key,segment_opt):
    output_file_name = results_subfolder+"/"+segment_key

    options = ["-test 20 -ofreq 1e0 -rk mRKC -rfreq 5 -spec true"]
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
    options.append("-va 0")
    options.append("-vs true")
    options.append(segment_opt)
    options.append("-ofile "+output_file_name)    

    output_redirection = "> "+output_file_name+".txt"

    #srun_command = "srun --time=02:00:00"
    #srun_command = "srun --mem=60000 -c 24 --oversubscribe"
    srun_command = "srun --mem=15000 -c 6 --oversubscribe"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options + [output_redirection, "&"]  #

    print(" ".join(run_command))
    if not dry_run:
        os.system(" ".join(run_command))     


# Launch several simulations with different parameters
for segment_key,segment_opt in segments.items():
    launch_sim(segment_key,segment_opt)