# AI use log and independent review record

This project was prepared with AI assistance in this Codex session on 2026-11-01. The candidate requested inspection of the assignment and implementation of a minimum viable C++17 workbench under a deadline.

| Use | AI contribution | Candidate review / validation |
|---|---|---|
| Requirements | Extracted assignment requirements from the referenced conversation's cached reading of the 3-page PDF. The PDF attachment metadata confirmed three pages. | Candidate must compare this checklist with the original PDF before submitting. |
| Software | Drafted C++17 thermal FEM, benchmark, backpropagation, mock hardware response, simple local HTTP service, frontend, samples, and build documentation. | Not yet independently reviewed by candidate. Local build and checks are recorded in `docs/STATUS.md` only after they are actually run. |
| Follow-up implementation | Added an ASCII STL and temperature-node canvas viewer, rotate/pan/zoom/fit controls, sample preview buttons, validation checks, and an unsigned ownership-statement draft. | Codex ran the parser checks and exercised bundled sample previews in the local browser. Candidate should independently review and repeat the checks before signing or submitting. |
| Final packaging preparation | Added C++ CLI validation for ASCII STL and finite temperature CSV data, malformed-file smoke checks, and Windows source configuration that statically links the MSVC runtime and opens the UI when launched as `Launch.exe`. | Clang and local parser tests pass. The Windows build path was not compiled or run on Windows; no executable is claimed. |
| Version control | Created timestamped local commits using the explicit author identity `Codex AI assistant`, then exported a Git bundle. | This does not claim prior candidate history; candidate should review and make a follow-up validation commit. |
| Claims | Explicitly marked simulated hardware and unverified Windows launch status; did not claim generated Git history, handwritten work, or demo video. | Candidate must add their own handwritten derivation, demo evidence, and ownership declaration. |

## Prompts and outputs

Prompt: “Help complete the PASS SCIENCE WORKS Unified Engineering Workbench internship assignment due tomorrow at 10:00 AM. First inspect the uploaded 3-page assignment PDF and any existing project files, then assess what is already implemented. Prioritize a truthful, testable minimum viable submission that satisfies the mandatory C++ backend, thermal FEA, and standalone-launch gates. Build or repair the C++17 backend, simple usable frontend, STL and CSV imports, thermal solver with verification/convergence outputs, serial/parallel benchmark, embedded mock adapters, handwritten backpropagation calculations and C++ validation, tests, README/report, AI-use log, demo evidence, and Windows packaging/configuration as feasible. Do not fabricate test results, hardware behaviour, Git history, AI interactions, or a working Windows Launch.exe if it cannot be verified.”

AI output: initial implementation and project artifacts in this directory. Candidate edits, independent validation, and the final submission date are not yet recorded.
