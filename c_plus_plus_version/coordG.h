#ifndef COORDG_H
#define COORDG_H
#define MAXN 500
#define NBGRAND 5



/**
Read the coordinate files in the TSPlib format
*/

using namespace std;

typedef struct tourneelem {
	int noeud, rang;
	tourneelem *precedent, *prochain;
}tourneelem;

typedef struct {
	float x, y;
	tourneelem *ptrtourne;
} coord;


class CoordG
{
public:


	/**
	Read the number of cities and the coordinates x and y from the file named fname
	*/
	void xyvalues(int n, int cities, double x, double y);

	/**
	Calculates the distance matrix
	*/
	void distances();




	int task;
	coord g[MAXN + 1];
	float d[MAXN + 1][MAXN + 1];



};

#endif
