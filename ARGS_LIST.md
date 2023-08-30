```
./bemi --help
This is the BEMI code for solving the EMI model with the boundary element method (BEM)
Different geometries (cells arrangements) are hardcoded in the executable. Look into src/emi_model/MultiDomain.cpp.
Run the code from the ./build folder as ./bemi OPTIONS, where OPTIONS is a combination of the below.
Results are stored in the ./results folder.

The following options are available:
--- General options:
    -ofile      : name of output file. Default: sol.
    -refsol     : name of the reference solution (if available). Default: ""
                  If available, errors are computed at the end of the simulation.
    -ofreq      : Output frequency. Default: -1
                  - If 0 writes solution only at the end of simulation.
                    In general, used to generate a reference solution.
                  - If >0 writes solution every ofreq time steps,
                  - If -1 never writes the solution.
                    In general, used to generate a solution and just compare it against a reference solution.
    -bin        : Writes solution in binary format. Default: false.
    -vtk        : Writes solution in vtk format. Default: false.
--- Time integration options:
    -tend       : Final time. Default: 1.
    -dt         : Time step size. Default: 1e-2.
                  When running with an adaptive step size, this is the initial step size.
    -rfreq      : Frequency at which the spectral radii are re-estimated. Default: 10
    -verb       : Enables or disables verbosity. Default: true.
    -dtadap     : Enables/disables time step adaptivity. Default: false.
    -atol       : sets the absolute tolerance for error control. Default: 1e-2.
    -rtol       : sets the relative tolerance. Default: rtol=atol.
    -oec        : sets the error controller. Values are 1,2,3 for
                  I=Integral, PI=Proportional I, PPI= Predictive PI controllers
                  respectively. Default: 3.
    -convtest   : If true, performs a time convergence test instead, i.e. runs several simulations and checks errors. Default: false
                  If not provided, a reference solution is computed on the fly.
    -maxpow     : Minimal step size used for the convergence test is dt=tend/2^maxpow. Default: 6
    -minpow     : Maximal step size used for the convergence test is dt=tend/2^minpow. Default: 3
--- EMI model options:
    -ndom       : choose the geometry. A number from 0 to 7. Default: 6.
                 - 0: Two non touching inner domains, smoothed half circles. Outer domain is a circle.
                 - 1: Two non touching inner domains, half circles. Outer domain is a circle.
                 - 2: Two touching inner domains, half circles. Outer domain is a circle.
                 - 3: One inner domain, a circle. Outer domain is a circle.
                 - 4: A spiral of cells. Outer domain is a square.
                 - 5: A cluster of ten cells. Outer domain is an ellipse.
                 - 6: A rectangular cluster of cells. Number of cells, size, shape, etc are specified via command line. See below.
                 - 7: The microcard logo
    -dG         : Target mesh size. Default: 1e-3.
    -Rl         : Gap junctions resistivity. Default: 0.00145.
    -si         : Inner cells electric conductivity.  Default: 3.
    -se         : Outer domain electric conductivity.  Default: 20.
    -Imax       : Stimulus intensity (applied for 1ms).  Default: 120.
--- Additional options only valid for ndom=6:
    -cl         : cells length. Default: 1e-2.
    -cw         : cells width.  Default: 0.2e-2.
    -Rl         : Longitudinal gap junctions resistivity. Default: 0.00145.
    -Rt         : Transversal gap junctions resistivity. Default: 0.00145.
    -nx         : Number of cells in horizontal direction.  Default: 10
    -ny         : Number of cells in vertical direction.  Default: 2
    -va         : Amplitude of vertical gap junctions waves.  Default: 5e-5.
    -vf         : Frequency of vertical gap junctions waves.  Default: 0 (a segment).
    -vs         : Smoothness of vertical gap junctions: waves or squared waves.  Default: true (wave).
    -ha         : Amplitude of horizontal permeble curves (PC).  Default: 0.
    -hl         : Length of horizontal PC.  Default: 20e-4.
    -hs         : Smoothness of horizontal PC: waves or squared waves.  Default: true (wave).
    -op         : Enables/disables the only perpendicular PC permeability. Default: false.
    -hp         : Relative position of horizontal PC. Default: 0.5 (centered).
    -halt       : Alternation of horizontal PC. Default: -1.
                  - -1: No alternation, no PC. The whole horizontal gap junction is a fully permeable segment.
                  -  0: No alternation. The PCs are placed one above the other.
                  -  1: Alternation. The PCs are placed at relative positions hp and 1-hp.
                  -  2: Random. The PCs are randomly placed on the horizontal gap junction.
    -hprob      : Probability at which the horizontal PC becomes impermeable. Default: 0
    -nic        : When ny=2, stimulates 1 or 2 of the leftmost cells. Default: 1
```