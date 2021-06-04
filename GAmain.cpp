/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 *                                                                         *
 *   You can redistribute it and/or modify it as you like                  *
 ***************************************************************************/

#include <cmath>
#include <cstdio>
#include <iostream>
#include <cstdlib>

#include "statistics.h"
#include "ga.h"
#include "chromosome.h"
#include "global.h"

using namespace std;

string target_cpp, target_file, target_src, target_opt, target_bin;

extern char* bash_exec(string, bool);

int main (int argc, char *argv[])
{

    if (argc != 10) {
        printf ("GA src_file ell nInitial selectionPressure pc pm maxGen maxFe repeat\n");
        return -1;
    }
    // TODO: check src_file existence
    printf("----------- Preparation -----------\n");
    char* target = argv[1];
    FILE *file;
    if (file = fopen(target, "r")) {
        fclose(file);
    }
    else{
        printf("source file \"%s\" doesn't exist\n",target);
        return -1;
    }
    target_cpp = target;
    target_file = target_cpp.substr(0,target_cpp.length()-4);
    target_src = target_file + ".ll";
    target_opt = target_file + "_opt.ll";
    target_bin = target_file + "_opt.o";
    string prepare_cmd = "clang++ -S -O1 -Xclang -disable-llvm-passes -emit-llvm ";
    prepare_cmd += target_cpp;
    prepare_cmd += (" -o "+target_src);
    printf("Preprocessing source file \"%s\"... \n",target);
    fflush(NULL);
    bash_exec(prepare_cmd,false);

    int ell = atoi (argv[2]);    // problem size
                                 // initial population size
    int nInitial = atoi (argv[3]);
                                 // selection pressure
    int selectionPressure = atoi (argv[4]);
    double pc = atof (argv[5]);  // pc
    double pm = atof (argv[6]);  // pm
    int maxGen = atoi (argv[7]); // max generation
    int maxFe = atoi (argv[8]);  // max fe
    int repeat = atoi (argv[9]); // how many time to repeat

    int i;

    Statistics stGenS, stGenF;
    int usedGen;

    int failNum = 0;

    for (i = 0; i < repeat; i++) {

        GA ga (ell, nInitial, selectionPressure, pc, pm, maxGen, maxFe);

        if (repeat == 1)
            usedGen = ga.doIt (true);
        else
            usedGen = ga.doIt (false);

        Chromosome ch(ell);
        if (ga.stFitness.getMax() < ch.getMaxFitness()) {
            printf ("-");
            failNum++;
            stGenF.record (usedGen);
        }
        else {
            printf ("+");
            stGenS.record (usedGen);
        }

        fflush (NULL);

    }

    printf ("\nAverage Gen of Success: %f\n", stGenS.getMean());
    printf ("Average Gen of Failure: %f\n", stGenF.getMean());
    printf ("Total number of Failure: %d\n", failNum);

    return EXIT_SUCCESS;
}
