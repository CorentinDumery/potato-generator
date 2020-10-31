# PotatoGenerator - Potatoes made simple

Potatoïd mesh generator. An initial shape is used (for example a sphere), and every vertex is then translated by a number of perturbations. Perturbations have a direction, a frequency, and an amplitude, all of which will impact the displacement on the mesh. High and low frequencies are combined to generate a realistic shape.

<p align="center">
<img src="https://github.com/CorentinDumery/PotatoGenerator/blob/master/images/potato.gif" />
</p>

## Usage

From the project's directory:
```
git submodule update --init
mkdir build
cd build
cmake ..
make
```


