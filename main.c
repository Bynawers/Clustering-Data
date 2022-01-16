#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Effectue l'affichage des étudiants
void print_students(struct Student* Students, int length) {

    for (int i = 0; i < length; i++) {
        printf("- %s-%d-%d-%d-%d-%s\n",Students[i].nom,Students[i].courage,Students[i].loyaute,Students[i].sagesse,
        Students[i].malice,Students[i].maison);
    }
}

//Effectue l'affichage du contenant des clusters
void print_clusters(struct Cluster* clusters, int length) {

    for (int i = 0; i < length; i++) {
        printf("\n\033[0;31m CLUSTER %d\n\033[0m", i);
        printf("représentant: \033[0m");
        print_students(&clusters[i].obj_rep, 1);

        printf("\033[0;32mdata list: \033[0m ");
        printf("\033[0;36mLength: %d\033[0m\n",clusters[i].length);
        print_students(clusters[i].cluster_data, clusters[i].length);
    }
}

//Teste si student se trouve dans le tableau student_data
int is_equal(struct Student* student_data, struct Student student, int length){
    
    if (length == 0){
        return 0;
    }
    
    for (int i = 0; i < length; i++){
        if (!strcmp(student.nom, student_data[i].nom)){
            if (student.courage == student_data[i].courage){
                return 1;
            }
        }
    }
    return 0;
}

//Lecture d'un fichier csv
//Et implémentation des structures contenant les données de celui-ci(voir les structures dans lib.h)
struct Student* csv_to_struct() {

    struct Student* Students;
    char c = 'a';
    char String[200];
    int size_of_file;
    int i;

    FILE* file;
    file = fopen("choixpeauMagique.csv","r");
    fseek(file, 0L, SEEK_END);
    size_of_file = ftell(file);
    rewind(file);

    Students = malloc(size_of_file * 2);
    i = 0;

    fscanf(file, "%[^\n]", String);
    fscanf(file, "%c", &c); 

    while(fscanf(file,"%c",&c) == 1){
    
        fscanf(file,"%[^;]",String);
        strncpy(Students[i].nom,String,strlen(String)-1);
        fscanf(file,"%c",&c);

        fscanf(file,"%d",&Students[i].courage);
        fscanf(file,"%c",&c);

        fscanf(file,"%d",&Students[i].loyaute);
        fscanf(file,"%c",&c);

        fscanf(file,"%d",&Students[i].sagesse);
        fscanf(file,"%c",&c);

        fscanf(file,"%d",&Students[i].malice);
        fscanf(file,"%c",&c);

        fscanf(file,"%c",&c); 
        fscanf(file,"%[^\n]",String); 
        strncpy(Students[i].maison, String,strlen(String)-2);
        fscanf(file,"%c",&c); 
        i++;
    }
    Students[0].length = i;
    fclose(file);
    return Students;
}

//Choisi aléatoirement les représentants des clusters
//renvoi le résultat dans un tableau de struct Student
struct Student* alea_students(struct Student* Students_data, int number_students, int k) {

    struct Student* Students_alea = malloc(sizeof(struct Student)*k);
    int alea = 0;

    srand(time(NULL));

    for (int i = 0; i < k; i++) {
        alea = rand()%number_students;

        if (is_equal(Students_alea, Students_data[alea], i)){
            i--;
            continue;
        }
        else {
            Students_alea[i] = Students_data[alea];
        }
    }
    return Students_alea;
}

//Renvoi la distance entre l'étudiant a et b
int distance(struct Student a, struct Student b) {
    int distance = abs(a.courage-b.courage)
    +abs(a.loyaute-b.loyaute)
    +abs(a.sagesse-b.sagesse)
    +abs(a.malice-b.malice);
    return distance;
}

//Implémentation des k clusters
struct Cluster* impl_cluster(struct Student* Students_data, struct Student* Students_rep, int number_students, int k) {

    struct Cluster* clusters = malloc(sizeof(struct Cluster)*k);
    int new_distance_value;
    int distance_value;
    int index = 0;

    for (int x = 0; x < k; x++){
        clusters[x].obj_rep = Students_rep[x];
        clusters[x].cluster_data[0] = Students_rep[x];
        clusters[x].length = 1;
    }

    for (int i = 0; i < number_students; i++){

        if (is_equal(Students_rep, Students_data[i], k)){
            continue;
        }
        distance_value = 100;

        for (int j = 0; j < k; j++){

            new_distance_value = distance(Students_rep[j], Students_data[i]);

            if (distance_value > new_distance_value){
                distance_value = new_distance_value;
                index = j;
            }
        }
        clusters[index].cluster_data[clusters[index].length] = Students_data[i];
        clusters[index].length++;
    }
    return clusters;
}

//Retourne le coût de la partition (tout les clusters)
int cost_clusters(struct Cluster* clusters, int k) {

    int cout = 0;

    for (int i = 0; i < k; i++) {

        for (int j = 0; j < clusters[i].length; j++) {
            cout = cout + distance(clusters[i].cluster_data[j], clusters[i].obj_rep);
        }
    }
    return cout;
}

// Algo principal: Choisi le meilleur changement qui améliore le coût de la partition
int cost(struct Cluster* clusters, struct Student* Students_data,struct Student* Students_rep,int file_length, int k){

    struct Student old_rep;
    struct Student best_rep;
    int modify_rep;
    int Stop;
    int old_cout;
    int new_cout;
    int cout;

    old_cout = cost_clusters(clusters,k);
    cout = 1000;
    Stop = 0;

    for (int i = 0; i < k; i++) {

        for (int j = 0; j < file_length; j++){

            if (is_equal(Students_rep, Students_data[j], k)){
                continue;
            }

            old_rep = Students_rep[i];
            Students_rep[i] = Students_data[j];
            clusters = impl_cluster(Students_data,Students_rep, file_length, k);
            new_cout = cost_clusters(clusters, k);

            if (new_cout < cout) {
                best_rep = Students_data[j];
                modify_rep = i;
                cout = new_cout;
            }
            Students_rep[i] = old_rep;
            free(clusters);
            clusters = impl_cluster(Students_data,Students_rep, file_length, k);   
        }
    }
    if (cout < old_cout) { 
        Students_rep[modify_rep] = best_rep;
        Stop = 1;
    }
    return Stop;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        exit (EXIT_FAILURE);
    }
    int k = atoi(argv[1]);
    int Stop = 1;

    //Partie 1
    printf("\n\033[0;31m------STUDENT LIST STUDENTS------ \033[0m\n\n");
    struct Student* Students_data = csv_to_struct();
    print_students(Students_data, Students_data[0].length);
    
    printf("\n\033[0;31m------STUDENT ALEA LIST STUDENTS------ \033[0m\n\n");
    struct Student* Students_rep = alea_students(Students_data, Students_data[0].length, k);
    print_students(Students_rep, k);


    //Répéter (partie 2)
    printf("\n\033[0;31m------CLUSTER LIST STUDENTS------ \033[0m\n");

    struct Cluster* Clusters_data = impl_cluster(Students_data, Students_rep, Students_data[0].length, k);
    print_clusters(Clusters_data, k);

    Stop = cost(Clusters_data, Students_data, Students_rep, Students_data[0].length, k);
    int t = 0;

    while (Stop) {
      
        free(Clusters_data);
        Clusters_data = impl_cluster(Students_data, Students_rep, Students_data[0].length, k);
        Stop = cost(Clusters_data, Students_data, Students_rep, Students_data[0].length, k);
        t++;
    }
    printf("\n\033[0;31m------FINAL CLUSTER LIST STUDENTS------ \033[0m\n");
    print_clusters(Clusters_data, k);
    printf("%d\n",t);
    printf("le cout de la partition est de: %d\n",cost_clusters(Clusters_data,k));

    free(Clusters_data);
    free(Students_data);
    free(Students_rep);
}