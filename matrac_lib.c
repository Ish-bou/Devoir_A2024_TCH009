/*
Auteur(s):
Date:

Ce module contient les implementations des fonctions d'analyse de traces GPS

*/

#define _CRT_SECURE_NO_WARNINGS

// Librairies standard
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "matrac_lib.h"

int lire_donnees_gps(char nom_fichier[],
	                 double tab_latitudes[],
	                 double tab_longitudes[],
	                 double tab_altitudes[],
	                 int taille_max) {
	int nb_point = 0;						            // nombre de points trouves
	char ligne[LONGUEUR_MAX_LIGNE];			            // chaine de caractere contenant la balise
	FILE* fp;								            // pointeur au fichier a lire
	int no, an, mois, jour, heure, minute, seconde;		// informations lues du fichier
	double latitude, longitude, altitude;

	// ouverture du fichier et verification du succes de l'ouverture
	fp = fopen(nom_fichier, "r"); // en mode lecture seulement

	if (fp == NULL) {
		nb_point = -1;
	} else {
		// sauter l'en-tete
		if (fgets(ligne, LONGUEUR_MAX_LIGNE - 1, fp) == NULL) {
			nb_point = -1;

			// continuer la lecture si fichier n'est pas vide
		} else {

			// boucle de lecture principale de chaque point (un par ligne)
			while ((fgets(ligne, LONGUEUR_MAX_LIGNE - 1, fp) != NULL) && (nb_point < taille_max)) {
				// lecture d'un point et identification des 10 champs attendus:
				// No,Latitude,Longitude,Altitude,Date(a/m/j),Time(h:m:s)
				if (sscanf(ligne, "%d,%lf,%lf,%lf,%d/%d/%d,%d:%d:%d", &no,
				                                                      &latitude,
				                                                      &longitude,
				                                                      &altitude,
				                                                      &an,
				                                                      &mois,
				                                                      &jour,
				                                                      &heure,
				                                                      &minute,
				                                                      &seconde) == 10){

					//si decodage correct,mise en memoire de la latitude, longitude,altitude
					tab_latitudes[nb_point] = latitude;
					tab_longitudes[nb_point] = longitude;
					tab_altitudes[nb_point] = altitude;

					// incrementer le nombre de points correctement lus
					nb_point++;
				}
			}
		}
		// fermeture du fichier
		fclose(fp);
	}

	if (nb_point >= 0) {
		printf("\n...%d points lus\n", nb_point);
	}else {
		printf("\n...probleme de lecture du fichier\n");
	}
	//renvoi du nombre de points trouves
	return nb_point;

}

// implementations des fonctions de matrac_lib (c.f. prototype et doc dans matrac_lib.h)
double convertir_en_radian(double angle) {
	return PI *angle/180;}        //retourne la valeur en rad

double distance_entre_2_points(double latitude1,
							   double longitude1,
							   double latitude2,
							   double longitude2,
							   double altidute1,
							   double altidute2) {
	double latitude1_rad=convertir_en_radian(latitude1);			//convertis les valeurs en rad
	double longitude1_rad=convertir_en_radian(longitude1);
	double latitude2_rad=convertir_en_radian(latitude2);
	double longitude2_rad=convertir_en_radian(longitude2);
	double d;
	double dtotal;
	d=RAYON_TERRE*2.0*asin(sqrt(pow(sin((latitude1_rad-latitude2_rad)/2),2)+cos(latitude1_rad)*cos(latitude2_rad)*pow(sin((longitude1_rad-longitude2_rad)/2),2))); //calcul de l'annexe
	return sqrt(pow(d,2)+pow((altidute2-altidute1)/1000.0,2));} // calcul de l'annexe


void afficher_tableau(double tableau[], int nb_valeurs) {
	int i;
	for (i=0; i<nb_valeurs; i++) {
		printf("%lf\n", tableau[i]);}}											// affiche une valeur par ligne

double distance_totale(double tab_latitudes[],
					   double tab_longitudes[],
					   double tab_altitudes[],
					   int taille,
					   int debut,
					   int fin) {
	int i;
	double distance_total=0.0;
	if (debut>fin||fin>=taille||debut>=taille||taille<=0||debut<0||fin<0) {
		// toutes les condition pour lesquelles la distances vaut 0
		return distance_total;
		/* retourne 0*/
	} else {
		for (i=debut; i<fin; i++) {												// commence au debut et arrete avant la fin car dans la boucle on prend toujours le point apres l'indice
			distance_total= distance_total + distance_entre_2_points(			// additione chaque distance entre 2 points pour trouver les distances totales
				tab_latitudes[i],												//latitude du point actuel
				tab_longitudes[i],												// longitude du point actuel
				tab_latitudes[i + 1],											//latitude du prochain point
				tab_longitudes[i + 1],											//longitude du prochain point
				tab_altitudes[i],												// altitude du point actuel
				tab_altitudes[i + 1]);
		}
	}		// altitude du prochain point
	return distance_total;} // retourne la distance totale


int minimum_tableau(double tableau[], int taille, double* minimum) {
	int i;
	if (taille<=0) {//si taille est nulle ou negative
		*minimum=0.0;																//minimum par defaut
		return 0;}																	//retourne 0 si le resultat est invalide
	*minimum=tableau[0];															// si la taille >0 le minimum par defaut est la premiere case
	for (i=1; i<taille; i++) {														// on commence a l'indice 1 car le 0 est deja pris
		if (tableau[i]<*minimum) {													// si on trouve une valeur plus basse
			*minimum=tableau[i];
		}
	}																				// on change la valeur du minimum
	return 1;}																		// on retourne 1 si le resultat est valide

int maximum_tableau(double tableau[], int taille, double* maximum) {
	int i;
	if (taille<=0) {																//si taille est nulle ou negative
		*maximum=0.0;																//maximum par defaut
		return 0;}																	//retourne 0 si le resultat est invalide
	*maximum=tableau[0];															// si la taille >0 le maximum par defaut est la premiere case
	for (i=1; i<taille; i++) {														// on commence a l'indice 1 car le 0 est deja pris
		if (tableau[i]>*maximum) {													// si on trouve une valeur plus grande
			*maximum=tableau[i];
		}
	}																				// on change la valeur du maximum
	return 1;}																		// on retourne 1 si le resultat est valide

double ascension(double tab_altitudes[], int taille) {
	int i;
	double ascension_total=0.0;														//valeur ascension par defaut
	if (taille<=0) {																//si la taille est nulle ou negative on renvoit 0
		return 0;}
	for (i=0; i<taille-1; i++) {													//on commence a l'indice 0 et on arrete a taille -1 car on utilise la valeur suivante dans la boucle for
		if (tab_altitudes[i]<tab_altitudes[i+1]) {									//on verifie si l'altutide est croissante
			ascension_total= ascension_total +(tab_altitudes[i+1]-tab_altitudes[i]);
		}
	}																				//on ajoute l'ascention a l'ascention totale
	return ascension_total;}														//retourne l'ascention totale

double pente_maximale(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille, int taille_fenetre) {
	double distance;																//distance de la fentre en km
	int i;
	double pente_max=0.0;															//valeur par defaut de la pente max
	double pente;																	//pente pour la fenetre
	if (taille<=2*taille_fenetre||taille_fenetre<=0) {
		return 0;}																	//retourne 0 si taille de netre est trop grande ou que la taille de fenetre est invalide
		for (i=taille_fenetre; i<taille - taille_fenetre; i++) {					//prend les points qui peuvent etre le centre de fenetre
			distance = distance_totale(tab_latitudes,								//calcul la distance entre les 2 extremites de la fenetre
				tab_longitudes,
				tab_altitudes,
				taille,
				i-taille_fenetre,													//indice du premier point de la fenetre
				i+taille_fenetre);												// indice du derniere point de la fenetre
			if (distance>0) {														// on s'assure que la distance est positive avant de faire une division
				pente = (((tab_altitudes[i+taille_fenetre]-							//calcul le pente qui est la variation d'altitude en km sur la distance fois 100 en %
					tab_altitudes[i-taille_fenetre])/1000)
					/distance)*100;
				if (pente>pente_max) { // garde la plus grande pente
					pente_max=pente;}}}
	return pente_max;} // retourne la pente maximale
