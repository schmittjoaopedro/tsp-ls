package com.github.schmittjoaopedro.ls_3opt;

import com.github.schmittjoaopedro.*;

import java.util.ArrayList;
import java.util.List;

public class OPT3Operator implements LSOperator {

    private int[] tour;
    private double[][] distances;
    private Graph graph;
    private boolean symmetric = true;

    @Override
    public void init(Graph graph, List<Vertex> route) {
        this.graph = graph;
        tour = new int[route.size()];
        distances = new double[tour.length - 1][tour.length - 1];
        double max = 0, min = Double.MAX_VALUE;
        for (int i = 0; i < route.size(); i++) {
            tour[i] = route.get(i).getId();
        }
        for (int i = 0; i < graph.getVertices().size(); i++) {
            for (int j = 0; j < graph.getVertices().size(); j++) {
                if (i != j) {
                    distances[i][j] = graph.getEdge(i, j).getDistance();
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
            distances = Asymmetric2Symmetric.convert(distances, min - 1.0, max + 1.0);
            int[] newTour = new int[2 * tour.length - 1];
            for (int i = 0; i < newTour.length; i++) {
                if (i % 2 == 0) {
                    newTour[i] = tour[i / 2];
                } else {
                    newTour[i] = tour.length - 1 + newTour[i - 1];
                }
            }
            tour = newTour;
        }
    }

    @Override
    public void optimize() {
        LocalSearch3Opt.threeOpt(tour, distances);
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
