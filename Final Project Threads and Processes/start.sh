#!/bin/bash

echo "Compilando PoliPapasHilos.c..."
gcc -pthread PoliPapasHilos.c -o hilos -lrt

echo "Compilando PoliPapasProcesos.c..."
gcc -pthread PoliPapasProcesos.c -o procesos -lrt

echo "Ejecutando ambos programas en paralelo..."
./hilos & ./procesos &
gnome-terminal -- bash -c "nano REPORTE_HILOS.txt; exec bash" &
gnome-terminal -- bash -c "nano REPORTE_PROCESOS.txt; exec bash" &
sleep 1
wait

