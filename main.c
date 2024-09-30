#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int x;
    int y;
    unsigned char couleur;
} pixel;

typedef struct
{
    pixel *pixels; // Tableau dynamique de pixels
    int nb_pixels; // Nombre de pixels dans la trace
} trace;

typedef struct
{
    int x;
    int y;
    unsigned char couleur;
} corner;

void tridecroissant(trace *traces, int nbtraces)
{
    for (int i = 0; i < nbtraces - 1; i++)
    {
        for (int j = 0; j < nbtraces - i - 1; j++)
        {
            if (traces[j].nb_pixels < traces[j + 1].nb_pixels)
            {
                // Swap traces[j] and traces[j + 1]
                trace temp = traces[j];
                traces[j] = traces[j + 1];
                traces[j + 1] = temp;
            }
        }
    }
}

int extract(const char *source_path, const char *dest_path)
{
    int nbtraces = 0;
    FILE *original = fopen(source_path, "rb");
    if (original == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier source");
        return -1;
    }

    short profondeur, largeur, hauteur;
    if (fread(&profondeur, sizeof(short), 1, original) != 1 || fread(&largeur, sizeof(short), 1, original) != 1 || fread(&hauteur, sizeof(short), 1, original) != 1)
    {
        perror("Erreur lors de la lecture des dimensions");
        fclose(original);
        return -1;
    }
     
    printf("%d, %d", largeur, hauteur);
     
    
    
    int nbpix = hauteur * largeur;

    pixel *tabpix = malloc(nbpix * sizeof(pixel));
    if (tabpix == NULL)
    {
        perror("Erreur d'allocation de mémoire pour tabpix");
        fclose(original);
        return -1;
    }

    for (int i = 0; i < nbpix; i++)
    {
        if (fread(&tabpix[i].couleur, sizeof(unsigned char), 1, original) != 1)
        {
            perror("Erreur lors de la lecture des pixels");
            free(tabpix);
            fclose(original);
            return -1;
        }
        tabpix[i].x = i % largeur;
        tabpix[i].y = hauteur - i / largeur;
    }
    
    fclose(original);

    int histogramme[256] = {0};
    for (int i = 0; i < nbpix; i++) // Parcours de l'image couleur
    {
        histogramme[tabpix[i].couleur]++;
    }

    for (int k = 0; k < 256; k++)
    {
        if (histogramme[k] <= 300 && histogramme[k] >= 50)
        {
            nbtraces++;
        }
    }

    trace *traces = malloc(nbtraces * sizeof(trace)); // Tableau de traces
    if (traces == NULL)
    {
        perror("Erreur d'allocation de mémoire pour traces");
        free(tabpix);
        return -1;
    }

    int trace_index = 0;
    for (int k = 0; k < 256; k++)
    {
        if (histogramme[k] <= 300 && histogramme[k] >= 50)
        {
            traces[trace_index].pixels = malloc(histogramme[k] * sizeof(pixel));
            if (traces[trace_index].pixels == NULL)
            {
                perror("Erreur d'allocation de mémoire pour traces[trace_index].pixels");
                for (int j = 0; j < trace_index; j++) free(traces[j].pixels);
                free(traces);
                free(tabpix);
                return -1;
            }
            traces[trace_index].nb_pixels = histogramme[k];
            int pixel_index = 0;
            for (int i = 0; i < nbpix; i++)
            {
                if (tabpix[i].couleur == k)
                {
                    traces[trace_index].pixels[pixel_index].x = tabpix[i].x;
                    traces[trace_index].pixels[pixel_index].y = tabpix[i].y;
                    traces[trace_index].pixels[pixel_index].couleur = tabpix[i].couleur;
                    pixel_index++;
                }
            }
            trace_index++;
        }
    }

    if (nbtraces > 5) // Garder les 5 plus grandes traces
    {
        tridecroissant(traces, nbtraces); // Trier les traces par nombre de pixels en ordre décroissant
        for (int i = 5; i < nbtraces; i++)
        {
            free(traces[i].pixels);
        }
        nbtraces = 5;
    }

    corner *corners = malloc(4 * sizeof(corner)); // Tableau de 4 coins
    if (corners == NULL)
    {
        perror("Erreur d'allocation de mémoire pour corners");
        free(tabpix);
        for (int i = 0; i < nbtraces; i++) free(traces[i].pixels);
        free(traces);
        return -1;
    }

    int corner_index = 0;
    for (int i = 0; i < nbpix; i++) // Iterate over the rows of tabpix
    {
        if (histogramme[tabpix[i].couleur] == 4) // Check if the color count is 4
        {
            corners[corner_index].x = tabpix[i].x;
            corners[corner_index].y = tabpix[i].y;
            corners[corner_index].couleur = tabpix[i].couleur;
            corner_index++;
            if (corner_index >= 4)
            {
                break;
            }
        }
    }

    if (corner_index < 4)
    {
        perror("Erreur lors de la lecture des coins");
        free(tabpix);
        for (int i = 0; i < nbtraces; i++) free(traces[i].pixels);
        free(traces);
        free(corners);
        return -1;
    }

    FILE *destination = fopen(dest_path, "wb");
    if (destination == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier destination");
        free(tabpix);
        for (int i = 0; i < nbtraces; i++) free(traces[i].pixels);
        free(traces);
        free(corners);
        return -1;
    }

    fprintf(destination, "Nombre de traces: %d\n", nbtraces); // Écrire le nombre de traces
    for (int i = 0; i < nbtraces; i++)
    {
        fprintf(destination, "Trace %d: Nombre de pixels: %d\n", i, traces[i].nb_pixels); // Écrire le nombre de pixels dans la trace
        for (int j = 0; j < traces[i].nb_pixels; j++)
        {
            fprintf(destination, "Pixel %d: x = %d, y = %d, couleur = %d\n", j, traces[i].pixels[j].x, traces[i].pixels[j].y, traces[i].pixels[j].couleur); // Écrire les coordonnées et la couleur du pixel
        }
    }

    for (int i = 0; i < 4; i++) // Écrire les 4 coins
    {
        fprintf(destination, "Coin %d: x = %d, y = %d, couleur = %d\n", i, corners[i].x, corners[i].y, corners[i].couleur);
    }
    fclose(destination);

    // Libérer la mémoire allouée
    free(tabpix);
    for (int i = 0; i < nbtraces; i++)
    {
        free(traces[i].pixels);
    }
    free(traces);
    free(corners);

    return 0;
}

int main()
{
    if (extract("/Users/adriensouche/Desktop/projectc1/pixmap2.bin", "/Users/adriensouche/Desktop/projectc1/desttest1.txt") == 0)
    {
        printf("Extraction réussie\n");
    }
    else
    {
        printf("Échec de l'extraction\n");
    }
    return 0;
}
