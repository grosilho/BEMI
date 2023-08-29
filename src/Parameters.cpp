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
    specific_output = cl.follow(specific_output,3,"-spec_output","-spec_out","-spec");
    
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
    
//    if(cl.search("--help"))
//    {
//        cout<<"This is a code for running ODE and SDE simulations. A list of problems\n"
//            <<"is hardcoded in the executable. Look into OdeList.h and SdeList.h.\n"
//            <<"Each problem has a number n depending on the order of apparition in the list."<<endl;
//        cout<<"The following options are available:\n"
//            <<"    -sde       : run an SDE simulation.\n"
//            <<"    -ode       : run an ODE simulation.\n"
//            <<"    -oec       : sets the ODE error controller. Values are 1,2,3 for\n"
//            <<"                 I=Integral, PI=Proportional I, PPI= Predictive PI controllers\n"
//            <<"                 respectively. Default is 3.\n"
//            <<"    -cee c     : if c=1 the integrator computes the exact local error (with a lot of Eulers steps).\n"
//            <<"    -ewd e     : if e=1 then the error controller writes data it collects into disk at each step.\n"
//            <<"    -ntest n   : chooses the problem. It takes the nth problem\n"
//            <<"                 from list SdeList.h or from OdeList.h.\n"
//            <<"    -dt h      : time step when running in fixed time step mode or initial time\n"
//            <<"                 step when running in adaptive time step mode.\n"
//            <<"    -dtadap dta: if dta=1 enables time step adaptivity, else in fixed time step mode.\n"
//            <<"    -contW W   : if W=1 uses a continuous brownian motion, if W=0 a discrete one.\n"
//            <<"    -atol at   : sets the absolute tolerance, by default at=1e-2.\n"
//            <<"    -rtol rt   : sets the relative tolerance. If not provided then rtol=atol.\n"
//            <<"    -ofile ofi : name of output file. By default ofi=solution.\n"
//            <<"    -iter it   : Number of Monte Carlo simulations.\n"
//            <<"    -ofreq ofr : frequency of write to disk. If ofr=0 writes at end of simulation\n"
//            <<"                 only. If ofr=-1 never writes solution. If it>1 we set ofr=-1.\n"
//            <<"    -verb v    : enables or disables verbosity. If it>1 then v=0.\n"
//            <<"    -seed s    : sets the seed for random numbers. By default is -1, which is a random seed.\n"
//            <<"    -solver rk : name of RungeKutta solver to use. The available solvers are:\n"
//            <<"                 -RKC2     Runge-Kutta-Chebychev order 2,\n"
//            <<"                 -ROCK2    Runge-Kutta-Orthogonal-Chebychev order 2,\n"
//            <<"                 -DROCK2   ROCK2 with increased damping,\n"
//            <<"                 -SROCK2   Stochastic Weak Order 2 ROCK2,\n"
//            <<"                 -MT       Weak order 2 Milstein-Talay.\n"<<endl;
//        cout<<"Error estimators are available for ROCK2 and SROCK2. Not implemented in DROCK2 and MT."<<endl;
//        cout<<"Files are written into folders with same name of the tests."<<endl;
//        return false;
//    }
    
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
