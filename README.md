# Ordonnancement CPU en C

Ce projet en langage C implémente plusieurs algorithmes d’ordonnancement CPU classiques.  
Il permet de simuler et d’analyser la gestion des processus dans un système d’exploitation, en affichant les résultats sous forme de diagrammes de Gantt et en calculant les performances de chaque algorithme.

## Algorithmes inclus:
- FCFS (First Come First Served) : les processus sont exécutés dans l’ordre d’arrivée.  
- SJF Non Préemptif: le processus le plus court est exécuté en priorité, sans interruption.  
- SRTF (SJF préemptif) : le processus le plus court peut interrompre un autre processus en cours.  
- Priorité Non Préemptive et Préemptive : les processus sont ordonnancés selon leur priorité, avec ou sans préemption.  
- Round Robin : chaque processus reçoit un quantum de temps et est exécuté de manière cyclique.

## Fonctionnalités:
- Saisie dynamique des processus (temps d’arrivée, durée d’exécution, priorité)  
- Calcul automatique des **temps d’attente** et **temps de rotation** pour chaque processus  
- Affichage clair des **diagrammes de Gantt** pour visualiser l’ordre d’exécution  
- Comparaison des algorithmes pour analyser lequel est le plus efficace selon les métriques

## Bibliothèques requises

Le programme utilise uniquement des bibliothèques standard du langage C :  
- <stdio.h> 
- <stdlib.h>
- <string.h>
- <limits.h>

Ce projet est idéal pour étudier les concepts fondamentaux des systèmes d’exploitation et pour comprendre le fonctionnement des algorithmes d’ordonnancement CPU.

