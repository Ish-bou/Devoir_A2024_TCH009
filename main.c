/*  Programme d'analyse (simple) de trace GPS.
	Auteur: David Marche
	Date : Septembre 2013

	Ce programme permet de lire un fichier de trace GPS et de realiser les calculs d’analyse suivants sur les données
	récupérées:
		- Distance totale parcourue
		- Ascension totale realisee
		- Altitude maximum/minimum atteinte
		- Pente maximale du parcours

	Supposition de simplification du probleme:
		- La trace se passe dans une seule journee. Calcul de temps seulement sur Heure,minute,seconde.
		- Le fichier de points doit se trouver dans le repertoire du projet

*/

#define _CRT_SECURE_NO_WARNINGS

// Librairies standard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Librairies du projet
#include "config.h"
#include "test.h"
#include "menu.h"
#include "matrac_lib.h"
#include "menu.h"

/* ---------------------------------------------------------------------------------------------------------------------
	Fonction: charger
	Auteur:
	Description:
		Cette fonction saisi un nom de fichier de donnees GPS, lit et place toutes les donnees dans des tableaux
		statiques fournis en parametres. Le format du fichier est le suivant :
		No,Latitude,Longitude,Altitude,Date,Time
		1,49.192273,-123.178443,-44.6,2013/06/21,11:02:00
		2,49.192272,-123.178318,-44.6,2013/06/21,11:02:03
		3,49.192184,-123.178061,-41.7,2013/06/21,11:02:10
		...

	Parametres:
		tab_latitudes	    tableau a remplir avec les valeurs de latitude
		tab_longitudes		tableau a remplir avec les valeurs de longitude
		tab_altitudes		tableau a remplir avec les valeurs d'altitude
		taille_max		    nombre maximum de valeurs pouvant etre chargees dans les tableaux

	Valeur de retour:
		Le nombre de points lus, 0 si aucun, -1 si probleme lors de la lecture du fichier,
	Note:
		- Les tableaux donnees en parametre sont modifies.
	Cas particuliers:
		- S'il y a eu un problème lors de la lecture, la fonction renvoie -1 sinon elle retourne le nombre de points lus;
*/
int charger(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille_max);

/* ---------------------------------------------------------------------------------------------------------------------
	Fonction: afficher
	Auteur:
	Description:
		Cette fonction affiche toutes les donnees de la trace presente en memoire avec la premiere ligne d'en-tete
		suivante:
			"index      lattitude     longitude   altitude"
	Parametres:
		tab_latitudes		tableau des valeurs de latitude
		tab_longitudes		tableau des valeurs de longitude
		tab_altitudes		tableau des valeurs d'altitude
		taille			    nombre de valeurs valides presentes dans les tableaux
	Valeur de retour:
		Aucune.
	Cas particuliers:
		- Quand la taille est nulle ou negative, seule la ligne d'en-tete est affichee.
*/
void afficher(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille);

/* ---------------------------------------------------------------------------------------------------------------------
	Fonction: analyser
	Auteur:
	Description:
		Cette fonction affiche l'analyse de la trace. Elle affiche la distance totale parcourue, l'ascension effectuee,
		l'altidude maximale et minimale.
	Parametres:
		tab_latitudes		tableau des valeurs de latitude
		tab_longitudes		tableau des valeurs de longitude
		tab_altitudes		tableau des valeurs d'altitude
		taille			    nombre de valeurs presentes dans les tableaux
	Valeur de retour:
		Aucune.
	Cas particulier:
		Si la valeur de la taille est nulle ou negative, les valeurs affichees seront vides.

*/
void analyser(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille);

/* ---------------------------------------------------------------------------------------------------------------------
	Fonction: calculer
	Auteur:
	Description:
		Cette fonction lit la taille de la fenetre pour calculer la pente maximale de la trace et affiche le resultat.

	Parametres:
		tab_latitudes		tableau des valeurs de latitude
		tab_longitudes		tableau des valeurs de longitude
		tab_altitudes		tableau des valeurs d'altitude
		taille			    nombre de valeurs presentes dans les tableaux
	Valeur de retour:
		aucune
*/
void calculer(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille);

// Programme principal
int main(void)
{
	double latitudes[NB_MAX_POINT];
	double altitudes[NB_MAX_POINT];
	double longitudes[NB_MAX_POINT];
	int nb_points=0;
	int choix;

	do {
		choix = afficher_menu();

		switch (choix) {
			case CHARGER:
				nb_points = charger(latitudes, longitudes, altitudes, NB_MAX_POINT);
				break;

			case AFFICHER:
				afficher(latitudes, longitudes, altitudes, nb_points);
				break;

			case ANALYSER:
				analyser(latitudes, longitudes, altitudes, nb_points);
				break;

			case CALCULER:
				calculer(latitudes, longitudes, altitudes, nb_points);
				break;

			case QUITTER:
				break;

			default:
				printf("Choix invalide.\n");
		}
	} while (choix != QUITTER);

	return EXIT_SUCCESS;
}

void afficher(double tab_latitudes[], double tab_longitudes[],
			  double tab_altitudes[], int taille)
{
	printf("%5s %12s %12s %10s\n",
		   "index", "latitude", "longitude", "altitude");

	for (int i = 0; i < taille; i++) {
		printf("%5d %12.6f %12.6f %10.2f\n",
			   i, tab_latitudes[i], tab_longitudes[i], tab_altitudes[i]);
	}
}
int charger(double tab_latitudes[], double tab_longitudes[],
			double tab_altitudes[], int taille_max)
{
	char nom_fichier[LONGUEUR_MAX_NOM_FICHIER];

	printf("Nom du fichier GPS : ");
	if (scanf("%99s", nom_fichier) != 1) {
		return -1;
	}

	return lire_donnees_gps(nom_fichier, tab_latitudes, tab_longitudes,
						  tab_altitudes, taille_max);
}
void calculer(double tab_latitudes[], double tab_longitudes[],
			  double tab_altitudes[], int taille) {
	int taille_fenetre;

	printf("Taille de la fenetre : ");
	if (scanf("%d", &taille_fenetre) != 1) {
		int caractere = getchar();
		while (caractere != '\n' && caractere != EOF) {
			caractere = getchar();
		}
		printf("Entre un nombre entier.\n");
		return;
	}

	double pente = pente_maximale(tab_latitudes, tab_longitudes,
								 tab_altitudes, taille, taille_fenetre);

	printf("Pente maximale : %.3f %%\n", pente);
}

void analyser(double tab_latitudes[], double tab_longitudes[],
			  double tab_altitudes[], int taille)
{
	if (taille <= 0) {
		printf("Distance totale (km) :\n");
		printf("Ascension totale (m) :\n");
		printf("Altitude minimale (m) :\n");
		printf("Altitude maximale (m) :\n");
		return;
	}

	double minimum;
	double maximum;

	double distance = distance_totale(tab_latitudes, tab_longitudes,
									  tab_altitudes, taille, 0, taille - 1);
	double montee = ascension(tab_altitudes, taille);

	minimum_tableau(tab_altitudes, taille, &minimum);
	maximum_tableau(tab_altitudes, taille, &maximum);

	printf("Distance totale : %.2f km\n", distance);
	printf("Ascension totale : %.1f m\n", montee);
	printf("Altitude minimale : %.1f m\n", minimum);
	printf("Altitude maximale : %.1f m\n", maximum);
}