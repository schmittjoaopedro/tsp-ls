package com.github.schmittjoaopedro;

import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.FileInputStream;

public class TSPConverter {

    public static Graph readGraph(String tspFile) {
        if(tspFile.endsWith(".atsp")) {
            return ATSPConverter.readGraph(tspFile);
        } else {
            return STSPConverter.readGraph(tspFile);
        }
    }

}
