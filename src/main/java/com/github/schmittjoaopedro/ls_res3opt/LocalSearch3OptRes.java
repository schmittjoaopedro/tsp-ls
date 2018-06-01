package com.github.schmittjoaopedro.ls_res3opt;

class LocalSearch3OptRes {

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
                        if (execute3Swap(newTour, distances, i, j, k) < 0) {
                            improvement = true;
                            System.arraycopy(newTour, 0, tour, 0, N);
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
        double d3 = dist[a][d] + dist[e][b] + dist[c][f];
        if (d0 > d3) {
            rearrange(tour, i, j, k);
            return -d0 + d3;
        }
        return d0;
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

}
