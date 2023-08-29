#ifndef CHEBYSHEVMETHODS_H
#define CHEBYSHEVMETHODS_H

#include "MainHeader.h"


namespace ChebyshevMethods
{
    void CoefficientsRKC1(vector<Real>& mu, vector<Real>& nu, vector<Real>& kappa, 
                          vector<Real>& c, vector<Real>& d, unsigned int s, Real eps=0.05);
    
    Real ls_RKC1(unsigned int s, Real eps=0.0);
    
    Real T(Real x, unsigned int s);
    Real dT(Real x, unsigned int s);
    Real ddT(Real x, unsigned int s);
    Real U(Real x, unsigned int s);
    Real dU(Real x, unsigned int s);
    Real ddU(Real x, unsigned int s);
};

#endif /* CHEBYSHEVMETHODS_H */

