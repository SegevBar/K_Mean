#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct 
{
    double* centroid;
    int num_of_points;
    double* sum_of_points;
} Cluster;

/*Prototypes*/


int main(int argc, char *argv[]){
    int hasMaxIter = 1;
    
    FILE *ifp = NULL;
    FILE *ofp = NULL;

    char c;
    double value;
    int N = 0;
    int dim = 1;
    Cluster* clusters;
    double** data_points;
    int same_average = 0;
    int cnt = 0;
    int i = 0;
    int j = 0;
    
    /*check input for enough params*/
    if (argc < 3) {
        printf("\nInvalid Input\n");
        exit(1);
    } else {
        if (argc == 3) {
            hasMaxIter = 0;
        }
    }
    
    /*find max_iter*/
    int max_iter = findMaxIter(hasMaxIter, *argv[]);  
    
    /*find vector dimentions and N*/
    ifp = fopen(argv[2 + hasMaxIter], "r");
    if (ifp != NULL) {
        
    }

    /*find k and check if valid*/ 
    int k = atoi(argv[1]);
    checkK(k, N, *argv[]);

    /*allocate memory*/
    matrixAllocation();
   
    /*Init k clusters*/
    initKClusters();

    /*main loop*/
    mainLoop();

    /*write to output file*/
    
    
    /*free memory*/
    freeMemory(clusters, data_points, k, num_of_points);

    return 0;
}

int findMaxIter(int hasMaxIter, char *argv[]) {
    int max_iter = 0;

    /*chack if max_iter is default or param*/
    if !(hasMaxIter) { 
        max_iter = 200;
    } else {
        if ((atoi(argv[2]) <= 0) || (strchr(argv[2], '.') != NULL)) {
            printf("\nInvalid Input!\n");
            exit(1);
        }
        max_iter = atoi(argv[2]);
    }
    return max_iter;
}

int findDimension() {
    int dim = 1;

    while(scanf("%lf%c", &value, &c) == 2){
        if((c==',') && num_of_points == 0){
            dimension++;
        }
        if(c == '\n'){
            num_of_points++;
        }
    }
    rewind(stdin);

    return dim;
}

int findN() {
    int N = 0;
    while(scanf("%lf%c", &value, &c) == 2){
        if((c==',') && num_of_points == 0){
            dimension++;
        }
        if(c == '\n'){
            num_of_points++;
        }
    }
    rewind(stdin);

    data_points = (double**) calloc(num_of_points, sizeof(*data_points));
    if (data_points == NULL) {
        printf("\nAn Error Has Occurred\n");
        exit(1);
    }
    return N;
}

void checkK(int k, int N, char *argv[]) {
    if ((N < k) || (k < 0) || (strchr(argv[1], '.') != NULL)) {
        printf("\nInvalid Input!\n");
        exit(1);
    }
}

void matrixAllocation() {
    for(i = 0; i < num_of_points; i++){
        data_points[i] = (double*) calloc(dimension,sizeof(*data_points[i]));
        assert(data_points[i] != NULL);
    }
    
   for(i = 0; i < num_of_points; i++){
       for(j = 0; j < dimension; j++){
           scanf("%lf%c", &value, &c);
           data_points[i][j] = value;
       }
   }
}

void initKClusters() {
    clusters = (Cluster*)calloc(k,sizeof(struct Cluster));
    for(i = 0; i < k; i++){
        clusters[i].centroid = (double*)calloc(dimension, sizeof(double));
        assert(clusters[i].centroid != NULL);

        memcpy(clusters[i].centroid, data_points[i], sizeof(double)*dimension); /*will be equal to the i'th vector
        */
        clusters[i].num_of_points = 0;
        clusters[i].sum_of_points = (double*)calloc(dimension, sizeof(double));
        assert(clusters[i].sum_of_points != NULL);
    }
}

void mainLoop() {
    cnt = 0;
    while ((cnt < max_iter) && (!same_average)) {
        same_average = 1;
        for(i = 0; i < num_of_points; i++){
            calcCluster(data_points[i], clusters, k, dimension);
        }
        
        same_average = updateMean(clusters, same_average, k, dimension);
        if(same_average == 1){
            break;
        }

        for(i = 0; i < k; i++){
            clusters[i].num_of_points = 0;
            for(j = 0; j < dimension; j++){
                clusters[i].sum_of_points[j] = 0;
            }
        }
        cnt++;
    }
}

void calcCluster(double* vector, Cluster* clusters, int k, int dimension){
    double min_distance = -1.0;
    int num_of_cluster = -1;
    double distance;
    int i = 0;

    for(i = 0; i < k; i++){
        distance = calcEuclidianDistance(vector, clusters[i].centroid, dimension);
        if((distance < min_distance) || (min_distance < 0)){
            min_distance = distance;
            num_of_cluster = i;
        }
    }
    clusters[num_of_cluster].num_of_points++;
    updateClusterSum(vector, num_of_cluster, clusters, dimension);
}

void updateClusterSum(double* vector, int loc, Cluster* clusters, int dimension){
    int i = 0;
    for(i = 0; i < dimension; i++){
        clusters[loc].sum_of_points[i] += vector[i];
    }
}

int updateMean(Cluster* clusters, int same_average, int k, int dimension){
    int i = 0;
    int j = 0;
    for(i = 0; i < k; i++){
        for(j = 0; j < dimension; j++){
            if((clusters[i].sum_of_points[j]/clusters[i].num_of_points)!=
                clusters[i].centroid[j]){
                    same_average = 0;
                    clusters[i].centroid[j] = clusters[i].sum_of_points[j]/clusters[i].num_of_points;
                }
        }
    }
    return same_average;
}

double calcEuclidianDistance(double* vector1, double* centroid, int dimension){
    double sum = 0.0;
    int xi = 0;
    for(xi = 0; xi < dimension; xi++){
        sum += (vector1[xi]-centroid[xi])*(vector1[xi]-centroid[xi]);
    } 
    return sum;
}

void freeMemory(Cluster* clusters, double** data_points, int k, int num_of_points){
    int i = 0;
    int j = 0;
    for(i = 0; i < num_of_points; i++){
        free(data_points[i]);
    }
    free(data_points);

    for(j = 0; j < k; j++){
        free(clusters[j].centroid);
        free(clusters[j].sum_of_points);
    }
    free(clusters);
}