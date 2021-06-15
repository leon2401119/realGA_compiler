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
#include <cstring>

#include "statistics.h"
#include "ga.h"
#include "chromosome.h"
#include "global.h"

using namespace std;

string target_cpp, target_file, target_src, target_opt, target_bin;

extern double base_exec_time, O3_exec_time;
extern char* bash_exec(string, bool);
extern int workers_global;

class args{
public:
    /*
        initialization of ell and nInitial does not matter here since it will be
        dynamically adjusted according to the time for one fitness eval call by default
    */
    args():target(NULL),threads(1),ell(10),nInitial(1000),selectionPressure(2),pc(1.0),pm(0.0),maxGen(3),maxFe(-1),repeat(1),lock_ell(false),lock_nInitial(false){};
    char* target;
    int threads;
    int ell;
    int nInitial;
    int selectionPressure;
    double pc;
    double pm;
    int maxGen;
    int maxFe;
    int repeat;
    bool lock_ell;
    bool lock_nInitial;
};

args argparser(int argc, char* argv[]){
    if(argc < 2){
        printf("Usage: GAPO [FILE]... [OPTION]...\n");
    }
    struct args main_args;
    main_args.target = argv[1];
    for(int i=2;i<argc;i++){
        if(!strcmp(argv[i],"--help") || !strcmp(argv[i],"-h")){
            printf("Usage: GAPO [FILE]... [OPTION]...\nOptions: len, popsize, sp, pc, pm, gen, nfe, threads, repeat\n");
            exit(0);
        }
        else if(!strcmp(argv[i],"--threads")){
            main_args.threads = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i],"--len")){
            main_args.ell = atoi(argv[++i]);
            main_args.lock_ell = true;
        }
        else if(!strcmp(argv[i],"--popsize")){
            main_args.nInitial = atoi(argv[++i]);
            main_args.lock_nInitial = true;
        }
        else if(!strcmp(argv[i],"--sp")){
            main_args.selectionPressure = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i],"--pc")){
            main_args.pc = atof(argv[++i]);
        }
        else if(!strcmp(argv[i],"--pm")){
            main_args.pm = atof(argv[++i]);
        }
        else if(!strcmp(argv[i],"--gen")){
            main_args.maxGen = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i],"--nfe")){
            main_args.maxFe = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i],"--repeat")){
            main_args.repeat = atoi(argv[++i]);
        }
        else{
            printf("Unknown argument \"%s\"\n",argv[i]);
            printf("Usage: GAPO [FILE]... [OPTION]...\nOptions: len, popsize, sp, pc, pm, gen, nfe, threads, repeat\n");
            exit(0);
        }
    }
    return main_args;
}

int main (int argc, char *argv[])
{
    args main_args = argparser(argc,argv);

    // TODO: check src_file existence
    printf("----------- Preparation -----------\n");
    char* target = main_args.target;
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

    int ell = main_args.ell;    // problem size
                                 // initial population size
    int nInitial = main_args.nInitial;
                                 // selection pressure
    int selectionPressure = main_args.selectionPressure;
    double pc = main_args.pc;  // pc
    double pm = main_args.pm;  // pm
    int maxGen = main_args.maxGen; // max generation
    int maxFe = main_args.maxFe;  // max fe
    int repeat = main_args.repeat; // how many time to repeat
    workers_global = main_args.threads;

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

        //Chromosome ch(ell);
        //if (ga.stFitness.getMax() < ch.getMaxFitness()) {
        if (ga.stFitness.getMax() < base_exec_time/O3_exec_time) {
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
