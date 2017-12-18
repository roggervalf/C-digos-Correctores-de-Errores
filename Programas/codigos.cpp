#include "codigos.h"
#include <iostream>

Hamming::Hamming()
{
}

Hamming::Hamming(int n)
{	
    Iniciar(n);		
}

Hamming::~Hamming()	
{	
}

void Hamming::Iniciar(int n)
{	
    tam_m=n;
    par_cont=0;
    Contador_Paridad();		
}

void Hamming::Contador_Paridad()	
{
    while(true) {
        if(tam_m + par_cont+1<=(1<<par_cont))
        {
            break;      
        }
        par_cont++;
    }
    tam_c=tam_m+par_cont+1;
}

void Hamming::Codificacion(int* a, int* b)	
{
    int i=0,j=0, k=0;
    	
    //Se inicializa los bits de paridad con 0
    b[0] = 0;
    for(i=1 ; i < tam_c ; i++) {
        if((1<<j) == i) {	
            j++;
        }
        else {            
            b[k+j+1] = a[k];
        k++;
        }
    }
    for(i=0 ; i < par_cont ; i++) {
        //Setear los bits de paridad
        int bp=(1<<i);
        ObtenerParidad(b,bp);
    }

    for(i=1 ; i < tam_c ; i++) {
        b[0] ^= b[i];
    }
}

void Hamming::ObtenerParidad(int *b, int lugar) {
        int limite=2*lugar;
        b[lugar]=0;
        
        for(int i=lugar ; i < tam_c ;) {
            if(i<limite){//Verifica
                b[lugar]^=b[i];
                
                i++;
            }
            else{//Ignora
                limite+=lugar<<1;
                //limite+=2*lugar;
                i+=lugar;
            }
        }
    }

int Hamming::Sindrome(int *b) {
    int sindrome=0;
    int paridad;
    for(int j=0 ; j < par_cont ; j++) {
        int lugar=(1<< j);
        paridad = b[lugar];
        int limite=2*lugar;
        for(int i=lugar+1 ; i < tam_c ;) {
            if(i<limite){//Verifica
                paridad^=b[i];
                
                i++;
            }
            else{//Ignora
                //limite+=2*lugar;
                limite+=lugar<<1;
                i+=lugar;
            }
        }
        sindrome+=paridad*lugar;
    }
    return sindrome;
}

void Hamming::Correccion(int *a,int *b) {
    int sindrome;
    sindrome=Sindrome(a);
    if(sindrome != 0) {
        a[sindrome] = !a[sindrome];
    }
    else {
        std::cout<<"No hay error en el mensaje enviado."<<std::endl;
    }

    int p=2;
    for(int i=2,j=0;i<tam_c;i++){
        if(p==(i))
            p=p<<1;
        else{
            
            b[j++]=a[i];
        }
    }
}

ReedMuller::ReedMuller()
{
}

ReedMuller::ReedMuller(int n)
{	
    Iniciar(n);	
}

ReedMuller::~ReedMuller()	
{
}

void ReedMuller::Iniciar(int n)	
{
    tam_m=n;
    tam_c=(1<<(n-1));
}

void ReedMuller::Codificacion(int* a, int* b)	
{
    int i=0,j=0, k=0;

    for(i=0 ; i < tam_c ; i++) {
        b[i] = a[0];
    }
    //Se inicializa los bits de paridad con 0
    for(i=1 ; i < tam_c ; i++) {
        for(j=1;j<tam_m;j++)	
	    b[i]^=a[j]*((i>>(j-1))%2);     
    }
}

void ReedMuller::Correccion(int *a, int* b) {
    int i,j,e,k=0;
    b[0]=a[0];
    for(i=1;i<tam_m;i++)
    {
        e=1<<(i-1);
        k=0;
        for(j=0;j<tam_c;){
            k+=a[j]^a[e+j];
            j++;
            if((j>>(i-1))%2==1)
                j+=e;
            
        }
        if(k>(tam_c>>2))
            b[i]=1;
        else
            b[i]=0;
    }
    int* cod=NULL;
    cod = (int*) malloc(sizeof(int)*tam_c);
    Codificacion(b,cod);
    k=0;
    for(i=0 ; i < tam_c ; i++)
       k+=a[i]^cod[i];
    if(k>(tam_c>>1))
       b[0]=!b[0];
    free(cod);
}
