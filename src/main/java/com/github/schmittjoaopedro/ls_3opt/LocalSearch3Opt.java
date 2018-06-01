package com.github.schmittjoaopedro.ls_3opt;

class LocalSearch3Opt {

    static void threeOpt(int[] tour, double[][] distances) {
        int N = tour.length;
        boolean improvement = true;
        int newTour[] = new int[tour.length];
        System.arraycopy(tour, 0, newTour, 0, tour.length);
        while (improvement) {
            improvement = false;
            for (int i = 1; i < N; i++) {
                for (int j = i + 2; j < N; j++) {
                    for (int k = j + 2; k < N; k++) {
                        if (execute3Swap(newTour, distances, i, j, k)) {
                            improvement = true;
                            System.arraycopy(newTour, 0, tour, 0, N);
                        }
                    }
                }
            }
        }
    }

    private static boolean execute3Swap(int[] tour, double[][] dist, int i, int j, int k) {
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
            return true;
        } else if (d0 > d2) {
            reverse(tour, j, k);
            return true;
        } else if (d0 > d4) {
            reverse(tour, i, k);
            return true;
        } else if (d0 > d3) {
            rearrange(tour, i, j, k);
            return true;
        }
        return false;
    }

    private static void rearrange(int[] route, int i, int j, int k) {
        int routeIJ[] = new int[j - i];
        int routeJK[] = new int[k - j];
        System.arraycopy(route, i, routeIJ, 0, routeIJ.length);
        System.arraycopy(route, j, routeJK, 0, routeJK.length);
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
        System.arraycopy(tour, i, sub, 0, sub.length);
        for (int k = i; k < i + sub.length; k++) {
            tour[k] = sub[j - k - 1];
        }
    }

}
