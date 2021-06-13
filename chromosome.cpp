/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <float.h>
#include "global.h"
#include "chromosome.h"

using namespace std;

extern string target_file;
extern string target_src;
extern string target_opt;
extern string target_bin;
double base_exec_time;
double O3_exec_time;

int num_of_flags = 38 + 1; // 1 null pass
string llvm_pass[] = {
    "argpromotion",
    "asan-globals-md",
    "codegenprepare",
    "constprop",
    "early-cse",
    "early-cse-memssa",
    "flattencfg",
    "functionattrs",
    "global-merge",
    "globalopt",
    "globalsplit",
    "gvn",
    "hardware-loops",
    "inferattrs",
    "instcombine",
    "interleaved-load-combine",
    "ipsccp",
    "lcssa",
    "lcssa-verification",
    "licm",
    "loop-simplifycfg",
    "loop-unroll",
    "loop-unswitch",
    "loop-vectorize",
    "lower-constant-intrinsics",
    "lower-widenable-condition",
    "mem2reg",
    "polly-canonicalize",
    "polly-optree",
    "polly-prepare",
    "polly-simplify",
    "prune-eh",
    "reg2mem",
    "slp-vectorizer",
    "sroa",
    "tailcallelim",
    "tbaa",
    "tti",
};

char* bash_exec(string cmd, bool retrieve_output)
{
    FILE* pipe = popen(cmd.c_str(),"r");
    if(!pipe){
        printf("popen failed to exec\n");
    }

    if(retrieve_output){
        char* output = new char[256];
        char c;
        int index = 0;
        while(c = fgetc(pipe)){
            if(c == EOF || index==255)
                break;
            output[index++] = c;
        }
        output[index] = '\0';
        fclose(pipe);
        /*if(strlen(output)){
            cout << '\a';   // beep beep i'm a sheep
        }*/
        return output;
    }

    fclose(pipe);
    return NULL;
}

Chromosome::Chromosome ()
{
    length = 0;
    gene = NULL;
    evaluated = false;
}


Chromosome::Chromosome (int n_length)
{
    gene = NULL;
    init (n_length);
}


Chromosome::~Chromosome ()
{
    delete[]gene;
}


void Chromosome::init (int n_length)
{
    length = n_length;

    if (gene != NULL)
        delete[]gene;

    gene = new int[length];
    evaluated = false;
}

int Chromosome::getVal (int index) const
{
    if (index < 0 || index > length)
        outputErrMsg ("Index overrange in Chromosome::operator[]");

    return gene[index];
}


void Chromosome::setVal (int index, int val)
{
    if (index < 0 || index > length)
        outputErrMsg ("Index overrange in Chromosome::operator[]");

    gene[index] = val;
    evaluated = false;
}


double Chromosome::getFitness (int worker_id, int repeat)
{
    if (evaluated)
        return fitness;
    else
        return (fitness = evaluate (worker_id, repeat));
}


bool Chromosome::isEvaluated () const
{
    return evaluated;
}


double Chromosome::evaluate (int worker_id, int repeat)
{
    evaluated = true;
    double d = eval_flag(worker_id, repeat);
    if(d>1.1){
        /*std::printf("found\n");
        fflush(NULL);*/
        string cmd = "echo ";
        cmd += (to_string(gene[0]) + " " + to_string(gene[1]) + " " + to_string(gene[2]));
        cmd += " >>subseq";
        bash_exec(cmd,false);
    }
    return d;
}


// OneMax
double Chromosome::oneMax () const
{
    int i;
    double result = 0;

    for (i = 0; i < length; i++)
        result += gene[i];

    return result;
}

double str2double(char* str){       // this is essentially atof, fuck me
    double result = 0, unit = 0.1;
    while(*str!='.' && *str!='\0'){
        //printf("%d\n",*str);
        result = result*10 + (*str - '0');
        str++;
    }
    str++;
    for(int i=0;i<2;i++){
        //printf("%d\n",*str);
        result += (*str - '0')*unit;
        unit /= 10;
        str++;
    }
    return result;
}

double Chromosome::eval_flag(int worker_id, int repeat) const
{
    char* out;

    // opt
    string cmd = "opt -S";
    for(int i=0;i<length;i++){
        if(gene[i])
            cmd += (" -"+llvm_pass[gene[i]-1]);
    }
    cmd += (" " + target_src);
    cmd += (" -o w"+ to_string(worker_id) + target_opt);
    cmd += (" 2>&1");
    // cout << "\n\n" << cmd << "\n\n" << endl;
    out = bash_exec(cmd,true);
    if(strlen(out)){
        // std::printf("optimization fail\n");
        // std::printf("%s",out);
        // exit(0);
        delete[] out;
        return 0;
    }
    delete[] out;

    // compile to obj file
    cmd = "llc --filetype=obj ";
    cmd += ("w" + to_string(worker_id) + target_opt);
    cmd += (" -o w" + to_string(worker_id) + target_bin);
    cmd += (" 2>&1");
    out = bash_exec(cmd,true);
    if(strlen(out)){
        // std::printf("compilation fail\n");
        // std::printf("%s",out);
        // exit(0);
        delete[] out;
        return 0;
    }
    delete[] out;

    // link
    cmd = "clang++ ";
    cmd += ("w" + to_string(worker_id) + target_bin);
    cmd += (" -o w" + to_string(worker_id) + target_file);
    cmd += (" 2>&1");
    out = bash_exec(cmd,true);
    if(strlen(out)){
        // std::printf("linking fail\n");
        // std::printf("%s",out);
        // exit(0);
        delete[] out;
        return 0;
    }
    delete[] out;

    // exec & measure
    cmd = "/usr/bin/time 2>&1 --format \"%S %U\" ";
    cmd += ("./w"+ to_string(worker_id) + target_file);
    cmd += ">/dev/null";
    /*cmd = "ls -l ";
    cmd += ("./"+target_file);*/

    /* TODO : adaptive repeat? longer single experiment time = less noisy = less repeated times */
    //int repeat = 1;
    double total_exec_time = 0;
    for(int i=0;i<repeat;i++){
        out = bash_exec(cmd,true);
        total_exec_time += (str2double(out) + str2double(out+5));
        //std::printf("\n\n\n\t\t\t%s   %f\n\n\n",out, total_exec_time);
        delete[] out;
    }

    if(!base_exec_time)
        return (total_exec_time/repeat);
    else
        return base_exec_time/(total_exec_time/repeat);   // speedup


    /*out = bash_exec(cmd,true);
    if(!out){
        std::printf("measuring fail\n");
        exit(0);
    }
    // parse output string
    int tokens = 0, id = 0;
    bool processing = false;
    string token;
    while(1){
        if(processing){
            if(out[id]!=' '){
                token += out[id];
            }
            else{
                tokens++;
                processing = false;
                if(tokens == 5)
                    break;
            }
            id++;
        }
        else{
            if(out[id]!=' '){
                token = "";
                token += out[id];
                processing = true;
            }
            id++;
        }
    }
    delete out;
    std::printf("filesize = %s\n",token.c_str());
    return atoi(token.c_str());*/
}


Chromosome & Chromosome::operator= (const Chromosome & c)
{
    int i;

    if (length != c.length) {
        length = c.length;
        delete[]gene;
        init (length);
    }

    evaluated = c.evaluated;
    fitness = c.fitness;

    for (i = 0; i < length; i++)
        gene[i] = c.gene[i];

    return *this;
}


void Chromosome::printf () const
{
    int i;
    for (i = 0; i < length; i++){
        if(gene[i])
            cout<<"--"<<llvm_pass[gene[i]-1]<<" ";
    }
}


int Chromosome::getLength () const
{
    return length;
}


double Chromosome::getMaxFitness () const
{
    // For OneMax
    return ((double)length-1e-6);
}