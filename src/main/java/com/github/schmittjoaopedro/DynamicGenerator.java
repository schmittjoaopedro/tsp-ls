package com.github.schmittjoaopedro;

public class DynamicGenerator {

    public static void execute(Graph graph, double magnitude, double fl, double fu) {
        for (Edge edge : graph.getEdges()) {
            if (edge.getFrom().getId() != edge.getTo().getId()) {
                if (Math.random() < magnitude) {
                    double factor = 1.0 + (fl + (fu - fl) * Math.random());
                    edge.setDistance(edge.getDistance() * factor);
                }
            }
        }
    }

}
