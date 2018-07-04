#ifndef CODIGOS_H
#define CODIGOS_H
#include <QObject>
#include <cstdlib>

class Hamming
{
    
    int par_cont;
public:
    int tam_c;
    int tam_m;
    Hamming(int n);
    Hamming();	
    ~Hamming();// Destructor
    void Iniciar(int n);
    void Codificacion(int* a, int* p);
    void Correccion(int *a,int *b);
private:
    void Contador_Paridad();
    int Sindrome(int *b);
    void ObtenerParidad(int *b, int lugar);
};

class ReedMuller
{
    public:
    int tam_m;
    int tam_c;
    ReedMuller(int n);
    ReedMuller();	
    ~ReedMuller();// Destructor
    
    void Codificacion(int* a, int* p);
    void Correccion(int *a,int* b);
    void Iniciar(int n);
    private:
};
#endif  // CODIGOS_H
