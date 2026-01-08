#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100

/* ================= STRUCTURES ================= */

typedef struct {
    char id[5];
    int arrivee;
    int burst;
    int priorite;
    int restant;
    int fin;
    int attente;
    int rotation;
} Processus;

typedef struct {
    char pid[5];
    int debut;
    int fin;
} BlocGantt;

typedef struct {
    char nom[30];
    float attente;
    float rotation;
} Resultat;

/* ================= OUTILS ================= */

void copierProcessus(Processus src[], Processus dest[], int n){
    for(int i=0;i<n;i++) dest[i]=src[i];
}

void calculerTemps(Processus p[], int n){
    for(int i=0;i<n;i++){
        p[i].rotation = p[i].fin - p[i].arrivee;
        p[i].attente  = p[i].rotation - p[i].burst;
    }
}

float moyAttente(Processus p[], int n){
    float s=0;
    for(int i=0;i<n;i++) s+=p[i].attente;
    return s/n;
}

float moyRotation(Processus p[], int n){
    float s=0;
    for(int i=0;i<n;i++) s+=p[i].rotation;
    return s/n;
}

void afficherProcessus(Processus p[], int n){
    printf("\n================ LISTE DES PROCESSUS ================\n");
    printf("ID   Arrivée   Burst   Priorité\n");
    printf("-------------------------------------\n");
    for(int i=0;i<n;i++)
        printf("%-4s %-9d %-7d %-8d\n", p[i].id, p[i].arrivee, p[i].burst, p[i].priorite);
}

void afficherGantt(BlocGantt g[], int k){
    printf("\n================ DIAGRAMME DE GANTT ================\n");
    for(int i=0;i<k;i++)
        printf("| %s [%d-%d] ", g[i].pid, g[i].debut, g[i].fin);
    printf("|\n");
}

/* ================= ALGORITHMES ================= */

// FCFS
void FCFS(Processus p[], int n, BlocGantt g[], int *k){
    int t=0; *k=0;
    for(int i=0;i<n;i++){
        if(t<p[i].arrivee) t=p[i].arrivee;
        strcpy(g[*k].pid,p[i].id);
        g[*k].debut=t;
        g[*k].fin=t+p[i].burst;
        (*k)++;
        t+=p[i].burst;
        p[i].fin=t;
    }
}

// SJF Non Préemptif
void SJF_NP(Processus p[], int n, BlocGantt g[], int *k){
    int fini[n], t=0, c=0;
    memset(fini,0,sizeof(fini));
    *k=0;

    while(c<n){
        int idx=-1, min=INT_MAX;
        for(int i=0;i<n;i++)
            if(!fini[i] && p[i].arrivee<=t && p[i].burst<min){
                min=p[i].burst; idx=i;
            }

        if(idx==-1){ t++; continue; }

        strcpy(g[*k].pid,p[idx].id);
        g[*k].debut=t;
        g[*k].fin=t+p[idx].burst;
        (*k)++;

        t+=p[idx].burst;
        p[idx].fin=t;
        fini[idx]=1;
        c++;
    }
}

// SRTF (SJF préemptif)
void SRTF(Processus p[], int n, BlocGantt g[], int *k){
    int reste[n], fini[n], t=0;
    memset(fini,0,sizeof(fini));
    *k=0;

    for(int i=0;i<n;i++) reste[i]=p[i].burst;

    while(1){
        int idx=-1, min=INT_MAX;
        int tous_finis=1;
        for(int i=0;i<n;i++){
            if(reste[i]>0) tous_finis=0;
            if(p[i].arrivee<=t && reste[i]>0 && reste[i]<min){
                min=reste[i];
                idx=i;
            }
        }
        if(tous_finis) break;

        if(idx==-1){ t++; continue; }

        strcpy(g[*k].pid,p[idx].id);
        g[*k].debut=t;
        g[*k].fin=t+1;
        (*k)++;
        reste[idx]--;
        t++;
        if(reste[idx]==0) p[idx].fin=t;
    }
}

// Priorité Non Préemptif
void Priorite_NP(Processus p[], int n, BlocGantt g[], int *k){
    int fini[n], t=0, c=0;
    memset(fini,0,sizeof(fini));
    *k=0;

    while(c<n){
        int idx=-1, min=INT_MAX;
        for(int i=0;i<n;i++)
            if(!fini[i] && p[i].arrivee<=t && p[i].priorite<min){
                min=p[i].priorite; idx=i;
            }

        if(idx==-1){ t++; continue; }

        strcpy(g[*k].pid,p[idx].id);
        g[*k].debut=t;
        g[*k].fin=t+p[idx].burst;
        (*k)++;

        t+=p[idx].burst;
        p[idx].fin=t;
        fini[idx]=1;
        c++;
    }
}

// Priorité Préemptive
void Priorite_P(Processus p[], int n, BlocGantt g[], int *k){
    int reste[n], t=0;
    *k=0;
    for(int i=0;i<n;i++) reste[i]=p[i].burst;

    while(1){
        int idx=-1, min=INT_MAX;
        int tous_finis=1;
        for(int i=0;i<n;i++){
            if(reste[i]>0) tous_finis=0;
            if(p[i].arrivee<=t && reste[i]>0 && p[i].priorite<min){
                min=p[i].priorite;
                idx=i;
            }
        }
        if(tous_finis) break;
        if(idx==-1){ t++; continue; }

        strcpy(g[*k].pid,p[idx].id);
        g[*k].debut=t;
        g[*k].fin=t+1;
        (*k)++;
        reste[idx]--;
        t++;
        if(reste[idx]==0) p[idx].fin=t;
    }
}

// Round Robin
void RoundRobin(Processus p[], int n, int q, BlocGantt g[], int *k){
    int file[MAX], d=0, f=0, t=0, finis=0;
    int present[n]; memset(present,0,sizeof(present));
    *k=0;

    for(int i=0;i<n;i++){
        p[i].restant=p[i].burst;
        if(p[i].arrivee==0){ file[f++]=i; present[i]=1; }
    }

    while(finis<n){
        if(d==f){ t++; continue; }

        int i=file[d++];
        int exec=(p[i].restant<q)?p[i].restant:q;

        strcpy(g[*k].pid,p[i].id);
        g[*k].debut=t;
        g[*k].fin=t+exec;
        (*k)++;

        t+=exec;
        p[i].restant-=exec;

        for(int j=0;j<n;j++)
            if(p[j].arrivee<=t && p[j].restant>0 && !present[j]){
                file[f++]=j;
                present[j]=1;
            }

        if(p[i].restant>0) file[f++]=i;
        else { p[i].fin=t; finis++; }
    }
}

/* ================= MENU ================= */

void menu(){
    printf("\n===== MENU =====\n");
    printf("1. FCFS\n");
    printf("2. SJF Non Préemptif\n");
    printf("3. SRTF\n");
    printf("4. Priorité Non Préemptif\n");
    printf("5. Priorité Préemptive\n");
    printf("6. Round Robin\n");
    printf("7. Comparaison des algorithmes\n");
    printf("0. Quitter\n");
    printf("Choix : ");
}

void menuComparaison(){
    printf("\n=== COMPARAISON ===\n");
    printf("1. Afficher tous les algorithmes\n");
    printf("2. Classement par temps d'attente\n");
    printf("3. Classement par temps de rotation\n");
    printf("0. Retour\n");
    printf("Choix : ");
}

/* ================= COMPARAISON ================= */

void comparer(Processus base[], int n, int quantum){
    Resultat r[6];
    int nb=0, choix;
    Processus p[MAX];
    BlocGantt g[MAX]; int k;

    char noms[6][30]={"FCFS","SJF NP","SRTF","Priorité NP","Priorité P","Round Robin"};

    for(int i=0;i<6;i++){
        copierProcessus(base,p,n);
        if(i==0) FCFS(p,n,g,&k);
        if(i==1) SJF_NP(p,n,g,&k);
        if(i==2) SRTF(p,n,g,&k);
        if(i==3) Priorite_NP(p,n,g,&k);
        if(i==4) Priorite_P(p,n,g,&k);
        if(i==5) RoundRobin(p,n,quantum,g,&k);

        calculerTemps(p,n);
        strcpy(r[nb].nom,noms[i]);
        r[nb].attente=moyAttente(p,n);
        r[nb].rotation=moyRotation(p,n);
        nb++;
    }

    do{
        menuComparaison();
        scanf("%d",&choix);

        if(choix==1){
            printf("\n--- Tous les algorithmes ---\n");
            for(int i=0;i<nb;i++)
                printf("%-15s | Attente=%.2f | Rotation=%.2f\n",
                       r[i].nom,r[i].attente,r[i].rotation);
        }
        else if(choix==2){
            // Tri par attente
            for(int i=0;i<nb-1;i++)
                for(int j=i+1;j<nb;j++)
                    if(r[i].attente>r[j].attente){ Resultat t=r[i]; r[i]=r[j]; r[j]=t; }

            printf("\n--- Classement par attente ---\n");
            for(int i=0;i<nb;i++)
                printf("%d. %s (%.2f)\n",i+1,r[i].nom,r[i].attente);
        }
        else if(choix==3){
            // Tri par rotation
            for(int i=0;i<nb-1;i++)
                for(int j=i+1;j<nb;j++)
                    if(r[i].rotation>r[j].rotation){ Resultat t=r[i]; r[i]=r[j]; r[j]=t; }

            printf("\n--- Classement par rotation ---\n");
            for(int i=0;i<nb;i++)
                printf("%d. %s (%.2f)\n",i+1,r[i].nom,r[i].rotation);
        }

    }while(choix!=0);
}

/* ================= MAIN ================= */

int main(){
    int n, choix, quantum;
    printf("=====Implémentation des Algorithmes d’Ordonnancement CPU=====\n");
    printf("Nombre de processus : ");
    scanf("%d",&n);

    Processus base[MAX];
    for(int i=0;i<n;i++){
        sprintf(base[i].id,"PR%d",i);
        base[i].arrivee=i;
        printf("Entrer la dureé d'exécution %s : ",base[i].id);
        scanf("%d",&base[i].burst);
        printf("Entrer la priorité %s : ",base[i].id);
        scanf("%d",&base[i].priorite);
    }

    printf("Quantum Round Robin : ");
    scanf("%d",&quantum);

    afficherProcessus(base,n);

    do{
        menu();
        scanf("%d",&choix);

        Processus p[MAX];
        BlocGantt g[MAX]; int k;

        switch(choix){
            case 1:
                copierProcessus(base,p,n);
                FCFS(p,n,g,&k);
                calculerTemps(p,n);
                afficherGantt(g,k);
                printf("Attente moyenne  : %.2f\n", moyAttente(p,n));
                printf("Rotation moyenne : %.2f\n", moyRotation(p,n));
                break;

            case 2:
                copierProcessus(base,p,n);
                SJF_NP(p,n,g,&k);
                calculerTemps(p,n);
                afficherGantt(g,k);
                printf("Attente moyenne  : %.2f\n", moyAttente(p,n));
                printf("Rotation moyenne : %.2f\n", moyRotation(p,n));
                break;

            case 3:
                copierProcessus(base,p,n);
                SRTF(p,n,g,&k);
                calculerTemps(p,n);
                afficherGantt(g,k);
                printf("Attente moyenne  : %.2f\n", moyAttente(p,n));
                printf("Rotation moyenne : %.2f\n", moyRotation(p,n));
                break;

            case 4:
                copierProcessus(base,p,n);
                Priorite_NP(p,n,g,&k);
                calculerTemps(p,n);
                afficherGantt(g,k);
                printf("Attente moyenne  : %.2f\n", moyAttente(p,n));
                printf("Rotation moyenne : %.2f\n", moyRotation(p,n));
                break;

            case 5:
                copierProcessus(base,p,n);
                Priorite_P(p,n,g,&k);
                calculerTemps(p,n);
                afficherGantt(g,k);
                printf("Attente moyenne  : %.2f\n", moyAttente(p,n));
                printf("Rotation moyenne : %.2f\n", moyRotation(p,n));
                break;

            case 6:
                copierProcessus(base,p,n);
                RoundRobin(p,n,quantum,g,&k);
                calculerTemps(p,n);
                afficherGantt(g,k);
                printf("Attente moyenne  : %.2f\n", moyAttente(p,n));
                printf("Rotation moyenne : %.2f\n", moyRotation(p,n));
                break;

            case 7:
                comparer(base,n,quantum);
                break;

            case 0:
                printf("Fin du programme.\n");
                break;

            default:
                printf("Choix invalide !\n");
        }

    }while(choix!=0);

    return 0;
}
