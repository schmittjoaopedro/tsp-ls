package com.github.schmittjoaopedro;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Utils {

    public static double getEuclideanDistance(Vertex n1, Vertex n2) {
        double x1 = n1.getX() - n2.getX();
        double y1 = n1.getY() - n2.getY();
        return Math.sqrt(x1 * x1 + y1 * y1);
    }

    public static List<Vertex> randomRoute(Graph graph) {
        List<Vertex> route = new ArrayList<>();
        List<Vertex> temp = new ArrayList<>(graph.getVertices());
        while (!temp.isEmpty()) {
            int position = (int) (Math.random() * temp.size());
            route.add(temp.get(position));
            temp.remove(position);
        }
        route.add(route.get(0));
        return route;
    }

    public static double getRouteCost(Graph graph, List<Vertex> route) {
        double cost = 0.0;
        for (int i = 0; i < route.size() - 1; i++) {
            cost += graph.getEdge(route.get(i).getId(), route.get(i + 1).getId()).getDistance();
        }
        return cost;
    }

    public static void printRouteCost(Graph graph, List<Vertex> route) {
        double cost = getRouteCost(graph, route);
        StringBuilder routePath = new StringBuilder();
        Set<Integer> routes = new HashSet<>();
        for (Vertex vertex : route) {
            routes.add(vertex.getId());
            routePath.append(vertex.getId() + 1).append("-");
        }
        if (route.get(0).getId() != route.get(route.size() - 1).getId() && route.size() - 1 != routes.size()) {
            throw new RuntimeException("Invalid route");
        }
        routePath.deleteCharAt(routePath.length() - 1);
        System.out.println("Cost = " + cost + ", Route = " + routePath);
    }


}
