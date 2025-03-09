// Copyright 2022-2023 Horia-Valentin Moroianu 314CA

#include "main.h"
#include "functions.h"

int main(void)
{
	char lexicon[LEX_SIZE][LEX_SIZE], command[MAX_VAL];
	init_lexicon(lexicon);

	file_info info;
	init_file_info(&info);

	int program_running = 1;
	while (program_running) {
		switch (read_command(command, lexicon)) {
		case 0:
			load_command(command, &info);
			break;
		case 1:
			select_all_command(&info);
			break;
		case 2:
			select_command(command, &info);
			break;
		case 3:
			histogram_command(command, info);
			break;
		case 4:
			equalize_command(info);
			break;
		case 5:
			rotate_command(command, &info);
			break;
		case 6:
			crop_command(&info);
			break;
		case 7:
			apply_command(command, info);
			break;
		case 8:
			save_command(command, info);
			break;
		case 9:
			program_running = exit_command(info);
			break;
		default:
			printf("Invalid command\n");
			break;
		}
	}
	return 0;
}

void load_command(char file_name[], file_info *info)
{
	// Daca este deja incarcat un fisier,
	// acesta este eliberat din memorie
	if (strcmp(info->name, ""))
		free_image(*info);

	// Se incearca deschiderea fisierului, iar daca esueaza
	// se reinitializeaza informatiile celui salvat in memorie
	FILE *input_file = open_input_file(file_name);
	if (!input_file) {
		init_file_info(info);
		return;
	}

	// Salveaza datele fisierului
	strcpy(info->name, file_name);
	read_file_header(input_file, info);
	read_image(input_file, info);
	set_full_selection(info);

	fclose(input_file);
	printf("Loaded %s\n", file_name);
}

void select_command(char arg[], file_info *info)
{
	if (nothing_loaded(*info))
		return;

	// Extrage din argumentul comenzii coordonatele selectiei
	int coords[4] = {0}, result = extract_numbers(arg, coords);

	// Verifica daca coordonatele au fost extrase cu succes
	if (result == INVALID_NUMBERS) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (result == INVALID_ARG || result != 4) {
		printf("Invalid command\n");
		return;
	}

	// Verifica daca coordonatele nu depasesc marginea imaginii
	if (coords[0] > info->wd || coords[2] > info->wd ||
		coords[1] > info->ht || coords[3] > info->ht ||
		coords[0] == coords[2] || coords[1] == coords[3]) {
		printf("Invalid set of coordinates\n");
		return;
	}

	// Rearanjeaza ordinea coordonatelor
	if (coords[0] > coords[2])
		swap_numbers(&coords[0], &coords[2]);
	if (coords[1] > coords[3])
		swap_numbers(&coords[1], &coords[3]);

	// Salveaza coordonatele in memorie
	info->x1 = coords[0];
	info->y1 = coords[1];
	info->x2 = coords[2];
	info->y2 = coords[3];
	printf("Selected %d %d %d %d\n", info->x1, info->y1, info->x2, info->y2);
}

void select_all_command(file_info *info)
{
	if (nothing_loaded(*info))
		return;
	set_full_selection(info);
	printf("Selected ALL\n");
}

void histogram_command(char arg[], file_info info)
{
	if (nothing_loaded(info))
		return;

	// Extrage din argumentul comenzii informatiile histogramei
	int nums[2] = {0}, result = extract_numbers(arg, nums);

	// Verifica daca informatiile au fost extrase cu succes
	if (result == INVALID_ARG || result != 2) {
		printf("Invalid command\n");
		return;
	}

	if (!image_is_pgm(info)) {
		printf("Black and white image needed\n");
		return;
	}

	int pixel_freq[MAX_VAL + 1] = {0};
	get_pixel_freq(info, pixel_freq);

	int bin_values[MAX_VAL + 1] = {0};
	int stars = nums[0], bins = nums[1], max_bin = 1;

	// Se parcurge fiecare interval in parte
	// si se calculeaza valoarea binului
	for (int i = 0; i < bins; i++) {
		int lower_end = (MAX_VAL + 1) / bins * i;
		int upper_end = (MAX_VAL + 1) / bins * (i + 1);

		for (int j = lower_end; j < upper_end; j++)
			bin_values[i] += pixel_freq[j];

		if (bin_values[i] > max_bin)
			max_bin = bin_values[i];
	}

	// Se calculeaza nr. necesar de stele
	// si se afiseaza in formatul dorit
	for (int i = 0; i < bins; i++) {
		int star_count = bin_values[i] * stars / max_bin;
		printf("%d\t|\t", star_count);

		for (int j = 0; j < star_count; j++)
			printf("*");
		printf("\n");
	}
}

void equalize_command(file_info info)
{
	if (nothing_loaded(info))
		return;

	if (!image_is_pgm(info)) {
		printf("Black and white image needed\n");
		return;
	}

	int pixel_freq[MAX_VAL + 1] = {0};
	get_pixel_freq(info, pixel_freq);

	// Se calculeaza vectorul de sumele partiale
	for (int i = 1; i < MAX_VAL + 1; i++)
		pixel_freq[i] += pixel_freq[i - 1];

	const double area = MAX_VAL * 1.0 / (info.ht * info.wd);
	double new_pixel_val;

	// Se parcurge toata imaginea si se calculeaza
	// noua valoare a pixelilor folosind formula specifica
	for (int i = 0; i < info.ht; i++) {
		for (int j = 0; j < info.wd; j++) {
			new_pixel_val = area * pixel_freq[info.pgm[i][j]];
			new_pixel_val = clamp(new_pixel_val, MIN_VAL, MAX_VAL);
			info.pgm[i][j] = round(new_pixel_val);
		}
	}
	printf("Equalize done\n");
}

void rotate_command(char arg[], file_info *info)
{
	if (nothing_loaded(*info))
		return;

	const int selected_wd = info->x2 - info->x1;
	const int selected_ht = info->y2 - info->y1;

	// Daca imaginea nu este selcteata complet
	// iar selectia nu este patratica
	if (selected_wd != info->wd &&
		selected_ht != info->ht &&
		selected_wd != selected_ht) {
		printf("The selection must be square\n");
		return;
	}

	// Afla semnul si calculeaza unghiul rotatiei
	int angle_sign = (arg[0] == '-'), degrees = 0;
	for (int i = angle_sign; i < (int)strlen(arg); i++)
		degrees = degrees * 10 + arg[i] - '0';

	if (degrees % 90 != 0 || degrees > 360) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// Calculeaza numarul de rotatii de 90 de grade care trebuie efectuat
	int rotations = angle_sign ? (360 - degrees) / 90 : degrees / 90;
	for (int i = 0; i < rotations; i++)
		rotate_image(info);

	if (angle_sign)
		printf("Rotated -%d\n", degrees);
	else
		printf("Rotated %d\n", degrees);
}

void crop_command(file_info *info)
{
	if (nothing_loaded(*info))
		return;

	const int selected_wd = info->x2 - info->x1;
	const int selected_ht = info->y2 - info->y1;

	if (image_is_pgm(*info))
		crop_pgm(info, selected_ht, selected_wd);
	else
		crop_ppm(info, selected_ht, selected_wd);

	// Actualizeaza informatiile despre fisier
	info->ht = selected_ht;
	info->wd = selected_wd;
	set_full_selection(info);
	printf("Image cropped\n");
}

void apply_command(char arg[], file_info info)
{
	if (nothing_loaded(info))
		return;

	if (!strcmp(arg, "")) {
		printf("Invalid command\n");
		return;
	}

	if (image_is_pgm(info)) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	// Identifica nucleul care trebuie aplicat
	int img_index = identify_kernel(arg);
	if (img_index == INVALID_ARG) {
		printf("APPLY parameter invalid\n");
		return;
	}

	const int selected_ht = info.y2 - info.y1;
	const int selected_wd = info.x2 - info.x1;

	pixel **tmp;
	if (!alloc_ppm(&tmp, selected_ht, selected_wd)) {
		free_image(info);
		exit(EXIT_CODE);
	}

	double ker[3][3];
	get_img_kernel(ker, img_index);

	// Parcurge matricea temporara si aplica nucleul de
	// imagine asupra fiecarui pixel
	for (int i = 0; i < selected_ht; i++) {
		for (int j = 0; j < selected_wd; j++)
			tmp[i][j] = process_pixel(info, j, i, ker);
	}

	// Copiaza in imaginea salvata in memorie noile valori ale pixelilor
	for (int i = 0; i < selected_ht; i++) {
		for (int j = 0; j < selected_wd; j++)
			info.ppm[info.y1 + i][info.x1 + j] = tmp[i][j];
	}

	free_ppm(tmp, selected_ht); // elibereaza din memorie matricea temporara
	printf("APPLY %s done\n", arg);
}

void save_command(char arg[], file_info info)
{
	if (nothing_loaded(info))
		return;

	// Afla tipul imaginii care trebuie salvata
	char new_name[MAX_VAL], new_type[3];
	get_saving_type(info, arg, new_type);
	strcpy(new_name, arg);

	FILE *output_file = open_output_file(new_name);
	if (!output_file)
		return;

	// Scrie in fisierul deschis antetul imaginii
	write_file_header(output_file, info, new_type);

	// In functie de tipul acesteia, imaginea este salvata in fisier
	if (new_type[1] == '2' || new_type[1] == '5')
		write_pgm(output_file, info, new_type);
	else
		write_ppm(output_file, info, new_type);

	fclose(output_file);
	printf("Saved %s\n", new_name);
}

int exit_command(file_info info)
{
	// Daca exista o imagine salvata in memorie, acesta este eliberata
	if (!nothing_loaded(info))
		free_image(info);
	return 0;
}
