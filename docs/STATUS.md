# Submission status (2026-11-01)

This status is updated only with observed evidence. “Implemented” means code exists, not that it meets every assignment subcriterion.

| Gate / deliverable | Status | Evidence or remaining work |
|---|---|---|
| C++17 backend | Verified locally with Clang 21 | Direct command-line compilation succeeded; CMake itself is unavailable in this environment. Loopback health, thermal/sample APIs, UI assets, and C++ CLI file validators are available. |
| Thermal FEM | Verified for assignment defaults | Midpoint errors: 10=1.25, 20=0.3125, 40=0.078125, 80=0.01953125 °C; each mesh doubling reduces error by 4. CSV and exact nodal comparisons are included. |
| Standalone Windows launch | **Not satisfied / unverified** | Windows build path now statically links the MSVC runtime, copies the binary to `Launch.exe`, and opens the local dashboard on double-click. It still requires CMake/MSVC to build, has not been run on Windows 11, and no `Launch.exe` is included. This is a mandatory assignment gate. |
| Usable UI | Partial | C++ HTTP service serves thermal, geometry, performance, mock hardware and gradient panels. |
| STL / CSV | Sample previews and parser checks verified | Canvas viewer parses ASCII STL triangles and `node_id,x_m,y_m,z_m,temperature_C`, supports rotate/pan/zoom/fit and temperature legend/node hover. C++ CLI validators and browser parser tests cover malformed/non-finite rejection. Sample STL and synthetic CSV were loaded in the browser. User file-picker selection, binary STL, quoted CSV parsing and HTTP file upload remain unverified/unsupported. |
| Benchmark | Verified on this host | Seed 20261101; 500 cases × 3 solves at 20 elements; five trials; 1/2/3/4 worker measurements in `results/csv/benchmark.csv`; output equality enforced at 1e-8. Host: Apple M5, 10 logical CPUs, 16 GiB RAM, Apple Clang 21. Timings are machine/load dependent and this small workload is noisy. |
| Embedded integration | Simulated | Mock discovery/capabilities/telemetry/recoverable timeout. Design notes included. No physical hardware claims. |
| Backpropagation | Implemented; handwritten evidence missing | C++ gradients, update, finite differences and 100-step loss plot are present. Copy the numeric derivation in `docs/backpropagation.md` by hand and add the scanned page(s). |
| Tests | Local checks pass | `tests/smoke.sh` compiles with Clang and checks FEM convergence, gradient tolerance, thermal CSV export, C++ STL/CSV validation, malformed/non-finite CSV rejection, and mock label. `node tests/geometry_test.js` checks browser parser behavior. Bundled samples were exercised in the browser. |
| Technical report PDF (max 6 pages) | Draft created and visually checked (2 pages) | `docs/technical_report.pdf` includes measured results, FEM convergence and training-loss charts, benchmark context, and delivery gaps. Candidate name and final review still need completion. Markdown source is `docs/technical_report.md`. |
| Demo evidence | Partial | Thermal, mock hardware, neural network, sample STL, temperature colors, rotation, and node hover were exercised. See `results/demo_logs/browser_walkthrough.md`. Screenshots were shown in the working chat but are not saved in the submission folder. Save timestamped screenshots or record a 3-5 minute demo before submission. |
| Git history / bundle | AI-authored starter history created; bundle pending | Real commits use the explicit author `Codex AI assistant` and current timestamps, with no earlier history claimed. Export `PASS_Workbench.bundle` after this final documentation commit. Candidate should review and add their own follow-up commit before submission. |
| Ownership statement | Draft prepared; unsigned | `docs/ownership_statement.md` discloses AI-assisted work, synthetic data, mock hardware, and local validation. Candidate must review, revise if needed, and sign it themselves. |
| ZIP package | Not created | Include only after Windows gate and submission contents are reviewed. |

## Immediate shortlist risks

The mandatory standalone launch gate remains unmet. The thermal and backend gates must be validated with actual local build/run evidence. This MVP must not be submitted as if all assignment requirements are complete.
