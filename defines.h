// Copyright 2022-2023 Horia-Valentin Moroianu 314CA

#ifndef DEFINES
#define DEFINES

#define LEX_SIZE 11			// dimensiunea dictionarului
#define MAX_VAL 255			// valoarea maxima a pixelilor
#define MIN_VAL 0			// valoarea minima a pixelilor
#define EXIT_CODE -1		// codul de iesire in cazul unei erori la alocare
#define INVALID_ARG -2		// cod pentru argumente invalide
#define INVALID_NUMBERS -3	// cod pentru numere invalide

// Structura care retine cele trei valori ale unui pixel color
typedef struct {
	unsigned char red, green, blue;
} pixel;

// Structura care retine informatiile despre fisier
typedef struct {
	char name[MAX_VAL];		// numele fisierului
	char type[3];			// formatul fisierului
	int wd, ht, maxval;		// dimensinile fisierului + valoarea maxima
	int x1, x2, y1, y2;		// coordonatele selectiei curente
	unsigned char **pgm;	// imaginea grayscale
	pixel **ppm;			// imaginea color
} file_info;

#endif
