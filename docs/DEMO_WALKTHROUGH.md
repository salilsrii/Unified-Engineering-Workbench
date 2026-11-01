# Demo walkthrough checklist

Observed in the local browser on 2026-11-01 (India time):

1. Open the local workbench at `http://127.0.0.1:8080` while `workbench serve 8080` is running.
2. On Thermal FEA, keep 20 elements selected and click **Run solver**. The UI reports a maximum midpoint error of `3.12500e-1 °C` and plots FEM/exact curves.
3. Click **Convergence** to show the 10/20/40/80 element errors.
4. On Hardware, click **Discover mock devices**. Point out the `SIMULATED` label, three mock devices, telemetry, and recoverable timeout.
5. On Backpropagation, click **Calculate forward pass + gradients**. Explain `h`, prediction, loss and the four gradients; use `workbench backprop` for the finite-difference check and update.
6. On Geometry, click **Load sample STL** to show the tetrahedron. Drag to rotate, shift-drag to pan, scroll to zoom, and click **Fit to view**. Click **Load synthetic temperature CSV** to show colored nodes and the temperature legend; hover a node to inspect it. The browser file picker wasn't exercised in the development session, so rehearse that path before recording.

The 2026-11-01 browser interaction verified thermal, mock-hardware, backpropagation, sample STL and synthetic CSV panels. Parser behavior was also checked with Node tests. Screenshots were visible in the development conversation but are not stored here as submission artifacts. Capture the walkthrough with the candidate's identity/date visible or record the requested short demo before submitting.
