package com.github.schmittjoaopedro.us;

import com.github.schmittjoaopedro.Graph;
import com.github.schmittjoaopedro.Vertex;

import java.util.ArrayList;
import java.util.List;

public class USOperator {

    public List<Vertex> optimize(Graph graph, List<Vertex> route) {
        int problem_size = graph.getVertexLength();
        int tour_length = route.size();
        // Init structure
        CoordG.MAXN = problem_size;
        CoordG tsp_file = new CoordG();
        for (int i = 0; i < graph.getVertexLength(); i++) {
            tsp_file.xyvalues(graph.getVertexLength(), i, graph.getVertex(i).getX(), graph.getVertex(i).getY());
        }
        tsp_file.distances();
        // Init genius
        tourneelem pri;
        int[] tour = new int[tour_length];
        for (int i = 0; i < tour_length; i++) {
            tour[i] = route.get(i).getId();
        }
        RouteG genius = new RouteG();
        genius.initialize();
        genius.initnneighbour(tsp_file.task);
        // Take the best ant for the considered interation;
        genius.petittourne(tour[0], tsp_file.g); //initial
        genius.ajoutenoeudprox(tour[0], tsp_file.task, tsp_file.d);
        for (int i = 1; i < problem_size; i++) {
            genius.ajoute_a_tourne(tour[i], tsp_file.g);
            genius.ajoutenoeudprox(tour[i], tsp_file.task, tsp_file.d);
        } //copy the tour and calculates the nearest neighbours of the nodes.

        //if (!genius.numerote_tourne()) return 1;
        if (!genius.numerote_tourne()) {
            //do nothing
        } else {
            //viz = MAXK;
            genius.route_copy(tsp_file.task, tsp_file.g, tsp_file.d, tour_length);
            //genius.showroute(problem_size);
            //COPY THE RESULTING TOUR
            pri = genius.t.ptr;
            for (int i = 0; i < problem_size; i++) {
                tour[i] = pri.noeud - 1;
                //cout << "ant[" << i << "]= " << ant_population[best_sol].tour[i] << "\n";
                pri = pri.prochain;
            }
            tour[problem_size] = tour[0];
        }
        List<Vertex> optimized = new ArrayList<>();
        for(int i = 0; i < tour.length; i++) {
            optimized.add(graph.getVertex(tour[i]));
        }
        return optimized;
    }

}