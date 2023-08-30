# BEMI
BEMI is a C++ implementation of a boundary element method (BEM) for solving the extracellular-membrane-intracellular (EMI) model in two dimensions. This code has been used for the numerical experiments presented in:

- Rosilho de Souza, G., Pezzuto, S., & Krause, R. (2023). _Boundary Integral Formulation of the Cell-by-Cell Model of Cardiac Electrophysiology_. Submitted to Engineering Analysis with Boundary Elements, [arXiv: 2302.05281](https://arxiv.org/abs/2302.05281).  
- Rosilho de Souza, G., Pezzuto, S., & Krause, R. (2023). _Effect of Gap Junction Distribution, Size, and Shape on the Conduction Velocity in a Cell-by-Cell Model for Electrophysiology_. Functional Imaging and Modeling of the Heart, 117–126, <https://doi.org/10.1007/978-3-031-35302-4_12>.


## Mathematical formulation

### The EMI model
The EMI model (a.k.a the Cell-by-Cell model) for cardiac electrophysiology is a system of reaction-diffusion equations describing the evolution of the electric potential within each cell. The model has a time derivative only on the transmembrane boundary and has algebraic conditions elsewhere, i.e. it reduces to a differential algebraic equation. 

### The BEM method
Employing a boundary integral formulation we recast the EMI model to an ordinary differential equation living only on the transmembrane boundary. Thanks to the boundary element method only the transmembrane boundary is discretized (not the cellular or extra-cellular domains) and therefore the number of degrees of freedom is drastically reduced.


## Install and Run
The easiest way to run the code is by creating a Docker image with the Dockerfile provided here and run the code within a container. Otherwise, one could install the dependencies and compile the code.

### Docker
To create the Docker image, from the root directory of the repository, run:
> docker build -t bemi .  

For running the code, execute:
> docker run --rm -ti -v "$(pwd)/results":/bemi/results bemi ARGS_LIST


### Compile
Before compiling the code install the dependencies: [eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page), [gmsh](https://gmsh.info/), and [fftw3](https://www.fftw.org/). Then, compile[^comp_err] the code running:
> mkdir build && cd build && cmake -S .. -B . && make

And run the code from the ``ARGS_LIST `` directory as
> ./bemi OPTIONS


## Command line arguments
For the full list of arguments use the `--help` option or check out the `ARGS_LIST.md` file, here we just provide an example.

To run a simulation

- with step size $\Delta t=0.01 ms$ and mesh size $\Delta x=10\mu m=1e\text{-}3cm$: `-dt 1e-2 -dG 1e-3`,
- for a duration of $10ms$: `-tend 10`,
- with output in `.vtk` format every 10 time steps: `-vtk true -ofreq 10`,
- on a block of 2x10 cells: `-nx 10 -ny 2`,
- with cells of length $c_l=100\mu m=0.01 cm$ and width $c_w=20\mu m=0.002 cm$: `-cl 1e-2 -cw 2e-3`,
- both leftmost cells are stimulated: `-nic 2`,
- with vertical gap junctions having a wave shape with amplitude $1\mu m=1e\text{-}4 cm$, frequency $2$ and being smooth: `-va 1e-4 -vf 2 -vs true`,
- with horizontal gap junctions having a wave shape with amplitude $2\mu m=2e\text{-}4 cm$, length $30\mu m=0.003 cm$ and being squared: `-ha 2e-4 -hl 3e-3 -hs false`,
- with horizontal gap junctions randomly placed along the horizontal cell side and their permeability being zero with probability 30% : `-halt 2 -hprob 0.3`

execute:

```
./bemi -dt 1e-2 -dG 1e-3 -tend 10 -vtk true -ofreq 10 -nx 10 -ny 2 -cl 1e-2 
-cw 2e-3 -nic 2 -va 1e-4 -vf 2 -vs true -ha 2e-4 -hl 3e-3 -hs false -halt 2 
-hprob 0.3
```

# Acknowledgements

This work was supported by the [European High-Performance Computing Joint Undertaking](https://eurohpc-ju.europa.eu/index_en) EuroHPC under grant agreement No 955495 ([MICROCARD](https://microcard.eu/index-en.html)) co-funded by the Horizon 2020 programme of the European Union (EU) and the [Swiss State Secretariat for Education, Research and Innovation](https://www.sbfi.admin.ch/sbfi/en/home.html).

We are also thankful to Michael Multerer for sharing a starting BEM code.

<p align="center">
  <img src="./docs/img/logo-MICROCARD.png" height="80"/>
  <img src="./docs/img/EuroHPC.jpg" height="80" />
  <img src="./docs/img/WBF_SBFI_EU_Frameworkprogramme_E_RGB_pos_quer.png" height="80" />
</p>


[^comp_err]: If ``cmake`` doesn't  find the ``fftw3`` library, we suggest to compile ``fftw3`` using ``cmake`` (instead of the traditional ``./configure``). Check out the `Dockerfile` to see the commands that we used. 

