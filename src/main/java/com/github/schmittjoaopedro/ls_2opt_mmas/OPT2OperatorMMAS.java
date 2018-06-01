package com.github.schmittjoaopedro.ls_2opt_mmas;

import com.github.schmittjoaopedro.Edge;
import com.github.schmittjoaopedro.Graph;
import com.github.schmittjoaopedro.Vertex;

import java.util.*;
import java.util.stream.Collectors;

public class OPT2OperatorMMAS {

    private int tour[];
    private int nn_list[][];
    private int nn_ls = 20;
    private int distances[][];
    private Graph graph;

    public OPT2OperatorMMAS(Graph graph, List<Vertex> route) {
        this.graph = graph;
        tour = route.stream().mapToInt(Vertex::getId).toArray();
        nn_ls = tour.length - 2;
        distances = new int[tour.length - 1][tour.length - 1];
        for (Edge edge : graph.getEdges()) {
            distances[edge.getFrom().getId()][edge.getTo().getId()] = (int) edge.getDistance();
        }
        Map<Integer, Map<Integer, Double>> nnList = new HashMap<>();
        int pos = 0;
        nn_list = new int[tour.length - 1][nn_ls];
        for (Vertex v1 : graph.getVertices()) {
            nnList.put(v1.getId(), new HashMap<>());
            for (Vertex v2 : graph.getVertices()) {
                if (v1 != v2) {
                    nnList.get(v1.getId()).put(v2.getId(), graph.getEdge(v1.getId(), v2.getId()).getDistance());
                }
            }
            Map<Integer, Double> nnTemp = nnList.get(v1.getId()).entrySet().stream().sorted(Map.Entry.comparingByValue()).collect(Collectors.toMap(Map.Entry::getKey, Map.Entry::getValue, (e1, e2) -> e1, LinkedHashMap::new));
            int pos2 = 0;
            for (Map.Entry<Integer, Double> entry : nnTemp.entrySet()) {
                if (pos2 == nn_ls) break;
                nn_list[pos][pos2] = entry.getKey();
                pos2++;
            }
            pos++;
        }
    }

    public void optimize() {
        LocalSearch2OptMMAS.two_opt_first(tour, nn_list, distances, tour.length - 1, nn_ls);
    }

    public List<Vertex> getResult() {
        List<Vertex> newRoute = new ArrayList<>();
        for (int i = 0; i < tour.length; i++) {
            newRoute.add(graph.getVertex(tour[i]));
        }
        return newRoute;
    }

}
