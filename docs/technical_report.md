# Unified Engineering Workbench — Technical Report

**Candidate:** [fill in] · **Assignment:** PASS Science Works R&D Internship · **Date:** 2026-11-01

## Summary

This MVP combines a C++17 thermal finite-element solver and local HTTP API with a lightweight browser interface. It also includes a deterministic compute benchmark, manual one-neuron backpropagation checks, import samples/validation, and explicitly simulated embedded-device responses. The result is educational and is not validated for engineering design use.

## Architecture

`src/cpp/main.cpp` implements the FEM, benchmark, gradient calculation and loopback service. `frontend/` contains the browser dashboard and a canvas-based ASCII STL/temperature point-cloud viewer with rotate, pan, zoom, fit, legend and node inspection. CMake configuration is provided, but this environment lacks CMake; validation used Clang directly. Windows/MSVC packaging remains unverified.

## Thermal method and verification

The rod uses L=1 m, A=1e-4 m², k=10 W/(m K), q=10000 W/m³, and endpoint temperatures 100°C and 20°C. Each element uses `kA/h [[1,-1],[-1,1]]` and consistent source load `qAh/2 [1,1]`. Endpoint temperatures are prescribed. The exact solution is the assignment's quadratic. The midpoint error is the difference between the FEM linear interpolation and the exact solution at each element midpoint.

| Elements | Maximum absolute midpoint error (°C) |
|---:|---:|
| 10 | 1.25 |
| 20 | 0.3125 |
| 40 | 0.078125 |
| 80 | 0.01953125 |

The error decreases by four per mesh doubling, consistent with the expected second-order behavior for linear interpolation of a quadratic field.

## Parallel benchmark

Command used: `workbench benchmark 500 20`, five trials per serial/parallel configuration, deterministic seed 20261101, 3 solver repeats per independent case. Host: Apple M5, 10 logical CPUs, 16 GiB RAM, Apple Clang 21. In the recorded run, median serial time was 1.30129 ms. The 1/2/3/4 worker median parallel times were 1.13933/0.937416/1.339/0.85775 ms, for speedups of 1.14215/1.38817/0.971839/1.5171 and efficiencies of 1.14215/0.694085/0.323946/0.379275. These short runs are noisy and machine-specific; thread startup overhead makes some worker settings slower. Every parallel case was compared with its serial counterpart at a tolerance of 1e-8, with a measured maximum result delta of 0.

## Neural network

For `h=tanh(wx+b)`, `yhat=vh+c`, and `J=1/2(yhat-y)^2`, the C++ implementation computes all four chain-rule derivatives and one step at learning rate 0.1. Its central finite-difference maximum gradient discrepancy was `1.1301892755e-10` using epsilon `1e-6`. A 100-update loss history is exported by the C++ `training` command and plotted in SVG. The candidate must still supply the required handwritten, scanned derivation.

## Scope limits and submission risks

The hardware adapters are mocks. Binary STL, quoted CSV parsing, and backend file ingestion are unsupported. The Windows standalone `Launch.exe`, Windows 11 verification, saved screenshot/video evidence, Git bundle, and handwritten appendix remain incomplete. An unsigned ownership statement draft is included. See `docs/STATUS.md` before presenting this as a finished submission.
