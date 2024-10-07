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

int compare_traces(const void *a, const void *b)
{
    trace *traceA = (trace *)a;
    trace *traceB = (trace *)b;
    return traceB->nb_pixels - traceA->nb_pixels; // Tri décroissant
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
     

    if (100>hauteur || 1000<hauteur) {//checking if hauteur is within  bounds
        perror("Erreur lors de la lecture des dimensions, hauteur non-adequate");
        fclose(original);
        return -1;
    }


    if (100>largeur || 1000<largeur) {//checking if largeur is within  bounds
        perror("Erreur lors de la lecture des dimensions, largeur non-adequate");
        fclose(original);
        return -1;
    }
    
    if(profondeur!=8)
    {
        perror("Erreur lors de la lecture des dimensions, profondeur non-adequate");
        fclose(original);
        return -1;
    }
        
    
    int nbpix = hauteur * largeur;

    pixel *tabpix = malloc(nbpix * sizeof(pixel));
    if (tabpix == NULL)
    {
        perror("Erreur d'allocation de mémoire pour tabpix");
        fclose(original);
        return -1;
    }
    
    int test=0;
    
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
    if (fread(&test, sizeof(unsigned char), 1, original) == 1)
    {
        perror("Trop de pixels");
        free(tabpix);
        fclose(original);
        return -1;
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
        qsort(traces, nbtraces, sizeof(trace), compare_traces); // Trier les traces par nombre de pixels en ordre décroissant
        for (int i = 5; i < nbtraces; i++)
        {
            free(traces[i].pixels);
        }
        nbtraces = 5;
    }
    
    if (nbtraces==0) {// Verifier si il y a au moins une trace
        perror("Erreur, pas de trace");
        free(tabpix);
        for (int i = 0; i < nbtraces; i++) free(traces[i].pixels);
        free(traces);
        return -1;
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
        }
    }

    if (corner_index != 4)
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

    fprintf(destination, "Corners= [\n");
    for (int i = 0; i < 4; i++) // Écrire les 4 coins
    {
        fprintf(destination, "%d, %d;\n",corners[i].x, corners[i].y);
    }
    fprintf(destination, "];\n");
    fprintf(destination,"\n");
    
    
    for (int i = 0; i < nbtraces; i++)
    {
        fprintf(destination, "C%d= [\n", i);
        for (int j = 0; j < traces[i].nb_pixels; j++)
        {
            fprintf(destination, "%d, %d;\n", traces[i].pixels[j].x, traces[i].pixels[j].y); // Écrire les coordonnées  du pixel
        }
        fprintf(destination, "];\n");
        fprintf(destination,"\n");
    }
    
    fprintf(destination, "T ={");
    for (int i = 0; i < nbtraces; i++)
    {
        fprintf(destination, "C%d ", i);
    }
    fprintf(destination, "};");
    
    
    fclose(destination);
    
    fprintf(stdout, "C:");
    fprintf(stdout, "%d\n", corners[0].couleur);
    
    fprintf(stdout, "T:");
    for (int i=0; i<nbtraces; i++) fprintf(stdout, "%d ", traces[i].pixels[i].couleur);
    fprintf(stdout, "\n");
    

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

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Erreur, veuyez bien faire appelle a la partie C\n");
        return 0;
    }

    if (extract("/Users/soren/Desktop/pixmap2.bin", "/Users/soren/Desktop/desttest1.txt") == 0)
    {
        printf("Extraction réussie\n");
    }
    else
    {
        printf("Échec de l'extraction\n");
    }
    return 0;
}
