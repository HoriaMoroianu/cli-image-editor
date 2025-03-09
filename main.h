// Copyright 2022-2023 Horia-Valentin Moroianu 314CA

#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "defines.h"

// Deschide fisierul 'file_name' si citeste antetul imaginii, urmand sa salveze
// in memorie informatiile despre aceasta in functie de formatul citit.
void load_command(char file_name[], file_info * info);

// Primeste un set de coordonate prin intermediul argumentului, le verifica
// si le salveaza in memorie in ordinea corespunzatoare.
void select_command(char arg[], file_info *info);

// Actualizeaza coordonatele de selctie salvate in memorie,
// astfel incat ele sa cuprinda toata imaginea.
void select_all_command(file_info *info);

// Calculeaza frecveta fiecarui bin si numarul de stele necesar in reprezentare,
// dupa care afiseaza histograma in formatul cerut.
void histogram_command(char arg[], file_info info);

// Recalculeaza valorile pixelilor folosind formula specifica
void equalize_command(file_info info);

// Roteste selectia curenta cu 90 de grade de atatea ori cat este necesar
// pentru a ajunge in pozitia dorita.
void rotate_command(char arg[], file_info *info);

// Pastreaza valorile pixelilor aflati in interiorul selectiei intr-o
// matrice temporara pe care ulterior o ataseaza la imaginea salvata in memorie.
void crop_command(file_info *info);

// Aloca o matrice temporara, de dimensiunea selectiei, asupra careia
// se aplica nucleul de imagine. La finalul calculeleor, se copiaza
// valorile din aceasta in imaginea salvata in memorie.
void apply_command(char arg[], file_info info);

// Obtine din argumentul comenzii noul format pentru imaginea din memorie.
// Folosind noul format, scrie in fisier informatiile despre imagine.
void save_command(char arg[], file_info info);

// Elibereaza din memorie imaginea salvata, daca aceasta exista, si
// intoarce codul de inchidere al programului.
int exit_command(file_info info);

#endif
