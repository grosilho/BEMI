import math
import os

segment = "flat"
#segment = "wave"
#segment = "squaredwave"
segment = "squaredwave_op"

# after setting kappa in Side and correcting code:
# launced squaredwave_op, squaredwave, wave

results_subfolder = "CVt_C_"+segment
results_folder = "../results/ODECellByCellModel"
program_name = "./main"

dry_run = True

nx = 1
ny = 30
#si = 3 #uncomment also in launch_sim() if you want to change these
#se = 20
#Rl = 0.00145
#Rt = 0.00145 # 10**16
cl = 100e-4
cw = 10e-4
tend = 2
Imax = 200
dt = 1e-3
dG = 1e-4
#dG = 2.5e-4    #change when get serious
seg_l = 10e-4
seg_a = 1e-4
seg_p = 0.2

# original
#seg_l_list = [i*2.5e-4 for i in range(2,9)]
#seg_a_list = [i*0.5e-4 for i in range(0,6)]
#seg_l_a_list = [[i*2.5e-4,i*0.5e-4] for i in range (1,6)]
#seg_p_list = [i*0.05 for i in range(2,11)]

# new try
seg_l_list = [i*2.5e-4 for i in range(1,13)]
seg_a_list = [i*0.5e-4 for i in range(0,11)]
seg_l_a_list = [[i*2.5e-4,i*0.5e-4] for i in range (1,11)]
seg_p_list = [i*0.025 for i in range(4,21)]

# dbg
#seg_l_list = [i*2.5e-4 for i in range(2,5)]
#seg_a_list = [i*0.5e-4 for i in range(0,3)]
#seg_l_a_list = [[i*2.5e-4,i*0.5e-4] for i in range (0,3)]
#seg_p_list = [i*0.05 for i in range(2,5)]

segment_pos = {"center":"-halt 0 -hp 0.5", "side":"-halt 0 -hp "+str(seg_p), "side_alt":"-halt 1 -hp "+str(seg_p)}
variable = ["length","amplitude","both"]

if segment=="flat":
    segment_opt = "-hs false -op false"
    seg_a = 0
    seg_a_list = []
    variable = ["length"]
elif segment=="wave":
    segment_opt = "-hs true -op false"
elif segment=="squaredwave":
    segment_opt = "-hs false -op false"
elif segment=="squaredwave_op":
    segment_opt = "-hs false -op true"


def dir_maker(pos,var):
    os.makedirs(results_subfolder+"/"+pos+"/"+var,exist_ok=True)
    os.makedirs(results_folder+"/"+results_subfolder+"/"+pos+"/"+var,exist_ok=True)
    
def write_list(pos,var,data_name,data_list):
    with open(results_folder+"/"+results_subfolder+"/"+pos+"/"+var+"/"+data_name+"_list.csv", mode='w') as f:
        f.write("n, "+data_name+"\n")
        for i,data in enumerate(data_list):
            f.write(str(i+1)+", "+str(data)+"\n")

if not dry_run:
    for pos in segment_pos.keys():
        for var in variable:
            dir_maker(pos,var)
    dir_maker("side_alt","p")

    for pos in segment_pos:
        write_list(pos,"length","length",seg_l_list)
        write_list(pos,"length","amplitude",[seg_a])
        write_list(pos,"length","p",[seg_p])
    if segment!="flat":
        for pos in segment_pos:
            write_list(pos,"amplitude","length",[seg_l])
            write_list(pos,"amplitude","amplitude",seg_a_list)   
            write_list(pos,"amplitude","p",[seg_p])
        for pos in segment_pos:
            write_list(pos,"both","length",[l for l,a in seg_l_a_list])
            write_list(pos,"both","amplitude",[a for l,a in seg_l_a_list])   
            write_list(pos,"both","p",[seg_p])

    write_list("side_alt","p","length",[seg_l])
    write_list("side_alt","p","amplitude",[seg_a])   
    write_list("side_alt","p","p",seg_p_list)



def launch_sim(pos,var,pos_opt,length,lengthstr,amplitude,amplitudestr,p,pstr):
    output_file_name = results_subfolder+"/"+pos+"/"+var+"/"+"length_"+lengthstr+"_amplitude_"+amplitudestr+"_p_"+pstr

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
    options.append("-hl "+str(length))
    options.append("-ha "+str(amplitude))
    options.append(segment_opt)
    options.append(pos_opt)
    options.append("-ofile "+output_file_name)    

    output_redirection = "> "+output_file_name+".txt"

    srun_command = "srun --time=00:15:00"
    #srun_command = "srun --mem=1000 --oversubscribe"

    run_command = [srun_command, program_name] + options + [output_redirection, "&"]  #
    #run_command = [program_name] + options + [output_redirection, "&"]  #

    print(" ".join(run_command))
    if not dry_run:
        os.system(" ".join(run_command))     

    global sim_count
    sim_count = sim_count+1

# Launch several simulations with different parameters
sim_count = 0
for pos,pos_opt in segment_pos.items():
    print("\nPos = ", pos, ", pos_opt = ",pos_opt)
    print("Launch sim for length variable")
    for i,length in enumerate(seg_l_list):
        launch_sim(pos,"length",pos_opt,length,str(i+1),seg_a,"1",seg_p,"1")
    if segment!="flat":
        print("Launch sim for amplitude variable")
        for i,amplitude in enumerate(seg_a_list):
            launch_sim(pos,"amplitude",pos_opt,seg_l,"1",amplitude,str(i+1),seg_p,"1")
        print("Launch sim for length and amplitude variable")
        for i,(length,amplitude) in enumerate(seg_l_a_list):
            launch_sim(pos,"both",pos_opt,length,str(i+1),amplitude,str(i+1),seg_p,"1")

print("Launch sim for p variable")
for i,p in enumerate(seg_p_list):
    launch_sim("side_alt","p","-halt 1 -hp "+str(p),seg_l,"1",seg_a,"1",p,str(i+1))

print("Launched {n} simulations.".format(n=sim_count))