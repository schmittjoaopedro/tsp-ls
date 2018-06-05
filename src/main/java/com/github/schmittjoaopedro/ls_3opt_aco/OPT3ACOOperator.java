package com.github.schmittjoaopedro.ls_3opt_aco;

import com.github.schmittjoaopedro.*;
import com.github.schmittjoaopedro.ls_2opt_aco.LocalSearch2OptACO;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class OPT3ACOOperator implements LSOperator {

    private int tour[];
    private int distances[][];
    private int n;
    private int nn_ls = 20;
    private int nn_list[][];
    private Graph graph;
    boolean symmetric = true;

    @Override
    public void init(Graph graph, List<Vertex> route) {
        this.graph = graph;
        tour = route.stream().mapToInt(Vertex::getId).toArray();
        n = graph.getVertexLength();
        distances = new int[tour.length - 1][tour.length - 1];
        int max = 0, min = Integer.MAX_VALUE;
        for (int i = 0; i < graph.getVertices().size(); i++) {
            for (int j = 0; j < graph.getVertices().size(); j++) {
                if (i != j) {
                    distances[i][j] = (int) graph.getEdge(i, j).getDistance();
                    max = Math.max(max, distances[i][j]);
                    min = Math.min(min, distances[i][j]);
                }
            }
        }
        for (int i = 0; i < distances.length; i++) {
            for (int j = i; j < distances.length; j++) {
                if (distances[i][j] != distances[j][i]) {
                    symmetric = false;
                    break;
                }
            }
        }
        if (!symmetric) {
            distances = Asymmetric2Symmetric.convert(distances, min - 1, max + 1);
            int[] newTour = new int[2 * tour.length - 1];
            for (int i = 0; i < newTour.length; i++) {
                if (i % 2 == 0) {
                    newTour[i] = tour[i / 2];
                } else {
                    newTour[i] = tour.length - 1 + newTour[i - 1];
                }
            }
            tour = newTour;
            n = tour.length - 1;
        }
        // NN List
        nn_list = new int[n][nn_ls];
        for (int i = 0; i < n; i++) {
            Set<Integer> selected = new HashSet<>();
            for (int k = 0; k < nn_ls; k++) {
                int currentBest = Integer.MAX_VALUE;
                int bestNode = 0;
                for (int j = 0; j < n; j++) {
                    if (j != i && !selected.contains(j) && distances[i][j] < currentBest) {
                        currentBest = distances[i][j];
                        bestNode = j;
                    }
                }
                selected.add(bestNode);
                nn_list[i][k] = bestNode;
            }
        }
    }

    @Override
    public void optimize() {
        LocalSearch3OptACO.three_opt_first(tour, n, distances, nn_list, nn_ls);
    }

    @Override
    public List<Vertex> getResult() {
        List<Vertex> newRoute = new ArrayList<>();
        for (int i = 0; i < tour.length; i++) {
            if (graph.getVertex(tour[i]) != null) {
                newRoute.add(graph.getVertex(tour[i]));
            }
        }
        return newRoute;
    }

}
