package com.github.schmittjoaopedro.us;

public class CoordG {

    public static int MAXN = 500;

    public static int NBGRAND = 5;

    public int task;

    public coord g[] = new coord[MAXN + 1];

    public double d[][] = new double[MAXN + 1][MAXN + 1];

    public void xyvalues(int n, int cities, double x, double y) {
        task = n;
        g[cities + 1] = new coord();
        g[cities + 1].x = x;
        g[cities + 1].y = y;
    }

    public void distances() {
        for (int i = 1; i <= (task); i++) {
            for (int j = 1; j <= (task); j++) {
                if (i != j) {
                    d[i][j] = Math.sqrt(Math.pow((g[i].x - g[j].x), 2) + Math.pow((g[i].y - g[j].y), 2)) + 0.5;
                    //d[i][j] = (int)d[i][j] * traffic_factors[i-1][j-1];
                }
            }
        }
    }

}
