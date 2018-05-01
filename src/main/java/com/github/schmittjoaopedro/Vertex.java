package com.github.schmittjoaopedro;

import java.util.HashSet;
import java.util.Set;

public class Vertex {

    private int id;

    private double x;

    private double y;

    private Set<Edge> edges;

    public Vertex() {
        super();
        this.edges = new HashSet<>();
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public double getX() {
        return x;
    }

    public void setX(double x) {
        this.x = x;
    }

    public double getY() {
        return y;
    }

    public void setY(double y) {
        this.y = y;
    }

    public Set<Edge> getEdges() {
        return this.edges;
    }

    @Override
    public String toString() {
        return this.getId() + " = " + this.getY() + "," + this.getX();
    }

}
