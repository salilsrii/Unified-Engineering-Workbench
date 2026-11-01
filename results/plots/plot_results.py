import csv
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "results" / "plots"

def read(path):
    with path.open(newline="") as f:
        return list(csv.DictReader(f))

def chart(path, title, xlabel, ylabel, series, logy=False):
    W,H=900,480; L,R,T,B=85,30,65,70; pw,ph=W-L-R,H-T-B
    xs=[p[0] for _,pts,_ in series for p in pts]
    ys=[p[1] for _,pts,_ in series for p in pts]
    x0,x1=min(xs),max(xs); y0,y1=min(ys),max(ys)
    if logy: y0=max(min(ys),1e-15); y1=max(ys)
    if y0==y1:y1=y0+1
    def px(x):return L+(x-x0)/(x1-x0 or 1)*pw
    def py(y):
        if logy:return T+(1-(__import__('math').log(max(y,y0))-__import__('math').log(y0))/(__import__('math').log(y1)-__import__('math').log(y0) or 1))*ph
        return T+(1-(y-y0)/(y1-y0))*ph
    colors=['#17a985','#ed9c42','#6388e8']
    parts=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">', '<rect width="100%" height="100%" fill="#0b1420"/>', f'<text x="{W/2}" y="34" fill="#e6edf4" text-anchor="middle" font-family="Arial" font-size="22">{title}</text>', f'<path d="M{L},{T} V{T+ph} H{L+pw}" fill="none" stroke="#93a8b7"/>']
    for idx,(name,pts,color) in enumerate(series):
        coords=' '.join(f'{px(x):.1f},{py(y):.1f}' for x,y in pts)
        parts.append(f'<polyline points="{coords}" fill="none" stroke="{color or colors[idx%len(colors)]}" stroke-width="3"/>')
        parts.append(f'<text x="{L+idx*210}" y="{H-16}" fill="{color or colors[idx%len(colors)]}" font-family="Arial" font-size="14">{name}</text>')
    parts += [f'<text x="{W/2}" y="{H-42}" fill="#c8d4df" text-anchor="middle" font-family="Arial" font-size="15">{xlabel}</text>', f'<text transform="translate(22 {H/2}) rotate(-90)" fill="#c8d4df" text-anchor="middle" font-family="Arial" font-size="15">{ylabel}</text>', '</svg>']
    path.write_text('\n'.join(parts))

thermal=read(ROOT/'results/csv/thermal_80.csv')
chart(OUT/'thermal_temperature.svg','Thermal FEM: nodal solution vs exact','Position (m)','Temperature (°C)', [('FEM',[(float(r['x_m']),float(r['temperature_C'])) for r in thermal],'#17a985'),('Exact',[(float(r['x_m']),float(r['exact_temperature_C'])) for r in thermal],'#ed9c42')])
conv=[]
for row in read(ROOT/'results/csv/convergence.csv'):
    conv.append((float(row['elements']),float(row['max_midpoint_error_C'])))
chart(OUT/'mesh_convergence.svg','Mesh convergence at element midpoints','Number of elements','Maximum absolute error (°C)', [('FEM error',conv,'#ed9c42')],logy=True)
training=read(ROOT/'results/csv/training_loss.csv')
chart(OUT/'backprop_loss.svg','One-neuron training loss (100 updates)','Training step','Squared-error loss', [('Training loss',[(float(r['step']),float(r['loss'])) for r in training],'#6388e8')])
