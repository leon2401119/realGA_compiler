/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include <string>
#include <iostream>
#include "global.h"
#include "chromosome.h"

using namespace std;

string target_file = "test_program";
string target_src = target_file + ".ll";
string target_opt = target_file + "_opt.ll";
string target_bin = target_file + "_opt.o";

int num_of_flags = 90;  // 89 actual passes, 1 empty pass = 0
string llvm_pass[] = {
    /* analysis passes */
    "aa-eval",
    "basic-aa",
    "basiccg",
    "count-aa",
    "da",
    "debug-aa",
    "domfrontier",
    "domtree",
    "dot-callgraph",
    "dot-cfg",
    "dot-cfg-only",
    "dot-dom",
    "dot-dom-only",
    "dot-postdom",
    "dot-postdom-only",
    "globalsmodref-aa",
    "instcount",
    "intervals",
    "iv-users",
    "lazy-value-info",
    "libcall-aa",
    "lint",
    "loops",
    "memdep",
    "module-debuginfo",
    "postdomfrontier",
    "postdomtree",      // several print analysis passes omitted
    "regions",
    "scalar-evolution",
    "scev-aa",
    "stack-safety",
    "targetdata",

    /* transformation passes */
    "acde",
    "always-inline",
    "argpromotion",
    "bb-vectorize",
    "block-placement",
    "break-crit-edges",
    "codegenprepare",
    "constmerge",
    "dce",
    "deadargelim",
    "deadtypeelim",
    "die",
    "dse",
    "function-attrs",
    "globaldce",
    "globalopt",
    "gvn",
    "indvars",
    "inline",
    "instcombine",
    "aggressive-instcombine",
    "internalize",
    "ipsccp",
    "jump-threading",
    "lcssa",
    "licm",
    "loop-deletion",
    "loop-extract",
    "loop-extract-single",
    "loop-reduce",
    "loop-rotate",
    "loop-simplify",
    "loop-unroll",
    "loop-unroll-and-jam",
    "loop-unswitch",
    "loweratomic",
    "lowerinvoke",
    "lowerswitch",
    "mem2reg",
    "memcpyopt",
    "mergefunc",
    "mergereturn",
    "partial-inliner",
    "prune-eh",
    "reassociate",
    "rel-lookup-table-converter",
    "reg2mem",
    "sroa",
    "sccp",
    "simplifycfg",
    "sink",
    "strip",
    "strip-dead-debug-info",
    "strip-dead-prototypes",
    "strip-debug-declare",
    "strip-nondebug",
    "tailcallelim"
};


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


double Chromosome::getFitness ()
{
    if (evaluated)
        return fitness;
    else
        return (fitness = evaluate ());
}


bool Chromosome::isEvaluated () const
{
    return evaluated;
}


double Chromosome::evaluate ()
{
    evaluated = true;
    return oneMax ();
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

char* bash_exec(string cmd, bool retrieve_output)
{
    FILE* pipe = popen(cmd.c_str(),"r");
    if(!pipe){
        printf("popen failed to exec\n");
    }

    if(retrieve_output){
        char* output_buffer = new char[256];
        fgets(output_buffer,sizeof(output_buffer),pipe);
        fclose(pipe);
        return output_buffer;
    }

    fclose(pipe);
    return NULL;
}


double Chromosome::eval_flag() const
{
    char* out;
    double exec_time;

    // opt
    string cmd = "opt -S";
    for(int i=0;i<num_of_flags;i++){
        if(gene[i])
            cmd += (" -"+llvm_pass[gene[i]-1]);
    }
    cmd += (target_src);
    cmd += ("-o "+target_opt);
    out = bash_exec(cmd,true);
    if(out){
        std::printf("optimization fail\n");
        std::printf("%s",out);
        exit(0);
    }

    // compile to obj file
    cmd = "llc --filetype=obj ";
    cmd += (target_opt);
    cmd += ("-o "+target_bin);
    out = bash_exec(cmd,true);
    if(out){
        std::printf("compilation fail\n");
        std::printf("%s",out);
        exit(0);
    }

    // link
    cmd = "clang++ ";
    cmd += (target_bin);
    cmd += ("-o "+target_file);
    out = bash_exec(cmd,true);
    if(out){
        std::printf("linking fail\n");
        std::printf("%s",out);
        exit(0);
    }

    // exec & measure
    cmd = "usr/bin/time --format \"%S %U\" ";
    cmd += (target_file);
    cmd += ">/dev/null";


    /* TODO : adaptive repeat? longer single experiment time = less noisy = less repeated times */
    int repeat = 1000;
    for(int i=0;i<repeat;i++){
        out = bash_exec(cmd,true);
        if(!out){
            std::printf("timing fail\n");
            exit(0);
        }
        exec_time += ((double)atof(out) + (double)atof(out+4));
        delete out;
    }

    return exec_time/repeat;
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
    for (i = 0; i < length; i++)
        ::printf ("%d", gene[i]);
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
