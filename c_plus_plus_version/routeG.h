#ifndef ROUTEG_H
#define ROUTEG_H
#define NBGRAND 5
#define MAXK 3
#define MINK 4
#define MAXN 500
#define MAXK1 15

/**
Initialize da data structures used inside the program
*/



typedef struct {
	int noeudinterne[MAXN + 1];
	tourneelem *ptr;
	int nbredenoeuds;
} tourne;

typedef struct {
	int nn[MAXK1], leplusloin;
	float maxdist;
} proxnoeud;


using namespace std;



class RouteG
{
public:

	/**
	Initialize data structures to store the results
	*/
	void initialize();

	/**
	Initialize data structures for nearest neighbours
	*/
	void initnneighbour(int n);
	/**
	Initialize the route with the first element
	*/
	void petittourne(int city, coord g[MAXN + 1]);

	/**
	Agregate a city in the route
	*/
	void ajoutenoeudprox(int aj, int n, const float d[][MAXN + 1]);

	/**
	Update the route with the city included
	*/
	void ajoute_a_tourne(int ind, coord g[MAXN + 1]);

	/**
	Update the route with the city included
	*/
	bool numerote_tourne();


	/**
	Update the route with the city included using GENIUS
	*/
	void ajoutx(int x, int k1, coord g[MAXN + 1], const float d[][MAXN + 1]);

	/**
	Compute the route cost
	*/
	float calculcoutt(const float d[][MAXN + 1]);

	/**
	show the route elements
	*/
	void showroute(int n);

	/**
	Now we perform the post-optimization phase using the procedure
	Unistringing and Stringing concluiding GENIUS
	*/

	void route_copy(int n, coord g[MAXN + 1], const float d[][MAXN + 1], const double tour);

	void oterx(int x, int k1, coord g[MAXN + 1], const float d[][MAXN + 1]);


	tourne t;
	tourne t2;
	proxnoeud p1[MAXN + 1], p2[MAXN + 1];
	float deltain, deltaout;

};

#endif
