# Backpropagation calculation sheet

Model: `h=tanh(wx+b)`, `yhat=vh+c`, `J=1/2(yhat-y)^2`.

For error `e=yhat-y`:

```text
dJ/dyhat = e
dJ/dv    = e h
dJ/dc    = e
dJ/dw    = e v (1-h^2) x
dJ/db    = e v (1-h^2)
```

With x=1, y=2, w=0.5, b=0, v=1, c=0, learning rate 0.1:

```text
h = tanh(0.5) = 0.46211715726
yhat = v*h+c = 0.46211715726
e = yhat-y = -1.53788284274
J = 0.5*e^2 = 1.182541819
1-h^2 = 0.78644773297

dJ/dw = e*v*(1-h^2)*x = -1.20946447524
dJ/db = e*v*(1-h^2)   = -1.20946447524
dJ/dv = e*h           = -0.710682047486
dJ/dc = e             = -1.53788284274

w_new = 0.620946447524
b_new = 0.120946447524
v_new = 1.07106820475
c_new = 0.153788284274
```

Run `workbench backprop` for full precision values, the four gradients, one gradient descent update, and the maximum discrepancy against central finite differences (epsilon 1e-6). The update is `parameter_new = parameter_old - 0.1 * gradient`. Copy the derivation above by hand onto the required one or two pages and add a photo/scan; this typed version is not a substitute for handwritten evidence.

**Handwritten appendix still required:** the candidate must reproduce the forward pass, chain-rule steps, and update by hand on at most two pages, then scan/photo it for the final submission. This typed derivation is not represented as handwritten evidence.
