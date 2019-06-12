#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "hangman.h"
#include "serverTP4.h"

typedef struct tipoDificultad{
  int cantPal;
  char ** palabras;
} tDificultad;

typedef tDificultad ** tDificultades;

int menu(void);
int eligeVidas();
int jugar(char * palabra, int vidas);
void print(char * palabra, char * lsErradas, int * lsAdivinadas, int vidas);
char pidoLetra(char * lsErradas, int * lsAdivinadas, char * palabra);
int controlaPalabra(char * palabra, char * lsErradas, int * lsAdivinadas, int * lsFaltantes, int vidas, char letra);

int mySock;

int startHangman(int sock){
  mySock = sock;
  int opcion, gano, vidas, i, auxP;
  char * palabra;

  char * facil[] = {"pez", "muerte", "lenia", "bicicleta", "aborto", "cadena", "luna"};
  char * med[] = {"recursar", "extraordinario", "obstetra", "incompetente", "omnipotente", "transaccion", "ahorcado"};
  char * dif[] = {"circunstancial", "pejelagarto", "lavavajillas", "trapezoide", "jazz", "desoxirribonucleico"};
  char * ext[] = {"frishknecht", "karpovich", "tarradellas", "asknckjendasp", "tarradkllas"};

  tDificultades dificultades = malloc(4*sizeof(*dificultades));
  for (i=0; i<4; i++)
    dificultades[i] = malloc(sizeof(tDificultad));

  dificultades[0]->palabras = facil;
  dificultades[0]->cantPal = 7;

  dificultades[1]->palabras = med;
  dificultades[1]->cantPal = 7;

  dificultades[2]->palabras = dif;
  dificultades[2]->cantPal = 6;

  dificultades[3]->palabras = ext;
  dificultades[3]->cantPal = 5;

  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\nolaaaaaaa bienvenido al peor juego de la historia\n\n\n");

  srand(time(0));
  opcion = menu();
  auxP = rand() % (dificultades[opcion]->cantPal-1);
  palabra = malloc(strlen(dificultades[opcion]->palabras[auxP]));
  strcpy(palabra, dificultades[opcion]->palabras[auxP]);
  vidas = eligeVidas();
  gano = jugar(palabra, vidas);

  if (gano){
    printf("\n\n PALABRA: %s\n", palabra);
    printf("\n\nEsssaaaa GANAASTTEEE byeeeee \n\n\n");
  }
  else printf("\n\nWAAAA WAAAAA WAAAAAAAAAAAAAA u ded\n\n\n");

  for(i=0; i<4; i++)
    free(dificultades[i]);
  free (dificultades);
  free (palabra);

  return 0;
}

int menu(void){
  int opcion;
  printf("Elige un nivel de dificultad:\n");
  do{
    printf(".1 para Facil\n.2 para Mediano\n.3 para Dificil\n\n\n");
    opcion = readInt(mySock);
    if (opcion<1 || opcion>4)
      printf("\nNivel invalido, ingrese un numero valido\n\n\n");
  } while (opcion<1||opcion>4);
  return opcion-1;
}

int eligeVidas(){
  int vidas;
  do{
    printf("\nElige cantidad de vidas (1~8)\n\n\n");
    vidas = readInt(mySock);
  } while (vidas<1||vidas>8);
  return vidas;
}

int jugar(char * palabra, int vidas){
  char letra;
  int lsFaltantes = strlen(palabra);
  int size = lsFaltantes;
  int * lsAdivinadas = calloc(lsFaltantes*sizeof(int), sizeof(int));
  char * lsErradas = calloc(vidas, sizeof(char));
  
  do{
    printf("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\nLetras: %d\n", size);
    print(palabra, lsErradas, lsAdivinadas, vidas);
    letra = pidoLetra(lsErradas, lsAdivinadas, palabra);
    vidas = controlaPalabra(palabra, lsErradas, lsAdivinadas, &lsFaltantes, vidas, letra);
    if (!lsFaltantes){
      free(lsAdivinadas);
      free(lsErradas);
      return 1;
    }
  } while (vidas);
  free(lsAdivinadas);
  free(lsErradas);
  return 0;
}

void print(char * palabra, char * lsErradas, int * lsAdivinadas, int vidas){
  int i, j;
  printf("\nLetras erradas:\n");
  for (i=0; lsErradas[i]; i++){
    printf("%c - ", lsErradas[i]);
  }
  printf("\nVidas: %d\n\n",vidas);
  for (j=0; palabra[j]; j++){
    if (lsAdivinadas[j]){
      printf("%c ", palabra[j]);
    }
    else{
      printf("__ ");
    }
  }
  return;
}

char pidoLetra(char * lsErradas, int * lsAdivinadas, char * palabra){
  int i;
  char letra, error;
  
  printf("\n\n~~~~~~~~~~~~~~~~~~\nIngrese una letra:\n");

  do{
    error=0;
    letra = readChar(mySock);

    for (i=0; lsErradas[i] && !error; i++){
      if (lsErradas[i]==letra)
        error = 1;
    }

    for (i=0; !error && palabra[i]; i++){
      if (palabra[i]==letra){
        if (lsAdivinadas[i]==1)
          error=1;
        else break;
      }
    }
    if (letra<'a'||letra>'z'||error)
      printf("\nLetra no valida, ingrese una nueva letra:\n");
  } while(letra<'a'||letra>'z'||error);
  return letra;
}

int controlaPalabra(char * palabra, char * lsErradas, int * lsAdivinadas, int * lsFaltantes, int vidas, char letra){
  int i, j, encontro=0;
  for (i=0; palabra[i]; i++){

    if (palabra[i]==letra){
      lsAdivinadas[i]=1;
      (*lsFaltantes) = (*lsFaltantes) - 1;
      encontro = 1;
    }
  }
  if (!encontro){
    for (j=0; lsErradas[j]; j++);
    lsErradas[j]=letra;
    vidas--;
  }
  return vidas;
}