# PASS Unified Engineering Workbench (MVP)

This is a deadline-focused, educational implementation of selected PASS Science Works R&D internship requirements. The numerical backend is C++17. The local web UI talks to that C++ process over loopback HTTP.

## Current verified scope

- 1D steady conduction FEM with two-node linear elements, uniform volumetric generation, prescribed endpoint temperatures, CSV export and exact-solution comparison.
- Mesh studies at 10, 20, 40 and 80 elements.
- Fixed-seed serial/parallel benchmark command with five timings per setting, up to four workers (subject to reported logical CPU count).
- One-hidden-neuron backpropagation, one update, and central finite-difference gradient check.
- Explicitly simulated MCU/FPGA/JTAG discovery and telemetry; it does not access physical hardware.
- Browser UI for thermal runs and charts, plus CSV/STL upload validation.

## Build and run on macOS/Linux

Requires a C++17 compiler and CMake 3.16+.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/workbench serve 8080
```

Open <http://127.0.0.1:8080>. Keep the process running while using the page.

Useful commands:

```sh
./build/workbench thermal 80 results/csv/thermal_80.csv
./build/workbench convergence
./build/workbench benchmark 20000 40
./build/workbench backprop
./build/workbench training results/csv/training_loss.csv
./build/workbench validate stl data/sample.stl
./build/workbench validate csv data/sample_temperature.csv
./build/workbench hardware
```

Run `tests/smoke.sh` to compile and check FEM convergence, gradient agreement, CSV output and mock-mode labeling. `convergence` prints four rows; when saving to CSV, add the header `elements,max_midpoint_error_C`. Benchmark rows are `workers,serial_median_ms,parallel_median_ms,speedup,efficiency,max_result_delta_C`. Run `python3 results/plots/plot_results.py` after generating the thermal, convergence and training CSVs.

## Windows build (not a verified standalone package)

Install Visual Studio Build Tools with the C++ workload and CMake, then run `build_windows.ps1`. `Launch.bat` builds if needed and starts the local service. This project does **not** currently contain a verified dependency-free `Launch.exe`; do not represent it as satisfying the standalone Windows gate until packaged and tested on Windows 11.

## Thermal formulation

The weak form is assembled with element stiffness `k A / h [[1,-1],[-1,1]]` and consistent load `q A h / 2 [1,1]`. Endpoint temperatures are imposed as Dirichlet conditions. The benchmark and web solver currently use the assignment defaults: L=1 m, A=1e-4 m², k=10 W/(m K), q=10000 W/m³, T(0)=100°C, T(L)=20°C. The reported midpoint error compares the average of adjacent FEM nodal temperatures with the exact quadratic at the element midpoint.

## Imports and example data

`data/sample.stl` is a hand-authored ASCII tetrahedron in metres. `data/sample_temperature.csv` is synthetic, uses metres and degrees Celsius, and is **not** an ANSYS export. The browser includes a simple canvas 3D viewer with rotation, pan, zoom, fit-to-view, temperature legend, and node inspection. C++ CLI validators handle ASCII STL and the documented CSV schema; the browser also validates inputs. Binary STL, quoted CSV parsing, HTTP upload to the backend, and production-grade mesh rendering remain incomplete.

## Project status and honest limitations

See `docs/STATUS.md` for the assignment checklist, known gaps, and evidence. The app is a minimum viable demonstration, not validated engineering software. Hardware behavior is mocked. The Windows executable, physical hardware, and demonstration recording were not verified. No Git history is claimed unless this folder is initialized and commits are made by the candidate. `docs/technical_report.pdf` is a two-page draft; complete the candidate field and review it before submission.

## Ownership and AI use

See `AI-use-log.md`. The candidate should review and understand every file, complete the handwritten appendix themselves, and confirm ownership statements before submission.

The local Git starter history records this AI-assisted implementation with honest current timestamps and the explicit author `Codex AI assistant`; it is not earlier candidate work. The portable repository bundle is `PASS_Workbench.bundle`. Review the project and add a candidate-authored validation commit before submission.
