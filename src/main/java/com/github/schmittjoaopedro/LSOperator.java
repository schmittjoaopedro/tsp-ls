package com.github.schmittjoaopedro;

import java.util.List;

public interface LSOperator {

    void init(Graph graph, List<Vertex> route);

    void optimize();

    List<Vertex> getResult();

}
