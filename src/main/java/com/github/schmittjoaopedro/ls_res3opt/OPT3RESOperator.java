package com.github.schmittjoaopedro.ls_res3opt;

import com.github.schmittjoaopedro.Graph;
import com.github.schmittjoaopedro.LSOperator;
import com.github.schmittjoaopedro.Vertex;

import java.util.ArrayList;
import java.util.List;

public class OPT3RESOperator implements LSOperator {

    private int[] tour;
    private double[][] distances;
    private Graph graph;

    @Override
    public void init(Graph graph, List<Vertex> route) {
        this.graph = graph;
        tour = new int[route.size()];
        distances = new double[tour.length - 1][tour.length - 1];
        for (int i = 0; i < route.size(); i++) {
            tour[i] = route.get(i).getId();
        }
        for (int i = 0; i < graph.getVertices().size(); i++) {
            for (int j = 0; j < graph.getVertices().size(); j++) {
                if (i != j) {
                    distances[i][j] = graph.getEdge(i, j).getDistance();
                }
            }
        }
    }

    @Override
    public void optimize() {
        LocalSearch3OptRes.threeOpt(tour, distances);
    }

    @Override
    public List<Vertex> getResult() {
        List<Vertex> newRoute = new ArrayList<>();
        for (int i = 0; i < tour.length; i++) {
            newRoute.add(graph.getVertex(tour[i]));
        }
        return newRoute;
    }
}
