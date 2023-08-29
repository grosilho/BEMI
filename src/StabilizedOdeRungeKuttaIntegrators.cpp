#include "StabilizedOdeRungeKuttaIntegrators.h"
#include "ChebyshevMethods.h"

mRKC::mRKC(Parameters* param_, MultirateOde* mode_)
:MultirateOdeRungeKuttaIntegrator(param_,mode_)
{
    Astable = false;
    err_order = 1;
    
    damping = 0.05;
    beta = 2.-4.*damping/3.;
       
    for(int i=0;i<4;i++)
        integr_add[i]=new Vector(mode->get_system_size());
    
    reinit_integrator();
}

mRKC::~mRKC()
{
    for(int i=0;i<4;i++)
        delete integr_add[i];
}

void mRKC::update_n_stages_and_h(Real& h)
{
    /**
     * Updates the number of stages needed by RKC. It follows the formula
     * \f$\rho \Delta t \leq \beta s^2 \f$ with some cautions.
     */
    
    unsigned int stages_limit = 1e6;
    unsigned int safe_add = 1;
    

    if(h*eigmax_S<1.5) //for s=1 we have beta=2 and not 2-4/3*damping, so maybe one stage is enough
        s=1;
    else if(h*eigmax_S<2.) // if close to 2 then add some stages for safety
        s=1+safe_add;
    else // if EE not stable then use RKC formula
    {
        s = ceil(sqrt(h*eigmax_S/beta));
        s += safe_add;
    }

    //we try to avoid more than max_s stages for roundoff errors reasons
    if(s>stages_limit)
    {
        //if more than max_s stages are needed we resize dt
        s=stages_limit; 
        h=0.95*beta*s*s/eigmax_S;     
        last=false;
        err_control.update_hn(h);
    }
    else if(dt_adaptivity)
        s = max(s,2); // at least two stages, needed to estimate the error

    // General ODE
//    m = ceil(sqrt(6.*eigmax_F*h/beta/beta/s/s+1.))+safe_add;
//    eta = 6.*m*m*h/(m*m-1.)/beta/s/s;
    
    // Parabolic ODE in refined mesh or ODE with scale separation
    eta = 2.*h/beta/s/s;
    m = ceil(sqrt(eta*eigmax_F/beta))+safe_add;    
    
//    write_parameters(h);
    
    s_max= max(s_max,s);
    m_max= max(m_max,m);
    s_avg += s;
    m_avg += m;
    
}

void mRKC::write_parameters(Real dt)
{
    static int nout=1;
    ofstream outfile;
            
    if(nout==1)
    {
        outfile.open(param->output_path+string("_params.csv"), ofstream::out);
        outfile<<"s, m, dt, eta, rhoF, rhoS"<<endl;
    }
    else
        outfile.open(param->output_path+string("_params.csv"), ofstream::out | ofstream::app);
    
    outfile<<setprecision(16)<<s<<", "<<m<<", "<<dt<<", "<<eta<<", "<<eigmax_F<<", "<<eigmax_S<<endl;      
     
    outfile.close();
    
    nout++;
}

void mRKC::step(const Real t, const Real& h)
{
    Vector*& fn= integr[0];
    Vector*& Kjm1= integr[1];
    Vector*& Kjm2= integr[2];
    Vector*& Kj= integr[3];
    Vector* swap_ptr=0;   
    
    vector<Real> c;
    vector<Real> d;
    vector<Real> nu;
    vector<Real> mu;
    vector<Real> kappa;
    
    ChebyshevMethods::CoefficientsRKC1(mu, nu, kappa, c, d, s, damping);
    
    // Stage 0
    *Kjm1 = *yn;
        
    // Stage 1
    f_eta(t,*Kjm1,*fn); 
    *Kj = mu[0]*h*(*fn);
    *Kj += *Kjm1;
    
    // Stages j=2,...,s
    for(unsigned int j=2;j<=s;j++)
    {
        swap_ptr=Kjm2;
        Kjm2=Kjm1;
        Kjm1=Kj;
        Kj=swap_ptr;               
                
        f_eta(t+h*c[j-2],*Kjm1,*Kj);           
        *Kj *= h*mu[j-1];
        *Kj += nu[j-1]*(*Kjm1);
        *Kj += kappa[j-1]*(*Kjm2);                                   
    }   
    
    swap_ptr=ynpu;
    ynpu=Kj;
    Kj=swap_ptr;

    //compute errors
    if(s>=2)
    {
        Real mult, a1, a2, a3;    
        if(s>2)
        {
            mult = (d[s-1]-1.)/((c[s-3]-1.)*(d[s-2]-d[s-1])-(c[s-2]-1)*(d[s-3]-d[s-1]));
            a1 = (1.-c[s-2])*mult;
            a2 = (c[s-3]-1.)*mult;
            a3 = (c[s-2]-c[s-3])*mult;
        }
        else
        {        
            mult = (d[s-1]-1.)/d[s-1]/c[s-2];
            a1 = (1.-c[s-2])*mult;
            a2 = -mult;
            a3 = c[s-2]*mult;
        }
        *Kjm2 *= a1;
        *Kjm2 += a2*(*Kjm1);
        *Kjm2 += a3*(*ynpu);

        for(int i=0;i<mode->get_system_size();i++)
            (*Kjm2)(i) = (*Kjm2)(i)/(a_tol+r_tol*max(abs((*ynpu)(i)),abs((*yn)(i))));

        err_control.get_errD() =Kjm2->norm()/sqrt(mode->get_system_size());
    }
    else
        err_control.get_errD() = 0.;
    
    n_fS_eval += s;
    n_fF_eval += s*m;
        
}  

void mRKC::f_eta(Real t, Vector& x, Vector& fx)
{
    Vector*& Ljm1= integr_add[0];
    Vector*& Ljm2= integr_add[1];
    Vector*& Lj= integr_add[2];
    Vector*& R0= integr_add[3];
    Vector* swap_ptr=0;   
    
    vector<Real> c;
    vector<Real> d;
    vector<Real> nu;
    vector<Real> mu;
    vector<Real> kappa;
    
    ChebyshevMethods::CoefficientsRKC1(mu, nu, kappa, c, d, m, damping);
    
    
    // Stage 0
    mode->fS(t,x,*R0);
    *Ljm1 = x;
    
    // Stage 1
    mode->fF(t,x,*Lj);
    *Lj += *R0;
    *Lj *= eta*mu[0];
    *Lj += *Ljm1;

    
    // Stages j=2,...,m
    for(unsigned int j=2;j<=m;j++)
    {
        swap_ptr=Ljm2;
        Ljm2=Ljm1;
        Ljm1=Lj;
        Lj=swap_ptr;               
                
        mode->fF(t+eta*c[j-2],*Ljm1,*Lj);   
        *Lj += *R0;
        *Lj *= eta*mu[j-1];
        *Lj += nu[j-1]*(*Ljm1);
        *Lj += kappa[j-1]*(*Ljm2);                                   
    }   
    
    fx = *Lj;
    fx -= x;
    fx *= (1./eta);        
}  

void mRKC::disp_step_info(Real& t, Real& h, bool accepted)
{
    std::string delta = u8"\u0394";
    string rho =u8"\u03C1";
    
    cout << scientific;
    
    cout<<"Step t = "<<setw(6)<<setprecision(4)<<t<<", "<<delta<<"t = "<<setw(8)<<setprecision(6)<<h
    <<", s = "<<setw(3)<<s<<", m = "<<setw(3)<<m<<", eta = "<<setw(3)<<eta
    <<", "<<rho<<"F = "<<setw(3)<<eigmax_F<<", "<<rho<<"S = "<<setw(3)<<eigmax_S
    <<" and |y_n| = "<<setw(7)<<setprecision(4)<<ynpu->lpNorm<Eigen::Infinity>()<<". ";
    if(accepted)
        cout<<" Accepted ";
    else
        cout<<" Rejected ";
    err_control.disp_info();
    cout<<endl;
}
