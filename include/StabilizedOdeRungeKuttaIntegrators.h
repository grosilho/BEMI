#ifndef ODESTABILIZEDINTEGRATORS_H
#define	ODESTABILIZEDINTEGRATORS_H

#include "MainHeader.h"

#include "OdeRungeKuttaIntegrator.h"
#include "MultirateOdeRungeKuttaIntegrator.h"

class mRKC: public MultirateOdeRungeKuttaIntegrator
{
public:
    mRKC(Parameters* param_, MultirateOde* mode_);
    virtual ~mRKC();
    
protected:    
    void step(const Real t, const Real& h);
    void f_eta(Real t, Vector& x, Vector& fx);
    
    void update_n_stages_and_h(Real& h);  
    void write_parameters(Real dt);
    
    virtual void disp_step_info(Real& t, Real& h, bool accepted);
    
protected:
    Vector* integr_add[4];
    Real damping, beta;
    Real eta;
};


#endif	/* ODESTABILIZEDINTEGRATORS_H */