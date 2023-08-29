#ifndef INIT_H
#define	INIT_H

#include "MainHeader.h"

class TimeIntegrator;
class OdeRungeKuttaIntegrator;
class Ode;

class GetPot;

class Parameters
{
public:
    Parameters();
    ~Parameters();
    
    bool read_command_line(int argc, char** argv);
    
    bool initOde(Ode*& ode);
    bool initOdeTimeIntegrator(OdeRungeKuttaIntegrator*& rk, Ode* ode);
    bool initOdeIntegration(OdeRungeKuttaIntegrator*& rk, Ode*& ode);
    
    void print_info();
    void print_info(Ode* ode);
    
public:
    int ndom;
    string output_path;
    string refsol_path;
    string output_file;
    string refsol_file;
    int output_freq;
    bool verbose;
    
    bool matlab_output;
    bool bin_output;
    bool specific_output;
    
    Real dt;
    unsigned int rho_freq;
    
    bool conv_test;
    unsigned int max_pow;
    unsigned int min_pow;
    
    bool dtadap;
    Real rtol;
    Real atol;
    Controller ode_contr;
    bool err_write_data;
    
    int problem_size;
    
//    EMI specific parameters
    Real P20_dG;
    Real P20_cell_l;
    Real P20_cell_w;
    unsigned P20_nx;
    unsigned P20_ny;
    Real P20_Rl;
    Real P20_Rt;
    Real P20_si;
    Real P20_se;
    Real P20_tend;
    Real P20_Imax;
    Real P20_vertamp;
    unsigned P20_vertfreq;
    bool P20_vsmoothwave;
    Real P20_horamp;
    Real P20_horlen;
    Real P20_horpos;
    int P20_horalt;
    bool P20_hsmoothwave;
    bool P20_onlyperp;
    unsigned P20_nic;
    Real P20_horprob;
};


#endif	/* INIT_H */

