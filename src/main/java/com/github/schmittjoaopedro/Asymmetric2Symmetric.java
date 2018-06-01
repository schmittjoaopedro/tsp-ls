package com.github.schmittjoaopedro;

public class Asymmetric2Symmetric {

    /**
     * Title: Transforming Asymmetric into Symmetric Travelling Salesman Problems
     * Authors: Roy JONKER and Ton VOLGENANT
     */
    public static double[][] convert(double[][] asymmetric, double M, double INF) {
        int n = asymmetric.length;
        // C" = | U' C'|
        //      | C  U"|
        double[][] symmetric = new double[2 * n][2 * n];

        // Define U'
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                symmetric[i][j] = INF; // Inf+
            }
        }

        // Define C' (Transpose of C)
        for (int i = 0; i < n; i++) {
            for (int j = n; j < 2 * n; j++) {
                if (i == j - n) {
                    symmetric[i][j] = -M; // -M
                } else {
                    symmetric[i][j] = asymmetric[j - n][i];
                }
            }
        }

        // Define C
        for (int i = n; i < 2 * n; i++) {
            for (int j = 0; j < n; j++) {
                if (i - n == j) {
                    symmetric[i][j] = -M; // -M
                } else {
                    symmetric[i][j] = asymmetric[i - n][j];
                }
            }
        }

        // Define U"
        for (int i = n; i < 2 * n; i++) {
            for (int j = n; j < 2 * n; j++) {
                symmetric[i][j] = INF; // Inf+
            }
        }

        // C"
        return symmetric;
    }

}
