package com.github.schmittjoaopedro.ls_3opt;

public class LocalSearch3Opt {

    public static void threeOpt(int[] tour, double[][] distances) {
        int N = tour.length;
        boolean improvement = true;
        double best_distance = getTourLength(tour, distances);
        int newTour[] = new int[tour.length];
        for (int i = 0; i < tour.length; i++) {
            newTour[i] = tour[i];
        }
        while (improvement) {
            improvement = false;
            for (int i = 1; i < N; i++) {
                for (int j = i + 2; j < N; j++) {
                    for (int k = j + 2; k < N; k++) {
                        if (execute3Swap(newTour, distances, i, j, k) < 0) {
                            improvement = true;
                            for (int t = 0; t < N; t++) {
                                tour[t] = newTour[t];
                            }
                        }
                    }
                }
            }
        }
    }

    private static double execute3Swap(int[] tour, double[][] dist, int i, int j, int k) {
        // [...,A-B,...,C-D,...,E-F,...]
        int a = tour[i - 1];
        int b = tour[i];
        int c = tour[j - 1];
        int d = tour[j];
        int e = tour[k - 1];
        int f = tour[k];
        double d0 = dist[a][b] + dist[c][d] + dist[e][f];
        double d1 = dist[a][c] + dist[b][d] + dist[e][f];
        double d2 = dist[a][b] + dist[c][e] + dist[d][f];
        double d3 = dist[a][d] + dist[e][b] + dist[c][f];
        double d4 = dist[f][b] + dist[c][d] + dist[e][a];
        if (d0 > d1) {
            reverse(tour, i, j);
            return -d0 + d1;
        } else if (d0 > d2) {
            reverse(tour, j, k);
            return -d0 + d2;
        } else if (d0 > d4) {
            reverse(tour, i, k);
            return -d0 + d4;
        } else if (d0 > d3) {
            rearrange(tour, i, j, k);
            return -d0 + d3;
        }
        return d0;
    }

    private static void rearrange(int[] route, int i, int j, int k) {
        int routeIJ[] = new int[j - i];
        int routeJK[] = new int[k - j];
        for (int t = 0; t < routeIJ.length; t++) {
            routeIJ[t] = route[i + t];
        }
        for (int t = 0; t < routeJK.length; t++) {
            routeJK[t] = route[j + t];
        }
        int countIJ = 0;
        int countJK = 0;
        for (int t = 0; t < route.length; t++) {
            if (t > i - 1 && t < i + routeJK.length) {
                route[t] = routeJK[countJK];
                countJK++;
            } else if (t >= i + routeJK.length && t < i + routeJK.length + routeIJ.length) {
                route[t] = routeIJ[countIJ];
                countIJ++;
            }
        }
    }

    private static void reverse(int[] tour, int i, int j) {
        if (i > j) {
            int aux = i;
            i = j;
            j = aux;
        }
        int sub[] = new int[j - i];
        for (int k = 0; k < sub.length; k++) {
            sub[k] = tour[i + k];
        }
        for (int k = i; k < i + sub.length; k++) {
            tour[k] = sub[j - k - 1];
        }
    }

    private static double getTourLength(int[] tour, double[][] distances) {
        double dist = 0;
        for (int i = 0; i < tour.length - 1; i++) {
            dist += distances[tour[i]][tour[i + 1]];
        }
        return dist;
    }
}
