# Helmsolve_tests
Set of tests that call Helmsolve() on various 3D domains with different combinations of boundary conditions. In all cases,  $\lambda=0$ and the forcing term/RHS is

$$
-(c_x^2+c_y^2+c_z^2)~u
$$

where

$$
u={\rm sin}(c_x x){\rm sin}(c_y y){\rm sin}(c_z z)
$$

and

$$
c_x=\frac{2\pi\omega_x}{x_{max}},~~ c_y=\frac{2\pi\omega_y}{y_{max}},~~c_z=\frac{2\pi\omega_z}{z_{max}}
$$

---
---

### Variable coefficients (VCs_ORIG_DDP)
In this example, all Helmsolve variable coefficients are defined via functions, e.g.:
```
        <FUNCTION NAME="d22">
            <E VAR="u" VALUE="1" />
        </FUNCTION>
```
but these definitions are ignored if `<P> skip_varcoeffs = 1 </P>` is also defined in the session xml.

---

**Steps (working in the root of the repository):**

1. Build the `helmsolve_tests` executable using CMake (see [build instructions](../../README.md))
2. Generate the mesh with:
```
./scripts/geo_to_xml.sh ./examples/helmsolve_tests/VCs_ORIG_DDP/cuboid5x5x10.geo
```
(Expects gmsh and NekMesh to be on your path. If they're not, use  ` <-g path_to_gmsh> <-m path_to_NekMesh> `)

1. Run the example with
```
./scripts/run_eg.sh helmsolve_tests VCs_ORIG_DDP
```
The solve runs, with errors of
```
 L infinity error: 2.22135e-07
 L 2 error:        1.31958e-06
 H 1 error:        1.99229e-05
```

Note that d00, d11 and d22 are reported as '`Not set`' because 'skip_varcoeffs' is defined in the session file.

4. Next, in `examples/helmsolve_tests/VCs_ORIG_DDP/helmsolve_tests.xml`, comment out 
```
<P> skip_varcoeffs = 1 </P>
```

 and run the example again.
 
 d00, d11 and d22 are reported as '`Defined via function`' and the solver reports a much higher error:
 ```
 L infinity error: 0.99503
 L 2 error:        5.59017
 H 1 error:        13.3901
 ```

 N.B. Setting d00,d11, d22 as constant factors via parameters works as expected, with errors matching those achieved when no variable coefficients are passed.
