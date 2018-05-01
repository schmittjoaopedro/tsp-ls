package com.github.schmittjoaopedro;

import java.util.*;

public class Graph {

    private List<Vertex> vertices;

    private Map<Integer, Vertex> verticesIndex;

    private Set<Edge> edges;

    private double upperX;

    private double lowerX;

    private double upperY;

    private double lowerY;

    public Graph() {
        this.vertices = new ArrayList<>();
        this.verticesIndex = new HashMap<>();
        edges = new HashSet<>();
        upperX = Double.MAX_VALUE * -1.0;
        lowerX = Double.MAX_VALUE;
        upperY = Double.MAX_VALUE * -1.0;
        lowerY = Double.MAX_VALUE;
    }

    public void addNode(int id, double x, double y) {
        if (!this.verticesIndex.containsKey(id)) {
            Vertex vertex = new Vertex();
            vertex.setId(id);
            vertex.setX(x);
            vertex.setY(y);
            this.vertices.add(vertex);
            this.verticesIndex.put(id, vertex);
            lowerX = Math.min(lowerX, x);
            upperX = Math.max(upperX, x);
            lowerY = Math.min(lowerY, y);
            upperY = Math.max(upperY, y);
        }
    }

    public Vertex getVertex(int id) {
        return this.verticesIndex.get(id);
    }

    public void addEdge(int idFrom, int idTo, double cost) {
        Vertex from = getVertex(idFrom);
        Vertex to = getVertex(idTo);
        if (from != null && to != null) {
            Edge edge = new Edge();
            edge.setFrom(from);
            from.getEdges().add(edge);
            edge.setTo(to);
            edge.setDistance(cost);
            edges.add(edge);
        }
    }

    public Set<Edge> getEdges(int from) {
        return getVertex(from).getEdges();
    }

    public Edge getEdge(int from, int to) {
        Set<Edge> edges = getEdges(from);
        for (Edge edge : edges) {
            if (edge.getTo().getId() == to) {
                return edge;
            }
        }
        return null;
    }

    public Set<Edge> getEdges() {
        return edges;
    }

    public int getVertexLength() {
        return vertices.size();
    }

    public int getEdgesLength() {
        return edges.size();
    }

    public List<Vertex> getVertices() {
        return vertices;
    }

    public double getUpperX() {
        return upperX;
    }

    public double getLowerX() {
        return lowerX;
    }

    public double getUpperY() {
        return upperY;
    }

    public double getLowerY() {
        return lowerY;
    }

}
