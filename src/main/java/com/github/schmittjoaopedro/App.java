package com.github.schmittjoaopedro;

import com.github.schmittjoaopedro.ls_2opt.OPT2Operator;
import com.github.schmittjoaopedro.ls_2opt_mmas.OPT2OperatorMMAS;
import com.github.schmittjoaopedro.ls_3opt.OPT3Operator;
import com.github.schmittjoaopedro.ls_us.USOperator;
import org.apache.commons.cli.*;

import java.util.List;

public class App {

    public static void main(String[] args) throws Exception {
        Options options = getOptions();
        CommandLineParser parser = new DefaultParser();
        CommandLine cmd = parser.parse(options, args);
        Graph graph;
        List<Vertex> randomRoute;
        if (cmd.hasOption("tsp")) {
            graph = TSPConverter.readGraph(cmd.getOptionValue("tsp"));
            randomRoute = Utils.randomRoute(graph);
            Utils.printRouteCost(graph, randomRoute);
        } else {
            throw new RuntimeException("Graph file is mandatory");
        }
        if ("all".equals(cmd.getOptionValue("ls"))) {
            // 2-opt operator
            execute(OPT2Operator.class, graph, randomRoute);
            // 3-opt operator
            execute(OPT3Operator.class, graph, randomRoute);
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

    public static Options getOptions() {
        Options options = new Options();
        options.addOption(Option.builder("tsp").hasArg().numberOfArgs(1).argName("tsp_file").desc("TSP file for processing.").build());
        options.addOption(Option.builder("ls").hasArg().numberOfArgs(1).argName("local_search")
                .desc("all = All local search\n" +
                        "us = Unstringing and Stringing\n" +
                        "2-opt = 2-opt moves\n" +
                        "3-opt = 3-opt moves").build());
        return options;
    }

    public static void execute(Class<? extends LSOperator> operatorClass, Graph graph, List<Vertex> tour) throws Exception {
        String message = operatorClass.getSimpleName();
        while (message.length() < 20) message += " ";
        message += " - ";
        LSOperator operator = operatorClass.getDeclaredConstructor().newInstance();
        operator.init(graph, tour);
        Long time = System.currentTimeMillis();
        operator.optimize();
        System.out.printf("%sTime(ms) = %.8f ", message, ((System.currentTimeMillis() - time) / 1000.0));
        Utils.printRouteCost(graph, operator.getResult());
    }

}
