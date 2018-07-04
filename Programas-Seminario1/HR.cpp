//g++ HR.cpp codigos.cpp -std=c++11 -o HR
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <ctime>
#include "codigos.h"

using namespace std;

int* CadenaVacia(int l)
{
    int *c=NULL;
    c= (int*) malloc(sizeof(int)*l);
    return c;
}

int* GeneradorCadena(int l)
{
    int *c=NULL;
    c= CadenaVacia(l);
    int n=rand()%(1<<l);
    for(int i=0;i<l;i++){
        c[i]=(n%2);
        n=n>>1;
    }
    return c;
}

void GenerarErrores(int *c,int tam, int e)
{
    int l;
    for(int i=0;i<e;i++){
        l=rand()%tam;
        c[l] = !c[l];
    }
}        

void ImprimirCadena(int* c, int tam){
    for(int i=0;i<tam;i++){
        cout<<c[tam-i-1]<<" ";
    }
    cout<<endl;
}

int main()
{
    string file1="HammingCodificacion.txt";
    string file2="HammingDecodificacion.txt";
    clock_t inicio,fin;
    double tiempo;
    int e;
    ofstream output1,output2;
    output1.open(file1.c_str());
    output2.open(file2.c_str());
    srand(time(NULL));
    int* cad_o=NULL,*cad_r=NULL,*cad_d=NULL;
    int longitud=4;//recomendable
    Hamming* H=new Hamming();
    ReedMuller* R=new ReedMuller();
    for(longitud=3;longitud<2048;longitud++){
        cad_o=GeneradorCadena(longitud);
        //ImprimirCadena(cad_o,longitud);
        H->Iniciar(longitud);
        cad_r = CadenaVacia(H->tam_c);
        
        inicio = clock();

        H->Codificacion(cad_o,cad_r);
        
        //ImprimirCadena(cad_r,H.tam_c);
        fin = clock();
        tiempo = (double) (fin-inicio)/CLOCKS_PER_SEC* 1000.0;
        output1<<longitud<<" "<<tiempo<<endl;
        cad_d = CadenaVacia(H->tam_m);
        
        GenerarErrores(cad_r,H->tam_c,1);
        inicio = clock();

        H->Correccion(cad_r,cad_d);
        
        fin = clock();
        tiempo = (double) (fin-inicio)/CLOCKS_PER_SEC* 1000.0;
        //ImprimirCadena(cad_d,longitud);
        output2<<longitud<<" "<<tiempo<<endl;
        free(cad_o);
        free(cad_r);
        free(cad_d);
        cad_r=NULL;
        cad_o=NULL;
        cad_d=NULL;
    }
    delete H;
    output1.close();
    output2.close();

    file1="ReedMullerCodificacion.txt";
    file2="ReedMullerDecodificacion.txt";
    output1.open(file1.c_str());
    output2.open(file2.c_str());
    for(longitud=4;longitud<25;longitud++){
        cad_o=GeneradorCadena(longitud);
        //ImprimirCadena(cad_o,longitud);
        R->Iniciar(longitud);
        cad_r=CadenaVacia(R->tam_c);
        inicio = clock();
        R->Codificacion(cad_o,cad_r);
        //ImprimirCadena(cad_r,R->tam_c);
        fin = clock();
        tiempo = (double) (fin-inicio)/CLOCKS_PER_SEC* 1000.0;
        output1<<longitud<<" "<<tiempo<<endl;
       
        GenerarErrores(cad_r,R->tam_c,(R->tam_c>>2)-1);
        cad_d = CadenaVacia(R->tam_m);
        
        inicio = clock();
        R->Correccion(cad_r,cad_d);
        
        fin = clock();
        tiempo = (double) (fin-inicio)/CLOCKS_PER_SEC* 1000.0;
        output2<<longitud<<" "<<tiempo<<endl;
        ImprimirCadena(cad_d,R->tam_m);
        free(cad_o);
        free(cad_r);
        free(cad_d);
        cad_r=NULL;
        cad_o=NULL;
        cad_d=NULL;
    }
    delete R;
    output1.close();
    output2.close();

    return 0;	
}
