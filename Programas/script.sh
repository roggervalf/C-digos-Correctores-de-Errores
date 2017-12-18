#!/bin/bash

#compila programa
echo "Compilando HR.cpp..."
g++ HR.cpp codigos.cpp -std=c++11 -o HR
echo "Ejecutando programa HR ..."
echo "Creando archivos de texto..."
./HR

nom="HammingCodificacion"
txt="$nom.txt"
echo -e "plot '$txt'" w l | gnuplot -persist
read -n1 -r -p "Presiona cualquier tecla para continuar..." key
txt="HammingDecodificacion.txt"
echo -e "plot '$txt'" w l | gnuplot -persist 
read -n1 -r -p "Presiona cualquier tecla para continuar..." key
txt="ReedMullerCodificacion.txt"
echo -e "plot '$txt'" w l | gnuplot -persist
read -n1 -r -p "Presiona cualquier tecla para continuar..." key
txt="ReedMullerDecodificacion.txt"
echo -e "plot '$txt'" w l | gnuplot -persist

