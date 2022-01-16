struct Student {
    char nom[20];
    int courage;
    int loyaute;
    int sagesse;
    int malice;
    char maison[20];
    int length;
};

struct Cluster {
    struct Student obj_rep;
    struct Student cluster_data[1000];
    int length;
};