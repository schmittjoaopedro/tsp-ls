package com.github.schmittjoaopedro;

import com.github.schmittjoaopedro.opt2.OPT2Operator;
import com.github.schmittjoaopedro.us.USOperator;
import org.apache.commons.cli.*;

import java.util.List;

public class App {

    public static void main(String[] args) throws Exception {
        Options options = getOptions();
        CommandLineParser parser = new DefaultParser();
        CommandLine cmd = parser.parse(options, args);

        if (cmd.hasOption("tsp") && "all".equals(cmd.getOptionValue("ls"))) {
            Graph graph = TSPConverter.readGraph(cmd.getOptionValue("tsp"));
            List<Vertex> randomRoute = Utils.randomRoute(graph);
            Utils.printRouteCost(graph, randomRoute);
            // US operator
            System.out.print("US Operator    - ");
            USOperator usOperator = new USOperator(graph, randomRoute);
            Long time = System.currentTimeMillis();
            usOperator.optimize();
            System.out.print("Time(s) = " + ((System.currentTimeMillis() - time) / 1000.0) + " ");
            Utils.printRouteCost(graph, usOperator.getResult());
            // 2-opt operator
            System.out.print("2-opt Operator - ");
            OPT2Operator opt2Operator = new OPT2Operator(graph, randomRoute);
            time = System.currentTimeMillis();
            opt2Operator.optimize();
            System.out.print("Time(s) = " + ((System.currentTimeMillis() - time) / 1000.0) + " ");
            Utils.printRouteCost(graph, opt2Operator.getResult());
        } else if (cmd.hasOption("tsp") && "us".equals(cmd.getOptionValue("ls"))) {
            Graph graph = TSPConverter.readGraph(cmd.getOptionValue("tsp"));
            List<Vertex> randomRoute = Utils.randomRoute(graph);
            Utils.printRouteCost(graph, randomRoute);
            USOperator usOperator = new USOperator(graph, randomRoute);
            usOperator.optimize();
            Utils.printRouteCost(graph, usOperator.getResult());
        } else if (cmd.hasOption("tsp") && "2opt".equals(cmd.getOptionValue("ls"))) {
            Graph graph = TSPConverter.readGraph(cmd.getOptionValue("tsp"));
            List<Vertex> randomRoute = Utils.randomRoute(graph);
            Utils.printRouteCost(graph, randomRoute);
            OPT2Operator opt2Operator = new OPT2Operator(graph, randomRoute);
            opt2Operator.optimize();
            Utils.printRouteCost(graph, opt2Operator.getResult());
        } else {
            HelpFormatter helpFormatter = new HelpFormatter();
            helpFormatter.printHelp("Local search for TSP instances.", options);
        }
    }

    public static Options getOptions() {
        Options options = new Options();
        options.addOption(Option.builder("tsp").hasArg().numberOfArgs(1).argName("tsp_file").desc("TSP file for processing.").build());
        options.addOption(Option.builder("ls").hasArg().numberOfArgs(1).argName("local_search")
                .desc("all = All local search\n" +
                        "us = Unstringing and Stringing\n" +
                        "2opt = 2-opt moves").build());
        return options;
    }

}
