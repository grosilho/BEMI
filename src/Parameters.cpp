#include "Parameters.h"
#include "StabilizedOdeRungeKuttaIntegrators.h"
#include "ODECellByCellModel.h"
#include <GetPot>
#include <filesystem>


Parameters::Parameters()
{
    problem_size = -1; //must be redefined later when initializind odes, sdes. If not an error will occur
    refsol_path = string("");
}        

Parameters::~Parameters()
{
}

bool Parameters::initOde(Ode*& ode)
{
    ode = new MultirateODECellByCellModel(ndom,*this);//5e-4
    
    if (!std::filesystem::is_directory("../results/"))
        std::filesystem::create_directory("../results/");
    
    string output_dir = "../results/" + ode->get_problem_name();
    if (!std::filesystem::is_directory(output_dir))
        std::filesystem::create_directory(output_dir);
    
    output_path = "../results/" + ode->get_problem_name() + "/" + output_file;
    if(refsol_file.compare(string(""))!=0)
        refsol_path = "../results/" + ode->get_problem_name() + "/" + refsol_file;
    
    problem_size = ode->get_system_size();
        
    return true;
}

bool Parameters::initOdeTimeIntegrator(OdeRungeKuttaIntegrator*& rk, Ode* ode)
{
    rk = new mRKC(this,dynamic_cast<MultirateOde*>(ode));
    
    return true;
}

bool Parameters::initOdeIntegration(OdeRungeKuttaIntegrator*& rk, Ode*& ode)
{
    if(!initOde(ode))
        return false;
    
    return initOdeTimeIntegrator(rk,ode);
}


bool Parameters::read_command_line(int argc, char** argv)
{
    GetPot cl(argc, argv); //command line parser
    
    ndom = cl.follow(ndom,2,"-ndom","-dom");
    output_file = cl.follow(output_file.c_str(),2,"-outputfile","-ofile");
    refsol_file = cl.follow(refsol_file.c_str(),3,"-refsol","-ref","-refsolfile");
    output_freq = cl.follow(output_freq,2,"-outputfreq","-ofreq");
    verbose = cl.follow(verbose,2,"-verbose","-verb");
    
    matlab_output = cl.follow(matlab_output,3,"-matlab_output","-matlab_out","-matlab");
    bin_output = cl.follow(bin_output,3,"-bin_output","-bin_out","-bin");
    specific_output = cl.follow(specific_output,4,"-spec_output","-spec_out","-spec","-vtk");
    
    dt = cl.follow(dt,3, "-dt", "-tau","-h");
    rho_freq = cl.follow(rho_freq,3, "-rhofreq", "-rho_freq", "-rfreq");
    
    if(cl.search("-convtest"))
        conv_test=true;
    else
        conv_test=false;
    max_pow = cl.follow(max_pow,2,"-maxpow","-max_pow");
    min_pow = cl.follow(min_pow,2,"-minpow","-min_pow");
    
    dtadap = cl.follow(dtadap,3,"-dtadaptivity","-dtadap","-dta");
    rtol = cl.follow(rtol,2,"-rtol","-rt");
    if(!cl.search(2,"-atol","-at"))
        atol=rtol;
    atol = cl.follow(atol,2,"-atol","-at");
    if(!cl.search(2,"-rtol","-rt"))
        rtol=atol;
    ode_contr = static_cast<Controller>(cl.follow(static_cast<int>(ode_contr),"-oec"));
    err_write_data = cl.follow(err_write_data,"-ewd");
    
    //Some problem specific parameters
    P20_dG = cl.follow(1e-3,"-dG");
    P20_cell_l = cl.follow(0.,"-cl");
    P20_cell_w = cl.follow(0.,"-cw");
    P20_nx = cl.follow(0,"-nx");
    P20_ny = cl.follow(0,"-ny");
    P20_Rl = cl.follow(0.,"-Rl");
    P20_Rt = cl.follow(0.,"-Rt");
    P20_si = cl.follow(0.,"-si");
    P20_se = cl.follow(0.,"-se");
    P20_tend = cl.follow(1.,"-tend");
    P20_Imax = cl.follow(0.,"-Imax");
    P20_vertamp = cl.follow(0.,"-va");
    P20_vertfreq = cl.follow(0,"-vf");
    P20_vsmoothwave = cl.follow(true,"-vs");
    P20_horamp = cl.follow(0.,"-ha");
    P20_horlen = cl.follow(0.,"-hl");
    P20_horpos = cl.follow(0.,"-hp");
    P20_horprob = cl.follow(0.,"-hprob");
    P20_horalt = cl.follow(-2,"-halt");
    P20_hsmoothwave = cl.follow(true,"-hs");
    P20_onlyperp = cl.follow(false,"-op");
    P20_nic = cl.follow(1,"-nic");
    
    if(conv_test)
    {
        verbose=false;
        dtadap=false;
        output_freq=-1;
        rho_freq = 1;
    }
    
    if(cl.search("--help"))
    {
        cout<<"This is the BEMI code for solving the EMI model with the boundary element method (BEM)\n"
            <<"Different geometries (cells arrangements) are hardcoded in the executable. Look into src/emi_model/MultiDomain.cpp.\n"
            <<"Run the code from the ./build folder as ./bemi OPTIONS, where OPTIONS is a combination of the below.\n"
              "Results are stored in the ./results folder.\n"<<endl;
        cout<<"The following options are available:\n"
            <<"--- General options:\n"           
            <<"    -ofile      : name of output file. Default: sol.\n"
            <<"    -refsol     : name of the reference solution (if available). Default: \"\"\n"
            <<"                  If available, errors are computed at the end of the simulation.\n"
            <<"    -ofreq      : Output frequency. Default: -1\n"
            <<"                  - If 0 writes solution only at the end of simulation.\n"
            <<"                    In general, used to generate a reference solution.\n"
            <<"                  - If >0 writes solution every ofreq time steps,\n"
            <<"                  - If -1 never writes the solution.\n"
            <<"                    In general, used to generate a solution and just compare it against a reference solution.\n"
            <<"    -bin        : Writes solution in binary format. Default: false.\n"
            <<"    -vtk        : Writes solution in vtk format. Default: false.\n"
            <<"    -matlab     : Writes solution in matlab format. Default: false.\n"
            <<"--- Time integration options:\n"
            <<"    -tend       : Final time. Default: 1.\n"
            <<"    -dt         : Time step size. Default: 1e-2.\n"
            <<"                  When running with an adaptive step size, this is the initial step size.\n"
            <<"    -rfreq      : Frequency at which the spectral radii are re-estimated. Default: 10\n"
            <<"    -verb       : Enables or disables verbosity. Default: true.\n"
            <<"    -dtadap     : Enables/disables time step adaptivity. Default: false.\n"
            <<"    -atol       : sets the absolute tolerance for error control. Default: 1e-2.\n"
            <<"    -rtol       : sets the relative tolerance. Default: rtol=atol.\n"
            <<"    -oec        : sets the error controller. Values are 1,2,3 for\n"
            <<"                  I=Integral, PI=Proportional I, PPI= Predictive PI controllers\n"
            <<"                  respectively. Default: 3.\n"
            <<"    -convtest   : If true, performs a time convergence test instead, i.e. runs several simulations and checks errors. Default: false\n"
            <<"                  If not provided, a reference solution is computed on the fly.\n"
            <<"    -maxpow     : Minimal step size used for the convergence test is dt=tend/2^maxpow. Default: 6\n"
            <<"    -minpow     : Maximal step size used for the convergence test is dt=tend/2^minpow. Default: 3\n"
            <<"--- EMI model options:\n"
            <<"    -ndom       : choose the geometry. A number from 0 to 7. Default: 6.\n"
            <<"                 - 0: Two non touching inner domains, smoothed half circles. Outer domain is a circle.\n"
            <<"                 - 1: Two non touching inner domains, half circles. Outer domain is a circle.\n"
            <<"                 - 2: Two touching inner domains, half circles. Outer domain is a circle.\n"
            <<"                 - 3: One inner domain, a circle. Outer domain is a circle.\n"
            <<"                 - 4: A spiral of cells. Outer domain is a square.\n"
            <<"                 - 5: A cluster of ten cells. Outer domain is an ellipse.\n"
            <<"                 - 6: A rectangular cluster of cells. Number of cells, size, shape, etc are specified via command line. See below.\n"
            <<"                 - 7: The microcard logo\n"        
            <<"    -dG         : Target mesh size. Default: 1e-3.\n"
            <<"    -Rl         : Gap junctions resistivity. Default: 0.00145.\n"
            <<"    -si         : Inner cells electric conductivity.  Default: 3.\n"
            <<"    -se         : Outer domain electric conductivity.  Default: 20.\n"
            <<"    -Imax       : Stimulus intensity (applied for 1ms).  Default: 120.\n"
            <<"--- Additional options only valid for ndom=6:\n"
            <<"    -cl         : cells length. Default: 1e-2.\n"
            <<"    -cw         : cells width.  Default: 0.2e-2.\n"
            <<"    -Rl         : Longitudinal gap junctions resistivity. Default: 0.00145.\n"
            <<"    -Rt         : Transversal gap junctions resistivity. Default: 0.00145.\n"
            <<"    -nx         : Number of cells in horizontal direction.  Default: 10\n"
            <<"    -ny         : Number of cells in vertical direction.  Default: 2\n"
            <<"    -va         : Amplitude of vertical gap junctions waves.  Default: 5e-5.\n"
            <<"    -vf         : Frequency of vertical gap junctions waves.  Default: 0 (a segment).\n"
            <<"    -vs         : Smoothness of vertical gap junctions: waves or squared waves.  Default: true (wave).\n"
            <<"    -ha         : Amplitude of horizontal permeble curves (PC).  Default: 0.\n"
            <<"    -hl         : Length of horizontal PC.  Default: 20e-4.\n"
            <<"    -hs         : Smoothness of horizontal PC: waves or squared waves.  Default: true (wave).\n"
            <<"    -op         : Enables/disables the only perpendicular PC permeability. Default: false.\n"
            <<"    -hp         : Relative position of horizontal PC. Default: 0.5 (centered).\n"
            <<"    -halt       : Alternation of horizontal PC. Default: -1.\n"
            <<"                  - -1: No alternation, no PC. The whole horizontal gap junction is a fully permeable segment.\n"
            <<"                  -  0: No alternation. The PCs are placed one above the other.\n"
            <<"                  -  1: Alternation. The PCs are placed at relative positions hp and 1-hp.\n"
            <<"                  -  2: Random. The PCs are randomly placed on the horizontal gap junction.\n"
            <<"    -hprob      : Probability at which the horizontal PC becomes impermeable. Default: 0\n"
            <<"    -nic        : When ny=2, stimulates 1 or 2 of the leftmost cells. Default: 1\n"
            <<endl;
        
        return false;
    }
    
    return true;
}

void Parameters::print_info()
{
    cout<<scientific;

    cout<<"--------------- ODE Integration ---------------"<<endl;        
    cout<<"Solver: mRKC."<<endl;
    cout<<"Step size: "<<dt<<"."<<endl;
    cout<<"Step size adaptivity: "<<(dtadap ? "yes.":"no.")<<endl;
    if(dtadap)
    {
        cout<<"Controller type: ";
        if(ode_contr==I)
            cout<<"I."<<endl;
        else if(ode_contr==PI)
            cout<<"PI."<<endl;
        else if(ode_contr==PPI)
            cout<<"PPI."<<endl;
        cout<<"Relative tolerance: "<<rtol<<"."<<endl;
        cout<<"Absolute tolerance: "<<atol<<"."<<endl;
        cout<<"Write controller data: "<<(err_write_data ? "yes.":"no.")<<endl;
    }
    if(conv_test)
    {
        cout<<"Convergence test parameters: min_pow = "<<min_pow<<", max_pow = "<<max_pow<<"."<<endl;
    }
    else
    {
        cout<<"Output file name: "<<output_path<<endl;
        cout<<"Output frequency: "<<output_freq<<endl;
        cout<<"Verbose: "<<(verbose ? "yes.":"no.")<<endl;                      
    }
    cout<<"-----------------------------------------------"<<endl;


}

void Parameters::print_info(Ode* ode)
{
    cout<<scientific;
    
    cout<<"----------------- ODE Problem -----------------"<<endl;
    cout<<"Problem name: "<<ode->get_problem_name()<<"."<<endl;
    cout<<"Problem size: "<<ode->get_system_size()<<"."<<endl;
    cout<<"Constant rho: "<<(ode->is_rho_constant() ? "yes.":"no.")<<endl;
    cout<<"Known rho estimation: "<<(ode->estimation_rho_known() ? "yes.":"no.")<<endl;
    cout<<"-----------------------------------------------"<<endl;
    
}
