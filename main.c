#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>

//il faut creer une structure
//size_t fread(&largeur, sizeof(short), 1, original); ???

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

int extract(const char *source_path, const char *dest_path)
{
    FILE *original = fopen(source_path, "rb");
    if (original == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier\n");//MESSAGE DERREUR
        return -1;
    }
    int nbtraces = 0;
    int histogramme[256];
    short largeur;
    fread(&largeur, sizeof(short), 1, original); 
    short hauteur;
    fread(&hauteur, sizeof(short), 1, original);
    pixel **tabpix = calloc(largeur * sizeof(unsigned char *)); 
    for (int i = 0; i < largeur; i++)
    {
        tabpix[i] = calloc(hauteur * sizeof(unsigned char)); 
    }
    for (int i=0; i<largeur; i++)
    {
        for (int j=0; j<hauteur; j++)
        {
            fread(&tabpix[i][j].couleur, sizeof(unsigned char), 1, original);
            tabpix[i][j].x = i;
            tabpix[i][j].y = j;
        }
    }
    for (int k=0; k<256; k++)
    {
        for (int i=0; i<largeur; i++)
        {
            for (int j=0; j<hauteur; j++)
            {
                if (tabpix[i][j].couleur == histogramme[k])
                {
                    histogramme[k]++;
                    //trouver un moyen de donner aussi la position du pixel (soit dedans soit dehors la boucle)
                }
            }
        }
        if (histogramme[k] <=300 &&  histogramme[k]>=50)
        {
            nbtraces++;
        }
    }
    if (nbtraces == 0)
    {
        printf("Erreur aucune trace\n");
        //MESSAGE DERREUR
        return -1;
    }
    int *trace = calloc(nbtraces * sizeof(int *)); 
    for ( int i = 0; i < nbtraces; i++)
    {
        trace[i].pixels = calloc(histogramme[i] * sizeof(pixel));
        trace[i].nb_pixels = histogramme[i];
    }

    for (int i = 0; i < largeur; i++) //liberation de la memoire
    {
        free(tabpix[i]);
    }
    free(tabpix);
    fclose(original);
}



int extract(const char *source_path, const char *dest_path)
{
    FILE *original = fopen(source_path, "rb");
    if (original == NULL)
    {
        //MESSAGE DERREUR
        return -1;
    }
    int nbtraces = 0;
    int histogramme[256];
    short largeur;
    fread(&largeur, sizeof(short), 1, original);
    short hauteur;
    fread(&hauteur, sizeof(short), 1, original);
    unsigned char **pixel = calloc(largeur * sizeof(unsigned char *));
    for (int i = 0; i < largeur; i++)
    {
        pixel[i] = calloc(hauteur * sizeof(unsigned char));
    }
    for (int i=0; i<largeur; i++)
    {
        for (int j=0; j<hauteur; j++)
        {
            fread(&pixel[i][j], sizeof(unsigned char), 1, original);
        }
    }
    for (int k=0; k<256; k++)
    {
        for (int i=0; i<largeur; i++)
        {
            for (int j=0; j<hauteur; j++)
            {
                if (pixel[i][j] == histogramme[k])
                {
                    histogramme[k]++;
                    //trouver un moyen de donner aussi la position du pixel (soit dedans soit dehors la boucle)
                }
            }
        }
        if (histogramme[k] <=300 &&  histogramme[k]>=50)
        {
            nbtraces++;
        }
    }
    if (nbtraces == 0)
    {
        //MESSAGE DERREUR
        return -1;
    }
    int *trace = calloc(nbtraces * sizeof(int *));
    for ( int i = 0; i < nbtraces; i++)
    {
        trace[i] = 
    }
}

















/*
int extract(const char *source_path, const char *dest_path)
{
    int nbtraces = 0;
    FILE *original = fopen(source_path, "rb"); // ouverture du fichier en lecture
    /*FILE *dest = fopen(dest_path, "w"); // ouverture du fichier en écriture
    const int couleur[256]; // TOUJOURS 256
                        /*const int profondeur = fread(&profondeur, sizeof(int), 1, original); // TOUJOURS 8 oui
    const short largeur = fread(&largeur, sizeof(short), 1, original); // largeur de l'image
    const short hauteur = fread(&hauteur, sizeof(short), 1, original);  // hauteur de l'image

     unsigned char **pixel = malloc(largeur * sizeof(unsigned char *));
    for (int i = 0; i < largeur; i++)
    {
        pixel[i] = malloc(hauteur * sizeof(unsigned char));
    } // tableau de pixels
            // ou sinon faire a 1 dimension et faire pixel[i*largeur+j]
for (int i=0; i<largeur; i++)
        {

            for (int j=0; j<hauteur; j++)
            {
                pixel[i][j]=fread(&pixel[i][j], sizeof(unsigned char), 1, original);
            }
        }
    





    for (int k=0; k<256; k++)
    {
        for (int i=0; i<largeur; i++)
        {

            for (int j=0; j<hauteur; j++)
            {
                pixel[i][j]=fread(&pixel[i][j], sizeof(unsigned char), 1, original); // lecture des pixels
                if (pixel[i][j] == couleur[k])
                {
                    couleur[k]++;
                    /*if (couleur[k] > 300)
                    {
                         goto next_k; // prochain de la boucle couleur
                    }
                }
            }
        }
        if (couleur[k] <=300 &&  couleur[k]>=50) //peut etre enlever le 300
        {
            nbtraces++;
        }
        next_k:
    }
    if (nbtraces == 0)
    {
        //MESSAGE DERREUR
    }
    int trace[nbtraces];

}
//lire fermer et apres verifier // mettre des calloc
*/


