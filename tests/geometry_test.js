const fs = require('fs');
const vm = require('vm');
const assert = require('assert');

const noop = () => {};
const gradient={addColorStop:noop};
const ctx = new Proxy({}, { get: (obj, key) => key in obj ? obj[key] : key==='createLinearGradient'?()=>gradient:noop, set: (obj, key, value) => (obj[key] = value, true) });
const elements = {
  '#geometryCanvas': { width: 900, height: 480, getContext: () => ctx, addEventListener: noop, setPointerCapture: noop },
  '#geoSummary': { textContent: '' }, '#geoInspect': { textContent: '' }, '#fitGeo': { onclick: null },
  '#sampleStl': { onclick: null }, '#sampleCsv': { onclick: null }, '#file': { onchange: null }
};
const sandbox = { document: { querySelector: selector => elements[selector] }, window: {}, console, Math, Number, Error };
vm.runInNewContext(fs.readFileSync('frontend/geometry.js', 'utf8'), sandbox);
const view = sandbox.window.GeometryView;

const stl = fs.readFileSync('data/sample.stl', 'utf8');
view.load('sample.stl', stl);
assert.match(elements['#geoSummary'].textContent, /4 triangles/);
assert.match(elements['#geoInspect'].textContent, /Loaded sample.stl/);

const csv = fs.readFileSync('data/sample_temperature.csv', 'utf8');
view.load('sample_temperature.csv', csv);
assert.match(elements['#geoSummary'].textContent, /5 nodes/);

view.load('bad.csv', 'node_id,x_m,y_m,z_m,temperature_C\n1,0,0,NaN,22');
assert.equal(elements['#geoSummary'].textContent, 'Import failed');
assert.match(elements['#geoInspect'].textContent, /Non-finite/);

view.load('bad.stl', 'solid bad\nvertex 0 0 nope\nendsolid bad');
assert.equal(elements['#geoSummary'].textContent, 'Import failed');
assert.match(elements['#geoInspect'].textContent, /complete vertex triplets/);
console.log('GEOMETRY CHECKS PASS: ASCII STL, finite temperature CSV, malformed and non-finite rejection');
