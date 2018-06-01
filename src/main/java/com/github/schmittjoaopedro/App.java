package com.github.schmittjoaopedro;

import com.github.schmittjoaopedro.ls_2opt.OPT2Operator;
import com.github.schmittjoaopedro.ls_3opt.OPT3Operator;
import com.github.schmittjoaopedro.ls_res3opt.OPT3RESOperator;
import com.github.schmittjoaopedro.ls_us.USOperator;
import org.apache.commons.cli.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class App {

    public static void main(String[] args) throws Exception {
        Options options = getOptions();
        CommandLineParser parser = new DefaultParser();
        CommandLine cmd = parser.parse(options, args);
        if (!cmd.hasOption("tsp")) {
            throw new RuntimeException("Graph file is mandatory");
        }
        System.out.println("Problem = " + cmd.getOptionValue("tsp"));
        Graph graph = TSPConverter.readGraph(cmd.getOptionValue("tsp"));
        if (graph != null) {
            List<Vertex> randomRoute = Utils.randomRoute(graph);
            Utils.printRouteCost(graph, randomRoute);
            if (cmd.hasOption("stat")) {
                loadTest(graph, Integer.valueOf(cmd.getOptionValue("stat")));
            } else if ("all".equals(cmd.getOptionValue("ls"))) {
                // 2-opt operator
                execute(OPT2Operator.class, graph, randomRoute);
                // 3-opt operator
                execute(OPT3Operator.class, graph, randomRoute);
                // res-3-opt operator
                execute(OPT3RESOperator.class, graph, randomRoute);
                // US operator
                execute(USOperator.class, graph, randomRoute);
            } else if (cmd.hasOption("tsp") && "us".equals(cmd.getOptionValue("ls"))) {
                execute(USOperator.class, graph, randomRoute);
            } else if (cmd.hasOption("tsp") && "2-opt".equals(cmd.getOptionValue("ls"))) {
                execute(USOperator.class, graph, randomRoute);
            } else if (cmd.hasOption("tsp") && "3-opt".equals(cmd.getOptionValue("ls"))) {
                execute(OPT3Operator.class, graph, randomRoute);
            } else {
                HelpFormatter helpFormatter = new HelpFormatter();
                helpFormatter.printHelp("Local search for TSP instances.", options);
            }
        }
    }

    private static Options getOptions() {
        Options options = new Options();
        options.addOption(Option.builder("tsp").hasArg().numberOfArgs(1).argName("tsp_file").desc("TSP file for processing.").build());
        options.addOption(Option.builder("stat").hasArg().numberOfArgs(1).argName("trials").desc("Statistic for all operators.").build());
        options.addOption(Option.builder("ls").hasArg().numberOfArgs(1).argName("local_search")
                .desc("all = All local search\n" +
                        "us = Unstringing and Stringing\n" +
                        "2-opt = 2-opt moves\n" +
                        "3-opt = 3-opt moves").build());
        return options;
    }

    private static void execute(Class<? extends LSOperator> operatorClass, Graph graph, List<Vertex> tour) throws Exception {
        StringBuilder message = new StringBuilder(operatorClass.getSimpleName());
        while (message.length() < 20) message.append(" ");
        message.append(" - ");
        LSOperator operator = operatorClass.getDeclaredConstructor().newInstance();
        operator.init(graph, tour);
        Long time = System.currentTimeMillis();
        operator.optimize();
        System.out.printf("%sTime(ms) = %.8f ", message.toString(), ((System.currentTimeMillis() - time) / 1000.0));
        Utils.printRouteCost(graph, operator.getResult());
    }

    private static void loadTest(Graph graph, int trials) {
        Map<String, List<Double>> means = new HashMap<>();
        means.put("rnd_tour_cost", new ArrayList<>());
        means.put("rnd_tour_time", new ArrayList<>());
        means.put("2opt_tour_cost", new ArrayList<>());
        means.put("2opt_tour_time", new ArrayList<>());
        means.put("us_tour_cost", new ArrayList<>());
        means.put("us_tour_time", new ArrayList<>());
        means.put("3opt_tour_cost", new ArrayList<>());
        means.put("3opt_tour_time", new ArrayList<>());
        means.put("res_3opt_tour_cost", new ArrayList<>());
        means.put("res_3opt_tour_time", new ArrayList<>());
        for (int i = 0; i < trials; i++) {
            // Random tour
            Long time = System.currentTimeMillis();
            List<Vertex> randomTour = Utils.randomRoute(graph);
            means.get("rnd_tour_cost").add(Utils.getRouteCost(graph, randomTour));
            means.get("rnd_tour_time").add((double) System.currentTimeMillis() - time);
            // 2opt tour
            LSOperator ls2opt = new OPT2Operator();
            ls2opt.init(graph, randomTour);
            time = System.currentTimeMillis();
            ls2opt.optimize();
            time = System.currentTimeMillis() - time;
            means.get("2opt_tour_cost").add(Utils.getRouteCost(graph, ls2opt.getResult()));
            means.get("2opt_tour_time").add((double) time);
            // 3opt tour
            LSOperator ls3opt = new OPT3Operator();
            ls3opt.init(graph, randomTour);
            time = System.currentTimeMillis();
            ls3opt.optimize();
            time = System.currentTimeMillis() - time;
            means.get("3opt_tour_cost").add(Utils.getRouteCost(graph, ls3opt.getResult()));
            means.get("3opt_tour_time").add((double) time);
            // res-3opt tour
            LSOperator lsRes3opt = new OPT3RESOperator();
            lsRes3opt.init(graph, randomTour);
            time = System.currentTimeMillis();
            lsRes3opt.optimize();
            time = System.currentTimeMillis() - time;
            means.get("res_3opt_tour_cost").add(Utils.getRouteCost(graph, lsRes3opt.getResult()));
            means.get("res_3opt_tour_time").add((double) time);
            // US tour
            LSOperator lsus = new USOperator();
            lsus.init(graph, randomTour);
            time = System.currentTimeMillis();
            lsus.optimize();
            time = System.currentTimeMillis() - time;
            means.get("us_tour_cost").add(Utils.getRouteCost(graph, lsus.getResult()));
            means.get("us_tour_time").add((double) time);
        }
        printValue("rnd_tour_time", means.get("rnd_tour_time"), trials);
        printValue("2opt_tour_time", means.get("2opt_tour_time"), trials);
        printValue("3opt_tour_time", means.get("3opt_tour_time"), trials);
        printValue("res_3opt_tour_time", means.get("res_3opt_tour_time"), trials);
        printValue("us_tour_time", means.get("us_tour_time"), trials);
        printValue("rnd_tour_cost", means.get("rnd_tour_cost"), trials);
        printValue("2opt_tour_cost", means.get("2opt_tour_cost"), trials);
        printValue("3opt_tour_cost", means.get("3opt_tour_cost"), trials);
        printValue("res_3opt_tour_cost", means.get("res_3opt_tour_cost"), trials);
        printValue("us_tour_cost", means.get("us_tour_cost"), trials);
    }

    public static void printValue(String key, List<Double> values, int trials) {
        double mean = 0;
        for (double val : values) {
            mean += val;
        }
        mean = mean / trials;
        StringBuilder message = new StringBuilder(key);
        while (message.length() < 15) message.append(" ");
        message.append(" = ").append(mean);
        System.out.println(message);
    }

}
