// Copyright 2022-2023 Horia-Valentin Moroianu 314CA

#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "defines.h"

// Initializeaza dictionarul comenzilor.
void init_lexicon(char lexicon[][LEX_SIZE]);
// Initializeaza datele imaginii salvate in memorie.
void init_file_info(file_info *info);
// Seteaza coordonatele selctiei pentru a cuprinde intreaga imagine.
void set_full_selection(file_info *info);
// Citeste si identifica comanda primita de la stdin si pastreaza in
// variabila 'command' argumentul agesteia.
int read_command(char command[], char lexicon[][LEX_SIZE]);

// Incearca deschiderea fisierului 'file_name' (pentru citire).
FILE *open_input_file(char file_name[]);
// Citeste antetul fisierului deschis.
void read_file_header(FILE *file, file_info *info);
// Verifica formatul imaginii si aloca spatiu in memorie pentru aceasta.
void read_image(FILE *file, file_info *info);
// Citeste valorile pixelilor pentru imaginile cu format pgm.
void read_pgm(FILE *file, file_info info);
// Citeste valorile pixelilor pentru imaginile cu format ppm.
void read_ppm(FILE *file, file_info info);

// Incearca deschiderea fisierului 'file_name' (pentru scriere).
FILE *open_output_file(char file_name[]);
// Scrie antetul fisierului deschis.
void write_file_header(FILE *file, file_info info, char type[]);
// Scrie valorile pixelilor pentru imaginile cu format pgm.
void write_pgm(FILE *file, file_info info, char type[]);
// Scrie valorile pixelilor pentru imaginile cu format ppm.
void write_ppm(FILE *file, file_info info, char type[]);

// Aloca spatiu in memorie in functie de formatul imaginii.
int alloc_pgm(unsigned char ***pgm, int height, int width);
int alloc_ppm(pixel ***ppm, int height, int width);

// Elibereaza matricele din memorie in functie de formatul imaginii.
void free_image(file_info info);
void free_pgm(unsigned char **pgm, int lines);
void free_ppm(pixel **ppm, int lines);

// Foloseste o matrice temporara, in care se pune selectia curenta, rotita
// la 90 de grade, si se modifica in imaginea salvata in memorie.
void rotate_image(file_info *info);

// Verifica si elimina o linie, daca aceasta este comentata (incepe cu '#').
void ignore_comments(FILE *file);

// Extrage in vectorul 'nums' numerele din argumentul comenzii si intoarce
// dimensiunea acestuia sau coduri de eroare daca argumentul este invalid.
int extract_numbers(char arg[], int nums[]);

// Extrage in vectorul 'freq' frecventa valorilor din imagine.
void get_pixel_freq(file_info info, int freq[]);

// Identifica noul tip pentru salvarea fisierului.
void get_saving_type(file_info info, char arg[], char new_type[]);

// Pastreaza in imagine doar selectia curenta, folosind o matrice temporara.
void crop_pgm(file_info *info, int height, int width);
void crop_ppm(file_info *info, int height, int width);

// Identifica indicele nucleului de imagine care trebuie aplicat.
int identify_kernel(char arg[]);
// Salveza in 'ker' nucleul de imagine corespunzator indicelui.
void get_img_kernel(double ker[][3], int id);
// Se parcurge o zona 3x3 in jurul pixelului si se calculeaza
// noua valoare a acestuia, folosind nucleul de imagine.
pixel process_pixel(file_info info, int pixel_x, int pixel_y, double ker[][3]);

// Returneaza '1' daca imaginea are format PGM si '0' in caz contrar.
int image_is_pgm(file_info info);
// Returneaza '1' daca exista o imagine incarcata in memorie
// si '0' si un mesaj de atentionare in caz contrar.
int nothing_loaded(file_info info);
// Interschimba cele doua numere.
void swap_numbers(int *num1, int *num2);
// Restrange valoarea primita in intervalul [min, max] si o returneaza.
double clamp(double value, double min, double max);

#endif
