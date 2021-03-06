/**************************************************************************************/
/* ACODBGP.cpp                                                                        */
/* This is an implementation of the MMAS with US operator for the                     */
/* DTSP with weight changes of the following paper:                                   */
/*                                                                                    */
/*   M. Mavrovouniotis, F. M. Müller, S. Yang. An ant colony optimization with        */
/*      local search for dynamic travelling salesman problems. IEEE Transactions on   */
/*      Cybernetics, accepted 16 April 2016                                           */
/*                                                                                    */
/* Compile:                                                                           */
/*   g++ -o ACODBGP ACODBGP.c                                                         */
/* Run:                                                                               */
/*   ./ACODBGP problem_instance change_mode change_degree change_speed                */
/*                                                                                    */
/*   e.g., ./ACODTSP kroA100.tsp 0.25 100                                             */
/*                                                                                    */
/* Written by:                                                                        */
/*   Michalis Mavrovouniotis                                                          */
/*                                                                                    */
/* If any query, please email Michalis Mavrovouniotis at mmavrovouniotis@dmu.ac.uk    */
/*                                                                                    */
/**************************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <math.h>
#include <fstream>
#include <cmath>
#include <time.h>
#include <limits.h>
#include "coordG.h"
#include "routeG.h"
#include "ls.h"
#include "utils.h"

using namespace std;

/* Declarations for GENIUS utilization */
CoordG tsp_file;
RouteG genius;
tourneelem *pri;

/*------------------DTSP with Traffic Factors Implementation-------------------------*/
#define EPSILON  0.000000000000000000000001
#define INFTY    INT_MAX
#define CHAR_LEN 100

struct object {
	int id;
	double x;
	double y;
};

struct env_struct {
	int id;
	double **base;
};

struct object *init_objects;     //Actual city list of the instance
struct env_struct *cyclic_bases; //Store enviornments for cyclic
double **distances;                 //Distance matrix
double **traffic_factors;        //Traffic factors matrix
double lambda_l = 0.0;             //Lower bound of traffic
double lambda_u = 2.0;             //Upper bound of traffic

int problem_size;                //Size of the instance
char* problem_instance;          //Name of the instance

double change_degree;            //Degree of changes
int change_speed;                //Peiord of changes in algorithmic iterations

int current_iteration;           //Used to calculate the period of change
int seed;                        //changing seed for the algorithms
int max_iterations;
int max_trials;
int local_search;				 // 1:us 2:2-opt 3:3-opt 4:res-3-opt

//Used to output offline performance and population diversity
int** performance;
double** diversity;

FILE *log_performance;
FILE *log_diversity;
FILE *log_varying_values;

//output files
char * perf_filename;
char * div_filename;
char * var_filename;


/****************************************************************/
/*Random number generator where the seed is the same in all runs*/
/****************************************************************/
double env_random_number(double low, double high) {
	return ((double)(rand() % 10000) / 10000.0)*(high - low) + low;
}

/****************************************************************/
/*           Initialize all traffic factors equally             */
/****************************************************************/
void initialize_traffic_factors() {
	int i, j;
	for (i = 0; i < problem_size; i++) {
		for (j = 0; j <= i; j++) {
			traffic_factors[i][j] = 1.0;
			traffic_factors[j][i] = traffic_factors[i][j];
		}
	}
}

/****************************************************************/
/*Compute and return the euclidean distance of two cities       */
/****************************************************************/
int euclidean_distance(int i, int j) {
	double xd, yd;
	int r = 0;
	xd = init_objects[i].x - init_objects[j].x;
	yd = init_objects[i].y - init_objects[j].y;
	r = sqrt(xd*xd + yd * yd) + 0.5;
	return r;
}

/****************************************************************/
/*Compute the distance matrix of the problem instance           */
/****************************************************************/
void compute_distances(void) {
	int i, j;
	for (i = 0; i < problem_size; i++) {
		for (j = 0; j < problem_size; j++) {
			distances[i][j] = (int)euclidean_distance(i, j) * traffic_factors[i][j];
			// cout << distances[i][j] << endl;
		}
	}
}

/****************************************************************/
/* Perform random changes by changing the distances between     */
/* cities                                                       */
/****************************************************************/
void add_random_change(void) {
	int i, j;
	for (i = 0; i < problem_size; i++) {
		for (j = 0; j < problem_size; j++) {
			if (env_random_number(0.0, 1.0) <= change_degree) {
				traffic_factors[i][j] = 1.0 + env_random_number(lambda_l, lambda_u);
				// traffic_factors[j][i] =  traffic_factors[i][j];
			}
			else {
				traffic_factors[i][j] = 1.0;
				// traffic_factors[j][i] = traffic_factors[i][j];
			}
		}
	}
}


/****************************************************************/
/* Read the problem instance and generate the city list         */
/****************************************************************/
void read_problem(char *filename) {
	char line[CHAR_LEN];
	char * keywords;
	char Delimiters[] = " :=\n\t\r\f\v";
	ifstream fin(filename);
	while ((fin.getline(line, CHAR_LEN - 1))) {
		if (!(keywords = strtok(line, Delimiters)))
			continue;
		//keywords = strupr(keywords);
		if (!strcmp(keywords, "DIMENSION")) {
			if (!sscanf(strtok(NULL, Delimiters), "%d", &problem_size)) {
				cout << "DIMENSION error" << endl;
				exit(0);
			}
		}
		else if (!strcmp(keywords, "EDGE_WEIGHT_TYPE")) {
			char * tempChar;
			if (!(tempChar = strtok(NULL, Delimiters))) {
				cout << "EDGE_WEIGHT_TYPE error" << endl;
				exit(0);
			}
			if (strcmp(tempChar, "EUC_2D")) {
				cout << "not EUC_2D" << endl;
				exit(0);
			}
		}
		else if (!strcmp(keywords, "NODE_COORD_SECTION")) {
			init_objects = new object[problem_size];
			int i;
			for (i = 0; i < problem_size; i++) {
				//store initial cities
				fin >> init_objects[i].id;
				fin >> init_objects[i].x >> init_objects[i].y;
				init_objects[i].id -= 1;
			}
		}
	}
	fin.close();

}

/****************************************************************/
/* Initialize the environment and perform the initial dynamic   */
/*  change/generate base states                                 */
/****************************************************************/
void initialize_environment() {
	//initialize distances with traffic factors
	distances = generate_2D_matrix_double(problem_size, problem_size);
	traffic_factors = generate_2D_matrix_double(problem_size, problem_size);
	initialize_traffic_factors();

	add_random_change();

	compute_distances();
	if (local_search == 1) {
		tsp_file.distances();
	}
}

/****************************************************************/
/* Perform the dynamic change every "period" iteration          */
/****************************************************************/
void change_environment() {

	add_random_change();

	compute_distances();
	if (local_search == 1) {
		tsp_file.distances();
	}
}

/****************************************************************/
/* Validate the values input by the user                        */
/****************************************************************/
void check_input_parameters() {

	if (problem_size == 0) {
		cout << "wrong problem instance file" << endl;
		exit(2);
	}

}

/****************************************************************/
/* Evaluate a TSP tour and return the length. This method can   */
/* be used in the optimizer integrated with DTSP. The TSP tour  */
/* to input is an array of integers, where each integer         */
/* corresponds to a city index                                  */
/****************************************************************/
int fitness_evaluation(int *t) {
	int i, tour_length = 0;
	for (i = 0; i < problem_size; i++) {
		tour_length += distances[t[i]][t[i + 1]];
	}
	return tour_length;
}
/*------------------------End of DTSP---------------------------*/







/*---------------------------MMAS_US----------------------------------*/
/* ACO methods are based on the original ACO framework implementation */
/*        Thomas Stuetzle. ACOTSP, Version 1.02. Available from       */
/*            http://www.aco-metaheuristic.org/aco-code, 2004.        */
/*--------------------------------------------------------------------*/

#define IA       16807
#define IM       2147483647
#define AM       (1.0/IM)
#define IQ       127773
#define IR       2836
#define MASK     123459876

//Ant or individual structure that represent the TSP tour and tour cost
struct ant {
	int *tour;
	bool *visited;
	int tour_length;
};

struct ant *ant_population;  //Population of ants or individuals
ant *best_so_far_ant;        //current best so far tour
ant *restart_best_ant;       //current best so far tour
ant *previous_best_so_far_ant;//current-1 best so far tour

double **pheromone;          //Pheromone matrix
double **heuristic;          //Heuristic information matrix
double **total;              //Pheromone + Heuristic information matrix

double *prob_of_selection;   //Selection probabilities

							 //General ACO parameters
double alpha;
double beta;
double q_0;
double trail0;
double rho;

double trail_max;       /* maximum pheromone trail in MMAS */
double trail_min;       /* minimum pheromone trail in MMAS */
int u_gb = INFTY;       /* every u_gb iterations update with best-so-far ant */
int found_best;
int restart_found_best;
int restart_iteration;

double found_branching;
double lambda;
double branching_factor;
double branch_fac = 1.00001;

bool ls_flag = true;     /* indicates whether and which local search is used */

int n_ants;                //Population size
int depth;                 //Candidate list size (nearest neighbour)
int **nn_list;             //Candidate lists


						   /****************************************************************/
						   /*                     Initialization                           */
						   /****************************************************************/
void set_algorithm_parameters(void) {
	//change the algorithm parameters according to your needs
	alpha = 1;
	beta = 5;
	q_0 = 0.0;
	//rho = 0.2;
	n_ants = 50;
	depth = 20;
}

void allocate_ants(void) {
	int i;

	ant_population = new ant[n_ants];
	for (i = 0; i < n_ants; i++) {
		ant_population[i].tour = new int[problem_size + 1];
		ant_population[i].visited = new bool[problem_size];
	}

	best_so_far_ant = new ant;
	best_so_far_ant->tour = new int[problem_size + 1];
	best_so_far_ant->visited = new bool[problem_size];

	restart_best_ant = new ant;
	restart_best_ant->tour = new int[problem_size + 1];
	restart_best_ant->visited = new bool[problem_size];

	prob_of_selection = new double[depth + 1];

	previous_best_so_far_ant = new ant;
	previous_best_so_far_ant->tour = new int[problem_size + 1];
	previous_best_so_far_ant->visited = new bool[problem_size];
}

void allocate_structures(void) {
	int i;

	pheromone = generate_2D_matrix_double(problem_size, problem_size);
	heuristic = generate_2D_matrix_double(problem_size, problem_size);
	total = generate_2D_matrix_double(problem_size, problem_size);
	nn_list = generate_2D_matrix_int(problem_size, depth);
}

void swap(int v[], int v2[], int i, int j) {
	int tmp;

	tmp = v[i];
	v[i] = v[j];
	v[j] = tmp;
	tmp = v2[i];
	v2[i] = v2[j];
	v2[j] = tmp;
}

void sort(int v[], int v2[], int left, int right) {
	int k, last;

	if (left >= right)
		return;
	swap(v, v2, left, (left + right) / 2);
	last = left;
	for (k = left + 1; k <= right; k++)
		if (v[k] < v[left])
			swap(v, v2, ++last, k);
	swap(v, v2, left, last);
	sort(v, v2, left, last);
	sort(v, v2, last + 1, right);
}

void compute_nn_lists(void) {
	int i, j;
	int *distance_vector;
	int *help_vector;

	distance_vector = new int[problem_size];
	help_vector = new int[problem_size];
	//compute the nearest neigbhours of the objects
	for (j = 0; j < problem_size; j++) {
		for (i = 0; i < problem_size; i++) {
			distance_vector[i] = distances[j][i];
			help_vector[i] = i;
		}
		distance_vector[j] = INT_MAX;
		sort(distance_vector, help_vector, 0, problem_size - 1);
		for (i = 0; i < depth; i++) {
			nn_list[j][i] = help_vector[i];

		}
	}
	//free memory
	delete[] distance_vector;
	delete[] help_vector;
}

void init_pheromone_trails(double initial_trail) {
	int i, j;

	for (i = 0; i < problem_size; i++) {
		for (j = 0; j <= i; j++) {
			pheromone[i][j] = initial_trail;
			pheromone[j][i] = initial_trail;
		}
	}
}

void init_heuristic_info(void) {
	int i, j;

	for (i = 0; i < problem_size; i++) {
		for (j = 0; j < problem_size; j++) {
			heuristic[i][j] = 1.0 / (double)(distances[i][j] + EPSILON); //small value to avoid 1 div 0
																		 // heuristic[j][i] = heuristic[i][j];
		}
	}
}

void compute_total_info(void) {
	int i, j;

	for (i = 0; i < problem_size; i++) {
		for (j = 0; j < problem_size; j++) {
			total[i][j] = pow(pheromone[i][j], alpha) * pow(heuristic[i][j], beta);
			// total[j][i] = total[i][j];
		}
	}
}

/****************************************************************/
/*                    Construct Solutions                       */
/****************************************************************/
void ant_empty_memory(ant *a) {
	int i;
	//clear previous ant solution
	for (i = 0; i < problem_size; i++) {
		a->visited[i] = false;
	}
}

double alg_random_number(int *idum) {
	int k;
	double ans;
	//uniformly distributed random number [0,1]
	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0) *idum += IM;
	ans = AM * (*idum);
	return ans;
}

void place_ant(ant *a, int step) {
	int rnd;
	//place ants to randomly selected cities
	rnd = (int)(alg_random_number(&seed) * (double)problem_size);
	a->tour[step] = rnd;
	a->visited[rnd] = true;
}

void choose_best_next(ant *a, int phase) {
	int i, current, next;
	double value_best;

	next = problem_size;
	current = a->tour[phase - 1]; //current object of ant
	value_best = -1.0;  //values in the list are always >=0.0
						//choose the next object with maximal (pheromone+heuristic) value
						//among all objects of the problem
	for (i = 0; i < problem_size; i++) {
		if (a->visited[i] == false) {
			//if object not visited
			if (total[current][i] > value_best) {
				next = i;
				value_best = total[current][i];
			}
		}
	}
	a->tour[phase] = next;
	a->visited[next] = true;
}

void neighbour_choose_best_next(ant *a, int phase) {
	int i, current, next, temp;
	double value_best, help;

	next = problem_size;
	current = a->tour[phase - 1]; //current object of ant
	value_best = -1.0; //values in the list are always >=0.0
					   //choose the next object with maximal (pheromone+heuristic) value
					   //among all the nearest neighbour objects
	for (i = 0; i < depth; i++) {
		temp = nn_list[current][i];
		if (a->visited[temp] == false) {
			//if object not visited
			help = total[current][temp];
			if (help > value_best) {
				value_best = help;
				next = temp;
			}
		}
	}
	if (next == problem_size) {
		//if all nearest neighnour objects are already visited
		choose_best_next(a, phase);
	}
	else {
		a->tour[phase] = next;
		a->visited[next] = true;
	}
}

void neighbour_choose_and_move_to_next(ant *a, int phase) {
	int i, help, current, select;
	double rnd;
	double partial_sum = 0.0;
	double sum_prob = 0.0;
	double *prob_ptr;

	if ((q_0 > 0.0) && (alg_random_number(&seed) < q_0)) {
		//with probability q_0 make the best possible choice
		neighbour_choose_best_next(a, phase);
		return;
	}

	prob_ptr = prob_of_selection; //selection probabilities of the nearest neigbhour objects
	current = a->tour[phase - 1]; //current object
								  //compute selection probabilities of nearest neigbhour objects
	for (i = 0; i < depth; i++) {
		if (a->visited[nn_list[current][i]]) {
			prob_ptr[i] = 0.0;
		}
		else {
			prob_ptr[i] = total[current][nn_list[current][i]];
			sum_prob += prob_ptr[i];
		}
	}
	if (sum_prob <= 0.0) {
		//in case all neigbhbour objects are visited
		choose_best_next(a, phase);
	}
	else {
		//proabilistic selection (roullete wheel)
		rnd = alg_random_number(&seed);
		rnd *= sum_prob;
		select = 0;
		partial_sum = prob_ptr[select];
		while (partial_sum <= rnd) {
			select++;
			partial_sum += prob_ptr[select];
		}
		//this may very rarely happen because of rounding if
		//rnd is close to 1
		if (select == depth) {
			neighbour_choose_best_next(a, phase);
			return;
		}
		help = nn_list[current][select];
		a->tour[phase] = help;
		a->visited[help] = true;
	}
}

void construct_solutions(void) {
	int k, step;
	compute_nn_lists();
	init_heuristic_info();
	compute_total_info();
	//clear memory of ants
	for (k = 0; k < n_ants; k++) {
		ant_empty_memory(&ant_population[k]);
	}
	step = 0;
	//place ants on a random object
	for (k = 0; k < n_ants; k++) {
		place_ant(&ant_population[k], step);
	}
	//select object until all objects are visited
	while (step < problem_size - 1) {
		step++;
		for (k = 0; k < n_ants; k++) {
			neighbour_choose_and_move_to_next(&ant_population[k], step);
		}
	}
	step = problem_size;
	for (k = 0; k < n_ants; k++) {
		//close TSP tour, i.e., the first object needs to be identical with the last one.
		ant_population[k].tour[problem_size] = ant_population[k].tour[0];
		ant_population[k].tour_length = fitness_evaluation(ant_population[k].tour);//evalute
	}
}

void choose_closest_next(ant *a, int phase) {
	int i, current, next, min;
	next = problem_size;
	current = a->tour[phase - 1]; //current object of ant
	min = INFTY;
	//choose closest object used in the nn_tour()
	for (i = 0; i < problem_size; i++) {
		if (a->visited[i] == false) {
			//if object not visited
			if (distances[current][i] < min) {
				next = i;
				min = distances[current][i];
			}
		}
	}
	a->tour[phase] = next;
	a->visited[next] = true;
}

int nn_tour(void) {
	int phase, help;
	phase = help = 0;
	ant_empty_memory(&ant_population[0]);
	place_ant(&ant_population[0], phase);
	//compute the tour length of the nearest neigbour heuristic
	//used to initialize the pheromone trails
	while (phase < problem_size - 1) {
		phase++;
		choose_closest_next(&ant_population[0], phase);
	}
	phase = problem_size;
	ant_population[0].tour[problem_size] = ant_population[0].tour[0];
	ant_population[0].tour_length = fitness_evaluation(ant_population[0].tour);
	help = ant_population[0].tour_length;
	ant_empty_memory(&ant_population[0]);

	return help;
}

/****************************************************************/
/*                    Pheromone Update                          */
/****************************************************************/
void check_pheromone_trail_limits(void) {
	int    i, j;

	for (i = 0; i < problem_size; i++) {
		for (j = 0; j < problem_size; j++) {
			if (pheromone[i][j] < trail_min) {
				pheromone[i][j] = trail_min;
				//pheromone[j][i] = trail_min;

			}
			else if (pheromone[i][j] > trail_max) {
				pheromone[i][j] = trail_max;
				//pheromone[j][i] = trail_max;

			}
		}
	}
}

void global_pheromone_deposit(ant *a) {
	int i, j, h;
	double d_tau;
	d_tau = 1.0 / (double)a->tour_length;

	for (i = 0; i < problem_size; i++) {
		j = a->tour[i];
		h = a->tour[i + 1];
		pheromone[j][h] += d_tau;
		// pheromone[h][j] = pheromone[j][h];
	}
}

double node_branching(double l) {
	int  i, m;
	double min, max, cutoff;
	double avg;
	double *num_branches = new double[problem_size];

	for (i = 0; i < problem_size; i++) {
		num_branches[i] = 0.0;
	}

	for (m = 0; m < problem_size; m++) {
		/* determine max, min to calculate the cutoff value */
		min = pheromone[m][nn_list[m][1]];
		max = pheromone[m][nn_list[m][1]];
		for (i = 1; i < depth; i++) {
			if (pheromone[m][nn_list[m][i]] > max)
				max = pheromone[m][nn_list[m][i]];
			if (pheromone[m][nn_list[m][i]] < min)
				min = pheromone[m][nn_list[m][i]];
		}
		cutoff = min + l * (max - min);

		for (i = 0; i < depth; i++) {
			if (pheromone[m][nn_list[m][i]] > cutoff)
				num_branches[m] += 1.0;
		}
	}
	avg = 0.0;
	for (m = 0; m < problem_size; m++) {
		avg += num_branches[m];
	}
	/* Norm branching factor to minimal value 1 */
	return (avg / (double)(problem_size * 2));


	delete[] num_branches;
}

void evaporation(void) {
	int i, j;


	for (i = 0; i < problem_size; i++) {
		for (j = 0; j < problem_size; j++) {
			pheromone[i][j] = (1.0 - rho) * pheromone[i][j];
			// pheromone[j][i] = pheromone[i][j];
		}
	}
}



int find_best(void) {
	int k, min, k_min;
	min = ant_population[0].tour_length;
	k_min = 0;
	for (k = 1; k < n_ants; k++) {
		if (ant_population[k].tour_length < min) {
			min = ant_population[k].tour_length;
			k_min = k;
		}
	}
	return k_min; //population best ant index
}

/*
* Add conversion from asymmetric matrix to symmetric matrix to execute 3-opt and 2-opt
*/
void lsNopt(ant *a, int opt) {
	int opt_problem_size = problem_size;
	bool symmetric = true;
	int **matrix;
	long int *tour_opt;
	int tour_length_opt;

	// Check symmetrie
	for (int i = 0; i < opt_problem_size; i++) {
		for (int j = i; j < opt_problem_size; j++) {
			if (distances[j][i] != distances[i][j]) {
				symmetric = false;
				break;
			}
		}
	}

	if (symmetric) {
		// Just copy the values
		matrix = generate_2D_matrix_int(opt_problem_size, opt_problem_size);
		for (int i = 0; i < opt_problem_size; i++) {
			for (int j = 0; j < opt_problem_size; j++) {
				matrix[i][j] = distances[i][j];
			}
		}
		tour_length_opt = opt_problem_size + 1;
		tour_opt = new long int[tour_length_opt];
		for (int i = 0; i < tour_length_opt; i++) {
			tour_opt[i] = (long int)a->tour[i];
		}
	}
	else {
		// Adapt the problem to symmetric problem
		matrix = asymmetricToSymmetric(distances, opt_problem_size);
		tour_length_opt = 2 * opt_problem_size + 1;
		tour_opt = new long int[tour_length_opt];
		for (int i = 0; i < tour_length_opt; i++) {
			if (i % 2 == 0) {
				tour_opt[i] = a->tour[i / 2];
			}
			else {
				tour_opt[i] = opt_problem_size + tour_opt[i - 1];
			}
		}
		opt_problem_size = tour_length_opt - 1;
	}

	//compute the nearest neigbhours of the objects
	int **nn_list_opt = generate_2D_matrix_int(opt_problem_size, depth);
	int i, j;
	int *distance_vector;
	int *help_vector;
	distance_vector = new int[opt_problem_size];
	help_vector = new int[opt_problem_size];
	for (j = 0; j < opt_problem_size; j++) {
		for (i = 0; i < opt_problem_size; i++) {
			distance_vector[i] = matrix[j][i];
			help_vector[i] = i;
		}
		distance_vector[j] = INT_MAX;
		sort(distance_vector, help_vector, 0, opt_problem_size - 1);
		for (i = 0; i < depth; i++) {
			nn_list_opt[j][i] = help_vector[i];
		}
	}
	if (opt == 2) {
		two_opt_first(tour_opt, opt_problem_size, matrix, nn_list_opt, depth);
	}
	else if (opt == 3) {
		three_opt_first(tour_opt, opt_problem_size, matrix, nn_list_opt, depth);
	}
	int p = 0;
	for (int i = 0; i < tour_length_opt; i++) {
		if (tour_opt[i] < problem_size) {
			a->tour[p] = tour_opt[i];
			p++;
		}
	}
	a->tour_length = fitness_evaluation(a->tour);
	//free memory
	delete[] distance_vector;
	delete[] help_vector;
	for (int i = 0; i < opt_problem_size; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
	delete[] tour_opt;
	for (i = 0; i < depth; i++) {
		delete[] nn_list_opt[i];
	}
	delete[] nn_list_opt;
}

void geni(ant *a) {
	genius.initialize();
	genius.initnneighbour(tsp_file.task);
	// Take the best ant for the considered interation;
	genius.petittourne(a->tour[0], tsp_file.g); //initial
	genius.ajoutenoeudprox(a->tour[0], tsp_file.task, tsp_file.d);
	for (int i = 1; i < problem_size; i++) {
		genius.ajoute_a_tourne(a->tour[i], tsp_file.g);
		genius.ajoutenoeudprox(a->tour[i], tsp_file.task, tsp_file.d);
	} //copy the tour and calculates the nearest neighbours of the nodes.

	  //if (!genius.numerote_tourne()) return 1;
	if (!genius.numerote_tourne()) {
		//do nothing
	}
	else {
		//viz = MAXK;
		genius.route_copy(tsp_file.task, tsp_file.g, tsp_file.d, a->tour_length);
		//genius.showroute(problem_size);
		//COPY THE RESULTING TOUR
		pri = genius.t.ptr;
		for (int i = 0; i < problem_size; i++) {
			a->tour[i] = pri->noeud - 1;
			//cout << "ant[" << i << "]= " << ant_population[best_sol].tour[i] << "\n";
			pri = pri->prochain;
		}
		a->tour[problem_size] = a->tour[0];
	}
}

void mmas_pheromone_update(void) {

	int iteration_best;

	if (current_iteration%u_gb) {
		iteration_best = find_best();

		//geni(&ant_population[iteration_best]);
		//cout << "iteration_best" << endl;
		global_pheromone_deposit(&ant_population[iteration_best]);
	}
	else {
		if (u_gb == 1 && (current_iteration - restart_found_best > 50)) {
			global_pheromone_deposit(best_so_far_ant);
		}
		else {
			global_pheromone_deposit(restart_best_ant);
		}
		// cout << "best_so_far" << endl;
	}
	if (ls_flag == true) {
		if ((current_iteration - restart_iteration) < 25)
			u_gb = 25;
		else if ((current_iteration - restart_iteration) < 75)
			u_gb = 5;
		else if ((current_iteration - restart_iteration) < 125)
			u_gb = 3;
		else if ((current_iteration - restart_iteration) < 250)
			u_gb = 2;
		else
			u_gb = 1;
	}
	else
		u_gb = 25;

}


void pheromone_update(void) {

	evaporation();
	mmas_pheromone_update();
	check_pheromone_trail_limits();

	//compute_total_info(); //heuristic info + pheromone trails
}

/****************************************************************/
/*                    Update Best Ants                          */
/****************************************************************/


void copy_from_to(ant *a1, ant *a2) {
	int i;
	//ant2 is a copy of ant1
	a2->tour_length = a1->tour_length;
	for (i = 0; i < problem_size; i++) {
		a2->tour[i] = a1->tour[i];
	}
	a2->tour[problem_size] = a2->tour[0];
}

void update_best(void) {
	int iteration_best = find_best();
	double p_x;

	if (ant_population[iteration_best].tour_length < best_so_far_ant->tour_length) {

		//if new best found then apply local search
		if (ls_flag == true) {
			//cout << "before ls: " << ant_population[iteration_best].tour_length << endl;
			if (local_search == 1) {
				geni(&ant_population[iteration_best]);
				ant_population[iteration_best].tour_length = fitness_evaluation(ant_population[iteration_best].tour);
			}
			else if (local_search == 2 || local_search == 3) {
				lsNopt(&ant_population[iteration_best], local_search);
			}
			else if (local_search == 4) {
				three_opt_res(ant_population[iteration_best].tour, distances, problem_size);
				ant_population[iteration_best].tour_length = fitness_evaluation(ant_population[iteration_best].tour);
			}
			//cout << "after ls: "<<ant_population[iteration_best].tour_length << endl;
			//cout << "local search improvement" << endl;

		}
		copy_from_to(&ant_population[iteration_best], best_so_far_ant);
		copy_from_to(&ant_population[iteration_best], restart_best_ant);
		// copy_from_to(&ant_population[iteration_best],restart_best_ant);

		found_best = current_iteration;
		restart_found_best = current_iteration;
		found_branching = node_branching(lambda);
		branching_factor = found_branching;
		if (!ls_flag) {
			p_x = exp(log(0.05) / problem_size);
			trail_min = 1.0 * (1.0 - p_x) / (p_x * (double)((depth + 1) / 2.0));
			trail_max = 1.0 / ((rho)* best_so_far_ant->tour_length);
			trail0 = trail_max;
			trail_min = trail_max * trail_min;
		}
		else {
			trail_max = 1. / ((rho)* best_so_far_ant->tour_length);
			trail_min = trail_max / (2. * problem_size);
			trail0 = trail_max;
		}
	}
	if (ant_population[iteration_best].tour_length < restart_best_ant->tour_length) {
		copy_from_to(&ant_population[iteration_best], restart_best_ant);
		restart_found_best = current_iteration;
		cout << "restart best: " << restart_best_ant->tour_length << " restart_found_best: " << restart_found_best << endl;
	}
}

/****************************************************************/
/*                Update Statistics and Output                  */
/****************************************************************/
void open_stats(void) {
	performance = generate_2D_matrix_int(max_trials, max_iterations);
	diversity = generate_2D_matrix_double(max_trials, max_iterations);
	//initialize and open output files
	perf_filename = new char[CHAR_LEN];
	sprintf(perf_filename, "results\\Performance_D_%.2f_S_%d_Evap_%.2f_%s_ls_%d.txt",
		change_degree, change_speed, rho, problem_instance, local_search);
	//for performance
	if ((log_performance = fopen(perf_filename, "a")) == NULL) { exit(2); }

	div_filename = new char[CHAR_LEN];
	sprintf(div_filename, "results\\Diversity_D_%.2f_S_%d_Evap_%.2f_%s_ls_%d.txt",
		change_degree, change_speed, rho, problem_instance, local_search);
	//for diversity
	if ((log_diversity = fopen(div_filename, "a")) == NULL) { exit(2); }

}

double mean(double* values, int size) {
	int i;
	double m = 0.0;
	for (i = 0; i < size; i++) {
		m += values[i];
	}
	m = m / (double)size;
	return m; //mean
}

double stdev(double* values, int size, double average) {
	int i;
	double dev = 0.0;

	if (size <= 1)
		return 0.0;
	for (i = 0; i < size; i++) {
		dev += ((double)values[i] - average) * ((double)values[i] - average);
	}
	return sqrt(dev / (double)(size - 1)); //standard deviation
}

double distance_between(ant *a1, ant *a2) {
	int i, j, h, pos, pred;
	int distance;
	int *pos2 = new int[problem_size];
	//indexes of cities of ant2
	for (int i = 0; i < problem_size; i++) {
		pos2[a2->tour[i]] = i;
	}
	distance = 0;
	for (i = 0; i < problem_size; i++) {
		j = a1->tour[i];
		h = a1->tour[i + 1];
		pos = pos2[j];
		if (pos - 1 < 0)
			pred = problem_size - 1;
		else
			pred = pos - 1;
		if (a2->tour[pos + 1] == h)
			distance++;  //common edges
		else if (a2->tour[pred] == h)
			distance++; //common edges
	}
	//free memory
	delete[] pos2;

	//1 - (common_edges/problem size)
	return 1.0 - (distance / (double)problem_size);
}

double calc_diversity(int size) {
	int i, j;
	double div = 0.0;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if (i != j) {
				div += distance_between(&ant_population[i], &ant_population[j]);//common edges
			}
		}
	}
	return (1.0 / (size * (size - 1))) * div; //population diversity
}

void close_stats(void) {
	int i, j;
	double perf_mean_value, perf_stdev_value;
	double div_mean_value, div_stdev_value;
	double* perf_of_trials = new double[max_trials];
	double* perf_of_iterations = new double[max_iterations];
	double* div_of_trials = new double[max_trials];
	double* div_of_iterations = new double[max_iterations];
	//Initialize
	for (int i = 0; i < max_iterations; i++) {
		perf_of_iterations[i] = 0.0;
		div_of_iterations[i] = 0.0;
	}
	for (int i = 0; i < max_trials; i++) {
		perf_of_trials[i] = 0.0;
		div_of_trials[i] = 0.0;
	}
	//For graph plots
	for (i = 0; i < max_iterations; i++) {
		for (j = 0; j < max_trials; j++) {
			perf_of_iterations[i] += performance[j][i];
			div_of_iterations[i] += diversity[j][i];
		}
		perf_of_iterations[i] /= ((double)max_trials);
		div_of_iterations[i] /= ((double)max_trials);
		fprintf(log_performance, "%.2f", perf_of_iterations[i]);
		fprintf(log_performance, "\n");
		fprintf(log_diversity, "%.2f", div_of_iterations[i]);
		fprintf(log_diversity, "\n");
	}
	fprintf(log_performance, "\n");
	fprintf(log_diversity, "\n");
	fprintf(log_performance, "Statistical results\n");
	fprintf(log_diversity, "Statistical results\n");
	//For statistics
	for (i = 0; i < max_trials; i++) {
		for (j = 0; j < max_iterations; j++) {
			perf_of_trials[i] += performance[i][j];
			div_of_trials[i] += diversity[i][j];
		}
		perf_of_trials[i] /= ((double)max_iterations);
		div_of_trials[i] /= ((double)max_iterations);
		fprintf(log_performance, "%.2f", perf_of_trials[i]);
		fprintf(log_performance, "\n");
		fprintf(log_diversity, "%.2f", div_of_trials[i]);
		fprintf(log_diversity, "\n");

	}
	perf_mean_value = mean(perf_of_trials, max_trials);
	perf_stdev_value = stdev(perf_of_trials, max_trials, perf_mean_value);
	div_mean_value = mean(div_of_trials, max_trials);
	div_stdev_value = stdev(div_of_trials, max_trials, div_mean_value);
	fprintf(log_performance, "Mean %f\t ", perf_mean_value);
	fprintf(log_performance, "\tStd Dev %f\t ", perf_stdev_value);
	fprintf(log_diversity, "Mean %f\t ", div_mean_value);
	fprintf(log_diversity, "\tStd Dev %f ", div_stdev_value);

	cout << "Mean " << perf_mean_value << " Std " << perf_stdev_value << endl;

	fclose(log_performance);
	fclose(log_diversity);

	//free memory
	delete[] perf_of_trials;
	delete[] perf_of_iterations;
	delete[] div_of_trials;
	delete[] div_of_iterations;
}


void statistics_and_output(int r, int t) {


	performance[r - 1][t - 1] = best_so_far_ant->tour_length;
	//diversity[r-1][t-1] = calc_diversity(n_ants);

	if (!(current_iteration % 100)) {
		branching_factor = node_branching(lambda);
		if ((branching_factor < branch_fac) && (current_iteration - restart_found_best > 250)) {
			cout << "INIT TRAILS!!!" << endl;
			restart_best_ant->tour_length = INFTY;
			init_pheromone_trails(trail_max);
			//compute_total_info();
			restart_iteration = current_iteration;

		}
		// cout << branching_factor << endl;
	}

	cout << "run: " << r << " iteration: " << current_iteration << " best_so_far " << best_so_far_ant->tour_length << endl;
}

void apply_to_algorithm() {
	//Reset best solution after a change to
	//calculate the modified offline performance
	best_so_far_ant->tour_length = INFTY;
}

void init_try(int t) {
	//initialize algorithmic structures
	//since the initial objects are added
	//at the beggining of each Run and the
	//dynamic environments are re-initialized
	//using initialize_environment()
	compute_nn_lists();             //reset nearest neighbour objects
	init_heuristic_info();          //initialize heuristic info

	trail_max = 1.0 / ((rho)* nn_tour());
	trail_min = trail_max / (2.0 * problem_size);
	init_pheromone_trails(trail_max);  //initialize pheromone trails
									   //compute_total_info();           //combine heuristic+pheromone
	best_so_far_ant->tour_length = INFTY; //reset best solution found
	restart_iteration = 1;
	lambda = 0.05;
	found_best = 0;

}


void free_memory() {
	//free memory from the DBGP impemenation
	delete[] init_objects;
	for (int i = 0; i < problem_size; i++) {
		delete[] distances[i];
	}
	delete[] distances;
	//free memory from the ACO implementation
	delete[] best_so_far_ant->tour;
	delete[] restart_best_ant->tour;
	for (int i = 0; i < n_ants; i++) {
		delete[] ant_population[i].tour;
		delete[] ant_population[i].visited;
	}
	delete[] ant_population;

	for (int i = 0; i < problem_size; i++) {
		delete[] pheromone[i];
		delete[] heuristic[i];
		delete[] total[i];
		delete[] nn_list[i];
	}
	delete[] pheromone;
	delete[] heuristic;
	delete[] total;
	delete[] nn_list;
	delete[] prob_of_selection;
	delete[] div_filename;
	delete[] perf_filename;
	delete[] var_filename;
	for (int i = 0; i < max_trials; i++) {
		delete[] performance[i];
		delete[] diversity[i];
	}
	delete[] performance;
	delete[] diversity;
}
/*---------------------------end of MMAS_US-------------------------*/



/****************************************************************/
/*                Main Function (DTSP+ACO)                      */
/****************************************************************/
int main(int argc, char *argv[]) {
	// read in parameters
	problem_instance = argv[1];
	change_degree = atof(argv[2]);
	change_speed = atoi(argv[3]);
	rho = atof(argv[4]);
	local_search = atof(argv[5]);

	int total_changes = 100;
	max_iterations = change_speed * total_changes;
	max_trials = 1;

	read_problem(problem_instance);  //Read TSP from file
	check_input_parameters();       //validate the parameters
									/*for GENIUS */

	if (local_search == 1) {
		ls_flag = true;
		for (int i = 0; i < problem_size; i++)
			tsp_file.xyvalues(problem_size, i, init_objects[i].x, init_objects[i].y);
	}
	else if (local_search > 1) {
		ls_flag = true;
	}
	else {
		ls_flag = false;
	}

	open_stats();

	//ACO algorithms initialization
	set_algorithm_parameters();
	allocate_ants();
	allocate_structures();

	seed = 6;                     //changable seed for the algorithm
	for (int run = 1; run <= max_trials; run++) {
		cout << "-------------Run: " << run << "------------------" << endl;
		srand(1);                       //static seed reset for the changes

		initialize_environment();       //set DBGP for a new Run
		current_iteration = 1;            //set iteration for a new Run
		init_try(run);                  //re-initialize algorithmic structures

		while (current_iteration <= max_iterations) {//termination not met
													 //ACO algorithm methods
			construct_solutions();
			update_best();
			pheromone_update();
			//output results to files
			statistics_and_output(run, current_iteration);

			if (current_iteration%change_speed == 0) { //period of change
				change_environment();
				apply_to_algorithm();
			}
			current_iteration++;
		}
	}

	close_stats();
	free_memory();
	return 0;
}
