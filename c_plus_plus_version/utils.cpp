#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include "utils.h"

double ran01(long *idum)
{
	long k;
	double ans;

	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0) *idum += IM;
	ans = AM * (*idum);
	return ans;
}


long int random_number(long *idum)
{
	long k;

	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0) *idum += IM;
	return *idum;
}

long int * generate_random_permutation(long int n) {
	long int  i, help, node, tot_assigned = 0;
	double    rnd;
	long int  *r;

	r = (long int *)malloc(n * sizeof(long int));

	for (i = 0; i < n; i++)
		r[i] = i;

	for (i = 0; i < n; i++) {
		/* find (randomly) an index for a free unit */
		rnd = ((double)(rand() % 10000) / 10000.0);
		node = (long int)(rnd  * (n - tot_assigned));
		assert(i + node < n);
		help = r[i];
		r[i] = r[i + node];
		r[i + node] = help;
		tot_assigned++;
	}
	return r;
}

/****************************************************************/
/*Generate and return a two-dimension array of type int         */
/****************************************************************/
int ** generate_2D_matrix_int(int n, int m) {
	int **matrix;
	matrix = new int*[n];
	for (int i = 0; i < n; i++) {
		matrix[i] = new int[m];
	}
	//initialize 2-d array
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			matrix[i][j] = 0;
		}
	}
	return matrix;
}

/****************************************************************/
/*Generate and return a two-dimension array of type double      */
/****************************************************************/
double ** generate_2D_matrix_double(int n, int m) {
	double **matrix;

	matrix = new double*[n];
	for (int i = 0; i < n; i++) {
		matrix[i] = new double[m];
	}
	//initialize the 2-d array
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			matrix[i][j] = 0.0;
		}
	}
	return matrix;
}

int ** asymmetricToSymmetric(double ** distance, int n)
{
	// Create new symmetric matrix
	int NS = 2 * n;
	int **matrix;
	matrix = generate_2D_matrix_int(NS, NS);
	// Compute M (min) e INF (max) values
	double M = 9999999999;
	double INF = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				if (distance[i][j] < M) M = distance[i][j];
				if (distance[i][j] > INF) INF = distance[i][j];
			}
		}
	}
	// Convert to symmetric matrix
	// Define U'
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			matrix[i][j] = INF; // Inf+
		}
	}

	// Define C' (Transpose of C)
	for (int i = 0; i < n; i++) {
		for (int j = n; j < 2 * n; j++) {
			if (i == j - n) {
				matrix[i][j] = -M; // -M
			}
			else {
				matrix[i][j] = distance[j - n][i];
			}
		}
	}

	// Define C
	for (int i = n; i < 2 * n; i++) {
		for (int j = 0; j < n; j++) {
			if (i - n == j) {
				matrix[i][j] = -M; // -M
			}
			else {
				matrix[i][j] = distance[i - n][j];
			}
		}
	}

	// Define U"
	for (int i = n; i < 2 * n; i++) {
		for (int j = n; j < 2 * n; j++) {
			matrix[i][j] = INF; // Inf+
		}
	}
	return matrix;
}
