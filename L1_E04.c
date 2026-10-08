#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ESERCIZIO 4:
// Le funzioni che sono state modificate in modo da poter operare sui puntatori sono:
// main(), swap(), bubblesort(), confronta_corse(), seleziona_dati(), ordina_str()
// Sono state aggiunte varianti di funzioni pre esistenti, ordina_str_r(), swap_r(), stampa_dati_ordinati
// E' stata aggiunta la funzione copia_puntatori()

// utilizzo delle struct per tenermi i dati riguardanti
// le corse, date, orario, e un enum per associare i comandi a dei numeri

typedef struct{
    int anno;      
    int mese;
    int giorno;
} data;

typedef struct{
    int ora;
    int minuti;
    int secondi;
} orario;

typedef struct{
    char code[7];
    char part[20];
    char dest[20];
    data data;
    orario iniz;
    orario fin;
    int ritardo;
}corsa;

typedef enum{                       
    r_stampaDati = 1,
    r_ordinaData,
    r_ordinaCodice,
    r_ordinaPartenza,
    r_ordinaCapolinea,
    r_ricercaCodice,
    r_ricercaPartenza,
    r_fine,
}comando;

// lista delle funzioni per le varie richieste, vengono definite nel dettaglio dopo la main
void stampa_dati(corsa lista[], int n);
void stampa_dati_ordinati(corsa *lista[], int n);
void bubbleSort(corsa **arr, int n);
void ordina_str_r(corsa **arr, int n, int opzione);
void ordina_str(corsa arr[], int n, int opzione);

void ricerca_lin(corsa lista[], int n, int opzione);
void ricerca_bin(corsa lista[], int n, int opzione);
int sub_ricerca_6(corsa copia_l[], char * codice, int l, int r);
void sub_ricerca_7(corsa copia_l[], char * staz_part, int l, int r, corsa *sel, int *count);

void swap(corsa *arr, int i, int j);
void stampa_menu();                            
int leggiComando(char str[]);
int confronta_corse(corsa *c1, corsa *c2);
int selezionaDati(corsa lista[], int n, char *scelta, corsa * lista1[], corsa * lista2[], corsa *lista3[], corsa *lista4[]);
void copia_puntatori(corsa lista[], corsa* vett_p[], int n);

int main(){
    FILE *fp;
    int n_corse, s, flag = -1;     // il flag mi serve per determinare se il programma è finito o meno
    char scelta[10];
    
    fp = fopen("corse.txt", "r");
    fscanf(fp, "%d", &n_corse);

    corsa* corse;   // vettore di struct corsa, 
    corse = (corsa *) malloc(n_corse * sizeof(corsa));

    for(int i = 0; i < n_corse; i++){          // leggo i dati dal file e me li salvo 
        fscanf(fp, "%s", corse[i].code);       // in un vettore di struct
        fscanf(fp, "%s", corse[i].part); 
        fscanf(fp, "%s", corse[i].dest);
        fscanf(fp, "%d/%d/%d", &corse[i].data.anno, &corse[i].data.mese, &corse[i].data.giorno);
        fscanf(fp, "%d:%d:%d", &corse[i].iniz.ora, &corse[i].iniz.minuti, &corse[i].iniz.secondi);
        fscanf(fp, "%d:%d:%d", &corse[i].fin.ora, &corse[i].fin.minuti, &corse[i].fin.secondi);
        fscanf(fp, "%d ", &corse[i].ritardo);
    }

    stampa_menu();   // presenta all'utente le varie opzioni

    // Qua ci metto tutti i vettori di puntatori a struct già ordinati. VETTORI DI PUNTATORI, ciò vuol dire che non
    // ordino le struct vere, ordino i loro puntatori. Modifico le funzioni per fare in modo che gli scambi avvengano fra i puntatori.

    // Per ogni ordinamento fornisco un vettore di puntatori a struct, gli copio dentro i puntatori del vettore da ordinare
    // e poi applico il corrispondente algoritmo di ordinamento


    corsa** c_sort_data;   // vettore di puntatori a struct corsa ordinato in base alla data
    c_sort_data = (corsa **) malloc(n_corse * sizeof(corsa *));
    copia_puntatori(corse, c_sort_data, n_corse);
    bubbleSort(c_sort_data, n_corse);

    corsa** c_sort_code;   // vettore di puntatori a struct corsa ordinato in base al codice
    c_sort_code = (corsa **) malloc(n_corse * sizeof(corsa *));
    copia_puntatori(corse, c_sort_code, n_corse);
    ordina_str_r(c_sort_code, n_corse, 3);

    corsa** c_sort_part;   // vettore di puntatori a struct corsa ordinato in base alla stazione di partenza
    c_sort_part = (corsa **) malloc(n_corse * sizeof(corsa *));
    copia_puntatori(corse, c_sort_part, n_corse);
    ordina_str_r(c_sort_part, n_corse, 4);

    corsa** c_sort_cap;   // vettore di puntatori a struct corsa ordinato in base al capolinea
    c_sort_cap = (corsa **) malloc(n_corse * sizeof(corsa *));
    copia_puntatori(corse, c_sort_cap, n_corse);
    ordina_str_r(c_sort_cap, n_corse, 5);

    while(flag != 0){
        printf("*********************************\n");
        printf("Inserire il servizio desiderato: ");           
        scanf("%s", scelta);       // si prende la scelta dell'utente e poi la manda alla selezionaDati()

        flag = selezionaDati(corse, n_corse, scelta, c_sort_data, c_sort_code, c_sort_part, c_sort_cap);       // la funzione esegue la richiesta dell'utente, e poi ritorna un flag per decidere
    }                                                   // se smettere di funzionare o prendere un altro comando, in base alla scelta dell'utente

    free(corse);
    return 0;
}



void stampa_menu(){
    printf("Lista Servizi:\n");                                              // lista di operazioni eseguibili
    printf("*********************************\n");
    printf("[stampa]: Stampa a video o su file i dati riguardanti le corse\n");
    printf("[ordina_data]: Ordina le corse per data\n");
    printf("[ordina_codice]: Ordina le corse in base al codice di tratta\n");
    printf("[ordina_partenza]: Ordina le tratte in base alla stazione di partenza\n");
    printf("[ordina_capolinea]: Ordina le tratte in base alla stazione di capolinea\n");
    printf("[ricerca_codice]: Restituisce la prima occorrenza di una tratta con un codice inserito\n");
    printf("[ricerca_partenza]: Restituisce la prima occorrenza di una tratta con un certa stazione di partenza\n");
    printf("[fine]: Termina il programma\n");
}



int leggiComando(char str[]){        // associa ad ogni comando dato dall'utente un valore dell'enum, che poi verrà usato dalla selezionaDati()
    if(strcmp(str, "stampa") == 0) return r_stampaDati;                               // per chiamare la funziona associata alla richiesta
    else if(strcmp(str, "ordina_data") == 0) return r_ordinaData;
    else if(strcmp(str, "ordina_codice") == 0) return r_ordinaCodice;
    else if(strcmp(str, "ordina_partenza") == 0) return r_ordinaPartenza;
    else if(strcmp(str, "ordina_capolinea") == 0) return r_ordinaCapolinea;
    else if(strcmp(str, "ricerca_codice") == 0) return r_ricercaCodice;
    else if(strcmp(str, "ricerca_partenza") == 0) return r_ricercaPartenza;
    else if(strcmp(str, "fine") == 0) return r_fine;

    return -1;
}

int selezionaDati(corsa lista[], int n, char *scelta, corsa * lista2[], corsa * lista3[], corsa *lista4[], corsa *lista5[]){
    int s = -1;
    s = leggiComando(scelta);     // mi converte la stringa in un numero per associarla alla richiesta corrispondente
                                  // che viene gestita da una funzione apposita
    switch(s){
        // Qua, ogni caso sarà semplicemente uno stampa_dati, che mi stampa ogni vettore già ordinato nel main, e passato come parametr
        // alla funzione

        case 1:
            stampa_dati(lista, n);
            break;

        case 2:
            stampa_dati_ordinati(lista2, n);
            break;
            
        case 3:
            stampa_dati_ordinati(lista3, n);
            break;

        case 4:
            
            stampa_dati_ordinati(lista4, n); 
            break;

        case 5:
            stampa_dati_ordinati(lista5, n);
            break;

        case 6:
            ricerca_lin(lista, n, s);
            ricerca_bin(lista, n, s);
            break;
        
        case 7:
            ricerca_lin(lista, n, s);
            ricerca_bin(lista, n, s);
            break;

        case 8:
            return 0;            // ritorna 0 al flag, che segnala l'uscita dal ciclo

        default:                 // se non è presente un comando valido, messaggio di errore
            printf("!!Comando inserito invalido, riprovare!!\n");
    }
    return 1;
}

// Stampa i dati a video o su file in base alla richiesta dell'utente
void stampa_dati(corsa lista[], int n){
    char t;
    printf("Stampare i dati su file o su video? [v = video || f = file]: ");
    scanf(" %c", &t);

    switch(t){
        case 'v':
            for(int i = 0; i < n; i++){
                printf("%s ", lista[i].code);       // stampa ogni dato per ogni elemento del vettore di struct, a video
                printf("%s ", lista[i].part); 
                printf("%s ", lista[i].dest);
                printf("%d/%d/%d ", lista[i].data.anno, lista[i].data.mese, lista[i].data.giorno);
                printf("%d:%d:%d ", lista[i].iniz.ora, lista[i].iniz.minuti, lista[i].iniz.secondi);
                printf("%d:%d:%d ", lista[i].fin.ora, lista[i].fin.minuti, lista[i].fin.secondi);
                printf("%d", lista[i].ritardo);
                printf("\n");
            }
            break;

        case 'f':
            FILE *fp;
            fp = fopen("dati.txt", "w");
            
            for(int i = 0; i < n; i++){
                fprintf(fp, "%s ", lista[i].code);       // fa la stessa cosa del caso precedente, ma su un file
                fprintf(fp, "%s ", lista[i].part); 
                fprintf(fp, "%s ", lista[i].dest);
                fprintf(fp, "%d/%d/%d ", lista[i].data.anno, lista[i].data.mese, lista[i].data.giorno);
                fprintf(fp, "%d:%d:%d ", lista[i].iniz.ora, lista[i].iniz.minuti, lista[i].iniz.secondi);
                fprintf(fp, "%d:%d:%d ", lista[i].fin.ora, lista[i].fin.minuti, lista[i].fin.secondi);
                fprintf(fp, "%d", lista[i].ritardo);
                fprintf(fp, "\n");
            }
            fclose(fp);
            break;

        default:
            printf("Operazione non valida");
    }
}

// Stampa i dati a video dal vettore di puntatori
void stampa_dati_ordinati(corsa *lista[], int n){
    for(int i = 0; i < n; i++){
        printf("%s ", lista[i]->code);       // stampa ogni dato per ogni elemento del vettore di puntatori a struct, a video
        printf("%s ", lista[i]->part); 
        printf("%s ", lista[i]->dest);
        printf("%d/%d/%d ", lista[i]->data.anno, lista[i]->data.mese, lista[i]->data.giorno);
        printf("%d:%d:%d ", lista[i]->iniz.ora, lista[i]->iniz.minuti, lista[i]->iniz.secondi);
        printf("%d:%d:%d ", lista[i]->fin.ora, lista[i]->fin.minuti, lista[i]->fin.secondi);
        printf("%d", lista[i]->ritardo);
        printf("\n");
    }     
}

// Ordinamento per data, a parità di date, si va per ora
// Algoritmo di ordinamento --> ne serve uno stabile --> Bubble Sort
void swap_r(corsa **arr, int i, int j)     
{
    corsa *temp = arr[i];      // semplice funzione di scambio struct per il sorting
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(corsa **arr, int n){    
    // Bubble sort riadattato. Come meccanismo di confronto, al posto di vedere se uno è più grande dell'altro
    // semplicemente il confronto viene gestito sull'intera data, ed eventualmente orario, da un'altra funzione

    // ES.4: Riceve un vettore di puntatore a struct, al posto di un vettore di struct, e opera su quello sia per i confronti
    // sia per lo swap

    for (int i = 0; i < n - 1; i++){

        for (int j = 0; j < n - i - 1; j++) {
            
            if (confronta_corse(arr[j], arr[j + 1]) > 0) {
                swap_r(arr, j, j + 1);
            }
        }
    
    }


}

int confronta_corse(corsa *c1, corsa *c2){      // riprendo l'algoritmo usato anche nell'esercizio precedente per confrontare due date
    // adesso devo dereferenziare, dato che riceve un puntatore a struct
    // Il confronto è il medesimo dell'esercizio precedente, ma avendo un puntatore questo va dereferenziato

    if (c1->data.anno != c2->data.anno) return c1->data.anno - c2->data.anno;
    if (c1->data.mese != c2->data.mese) return c1->data.mese - c2->data.mese;
    if (c1->data.giorno != c2->data.giorno) return c1->data.giorno - c2->data.giorno;

    // se arriva a questo punto, vuol dire che le due date sono uguali, ritorna un valore in base al confronto
    // degli orari, gestito esattamente come il confronto fra le date
    if(c1->iniz.ora != c2->iniz.ora) return c1->iniz.ora - c2->iniz.ora;
    if(c1->iniz.minuti != c2->iniz.minuti) return c1->iniz.minuti - c2->iniz.minuti;
    return c1->iniz.secondi - c2->iniz.secondi;
    
}

void ordina_str_r(corsa **arr, int n, int opzione){
    // riprendo il bubble sort, ma uso la strcmp per fare i confronti, l'opzione scelta dall'utente viene
    // mandata come parametro alla funzione per decidere in base a cosa fare l'ordinamento

    // ES.4: Modifico le funzioni per operare, invece che su un vettore di struct, su un vettore di puntatori
    // ciò che cambia è sostanzialmente il crieterio del confronto

    if(opzione == 3){     // ordina in base al codice di tratta
        for (int i = 0; i < n - 1; i++){

            for (int j = 0; j < n - i - 1; j++) {
            
                if (strcmp(arr[j]->code, arr[j+1]->code) > 0) {
                    swap_r(arr, j, j + 1);
                }
            }
    
        }
    }

    else if(opzione == 4){         // ordina in base alla stazione di partenza
        for (int i = 0; i < n - 1; i++){

            for (int j = 0; j < n - i - 1; j++) {
            
                if (strcmp(arr[j]->part, arr[j+1]->part) > 0) {
                    swap_r(arr, j, j + 1);
                }
            }
    
        }
    }

    else if(opzione == 5){         // ordina in base alla stazione di capolinea
        for (int i = 0; i < n - 1; i++){

            for (int j = 0; j < n - i - 1; j++) {
            
                if (strcmp(arr[j]->dest, arr[j+1]->dest) > 0) {
                    swap_r(arr, j, j + 1);
                }
            }
    
        }
    }
}

// Ricerca lineare:
void ricerca_lin(corsa lista[], int n, int opzione){
    if(opzione == 6){                   // se l'utente ha richiesto la ricerca in base al codice

        char in_codice[3];
        char codice[7] = "GTT";
        printf("Inserire il codice a tre cifre della tratta desiderata [XXX]: ");
        scanf("%s", in_codice);

        // unisco il prefisso "GTT" al codice di tratta inserito dall'utente in modo da non dover tagliare una parte di stringa dal lista[i].codice, ad
        // ogni ripetizione del ciclo
        strcat(codice, in_codice);         

        for(int i = 0; i < n; i++){

            if(strcmp(lista[i].code, codice) == 0){          // se il codice inserito è uguale a quello della tratta
                printf("%s ", lista[i].code);   
                printf("%s ", lista[i].part); 
                printf("%s ", lista[i].dest);
                printf("%d/%d/%d ", lista[i].data.anno, lista[i].data.mese, lista[i].data.giorno);
                printf("%d:%d:%d ", lista[i].iniz.ora, lista[i].iniz.minuti, lista[i].iniz.secondi);
                printf("%d:%d:%d ", lista[i].fin.ora, lista[i].fin.minuti, lista[i].fin.secondi);
                printf("%d", lista[i].ritardo);
                printf("\n");
            
                break;
            }
        }
    }

    // se si raggiunge questo punto, vuol dire che opzione == 7, quindi la ricerca è da fare sulle partenze, e bisogna salvare
    // più struct in un vettore apposito, per poi stamparle.

    if(opzione == 7){
        corsa *sel = (corsa *)malloc(n * sizeof(corsa));   // alloco un vettore per metterci dentro i valori selezionati dall'opzione 7
        int count = 0;
        char nome_part[30];
        printf("Inserire il nome della stazione di partenza: ");
        scanf("%s", nome_part);


        // per ogni tratta, se riconosce nella sottostringa
        for(int j = 0; j < n; j++){
            if(strstr(lista[j].part, nome_part) != NULL && (strstr(lista[j].part, nome_part))[0] == lista[j].part[0]){
                sel[count] = lista[j];
                count++;
            }
        }

        if(count == 0){        // Se non trova nessuna tratta manda un messaggio d'errore
            printf("Nessuna tratta in partenza nella stazione selezionata\n");
            return;
        }

        for(int k = 0; k < count; k++){      // se no procede a stampare a video il vettore delle corse valide
            printf("%s ", sel[k].code);   
            printf("%s ", sel[k].part); 
            printf("%s ", sel[k].dest);
            printf("%d/%d/%d ", sel[k].data.anno, sel[k].data.mese, sel[k].data.giorno);
            printf("%d:%d:%d ", sel[k].iniz.ora, sel[k].iniz.minuti, sel[k].iniz.secondi);
            printf("%d:%d:%d ", sel[k].fin.ora, sel[k].fin.minuti, sel[k].fin.secondi);
            printf("%d", sel[k].ritardo);
            printf("\n");
        }
    }
}

// Ricerca binaria:
void ricerca_bin(corsa lista[], int n, int opzione){
    // Prima di tutto mi serve che l'insieme su cui faccio ricerca sia ordinato
    // per cui mi serve un vettore di copia per non alterare il vettore originale di struct, non essendo 
    // espressamente richiesto, come in un altro dei comandi

    corsa *copia_l = lista;
    
    int m = n/2;   // prendo il mio valore di metà

    if(opzione == 6){
        ordina_str(copia_l, n, 3); // adesso ho il mio vettore codice-ordinato di struct, posso lavorarci

        char in_codice[3];
        char codice[7] = "GTT";
        int i;  // indice del risultato ottenuto
        printf("Inserire il codice a tre cifre della tratta desiderata [XXX]: ");
        scanf("%s", in_codice);

        // unisco il prefisso "GTT" al codice di tratta inserito dall'utente in modo da non dover tagliare una parte di stringa dal lista[i].codice, ad
        // ogni ripetizione del ciclo
        strcat(codice, in_codice);

        // mi controlla se è presente al centro, poi mi vede se è maggiore, richiama se stesso nella seconda
        // metà, se no nella prima. Condizione di uscita si ha quando m == 2;
         i = sub_ricerca_6(copia_l, codice, 0, n);
        
        if(i == -1){
            printf("Codice non trovato\n");         // sempre messaggiod d'errore e successiva stampa
        }
        else{
            printf("%s ", lista[i].code);   
            printf("%s ", lista[i].part); 
            printf("%s ", lista[i].dest);
            printf("%d/%d/%d ", lista[i].data.anno, lista[i].data.mese, lista[i].data.giorno);
            printf("%d:%d:%d ", lista[i].iniz.ora, lista[i].iniz.minuti, lista[i].iniz.secondi);
            printf("%d:%d:%d ", lista[i].fin.ora, lista[i].fin.minuti, lista[i].fin.secondi);
            printf("%d", lista[i].ritardo);
            printf("\n");
        }
    }
    else{
        // qua ci troviamo nel caso in cui opzione == 7:
        ordina_str(copia_l, n, 4);     // avendolo ordinato ora ci posso lavorare

        corsa *sel = (corsa *) malloc(n * sizeof(corsa));
        char nome_part[30];
        int count = 0;
        printf("Inserire il nome della stazione di partenza: ");
        scanf("%s", nome_part);

        sub_ricerca_7(copia_l, nome_part, 0, n, sel, &count);

        // adesso posso stampare i valori ottenuti 
        if(count == 0){
            printf("Nessuna tratta in partenza nella stazione selezionata\n");
        }

        for(int h = 0; h < count; h++){
            printf("%s ", sel[h].code);   
            printf("%s ", sel[h].part); 
            printf("%s ", sel[h].dest);
            printf("%d/%d/%d ", sel[h].data.anno, sel[h].data.mese, sel[h].data.giorno);
            printf("%d:%d:%d ", sel[h].iniz.ora, sel[h].iniz.minuti, sel[h].iniz.secondi);
            printf("%d:%d:%d ", sel[h].fin.ora, sel[h].fin.minuti, sel[h].fin.secondi);
            printf("%d", sel[h].ritardo);
            printf("\n");
        }
        

    }
}
// per la ricerca dicotomica, o faccio un algoritmo ricorsivo, che chiama a se stesso fino a che non fa
// il confronto fra due elementi
// RICERCA DICOTOMICA PER CODICE
int sub_ricerca_6(corsa copia_l[], char * codice, int l, int r){
    int m = (l + r) / 2;

    if (l > r){
        return -1;
    }

    if(strcmp(copia_l[m].code, codice) == 0){
        return m; 
    }
    else if(strcmp(copia_l[m].code, codice) > 0){
        return sub_ricerca_6(copia_l, codice, 0, m);

    }
    else{
        return sub_ricerca_6(copia_l, codice, m, r);
    }
}


// RICERCA DICOTOMICA PER STAZIONE DI PARTENZA
void sub_ricerca_7(corsa copia_l[], char * staz_part, int l, int r, corsa *sel, int *count){
    int m = (l + r) / 2;
    
    // qua ricorsivamente continua a chiamarsi e a fare confronti, e mano a mano che trova tratte che 
    // vanno bene le aggiunge al vettore sel[]
    if (l > r){
        return;
    }

    if(strstr(copia_l[m].part, staz_part) != NULL && (strstr(copia_l[m].part, staz_part))[0] == copia_l[m].part[0]){             // se il valore al centro va bene, richiama comunque a destra e a sinistra
        sel[*count] = copia_l[m];   
        (*count)++;

        // richiamo la funzione escludendo m
        sub_ricerca_7(copia_l, staz_part, l, m - 1, sel, count);
        sub_ricerca_7(copia_l, staz_part, m + 1, r, sel, count);
    }
    else if(strcmp(copia_l[m].part, staz_part) > 0){
        sub_ricerca_7(copia_l, staz_part, l, m - 1, sel, count);

    }
    else{
        sub_ricerca_7(copia_l, staz_part, m + 1, r, sel, count);
    }
}

// Affinchè gli ordinamenti vengano salvati e siano sempre pronti all'uso, devo salvare i puntatori
// alle struct in un puntatore. Funzione di copia apposita:
void copia_puntatori(corsa lista[], corsa* vett_p[], int n){
    for(int i = 0; i < n; i++){
        vett_p[i] = &lista[i];
    }
}

// Ordina_str originale, che opera sul vettore di struc
void ordina_str(corsa arr[], int n, int opzione){
    // riprendo il bubble sort, ma uso la strcmp per fare i confronti, l'opzione scelta dall'utente viene
    // mandata come parametro alla funzione per decidere in base a cosa fare l'ordinamento

    if(opzione == 3){     // ordina in base al codice di tratta
        for (int i = 0; i < n - 1; i++){

            for (int j = 0; j < n - i - 1; j++) {
            
                if (strcmp(arr[j].code, arr[j+1].code) > 0) {
                    swap(arr, j, j + 1);
                }
            }
    
        }
    }

    else if(opzione == 4){         // ordina in base alla stazione di partenza
        for (int i = 0; i < n - 1; i++){

            for (int j = 0; j < n - i - 1; j++) {
            
                if (strcmp(arr[j].part, arr[j+1].part) > 0) {
                    swap(arr, j, j + 1);
                }
            }
    
        }
    }

    else if(opzione == 5){         // ordina in base alla stazione di capolinea
        for (int i = 0; i < n - 1; i++){

            for (int j = 0; j < n - i - 1; j++) {
            
                if (strcmp(arr[j].dest, arr[j+1].dest) > 0) {
                    swap(arr, j, j + 1);
                }
            }
    
        }
    }
}

// Swap originale, che opera sul vettore di struct
void swap(corsa *arr, int i, int j)     
{
    corsa temp = arr[i];      // semplice funzione di scambio struct per il sorting
    arr[i] = arr[j];
    arr[j] = temp;
}