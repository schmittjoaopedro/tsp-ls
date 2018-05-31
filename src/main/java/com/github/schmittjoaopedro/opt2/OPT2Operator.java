package com.github.schmittjoaopedro.opt2;

import com.github.schmittjoaopedro.Edge;
import com.github.schmittjoaopedro.Graph;
import com.github.schmittjoaopedro.Vertex;

import java.util.ArrayList;
import java.util.List;

public class OPT2Operator {

    private int tour[];
    private double distances[][];
    private Graph graph;

    public OPT2Operator(Graph graph, List<Vertex> route) {
        this.graph = graph;
        tour = route.stream().mapToInt(Vertex::getId).toArray();
        distances = new double[tour.length - 1][tour.length - 1];
        for (Edge edge : graph.getEdges()) {
            distances[edge.getFrom().getId()][edge.getTo().getId()] = edge.getDistance();
        }
    }

    public void optimize() {
        LocalSearch2Opt.twoOpt(tour, distances);
    }

    public List<Vertex> getResult() {
        List<Vertex> newRoute = new ArrayList<>();
        for (int i = 0; i < tour.length; i++) {
            newRoute.add(graph.getVertex(tour[i]));
        }
        return newRoute;
    }

}
