package com.github.schmittjoaopedro.ls_2opt;

import com.github.schmittjoaopedro.Edge;
import com.github.schmittjoaopedro.Graph;
import com.github.schmittjoaopedro.LSOperator;
import com.github.schmittjoaopedro.Vertex;

import java.util.ArrayList;
import java.util.List;

public class OPT2Operator implements LSOperator {

    private int tour[];
    private double distances[][];
    private Graph graph;

    @Override
    public void init(Graph graph, List<Vertex> route) {
        this.graph = graph;
        tour = route.stream().mapToInt(Vertex::getId).toArray();
        distances = new double[tour.length - 1][tour.length - 1];
        for (Edge edge : graph.getEdges()) {
            distances[edge.getFrom().getId()][edge.getTo().getId()] = edge.getDistance();
        }
    }

    @Override
    public void optimize() {
        LocalSearch2Opt.twoOpt(tour, distances);
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
