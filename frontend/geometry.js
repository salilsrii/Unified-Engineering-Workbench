(() => {
  const canvas = document.querySelector('#geometryCanvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const summary = document.querySelector('#geoSummary');
  const inspect = document.querySelector('#geoInspect');
  const state = { kind: '', vertices: [], nodes: [], rx: -0.5, ry: 0.65, zoom: 1, panX: 0, panY: 0, drag: null };
  const colors = ['#4fa6dc', '#6ccaa4', '#f0be65', '#ee756e'];

  function fit() {
    const pts = state.kind === 'stl' ? state.vertices : state.nodes.map(n => n.p);
    if (!pts.length) return;
    const lo = [0, 1, 2].map(k => Math.min(...pts.map(p => p[k])));
    const hi = [0, 1, 2].map(k => Math.max(...pts.map(p => p[k])));
    state.center = lo.map((x, k) => (x + hi[k]) / 2);
    state.baseScale = Math.min(canvas.width, canvas.height) * 0.68 / (Math.max(...hi.map((x, k) => x - lo[k])) || 1);
    state.zoom = 1; state.panX = 0; state.panY = 0; draw();
  }
  function project(p) {
    let [x, y, z] = p.map((v, i) => (v - state.center[i]) * state.baseScale * state.zoom);
    const cy = Math.cos(state.ry), sy = Math.sin(state.ry);
    [x, z] = [x * cy - z * sy, x * sy + z * cy];
    const cx = Math.cos(state.rx), sx = Math.sin(state.rx);
    [y, z] = [y * cx - z * sx, y * sx + z * cx];
    return { x: canvas.width / 2 + x + state.panX, y: canvas.height / 2 - y + state.panY, z };
  }
  function temperatureColor(t, min, max) {
    const v = (t - min) / (max - min || 1);
    const hue = 220 * (1 - Math.max(0, Math.min(1, v)));
    return `hsl(${hue} 82% 58%)`;
  }
  function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#0b1420'; ctx.fillRect(0, 0, canvas.width, canvas.height);
    if (!state.kind) return;
    ctx.save();
    ctx.strokeStyle = '#354b5e'; ctx.lineWidth = 1;
    const o = project(state.center);
    [[1,0,0,'#ee756e'], [0,1,0,'#6ccaa4'], [0,0,1,'#4fa6dc']].forEach(([x,y,z,c]) => {
      const q = project(state.center.map((v,i)=>v+[x,y,z][i] / state.baseScale));
      ctx.strokeStyle=c; ctx.beginPath(); ctx.moveTo(o.x,o.y); ctx.lineTo(q.x,q.y); ctx.stroke();
    });
    if (state.kind === 'stl') {
      const faces=[];
      for(let i=0;i<state.vertices.length;i+=3){
        const v=state.vertices.slice(i,i+3), q=v.map(project);
        const ax=v[1][0]-v[0][0], ay=v[1][1]-v[0][1], az=v[1][2]-v[0][2];
        const bx=v[2][0]-v[0][0], by=v[2][1]-v[0][1], bz=v[2][2]-v[0][2];
        const nx=ay*bz-az*by, ny=az*bx-ax*bz, nz=ax*by-ay*bx;
        const shade=Math.max(.25,Math.min(.95,.48+.45*Math.abs(nx*.35+ny*.55+nz*.75)/(Math.hypot(nx,ny,nz)||1)));
        faces.push({q,depth:q.reduce((a,p)=>a+p.z,0)/3,shade});
      }
      faces.sort((a,b)=>a.depth-b.depth).forEach(f=>{
        ctx.beginPath();ctx.moveTo(f.q[0].x,f.q[0].y);f.q.slice(1).forEach(p=>ctx.lineTo(p.x,p.y));ctx.closePath();
        ctx.fillStyle=`hsl(158 48% ${Math.round(f.shade*65)}%)`;ctx.fill();ctx.strokeStyle='#9edcc7';ctx.lineWidth=1;ctx.stroke();
      });
      summary.textContent=`ASCII STL · ${state.vertices.length/3} triangles · drag to rotate · shift-drag to pan · wheel to zoom`;
    } else {
      const temps=state.nodes.map(n=>n.t),min=Math.min(...temps),max=Math.max(...temps);
      state.nodes.map(n=>({n,q:project(n.p)})).sort((a,b)=>a.q.z-b.q.z).forEach(({n,q})=>{
        ctx.beginPath();ctx.arc(q.x,q.y,6,0,Math.PI*2);ctx.fillStyle=temperatureColor(n.t,min,max);ctx.fill();ctx.strokeStyle='#dce9f2';ctx.lineWidth=.8;ctx.stroke();
      });
      const grad=ctx.createLinearGradient(canvas.width-36,45,canvas.width-36,190);
      grad.addColorStop(0,temperatureColor(max,min,max));grad.addColorStop(1,temperatureColor(min,min,max));
      ctx.fillStyle=grad;ctx.fillRect(canvas.width-36,45,12,145);ctx.fillStyle='#e6edf4';ctx.font='12px system-ui';
      ctx.fillText(`${max.toFixed(1)} C`,canvas.width-70,38);ctx.fillText(`${min.toFixed(1)} C`,canvas.width-70,207);
      summary.textContent=`Temperature CSV · ${state.nodes.length} nodes · ${min.toFixed(2)} to ${max.toFixed(2)} °C · drag to rotate · shift-drag to pan · wheel to zoom`;
    }
    ctx.restore();
  }
  function parseCsv(text) {
    const rows=text.trim().split(/\r?\n/).map(r=>r.split(',').map(x=>x.trim()));
    const required=['node_id','x_m','y_m','z_m','temperature_C'];
    const header=rows.shift();
    if(!header||required.some(c=>!header.includes(c))) throw Error(`Expected columns: ${required.join(', ')}`);
    const ix=Object.fromEntries(required.map(c=>[c,header.indexOf(c)]));
    if(!rows.length) throw Error('CSV has no node rows');
    return rows.map((r,i)=>{
      if(r.length!==header.length||r.some(v=>v==='')) throw Error(`Malformed row ${i+2}`);
      const vals=required.map(c=>Number(r[ix[c]]));
      if(vals.some(v=>!Number.isFinite(v))) throw Error(`Non-finite or non-numeric value on row ${i+2}`);
      return {id:vals[0],p:vals.slice(1,4),t:vals[4]};
    });
  }
  function parseStl(text) {
    if(!/^\s*solid\b/i.test(text)||!/\bendsolid\b/i.test(text)) throw Error('Expected ASCII STL beginning with solid and ending with endsolid');
    const vertices=[...text.matchAll(/\bvertex\s+([-+\d.eE]+)\s+([-+\d.eE]+)\s+([-+\d.eE]+)/gi)].map(m=>m.slice(1,4).map(Number));
    if(vertices.length<3||vertices.length%3) throw Error('STL must contain complete vertex triplets');
    if(vertices.some(v=>v.some(x=>!Number.isFinite(x)))) throw Error('STL contains non-finite coordinates');
    return vertices;
  }
  window.GeometryView={
    load(name,text){
      try{
        if(name.toLowerCase().endsWith('.stl')){state.vertices=parseStl(text);state.nodes=[];state.kind='stl';inspect.textContent=`Loaded ${name}: ${state.vertices.length/3} triangles.`;}
        else if(name.toLowerCase().endsWith('.csv')){state.nodes=parseCsv(text);state.vertices=[];state.kind='csv';inspect.textContent='Hover over a node to inspect its id, coordinates, and temperature.';}
        else throw Error('Choose an ASCII STL or CSV file');
        fit();
      }catch(e){state.kind='';draw();summary.textContent='Import failed';inspect.textContent=`Import error: ${e.message}`;}
    }
  };
  async function loadSample(kind){
    try{const response=await fetch(`/api/sample/${kind}`);if(!response.ok)throw Error('Sample file unavailable');window.GeometryView.load(kind==='stl'?'sample.stl':'sample_temperature.csv',await response.text());}
    catch(e){summary.textContent='Sample load failed';inspect.textContent=e.message;}
  }
  document.querySelector('#sampleStl').onclick=()=>loadSample('stl');
  document.querySelector('#sampleCsv').onclick=()=>loadSample('csv');
  document.querySelector('#fitGeo').onclick=fit;
  canvas.addEventListener('contextmenu',e=>e.preventDefault());
  canvas.addEventListener('pointerdown',e=>{canvas.setPointerCapture(e.pointerId);state.drag={x:e.clientX,y:e.clientY,pan:e.shiftKey||e.button===2};});
  canvas.addEventListener('pointermove',e=>{
    if(state.drag){const dx=e.clientX-state.drag.x,dy=e.clientY-state.drag.y;state.drag.x=e.clientX;state.drag.y=e.clientY;if(state.drag.pan){state.panX+=dx;state.panY+=dy;}else{state.ry+=dx*.008;state.rx+=dy*.008;}draw();return;}
    if(state.kind==='csv'){
      const rect=canvas.getBoundingClientRect(),mx=(e.clientX-rect.left)*canvas.width/rect.width,my=(e.clientY-rect.top)*canvas.height/rect.height;
      let best=null,dist=14;
      for(const n of state.nodes){const q=project(n.p),d=Math.hypot(q.x-mx,q.y-my);if(d<dist){best={n,d};dist=d;}}
      inspect.textContent=best?`Node ${best.n.id} · (${best.n.p.map(v=>v.toPrecision(4)).join(', ')}) m · ${best.n.t} °C`:'Hover over a node to inspect its id, coordinates, and temperature.';
    }
  });
  canvas.addEventListener('pointerup',()=>state.drag=null);canvas.addEventListener('pointercancel',()=>state.drag=null);
  canvas.addEventListener('wheel',e=>{e.preventDefault();state.zoom=Math.max(.15,Math.min(12,state.zoom*(e.deltaY<0?1.1:.9)));draw();},{passive:false});
  document.querySelector('#file').onchange=async e=>{
    const file=e.target.files[0]; if(!file)return;
    document.querySelector('#geoSummary').textContent='Reading and validating file…';
    window.GeometryView.load(file.name,await file.text());
  };
})();
