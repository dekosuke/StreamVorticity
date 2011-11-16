#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

#define X 401 //mesh size x
#define Y 101 //mesh size y

using namespace std;

class Fluid{
    vector<double> _psi;
    vector<double> _omega;
    vector<double> _d_omega_prev;
    vector<double> _d_omega;
    double DT, H, H2, D2, Re, Alp;
     //DT=time delta H=dx(dy) Re - Reynolds number
    int loop;//iteration counter
    int WXS, WXE, WYS, WYE;
public:
    Fluid(){ init(); }
    int to1d(int x, int y){ // (x,y) -> i
        return x*Y+y;
    }
    void init(){
        //memory allocation
        _psi = vector<double>(X*Y, 0);
        _omega = vector<double>(X*Y, 0);
        _d_omega_prev = vector<double>(X*Y, 0);
        _d_omega = vector<double>(X*Y, 0);
        //noise to break symmetry
        for(int i=55;i<65;++i)
            for(int j=40;j<50;++j)
                _omega[to1d(i,j)]=0.001*(i-55)*(j-45);
        //init vars
        H = 0.1;
        DT = 0.001;
        Re = 223;
        Alp = 1.2;
        D2 = 1.0/(H*H);
        H2 = H*H;
        loop=0;
        //init hole
        WXS=45;
        WYS=45;
        WXE=55;
        WYE=55;
    }
    double getPhi(int i, int j){ return _psi[to1d(i,j)]; }
    void setPhi(int i, int j, double v){ _psi[to1d(i,j)]=v; }
    double getOmega(int i, int j){ return _omega[to1d(i,j)]; }
    void setOmega(int i, int j, double v){ _omega[to1d(i,j)]=v; }
    double getDomega(int i, int j){ return _d_omega[to1d(i,j)]; }
    void setDomega(int i, int j, double v){ _d_omega[to1d(i,j)]=v; }
    double getDomegaPrev(int i, int j){ return _d_omega_prev[to1d(i,j)]; }
    void setDomegaPrev(int i, int j, double v){ _d_omega_prev[to1d(i,j)]=v; }
    void update(){ //iteration
        //set omega at boundary and hole
        for(int i=0;i<X;i++){
            setOmega(i,0,0);
            setOmega(i,Y-1,0);
        }
        for(int j=0;j<Y;j++){
            setOmega(0,j,0);
            setOmega(X-1,j,getOmega(X-2,j));
        }
        for(int i=WXS;i<=WXE;i++){
            setOmega(i,WYS,-2*getPhi(i,WYS-1)*D2);
            setOmega(i,WYE,-2*getPhi(i,WYE+1)*D2);
        }
        for(int j=WYS;j<=WYE;j++){
            setOmega(WXS,j,-2*getPhi(WXS-1,j)*D2);
            setOmega(WXE,j,-2*getPhi(WXE+1,j)*D2);
        }
        //set omega at inner region
        for(int i=1;i<X-1;++i){
            for(int j=1;j<Y-1;++j){
                double j1 = 
                     (getPhi(i+1,j)-getPhi(i-1,j))*(getOmega(i,j+1)-getOmega(i,j-1))
                    -(getPhi(i,j+1)-getPhi(i,j-1))*(getOmega(i+1,j)-getOmega(i-1,j)) ;
                double j2 = (getPhi(i+1,j)*(getOmega(i+1,j+1)-getOmega(i+1,j-1)) -
                             getPhi(i-1,j)*(getOmega(i-1,j+1)-getOmega(i-1,j-1)) -
                             getPhi(i,j+1)*(getOmega(i+1,j+1)-getOmega(i-1,j+1)) +
                             getPhi(i,j-1)*(getOmega(i+1,j-1)-getOmega(i-1,j-1)) );
                double j3 = (getPhi(i,j+1)*(getOmega(i+1,j+1)-getOmega(i-1,j+1)) -
                             getPhi(i,j-1)*(getOmega(i+1,j-1)-getOmega(i-1,j-1)) -
                             getPhi(i+1,j)*(getOmega(i+1,j+1)-getOmega(i+1,j-1)) +
                             getPhi(i-1,j)*(getOmega(i-1,j+1)-getOmega(i-1,j-1)) );
                double viscosity = (j1 + j2 + j3)/(3.0*4.0); 
                double advection = ( getOmega(i,j-1)+getOmega(i-1,j)-4.0*getOmega(i,j)+
                                     getOmega(i+1,j)+getOmega(i,j+1) )/Re;
                setDomega(i, j, DT * D2 * (viscosity + advection));
            }
        }
        //set Omega
        if(loop==0){
            for(int i=1;i<X-1;++i){
                for(int j=1;j<Y-1;++j){
                    if(i>WXE || i<WXS || j>WYE || j<WYS)
                        setOmega(i, j, getOmega(i,j)+getDomega(i, j));
                }
            }
        }else{
            for(int i=1;i<X-1;++i){
                for(int j=1;j<Y-1;++j){
                    if(i>WXE || i<WXS || j>WYE || j<WYS)
                        setOmega(i, j, getOmega(i,j) + 1.5*getDomega(i, j)-0.5*getDomegaPrev(i, j));
                }
            }
        }
        //set hole
        for(int i=WXS;i<=WXE;++i){
            for(int j=WYS;j<=WYE;++j){
                setPhi(i, j, 0);
            }
        }
        //phi boundary
        for(int j=0;j<Y;j++){
            setPhi(0,j,1*H*(j-Y/2));
            setPhi(X-1,j,getPhi(X-2,j));
        }
        for(int i=0;i<X-1;i++){
            setPhi(i,0,1*H*(-Y/2));
            setPhi(i,Y-1,1*H*(Y/2));
        }
        //solve phi inner by Gauss-Zeider method
        for(int l=0;l<10;++l){
            for(int i=1;i<X-1;++i){
                for(int j=1;j<Y-1;++j){
                    double R = (H2*getOmega(i,j)+getPhi(i,j-1)+getPhi(i-1,j) +
                                getPhi(i+1,j)+getPhi(i,j+1) )/4.0 - getPhi(i, j);
                    if(i>WXE || i<WXS || j>WYE || j<WYS) setPhi(i, j, R*Alp+getPhi(i,j));
                }
            }
        }
        for(int i=0;i<X;++i){
            for(int j=0;j<Y;++j){
                setDomegaPrev(i, j, getDomega(i, j));
            }
        }
        loop++;
   }
    void Print(){
        cout << "phi:" << endl;
        for(int i=0;i<X;++i){
            for(int j=0;j<Y;++j){
                cout << getPhi(i,j) << " " ;
            }
            cout << endl;
        }
        cout << "omega:" << endl;
        for(int i=0;i<X;++i){
            for(int j=0;j<Y;++j){
                cout << getOmega(i,j) << " " ;
            }
            cout << endl;
        }
    }
};

int main(){
    Fluid fluid;
    for(int l=0;l<1000;++l){
        cout << "iteration : " << l << endl;
        fluid.Print();
        cout << "iter start : " << endl;
        for(int t=0;t<100;++t){
            fluid.update();
        }
    }
    return 0;
}
