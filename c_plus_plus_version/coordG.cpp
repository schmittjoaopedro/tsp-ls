#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cmath>

#include "coordG.h"
#include "ACODTSP.h"

using namespace std;

void CoordG::xyvalues(int n, int cities, double x, double y)
{

	task = n;
	g[cities + 1].x = x;
	g[cities + 1].y = y;

}

void CoordG::distances()
{
	// cout << "new" << endl;
	for (int i = 1; i <= (task); i++)
	{
		for (int j = 1; j <= (task); j++)
		{
			if (i != j) {
				d[i][j] = sqrt(pow((g[i].x - g[j].x), 2) + pow((g[i].y - g[j].y), 2)) + 0.5;
				d[i][j] = (int)d[i][j] * traffic_factors[i - 1][j - 1];
				// cout << d[i][j] << " , ";
			}
			// d[j][i] = d[i][j];
		}
	}
	// cout << "" << endl;
}

