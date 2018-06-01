package com.github.schmittjoaopedro;

import org.moeaframework.problem.tsplib.TSPInstance;

import java.io.File;
import java.io.IOException;

public class ATSPConverter {

    public static Graph readGraph(String tspFile) {
        Graph graph = new Graph();
        try {
            File instanceData = new File(tspFile);
            TSPInstance problem = new TSPInstance(instanceData);
            for (int i = 0; i < problem.getDimension(); i++) {
                graph.addNode(i, i, i);
            }
            for (int i = 1; i <= problem.getDimension(); i++) {
                for (int j = 1; j <= problem.getDimension(); j++) {
                    graph.addEdge(i - 1, j - 1, problem.getDistanceTable().getDistanceBetween(i, j));
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        return graph;
    }

}
