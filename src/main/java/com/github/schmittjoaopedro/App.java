package com.github.schmittjoaopedro;

import com.github.schmittjoaopedro.us.USOperator;
import org.apache.commons.cli.*;

import java.util.List;

public class App {

    public static void main(String[] args) throws Exception {
        Options options = getOptions();
        CommandLineParser parser = new DefaultParser();
        CommandLine cmd = parser.parse(options, args);

        if (cmd.hasOption("tsp") && "us".equals(cmd.getOptionValue("ls"))) {
            Graph graph = TSPConverter.readGraph(cmd.getOptionValue("tsp"));
            List<Vertex> randomRoute = Utils.randomRoute(graph);
            Utils.printRouteCost(graph, randomRoute);
            USOperator usOperator = new USOperator();
            List<Vertex> optimized = usOperator.optimize(graph, randomRoute);
            Utils.printRouteCost(graph, optimized);
        } else {
            HelpFormatter helpFormatter = new HelpFormatter();
            helpFormatter.printHelp("Local search for TSP instances.", options);
        }
    }

    public static Options getOptions() {
        Options options = new Options();
        options.addOption(Option.builder("tsp").hasArg().numberOfArgs(1).argName("tsp_file").desc("TSP file for processing.").build());
        options.addOption(Option.builder("ls").hasArg().numberOfArgs(1).argName("local_search").desc("Local search operator: us (Unstringing and Stringing).").build());
        return options;
    }

}
