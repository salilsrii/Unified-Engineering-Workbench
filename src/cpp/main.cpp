#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
#define CLOSESOCK closesocket
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
#define CLOSESOCK close
#endif

namespace {
struct Params { double L=1.0,A=1e-4,k=10.0,q=10000.0,left=100.0,right=20.0; };
struct Node { double x,T,exact; };
struct Result { int elements; double maxMidError; std::vector<Node> nodes; };
double exactT(const Params&p,double x){return p.left+(p.right-p.left)*x/p.L+(p.q/(2*p.k))*x*(p.L-x);}
Result solve(const Params&p,int n){
 if(n<2||n>100000) throw std::runtime_error("elements must be between 2 and 100000");
 if(!(p.L>0&&p.A>0&&p.k>0)) throw std::runtime_error("L, area and conductivity must be positive");
 const double h=p.L/n, diag=2*p.k*p.A/h, off=-p.k*p.A/h, fe=p.q*p.A*h/2;
 // Each interior node receives the consistent load contribution from both adjacent elements.
 std::vector<double> rhs(n+1,2*fe), lower(n+1,off), d(n+1,diag), upper(n+1,off);
 rhs[0]=p.left; d[0]=1; upper[0]=0; lower[0]=0;
 rhs[n]=p.right; d[n]=1; lower[n]=0; upper[n]=0;
 for(int i=1;i<=n;i++){ double m=lower[i]/d[i-1]; d[i]-=m*upper[i-1]; rhs[i]-=m*rhs[i-1]; }
 std::vector<double> t(n+1); t[n]=rhs[n]/d[n]; for(int i=n-1;i>=0;i--)t[i]=(rhs[i]-upper[i]*t[i+1])/d[i];
 Result r; r.elements=n; r.maxMidError=0;
 for(int i=0;i<=n;i++){double x=i*h;r.nodes.push_back({x,t[i],exactT(p,x)}); if(i<n){double xm=(i+.5)*h, tm=(t[i]+t[i+1])/2;r.maxMidError=std::max(r.maxMidError,std::abs(tm-exactT(p,xm)));}}
 return r;
}
void saveCsv(const Result&r,const std::string&path){std::ofstream o(path);if(!o)throw std::runtime_error("cannot write CSV: "+path);o<<"x_m,temperature_C,exact_temperature_C\n"<<std::setprecision(12);for(auto n:r.nodes)o<<n.x<<','<<n.T<<','<<n.exact<<'\n';}
std::string jsonResult(const Result&r){std::ostringstream o;o<<std::setprecision(10)<<"{\"elements\":"<<r.elements<<",\"max_midpoint_error_C\":"<<r.maxMidError<<",\"nodes\":[";for(size_t i=0;i<r.nodes.size();i++){if(i)o<<',';auto n=r.nodes[i];o<<"{\"x_m\":"<<n.x<<",\"temperature_C\":"<<n.T<<",\"exact_C\":"<<n.exact<<'}';}o<<"]}";return o.str();}
struct NN {double h,yhat,loss,dw,db,dv,dc;};
NN nn(double x,double y,double w,double b,double v,double c){double h=std::tanh(w*x+b),yh=v*h+c,e=yh-y;return{h,yh,.5*e*e,e*v*(1-h*h)*x,e*v*(1-h*h),e*h,e};}
void saveTrainingCsv(const std::string&path){std::ofstream out(path);if(!out)throw std::runtime_error("cannot write training CSV: "+path);out<<"step,loss\n"<<std::setprecision(12);double w=.5,b=0,v=1,c=0;for(int step=0;step<=100;step++){auto z=nn(1,2,w,b,v,c);out<<step<<','<<z.loss<<'\n';w-=.1*z.dw;b-=.1*z.db;v-=.1*z.dv;c-=.1*z.dc;}}
std::string urlPath(std::string s){auto q=s.find('?');if(q!=s.npos)s.resize(q);return s;}
std::string queryValue(const std::string&req,const std::string&key,const std::string&fallback){auto p=req.find(key+"=");if(p==req.npos)return fallback;p+=key.size()+1;auto e=req.find_first_of(" &\r\n",p);return req.substr(p,e==req.npos?e:e-p);}
void respond(socket_t c,const std::string&body,const std::string&type="application/json"){std::ostringstream h;h<<"HTTP/1.1 200 OK\r\nContent-Type: "<<type<<"; charset=utf-8\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: "<<body.size()<<"\r\nConnection: close\r\n\r\n";auto all=h.str()+body;send(c,all.data(),static_cast<int>(all.size()),0);CLOSESOCK(c);}
int serve(int port){
#ifdef _WIN32
 WSADATA wd{};if(WSAStartup(MAKEWORD(2,2),&wd)!=0)throw std::runtime_error("Winsock init failed");
#endif
 socket_t s=socket(AF_INET,SOCK_STREAM,0);if(s<0)throw std::runtime_error("socket creation failed");int yes=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&yes),sizeof(yes));sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);addr.sin_port=htons(static_cast<unsigned short>(port));if(bind(s,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))<0||listen(s,16)<0)throw std::runtime_error("cannot bind local API port");std::cout<<"Workbench API at http://127.0.0.1:"<<port<<" (Ctrl+C to stop)\n";
 for(;;){socket_t c=accept(s,nullptr,nullptr);if(c<0)continue;char b[8192]{};int n=recv(c,b,sizeof(b)-1,0);if(n<=0){CLOSESOCK(c);continue;}std::string req(b,static_cast<size_t>(n));std::istringstream line(req);std::string method,path;line>>method>>path;try{if(path=="/app.js"||path=="/style.css"||path=="/geometry.js"){std::string file=path=="/app.js"?"/app.js":(path=="/style.css"?"/style.css":"/geometry.js");std::ifstream f(std::string("frontend")+file);std::ostringstream body;body<<f.rdbuf();respond(c,body.str(),path=="/style.css"?"text/css":"application/javascript");}else if(path=="/api/sample/stl"||path=="/api/sample/csv"){std::ifstream f(path=="/api/sample/stl"?"data/sample.stl":"data/sample_temperature.csv");std::ostringstream body;body<<f.rdbuf();respond(c,body.str(),"text/plain");}else if(path=="/api/health"){respond(c,"{\"status\":\"ok\",\"backend\":\"C++17\"}");}else if(urlPath(path)=="/api/thermal"){Params p;int ne=std::stoi(queryValue(path,"n","20"));respond(c,jsonResult(solve(p,ne)));}else if(urlPath(path)=="/api/convergence"){Params p;std::ostringstream o;o<<"[";int ns[]={10,20,40,80};for(int i=0;i<4;i++){if(i)o<<',';o<<jsonResult(solve(p,ns[i]));}o<<']';respond(c,o.str());}else if(urlPath(path)=="/api/nn"){auto z=nn(1,2,.5,0,1,0);std::ostringstream o;o<<std::setprecision(12)<<"{\"hidden\":"<<z.h<<",\"prediction\":"<<z.yhat<<",\"loss\":"<<z.loss<<",\"gradients\":{\"w\":"<<z.dw<<",\"b\":"<<z.db<<",\"v\":"<<z.dv<<",\"c\":"<<z.dc<<"}}";respond(c,o.str());}else if(urlPath(path)=="/api/hardware"){respond(c,"{\"mode\":\"SIMULATED\",\"devices\":[{\"name\":\"Mock MCU\",\"voltage_V\":3.3},{\"name\":\"Mock FPGA\",\"voltage_V\":3.3},{\"name\":\"Mock JTAG adapter\",\"connected\":true}],\"telemetry\":\"temperature=31.2 C\",\"recoverable_error\":\"mock timeout; retry available\"}");}else if(urlPath(path)=="/api/import/csv"){respond(c,"{\"error\":\"Use the documented node_id,x_m,y_m,z_m,temperature_C CSV schema; this demo endpoint is read-only\"}");}else{respond(c,"<!doctype html><html><head><meta charset=utf-8><title>PASS Engineering Workbench</title><link rel=stylesheet href=/style.css></head><body><main><header><small>PASS SCIENCE WORKS · R&D INTERNSHIP</small><h1>Unified Engineering Workbench</h1><p>C++17 computational core · local demo</p></header><nav><button data-tab=thermal>Thermal FEA</button><button data-tab=geometry>Geometry</button><button data-tab=performance>Performance</button><button data-tab=hardware>Hardware</button><button data-tab=learning>Backpropagation</button></nav><section id=thermal class=panel><h2>1D steady thermal analysis</h2><p>Linear two-node elements · Dirichlet endpoint temperatures · uniform generation</p><label>Elements <select id=mesh><option>10</option><option selected>20</option><option>40</option><option>80</option></select></label><button id=solve>Run solver</button><button id=conv>Convergence</button><a id=download href=/results.csv download>CSV export (CLI)</a><div id=thermalOut class=out>Ready</div><canvas id=chart width=900 height=240></canvas></section><section id=geometry class='panel hide'><h2>Geometry imports</h2><input id=file type=file accept='.stl,.csv'><p id=fileMsg>ASCII STL and temperature CSV sample data are included. Upload preview validates the supported format.</p><div class=out><div id=geoSummary>Import an ASCII STL mesh or temperature CSV.</div><button id=fitGeo>Fit to view</button><button id=sampleStl>Load sample STL</button><button id=sampleCsv>Load synthetic temperature CSV</button><canvas id=geometryCanvas width=900 height=480 aria-label=\"Interactive 3D geometry and temperature viewer\"></canvas><div id=geoInspect>Drag to rotate. Shift-drag to pan. Scroll to zoom.</div></div></section><section id=performance class='panel hide'><h2>Serial / parallel benchmark</h2><p>Use the C++ CLI: <code>workbench benchmark 10000 80</code>. Runs fixed-seed thermal cases, checks output agreement and reports median timings.</p><pre id=bench>Benchmark results are written to results/csv/benchmark.csv when run locally.</pre></section><section id=hardware class='panel hide'><h2>Embedded system design</h2><p>Clearly labeled mock adapter behavior. No physical device access.</p><button id=hw>Discover mock devices</button><pre id=hwOut></pre><p>See docs/hardware_design.md for voltage, grounding, pin and JTAG ownership notes.</p></section><section id=learning class='panel hide'><h2>Handwritten backpropagation</h2><button id=nn>Calculate forward pass + gradients</button><pre id=nnOut></pre><p>Hand derivation and finite difference check are in docs/backpropagation.md.</p></section><footer>For engineering education only · inspect assumptions before use</footer></main><script src=/app.js></script><script src=/geometry.js></script></body></html>","text/html");} }catch(const std::exception&e){respond(c,std::string("{\"error\":\"")+e.what()+"\"}");}}
}
}
int main(int argc,char**argv){try{std::string cmd=argc>1?argv[1]:"help";if(cmd=="serve")return serve(argc>2?std::stoi(argv[2]):8080);if(cmd=="thermal"){Params p;int n=argc>2?std::stoi(argv[2]):20;auto r=solve(p,n);std::cout<<"max midpoint error (C): "<<std::setprecision(12)<<r.maxMidError<<"\n";if(argc>3)saveCsv(r,argv[3]);else std::cout<<jsonResult(r)<<'\n';return 0;}if(cmd=="convergence"){Params p;for(int n:{10,20,40,80}){auto r=solve(p,n);std::cout<<n<<','<<std::setprecision(12)<<r.maxMidError<<'\n';}return 0;}if(cmd=="backprop"){double x=1,y=2,w=.5,b=0,v=1,c=0,lr=.1;auto z=nn(x,y,w,b,v,c);std::cout<<std::setprecision(12)<<"h="<<z.h<<" yhat="<<z.yhat<<" loss="<<z.loss<<"\ngradients w="<<z.dw<<" b="<<z.db<<" v="<<z.dv<<" c="<<z.dc<<"\nupdated w="<<w-lr*z.dw<<" b="<<b-lr*z.db<<" v="<<v-lr*z.dv<<" c="<<c-lr*z.dc<<'\n';double eps=1e-6;auto loss=[&](double a,double bb,double vv,double cc){return nn(x,y,a,bb,vv,cc).loss;};double fdw=(loss(w+eps,b,v,c)-loss(w-eps,b,v,c))/(2*eps),fdb=(loss(w,b+eps,v,c)-loss(w,b-eps,v,c))/(2*eps),fdv=(loss(w,b,v+eps,c)-loss(w,b,v-eps,c))/(2*eps),fdc=(loss(w,b,v,c+eps)-loss(w,b,v,c-eps))/(2*eps);std::cout<<"finite-difference max abs gradient delta="<<std::max({std::abs(fdw-z.dw),std::abs(fdb-z.db),std::abs(fdv-z.dv),std::abs(fdc-z.dc)})<<'\n';return 0;}if(cmd=="training"){saveTrainingCsv(argc>2?argv[2]:"results/csv/training_loss.csv");return 0;}if(cmd=="benchmark"){int cases=argc>2?std::stoi(argv[2]):20000,n=argc>3?std::stoi(argv[3]):40;std::mt19937 gen(20261101);std::uniform_real_distribution<double>kd(2,25),qd(1000,30000);std::vector<Params> ps(cases);for(auto&p:ps){p.k=kd(gen);p.q=qd(gen);}auto work=[&](int i){volatile double sink=0;auto p=ps[i];for(int rep=0;rep<3;rep++)sink+=solve(p,n).nodes.back().T;return static_cast<double>(sink);};std::vector<double> serial(5),parallel(5),reference(cases);for(int trial=0;trial<5;trial++){auto a=std::chrono::steady_clock::now();for(int i=0;i<cases;i++){double value=work(i);if(trial==0)reference[i]=value;}serial[trial]=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-a).count();}double verifiedDelta=0;for(unsigned workers=1;workers<=std::min(4u,std::max(1u,std::thread::hardware_concurrency()));workers++){for(int trial=0;trial<5;trial++){auto a=std::chrono::steady_clock::now();std::atomic<int> next{0};std::vector<double> values(cases);std::vector<std::thread> ts;for(unsigned w=0;w<workers;w++)ts.emplace_back([&]{for(;;){int i=next.fetch_add(1);if(i>=cases)break;values[i]=work(i);}});for(auto&t:ts)t.join();double maxDelta=0;for(int i=0;i<cases;i++)maxDelta=std::max(maxDelta,std::abs(values[i]-reference[i]));if(maxDelta>1e-8)throw std::runtime_error("serial/parallel result mismatch");verifiedDelta=std::max(verifiedDelta,maxDelta);parallel[trial]=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-a).count();}auto med=[](auto v){std::sort(v.begin(),v.end());return v[v.size()/2];};double s=med(serial),p=med(parallel);std::cout<<workers<<','<<s<<','<<p<<','<<s/p<<','<<(s/p)/workers<<','<<verifiedDelta<<'\n';}return 0;}if(cmd=="hardware"){std::cout<<"SIMULATED: discover MCU, FPGA, JTAG; telemetry 31.2 C; recoverable timeout; retry succeeds (no physical hardware accessed).\n";return 0;}std::cout<<"PASS Unified Workbench\nCommands: serve [port] | thermal [elements] [csv-path] | convergence | benchmark [cases] [elements] | backprop | hardware\n";return 0;}catch(const std::exception&e){std::cerr<<"error: "<<e.what()<<'\n';return 2;}}
