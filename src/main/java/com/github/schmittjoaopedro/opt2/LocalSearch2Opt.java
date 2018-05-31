package com.github.schmittjoaopedro.opt2;

public class LocalSearch2Opt {

    public static void twoOpt(int tour[], double[][] distances) {
        // Get tour size
        int size = tour.length;
        int newTour[] = new int[size];

        //CHECK THIS!!
        for (int i = 0; i < size; i++) {
            newTour[i] = tour[i];
        }

        // repeat until no improvement is made
        boolean improve = true;

        while (improve) {
            double best_distance = tourDistance(tour, distances);
            improve = false;
            for (int i = 1; i < size - 1; i++) {
                for (int k = i + 1; k < size - 1; k++) {
                    twoOptSwap(tour, newTour, i, k);
                    double new_distance = tourDistance(newTour, distances);
                    if (new_distance < best_distance) {
                        // Improvement found so reset
                        improve = true;
                        for (int j = 0; j < size; j++) {
                            tour[j] = newTour[j];
                        }
                        best_distance = new_distance;
                    }
                }
            }
        }
    }

    private static void twoOptSwap(int[] tour, int[] newTour, int i, int k) {
        int size = tour.length;

        // 1. take route[0] to route[i-1] and add them in order to new_route
        for (int c = 0; c <= i - 1; ++c) {
            newTour[c] = tour[c];
        }

        // 2. take route[i] to route[k] and add them in reverse order to new_route
        int dec = 0;
        for (int c = i; c <= k; ++c) {
            newTour[c] = tour[k - dec];
            dec++;
        }

        // 3. take route[k+1] to end and add them in order to new_route
        for (int c = k + 1; c < size; ++c) {
            newTour[c] = tour[c];
        }
    }

    private static double tourDistance(int[] tour, double[][] distances) {
        double distance = 0;
        for (int i = 0; i < tour.length - 1; i++) {
            distance += distances[tour[i]][tour[i + 1]];
        }
        return distance;
    }

}
