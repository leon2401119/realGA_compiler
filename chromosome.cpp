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

int num_of_flags = 272; // 271 + 1 null pass
string llvm_pass[] = {
    "aa",
    "aa-eval",
    "adce",
    "add-discriminators",
    "aggressive-instcombine",
    "alignment-from-assumptions",
    "alloca-hoisting",
    "always-inline",
    "amode-opt",
    "argpromotion",
    "asan",
    "asan-globals-md",
    "asan-module",
    "assumption-cache-tracker",
    "atomic-expand",
    "attributor",
    "barrier",
    "basicaa",
    "basiccg",
    "bdce",
    "block-freq",
    "bool-ret-to-int",
    "bounds-checking",
    "bpf-abstract-member-access",
    "bpf-mi-trunc-elim",
    "bpf-mi-zext-elim",
    "branch-prob",
    "break-crit-edges",
    "called-value-propagation",
    "callsite-splitting",
    "canonicalize-aliases",
    "cfl-anders-aa",
    "cfl-steens-aa",
    "check-debugify",
    "check-debugify-function",
    "chr",
    "codegenprepare",
    "consthoist",
    "constmerge",
    "constprop",
    "coro-cleanup",
    "coro-early",
    "coro-elide",
    "coro-split",
    "correlated-propagation",
    "cost-model",
    "cross-dso-cfi",
    "cseinfo",
    "da",
    "dce",
    "deadargelim",
    "deadarghaX0r",
    "debugify",
    "debugify-function",
    "delinearize",
    "demanded-bits",
    "dfsan",
    "die",
    "div-rem-pairs",
    "divergence",
    "domfrontier",
    "domtree",
    "dse",
    "dwarfehprepare",
    "early-cse",
    "early-cse-memssa",
    "edge-bundles",
    "ee-instrument",
    "elim-avail-extern",
    "expand-reductions",
    "expandmemcmp",
    "external-aa",
    "extract-blocks",
    "flattencfg",
    "float2int",
    "forceattrs",
    "functionattrs",
    "generic-to-nvvm",
    "gisel-known-bits",
    "global-merge",
    "globaldce",
    "globalopt",
    "globals-aa",
    "globalsplit",
    "guard-widening",
    "gvn",
    "gvn-hoist",
    "gvn-sink",
    "hardware-loops",
    "hotcoldsplit",
    "hwasan",
    "hwloops",
    "indirectbr-expand",
    "indvars",
    "infer-address-spaces",
    "inferattrs",
    "inject-tli-mappings",
    "inline",
    "insert-gcov-profiling",
    "instcombine",
    "instcount",
    "instnamer",
    "instrorderfile",
    "instrprof",
    "instsimplify",
    "interleaved-access",
    "interleaved-load-combine",
    "intervals",
    "ipconstprop",
    "ipsccp",
    "irce",
    "irtranslator",
    "iv-users",
    "jump-threading",
    "lazy-block-freq",
    "lazy-branch-prob",
    "lazy-value-info",
    "lcssa",
    "lcssa-verification",
    "legalizer",
    "libcalls-shrinkwrap",
    "licm",
    "lint",
    "liveintervals",
    "liveregmatrix",
    "load-store-vectorizer",
    "localizer",
    "loop-accesses",
    "loop-data-prefetch",
    "loop-deletion",
    "loop-distribute",
    "loop-extract",
    "loop-extract-single",
    "loop-fusion",
    "loop-guard-widening",
    "loop-idiom",
    "loop-instsimplify",
    "loop-interchange",
    "loop-load-elim",
    "loop-predication",
    "loop-reduce",
    "loop-reroll",
    "loop-rotate",
    "loop-simplify",
    "loop-simplifycfg",
    "loop-sink",
    "loop-unroll",
    "loop-unroll-and-jam",
    "loop-unswitch",
    "loop-vectorize",
    "loop-versioning",
    "loop-versioning-licm",
    "loops",
    "lower-constant-intrinsics",
    "lower-expect",
    "lower-guard-intrinsic",
    "lower-matrix-intrinsics",
    "lower-widenable-condition",
    "loweratomic",
    "lowerinvoke",
    "lowerswitch",
    "lowertypetests",
    "make-guards-explicit",
    "mem2reg",
    "memcpyopt",
    "memdep",
    "memoryssa",
    "mergefunc",
    "mergeicmps",
    "mergereturn",
    "micromips-reduce-size",
    "mldst-motion",
    "module-debuginfo",
    "module-summary-analysis",
    "msan",
    "mve-gather-scatter-lowering",
    "mve-tail-predication",
    "name-anon-globals",
    "nary-reassociate",
    "newgvn",
    "nvvm-intr-range",
    "nvvm-reflect",
    "opt-remark-emitter",
    "pa-eval",
    "partial-inliner",
    "partially-inline-libcalls",
    "pgo-icall-prom",
    "pgo-instr-gen",
    "pgo-instr-use",
    "pgo-memop-opt",
    "phi-values",
    "place-backedge-safepoints-impl",
    "place-safepoints",
    "polly-ast",
    "polly-canonicalize",
    "polly-cleanup",
    "polly-codegen",
    "polly-dce",
    "polly-delicm",
    "polly-dependences",
    "polly-detect",
    "polly-dump-module",
    "polly-export-jscop",
    "polly-flatten-schedule",
    "polly-function-dependences",
    "polly-function-scops",
    "polly-import-jscop",
    "polly-mse",
    "polly-opt-isl",
    "polly-optree",
    "polly-prepare",
    "polly-prune-unprofitable",
    "polly-rewrite-byref-params",
    "polly-scop-inliner",
    "polly-scops",
    "polly-simplify",
    "polyhedral-info",
    "post-inline-ee-instrument",
    "postdomtree",
    "pre-isel-intrinsic-lowering",
    "profile-summary-info",
    "prune-eh",
    "reaching-deps-analysis",
    "reassociate",
    "redundant-dbg-inst-elim",
    "reg2mem",
    "regions",
    "rewrite-statepoints-for-gc",
    "rewrite-symbols",
    "rpo-functionattrs",
    "safe-stack",
    "sample-profile",
    "sancov",
    "scalar-evolution",
    "scalarize-masked-mem-intrin",
    "scalarizer",
    "sccp",
    "scev-aa",
    "scoped-noalias",
    "separate-const-offset-from-gep",
    "sjljehprepare",
    "slp-vectorizer",
    "slsr",
    "speculative-execution",
    "sroa",
    "stack-safety",
    "stack-safety-local",
    "stack-tagging",
    "strip",
    "strip-dead-debug-info",
    "strip-dead-prototypes",
    "strip-debug-declare",
    "strip-gc-relocates",
    "strip-nondebug",
    "strip-nonlinetable-debuginfo",
    "structurizecfg",
    "t2-reduce-size",
    "tailcallelim",
    "tbaa",
    "transform-warning",
    "tsan",
    "tti",
    "type-promotion",
    "unreachableblockelim",
    "vec-merger",
    "verify",
    "verify-safepoint-ir",
    "virtregmap",
    "wholeprogramdevirt",
    "winehprepare",
    "write-bitcode"
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