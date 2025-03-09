// Copyright 2022-2023 Horia-Valentin Moroianu 314CA

#include "functions.h"

void init_lexicon(char lexicon[][LEX_SIZE])
{
	strcpy(lexicon[0], "LOAD");
	strcpy(lexicon[1], "SELECT ALL");
	strcpy(lexicon[2], "SELECT");
	strcpy(lexicon[3], "HISTOGRAM");
	strcpy(lexicon[4], "EQUALIZE");
	strcpy(lexicon[5], "ROTATE");
	strcpy(lexicon[6], "CROP");
	strcpy(lexicon[7], "APPLY");
	strcpy(lexicon[8], "SAVE");
	strcpy(lexicon[9], "EXIT");
}

void init_file_info(file_info *info)
{
	strcpy(info->name, "");
	strcpy(info->type, "");
	info->wd = 0;
	info->ht = 0;
	info->maxval = 0;
	info->pgm = NULL;
	info->ppm = NULL;
	set_full_selection(info);
}

void set_full_selection(file_info *info)
{
	info->x1 = 0;
	info->y1 = 0;
	info->x2 = info->wd;
	info->y2 = info->ht;
}

int read_command(char command[], char lexicon[][LEX_SIZE])
{
	fgets(command, MAX_VAL, stdin);

	// Verifica si elimina '\n' citit de fgets
	if (command[strlen(command) - 1] == '\n')
		command[strlen(command) - 1] = '\0';

	// Parcurge dictionarul petru identificarea comenzii
	for (int i = 0; i <= 9; i++) {
		if (strstr(command, lexicon[i])) {
			// Pastreaza in 'command' doar argumentul comenzii
			char tmp[MAX_VAL];
			strcpy(tmp, command + strlen(lexicon[i]) + 1);
			strcpy(command, tmp);

			return i; // intoarce indicele comenzii identificate
		}
	}
	return INVALID_ARG; // comanda nu a fost identificata
}

FILE *open_input_file(char file_name[])
{
	FILE *file = fopen(file_name, "rb");
	if (!file) {
		printf("Failed to load %s\n", file_name);
		return NULL;
	}
	return file;
}

void read_file_header(FILE *file, file_info *info)
{
	ignore_comments(file);
	fscanf(file, "%s", info->type);
	ignore_comments(file);
	fscanf(file, "%d", &info->wd);
	ignore_comments(file);
	fscanf(file, "%d", &info->ht);
	ignore_comments(file);
	fscanf(file, "%d", &info->maxval);
	ignore_comments(file);
}

void read_image(FILE *file, file_info *info)
{
	fgetc(file);
	if (image_is_pgm(*info)) {
		if (!alloc_pgm(&info->pgm, info->ht, info->wd)) {
			fclose(file);
			exit(EXIT_CODE);
		}
		read_pgm(file, *info);
	} else {
		if (!alloc_ppm(&info->ppm, info->ht, info->wd)) {
			fclose(file);
			exit(EXIT_CODE);
		}
		read_ppm(file, *info);
	}
}

void read_pgm(FILE *file, file_info info)
{
	for (int i = 0; i < info.ht; i++) {
		if (info.type[1] == '5') {
			fread(info.pgm[i], sizeof(unsigned char), info.wd, file);

		} else {
			for (int j = 0; j < info.wd; j++)
				fscanf(file, "%hhu", &info.pgm[i][j]);
		}
	}
}

void read_ppm(FILE *file, file_info info)
{
	for (int i = 0; i < info.ht; i++) {
		if (info.type[1] == '6') {
			fread(info.ppm[i], sizeof(pixel), info.wd, file);

		} else {
			for (int j = 0; j < info.wd; j++) {
				fscanf(file, "%hhu", &info.ppm[i][j].red);
				fscanf(file, "%hhu", &info.ppm[i][j].green);
				fscanf(file, "%hhu", &info.ppm[i][j].blue);
			}
		}
	}
}

FILE *open_output_file(char file_name[])
{
	FILE *file = fopen(file_name, "wb");
	if (!file) {
		printf("Failed to save %s\n", file_name);
		return NULL;
	}
	return file;
}

void write_file_header(FILE *file, file_info info, char type[])
{
	fprintf(file, "%s\n", type);
	fprintf(file, "%d\n", info.wd);
	fprintf(file, "%d\n", info.ht);
	fprintf(file, "%d\n", info.maxval);
}

void write_pgm(FILE *file, file_info info, char type[])
{
	for (int i = 0; i < info.ht; i++) {
		if (type[1] == '5') {
			fwrite(info.pgm[i], sizeof(unsigned char), info.wd, file);

		} else {
			for (int j = 0; j < info.wd; j++)
				fprintf(file, "%d ", info.pgm[i][j]);
			if (i == info.ht - 1)
				fprintf(file, "\n");
		}
	}
}

void write_ppm(FILE *file, file_info info, char type[])
{
	for (int i = 0; i < info.ht; i++) {
		if (type[1] == '6') {
			fwrite(info.ppm[i], sizeof(pixel), info.wd, file);

		} else {
			for (int j = 0; j < info.wd; j++) {
				fprintf(file, "%d ", info.ppm[i][j].red);
				fprintf(file, "%d ", info.ppm[i][j].green);
				fprintf(file, "%d ", info.ppm[i][j].blue);
			}
			if (i == info.ht - 1)
				fprintf(file, "\n");
		}
	}
}

int alloc_pgm(unsigned char ***pgm, int height, int width)
{
	*pgm = (unsigned char **)malloc(height * sizeof(unsigned char *));
	if (!*pgm) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 0;
	}

	for (int i = 0; i < height; i++) {
		(*pgm)[i] = (unsigned char *)malloc(width * sizeof(unsigned char));

		if ((*pgm)[i] == NULL) {
			fprintf(stderr, "Failed to allocate memory\n");
			for (int j = 0; j < i; j++)
				free((*pgm)[j]);

			free(*pgm);
			return 0;
		}
	}
	return 1;
}

int alloc_ppm(pixel ***ppm, int height, int width)
{
	*ppm = (pixel **)malloc(height * sizeof(pixel *));
	if (!*ppm) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 0;
	}

	for (int i = 0; i < height; i++) {
		(*ppm)[i] = (pixel *)malloc(width * sizeof(pixel));

		if ((*ppm)[i] == NULL) {
			fprintf(stderr, "Failed to allocate memory\n");
			for (int j = 0; j < i; j++)
				free((*ppm)[j]);
			free(*ppm);
			return 0;
		}
	}
	return 1;
}

void free_image(file_info info)
{
	if (image_is_pgm(info))
		free_pgm(info.pgm, info.ht);
	else
		free_ppm(info.ppm, info.ht);
}

void free_pgm(unsigned char **pgm, int lines)
{
	for (int i = 0; i < lines; i++)
		free(pgm[i]);
	free(pgm);
}

void free_ppm(pixel **ppm, int lines)
{
	for (int i = 0; i < lines; i++)
		free(ppm[i]);
	free(ppm);
}

void rotate_image(file_info *info)
{
	const int selected_ht = info->x2 - info->x1;
	const int selected_wd = info->y2 - info->y1;

	unsigned char **tmp_pgm = NULL;
	pixel **tmp_ppm = NULL;

	// Aloca spatiu in memorie in functie de formatul imaginii
	if (image_is_pgm(*info)) {
		if (!alloc_pgm(&tmp_pgm, selected_ht, selected_wd)) {
			free_image(*info);
			exit(EXIT_CODE);
		}
	} else {
		if (!alloc_ppm(&tmp_ppm, selected_ht, selected_wd)) {
			free_image(*info);
			exit(EXIT_CODE);
		}
	}

	// Parcurge selectia si salveaza in matricea temporara noile valori
	for (int i = 0; i < selected_wd; i++) {
		for (int j = 0; j < selected_ht; j++) {
			if (image_is_pgm(*info)) {
				unsigned char new_val = info->pgm[info->y1 + i][info->x1 + j];
				// indici pentru intoarcere la 90 de grade:
				tmp_pgm[j][selected_wd - i - 1] = new_val;
			} else {
				pixel new_val = info->ppm[info->y1 + i][info->x1 + j];
				// indici pentru intoarcere la 90 de grade:
				tmp_ppm[j][selected_wd - i - 1] = new_val;
			}
		}
	}

	// Daca imaginea este selectata complet, se actualizeaza
	// informatiile salvate despre aceasta
	if (selected_ht == info->wd && selected_wd == info->ht) {
		free_image(*info);		// elibereaza vechea imagine
		info->pgm = tmp_pgm;	// atribuie imaginea rotita:
		info->ppm = tmp_ppm;	// (NULL pentru celalat format)
		info->ht = selected_ht;
		info->wd = selected_wd;
		set_full_selection(info);
		return;
	}

	// Daca imaginea nu este selctata complet, se parcurge selectia
	// si se salveaza in matrice noile valori ale pixelilor
	for (int i = 0; i < selected_wd; i++) {
		for (int j = 0; j < selected_ht; j++) {
			if (image_is_pgm(*info))
				info->pgm[info->y1 + i][info->x1 + j] = tmp_pgm[i][j];
			else
				info->ppm[info->y1 + i][info->x1 + j] = tmp_ppm[i][j];
		}
	}

	if (image_is_pgm(*info)) // elibereaza din memorie matricea temporara
		free_pgm(tmp_pgm, selected_ht);
	else
		free_ppm(tmp_ppm, selected_ht);
}

void ignore_comments(FILE *file)
{
	char comment_line[MAX_VAL];
	int first_char = fgetc(file);

	if (first_char == '#') {
		fgets(comment_line, MAX_VAL, file);
		ignore_comments(file);
	} else {
		fseek(file, -1, SEEK_CUR);
	}
}

int extract_numbers(char arg[], int nums[])
{
	int nums_size = 0; // dimensiunea vectorului de numere
	char *word = strtok(arg, " ");

	while (word) {
		if (word[0] == '-') // verifica daca numerele sunt negative
			return INVALID_NUMBERS; // numere invalide

		for (int i = 0; i < (int)strlen(word); i++) {
			if (word[i] >= '0' && word[i] <= '9')
				nums[nums_size] = nums[nums_size] * 10 + word[i] - '0';
			else
				return INVALID_ARG; // argument invalid
		}
		nums_size++;
		word = strtok(NULL, " ");
	}
	return nums_size;
}

void get_pixel_freq(file_info info, int freq[])
{
	for (int i = 0; i < info.ht; i++) {
		for (int j = 0; j < info.wd; j++)
			freq[info.pgm[i][j]]++;
	}
}

void get_saving_type(file_info info, char arg[], char new_type[])
{
	// Cauta daca fisierul trebuie salvat in format ascii
	char *save_ascii = strstr(arg, "ascii");
	if (save_ascii) {
		// Elimina parametrul din numele fisierului
		arg[save_ascii - arg] = '\0';

		if (image_is_pgm(info))
			strcpy(new_type, "P2");
		else
			strcpy(new_type, "P3");
	} else {
		if (image_is_pgm(info))
			strcpy(new_type, "P5");
		else
			strcpy(new_type, "P6");
	}
}

void crop_pgm(file_info *info, int height, int width)
{
	unsigned char **tmp;
	if (!alloc_pgm(&tmp, height, width)) {
		free_image(*info);
		exit(EXIT_CODE);
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			tmp[i][j] = info->pgm[info->y1 + i][info->x1 + j];
	}

	free_image(*info);
	info->pgm = tmp;
}

void crop_ppm(file_info *info, int height, int width)
{
	pixel **tmp;
	if (!alloc_ppm(&tmp, height, width)) {
		free_image(*info);
		exit(EXIT_CODE);
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			tmp[i][j] = info->ppm[info->y1 + i][info->x1 + j];
	}

	free_image(*info);
	info->ppm = tmp;
}

int identify_kernel(char arg[])
{
	if (!strcmp(arg, "EDGE"))
		return 0;

	if (!strcmp(arg, "SHARPEN"))
		return 1;

	if (!strcmp(arg, "BLUR"))
		return 2;

	if (!strcmp(arg, "GAUSSIAN_BLUR"))
		return 3;

	return INVALID_ARG;
}

void get_img_kernel(double ker[][3], int id)
{
	double img_ker[4][3][3] = {
		{{-1, -1, -1}, {-1,  8, -1}, {-1, -1, -1}},	// edge
		{{ 0, -1,  0}, {-1,  5, -1}, { 0, -1,  0}},	// sharpen
		{{ 1,  1,  1}, { 1,  1,  1}, { 1,  1,  1}},	// blur incomplet
		{{ 1,  2,  1}, { 2,  4,  2}, { 1,  2,  1}},	// gaussian_blur incomplet
	};

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			img_ker[2][i][j] /= 9;	// blur complet
			img_ker[3][i][j] /= 16;	// gaussian_blur complet
			ker[i][j] = img_ker[id][i][j];
		}
	}
}

pixel process_pixel(file_info info, int pixel_x, int pixel_y, double ker[][3])
{
	double red_sum = 0, green_sum = 0, blue_sum = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			int i_offset = info.y1 + pixel_y - 1 + i;
			int j_offset = info.x1 + pixel_x - 1 + j;

			// Daca pixelul se afla pe marginea imaginii,
			// se returneaza vechea valoare
			if (i_offset < 0 || i_offset >= info.ht ||
				j_offset < 0 || j_offset >= info.wd) {
				return info.ppm[info.y1 + pixel_y][info.x1 + pixel_x];

			} else {
				red_sum += info.ppm[i_offset][j_offset].red * ker[i][j];
				green_sum += info.ppm[i_offset][j_offset].green * ker[i][j];
				blue_sum += info.ppm[i_offset][j_offset].blue * ker[i][j];
			}
		}
	}

	red_sum = clamp(red_sum, MIN_VAL, MAX_VAL);
	green_sum = clamp(green_sum, MIN_VAL, MAX_VAL);
	blue_sum = clamp(blue_sum, MIN_VAL, MAX_VAL);

	pixel new_value;
	new_value.red = round(red_sum);
	new_value.green = round(green_sum);
	new_value.blue = round(blue_sum);
	return new_value;
}

int image_is_pgm(file_info info)
{
	if (info.type[1] == '2' || info.type[1] == '5')
		return 1;
	return 0;
}

int nothing_loaded(file_info info)
{
	if (!strcmp(info.name, "")) {
		printf("No image loaded\n");
		return 1;
	}
	return 0;
}

void swap_numbers(int *num1, int *num2)
{
	int aux = *num1;
	*num1 = *num2;
	*num2 = aux;
}

double clamp(double value, double min, double max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}
