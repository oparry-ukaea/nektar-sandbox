# nektar-sandbox
Sandbox for testing Nektar-based apps

## Build
To build app/test executables:
```
build_dir="./builds/release" && mkdir -p "$build_dir"
cmake -DNektar++_DIR=<path_to_Nektar_install> -B "$build_dir"
cmake --build "$build_dir" && cmake --install "$build_dir"
```
(`build_dir` can be set to something else, but the run script looks for executables in `./builds/release` by default)


---
## Examples
[Helmsolve_tests](examples/helmsolve_tests/README.md)