/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <float.h>
#include <math.h>
#include <time.h>
#include "global.h"
#include "statistics.h"
#include "myrand.h"
#include "ga.h"
#include <vector>
#include <queue>
#include <pthread.h>

extern int num_of_flags;
extern std::string llvm_pass[];
extern double base_exec_time;
extern double O3_exec_time;
extern std::string target_cpp;

void* runner(void*);
double str2double(char*);
char* bash_exec(std::string, bool);

GA::GA ()
{
    ell = 0;
    nInitial = 0;
    nCurrent = 0;
    fe = 0;

    nNextGeneration = 0;
    maxGen = -1;
    maxFe = -1;

    population = NULL;
    offspring = NULL;
    selectionIndex = NULL;

}


GA::GA (int n_ell, int n_nInitial, int n_selectionPressure, double n_pc, double n_pm, int n_maxGen, int n_maxFe)
{
    init (n_ell, n_nInitial, n_selectionPressure, n_pc, n_pm, n_maxGen, n_maxFe);
}


GA::~GA ()
{

    delete[]population;
    delete[]offspring;
    delete[]selectionIndex;
}


void
GA::init (int n_ell, int n_nInitial, int n_selectionPressure, double n_pc,
double n_pm, int n_maxGen, int n_maxFe)
{
    int i;

    ell = n_ell;
    nInitial = n_nInitial;
    nCurrent = nInitial;
    selectionPressure = n_selectionPressure;
    pc = n_pc;
    pm = n_pm;
    maxGen = n_maxGen;
    maxFe = n_maxFe;

    Chromosome *base = new Chromosome;
    base->init(ell);
    for(int i=0;i<ell;i++)
        base->setVal(i,0);  // empty pass

    /* test the flags one by one and eliminate infeasible ones */
    /*FILE* p;
    int temp;
    p = fopen ("invalid_indexes","r");
    temp = p? restore_flags(p):test_flags(p,base);
    num_of_flags -= temp;*/
    /* end test */

    // measure comparative exec time
    int star_repeat = 10;

    base_exec_time = 0;
    printf("Measuring performance... \n");
    fflush(NULL);
    base->setVal(0,0);
    base_exec_time = base->getFitness(1,star_repeat);
    delete base;

    /* test O3 */
    char *out;
    O3_exec_time = 0;
    std::string cmd = "clang++ -O3 ";
    cmd += target_cpp;
    cmd += " -o O3 && /usr/bin/time 2>&1 --format \"%S %U\" ./O3 >/dev/null";
    for(int i=0;i<star_repeat;i++){
        out = bash_exec(cmd,true);
        O3_exec_time += (str2double(out) + str2double(out+5));
        delete[] out;
    }
    O3_exec_time/=star_repeat;
    printf("\tBase:%.2fs\n",base_exec_time);
    printf("\tO3:%.2fs  (Speedup = %.2fx)\n",O3_exec_time,base_exec_time/O3_exec_time);

    printf("\nDONE!\n\n");

    /*population = new Chromosome[nInitial];
    offspring = new Chromosome[nInitial];
    selectionIndex = new int[nInitial];

    for (i = 0; i < nInitial; i++) {
        population[i].init (ell);
        offspring[i].init (ell);
    }

    initializePopulation ();*/

    ell = 3;
    nInitial = num_of_flags * num_of_flags * num_of_flags;
    nCurrent = nInitial;
    population = new Chromosome[nInitial];
    offspring = new Chromosome[nInitial];
    selectionIndex = new int[nInitial];

    for (i = 0; i < nInitial; i++) {
        population[i].init (ell);
        offspring[i].init (ell);
    }

    initializePopulation ();
}

int GA::test_flags(FILE* p, Chromosome* base){
    std::queue<int> forbidden;
    Chromosome *tester = base;
    double exec_time;
    int temp;
    for(int i=0;i<num_of_flags-1;i++){
        tester->setVal(0,i+1);
        printf("\rTesting flag %d of %d...",i+1,num_of_flags-1);
        fflush(NULL);
        exec_time = tester->getFitness(1,1);
        if(!exec_time)
            forbidden.push(i);
    }

    p = fopen ("invalid_indexes","w");
    temp = forbidden.size();
    printf("\nDetected %d invalid flags! Which are :\n",temp);
    for(int i=0,id=0;i<num_of_flags;i++){
        if(i!=forbidden.front())
            llvm_pass[id++] = llvm_pass[i];
        else{
            fprintf(p,"%d\n",forbidden.front());
            printf("\t-%s\n",llvm_pass[forbidden.front()].c_str());
            forbidden.pop();
        }
    }
    fclose(p);
    return temp;
}

int GA::restore_flags(FILE* p){
    std::queue<int> forbidden;
    int tmp;
    while(fscanf(p,"%d",&tmp)!=EOF){
        forbidden.push(tmp);
    }
    fclose(p);

    tmp = forbidden.size();
    printf("Recovered %d invalid flags!\n",tmp);
    for(int i=0,id=0;i<num_of_flags;i++){
        if(i!=forbidden.front())
            llvm_pass[id++] = llvm_pass[i];
        else{
            //printf("\t-%s\n",llvm_pass[forbidden.front()].c_str());
            forbidden.pop();
        }
    }
    return tmp;
}


void GA::initializePopulation ()
{
    /*
    int i, j;
    double p = 0.5;

    for (i = 0; i < nInitial; i++)
        for (j = 0; j < ell; j++)
            population[i].setVal (j, myRand.uniformInt(0, num_of_flags-1));

    */
    int pop_size = nCurrent;
    for(int i=0;i<pop_size;i++){
        int tmp = i;
        for(int j=2;j>=0;j--){
            population[i].setVal(j,tmp%num_of_flags);
            tmp/=num_of_flags;
        }
    }
}

struct args4thread{
    Chromosome* c;
    int worker_id;
};

void* runner(void* arg){
    Chromosome* c = ((args4thread*)arg)->c;
    int worker_id = ((args4thread*)arg)->worker_id;
    c->getFitness(worker_id);
    return NULL;
}

void GA::getAllFitness(){
    int num_workers = 6;
    pthread_t* workers = new pthread_t[num_workers];

    /* create placeholder for args to pass into thread */
    args4thread* args = new args4thread[num_workers];
    for(int i=0;i<num_workers;i++){
        args[i].c = NULL;
        args[i].worker_id = i+1;
    }

    for(int i=0;i<nCurrent/num_workers;i++){
        for(int j=0;j<num_workers;j++){
            /* fill placeholder */
            args[j].c = &population[i*num_workers+j];
            pthread_create(&(workers[j]),NULL,&runner,(void*)&(args[j]));
        }
        for(int j=0;j<num_workers;j++){
            /*if(!((i*num_workers+j)%(nCurrent/100))){
                printf("\rEvaluating generation... %d%%", (int)(100.0*(i*num_workers+j)/nCurrent));
            }*/
            printf("\rEvaluating %d of %d", i*num_workers+j,nCurrent);
            fflush(NULL);
            pthread_join(workers[j],NULL);
        }
    }
    for(int i=0;i<nCurrent%num_workers;i++){
        args[i].c = &population[nCurrent-1-i];
        pthread_create(&(workers[i]),NULL,&runner,(void*)&(args[i]));
    }
    //printf("\rEvaluating generation... 100%%\n");
    printf("\n");
    for(int i=0;i<nCurrent%num_workers;i++)
        pthread_join(workers[i],NULL);

    delete []workers;
    delete []args;
}


// For now, assuming fixed population size
int GA::getNextPopulation ()
{
    return nCurrent;
}

void GA::selection ()
{
    //rwSelection ();
    tournamentSelection ();
}

// Roulette wheel selection
// This is a O(n^2) implementation
// You can achieve O(nlogn) by using binary search
void GA::rwSelection ()
{
    int i, j;

    // Adjusting population size 
    nNextGeneration = getNextPopulation ();

    if (nNextGeneration != nCurrent) {
        delete[]selectionIndex;
        delete[]offspring;
        selectionIndex = new int[nNextGeneration];
        offspring = new Chromosome[nNextGeneration];

        for (i = 0; i < nNextGeneration; i++)
            offspring[i].init (ell);
    }

    double totalFitness = 0.0;
    for (i = 0; i < nCurrent; i++) 
	totalFitness += population[i].getFitness();

    for (i = 0; i < nNextGeneration; i++) {
	double pointer = totalFitness * myRand.uniform();
	int index = -1;
	double partialSum = 0.0;
	for (j = 0; j < nCurrent; j++) {
	    partialSum += population[j].getFitness();
            if (partialSum >= pointer) {
                index = j;
                break;
            }
	}
	if (index == -1) index = nCurrent - 1;

	selectionIndex[i] = index;
    }

}

// tournamentSelection without replacement
void GA::tournamentSelection ()
{
    int i, j;

    // Adjusting population size 
    nNextGeneration = getNextPopulation ();

    if (nNextGeneration != nCurrent) {
        delete[]selectionIndex;
        delete[]offspring;
        selectionIndex = new int[nNextGeneration];
        offspring = new Chromosome[nNextGeneration];

        for (i = 0; i < nNextGeneration; i++)
            offspring[i].init (ell);
    }

    int randArray[selectionPressure * nNextGeneration];

    int q = (selectionPressure * nNextGeneration) / nCurrent;
    int r = (selectionPressure * nNextGeneration) % nCurrent;

    for (i = 0; i < q; i++)
        myRand.uniformArray (randArray + (i * nCurrent), nCurrent, 0, nCurrent - 1);

    myRand.uniformArray (randArray + (q * nCurrent), r, 0, nCurrent - 1);

    for (i = 0; i < nNextGeneration; i++) {

        int winner = 0;
        double winnerFitness = -DBL_MAX;

        for (j = 0; j < selectionPressure; j++) {
            int challenger = randArray[selectionPressure * i + j];
            double challengerFitness = population[challenger].getFitness ();

            if (challengerFitness > winnerFitness) {
                winner = challenger;
                winnerFitness = challengerFitness;
            }

        }
        selectionIndex[i] = winner;
    }
}


void GA::crossover ()
{
    int i;

    if ((nNextGeneration & 0x1) == 0) { 
    	// nNextGeneration is even
    	
        for (i = 0; i < nNextGeneration; i += 2)
            pairwiseXO (population[selectionIndex[i]], population[selectionIndex[i + 1]],
                offspring[i], offspring[i + 1]);

    }
    else {
        for (i = 0; i < nNextGeneration - 1; i += 2) {
            pairwiseXO (population[selectionIndex[i]], population[selectionIndex[i + 1]],
                offspring[i], offspring[i + 1]);
        }
        offspring[nNextGeneration - 1] =
            population[selectionIndex[nNextGeneration - 1]];
    }

}


void GA::pairwiseXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    if (myRand.uniform () < pc) {
    //flatXO(p1, p2, c1, c2);
	onePointXO (p1, p2, c1, c2);
    //uniformXO (p1, p2, c1, c2, 0.5);
    }
    else {
        c1 = p1;
        c2 = p2;
    }
}

void GA::onePointXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    int i;
    int crossSite = myRand.uniformInt(1, ell-1);

    for (i = 0; i < crossSite; i++) {
            c1.setVal (i, p1.getVal(i));
            c2.setVal (i, p2.getVal(i));
    }

    for (i = crossSite; i < ell; i++) {
            c1.setVal (i, p2.getVal(i));
            c2.setVal (i, p1.getVal(i));
    }
}
void GA::arithmaticalXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    
    double alpha = myRand.uniform();
    for (int i = 0; i < ell; i++) {
        c1.setVal (i, alpha*p1.getVal(i)+(1-alpha)*p2.getVal(i));
    }
    alpha = myRand.uniform();
    for (int i = 0; i < ell; i++) {
        c2.setVal (i, alpha*p1.getVal(i)+(1-alpha)*p2.getVal(i));
    }
}
void GA::extendedXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    
    double alpha = myRand.uniform(-0.25,1.25);
    for (int i = 0; i < ell; i++) {
        c1.setVal (i, alpha*p1.getVal(i)+(1-alpha)*p2.getVal(i));
    }
    alpha = myRand.uniform(-0.25,1.25);
    for (int i = 0; i < ell; i++) {
        c2.setVal (i, alpha*p1.getVal(i)+(1-alpha)*p2.getVal(i));
    }
}
void GA::uniformXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2, double prob)
{
    int i;

    for (i = 0; i < ell; i++) {
        if (myRand.flip (prob)) {
            c1.setVal (i, p1.getVal(i));
            c2.setVal (i, p2.getVal(i));
        }
        else {
            c1.setVal (i, p2.getVal(i));
            c2.setVal (i, p1.getVal(i));
        }
    }
}
void GA::flatXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    for (int i = 0; i < ell; i++) {
        c1.setVal (i, myRand.uniform(p1.getVal(i), p2.getVal(i)));
        c2.setVal (i, myRand.uniform(p1.getVal(i), p2.getVal(i)));
    }
}
void GA::BLXXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    float cmin,cmax,I;
    for (int i = 0; i < ell; i++) {
        cmin=(p1.getVal(i) < p2.getVal(i))?p1.getVal(i): p2.getVal(i);
        cmax=(p1.getVal(i) > p2.getVal(i))?p1.getVal(i): p2.getVal(i);
        I=cmax-cmin;
        c1.setVal (i, myRand.uniform(cmin-0.5*I, cmax+0.5*I));
        c2.setVal (i, myRand.uniform(cmin-0.5*I, cmax+0.5*I));
    }
}
/*void GA::SPX-XO (Vector<int>parentList, Chromosome & c1, Chromosome & c2)
{
    float cmin,cmax,I;
    for (int i = 0; i < ell; i++) {
        cmin=(p1.getVal(i) < p2.getVal(i))?p1.getVal(i): p2.getVal(i);
        cmax=(p1.getVal(i) > p2.getVal(i))?p1.getVal(i): p2.getVal(i);
        I=cmax-cmin;
        c1.setVal (i, myRand.uniform(cmin-0.5*I, cmax+0.5*I));
        c2.setVal (i, myRand.uniform(cmin-0.5*I, cmax+0.5*I));
    }
}*/
void GA::mutation ()
{
    //simpleMutation ();
    mutationClock ();
}


void GA::simpleMutation ()
{
    int i, j;

    for (i = 0; i < nNextGeneration; i++)
        for (j = 0; j< ell; j++)
            if (myRand.flip(pm)) {
                int val = offspring[i].getVal(j);
                offspring[i].setVal(j, 1-val);
            }
}

void GA::mutationClock ()
{
    if (pm <= 1e-6) return; // can't deal with too small pm

    int pointer = (int) (log(1-myRand.uniform()) / log(1-pm) + 1);

    while (pointer < nNextGeneration * ell) {

	int q = pointer / ell;
	int r = pointer % ell;

        int val = offspring[q].getVal(r);
        offspring[q].setVal(r, 1-val);

	// Compute next mutation clock
	pointer += (int) (log(1-myRand.uniform()) / log(1-pm) + 1);
    };
}


void GA::showStatistics ()
{
    printf ("\nSummary:\n\tFitness:(Max/Mean/Min): %.5f/%.5f/%.5f \n\tChromsome Length: %d\n\tInfeasible%%: %d%%\n\tElapsed time: %.2f seconds\n",
        stFitness.getMax (), stFitness.getMean (),
        stFitness.getMin (), population[0].getLength (), 100*infeasible/nCurrent, elapsed);
    printf("\nBest fitness: %.3f", population[bestIndex].getFitness());
    printf ("\nBest sequence: ");
    //printf("\n\n%d\n\n", bestIndex);
    population[bestIndex].printf ();
    printf ("\n\n\n");
}


void GA::replacePopulation ()
{
    int i;

    if (nNextGeneration != nCurrent) {
        delete[]population;
        population = new Chromosome[nNextGeneration];
    }

    for (i = 0; i < nNextGeneration; i++)
        population[i] = offspring[i];

    nCurrent = nNextGeneration;
}


void GA::oneRun (bool output)
{
    infeasible = 0;
    time_t start,end;
    time(&start);

    printf("----------- Generation %d -----------\n", generation);
    printf("Selecting...\n");
    selection ();
    printf("Mating...\n");
    crossover ();
    printf("Alienizing...\n");
    mutation ();
    printf("Repopulating...\n");
    replacePopulation ();

    getAllFitness();

    time(&end);

    double max = -DBL_MAX;
    stFitness.reset ();
    for (int i = 0; i < nCurrent; i++) {
        double fitness = population[i].getFitness ();
        if (fitness > max) {
            max = fitness;
            bestIndex = i;
        }
        infeasible += (!fitness)?1:0;
        stFitness.record (fitness);
    }

    elapsed = difftime(end,start);
    // printf("Elapsed time: %.2f seconds\n", difftime(end,start));

    if (output)
        showStatistics ();

    generation++;
}


int GA::doIt (bool output)
{
    generation = 0;
    infeasible = 0;

    printf("----------- Initialization -----------\n");
    time_t start,end;
    time(&start);
    getAllFitness();
    time(&end);
    double max = -DBL_MAX;
    stFitness.reset ();
    for (int i = 0; i < nCurrent; i++) {
        double fitness = population[i].getFitness ();
        if (fitness > max) {
            max = fitness;
            bestIndex = i;
        }
        infeasible += (!fitness)?1:0;
        stFitness.record (fitness);
    }
    elapsed = difftime(end,start);
    showStatistics ();

    /*
    while (!shouldTerminate ()) {
        oneRun (output);
    }
    */
    return generation;
}


bool GA::shouldTerminate ()
{
    bool termination = false;

    // Reach maximal # of function evaluations
    if (maxFe != -1) {
        if (fe > maxFe)
            termination = true;
    }

    // Reach maximal # of generations
    if (maxGen != -1) {
        if (generation > maxGen)
            termination = true;
    }

    // Found a satisfactory solution
    /*if (stFitness.getMax() >= population[0].getMaxFitness())
        termination = true;*/

    // The population loses diversity
    if (stFitness.getMax()-1e-6 < stFitness.getMean())
	termination = true;

    return termination;

}
