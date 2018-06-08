#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

double ran01(long *idum);

long int random_number(long *idum);

long int * generate_random_permutation(long int n);

int ** asymmetricToSymmetric(double **distance, int n);

int ** generate_2D_matrix_int(int n, int m);

double ** generate_2D_matrix_double(int n, int m);