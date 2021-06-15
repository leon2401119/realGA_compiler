/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "global.h"
#include "statistics.h"
#include "myrand.h"
#include "ga.h"
#include <algorithm>
#include <vector>
GA::GA()
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

GA::GA(int n_ell, int n_nInitial, int n_selectionPressure, double n_pc, double n_pm, int n_maxGen, int n_maxFe)
{
    init(n_ell, n_nInitial, n_selectionPressure, n_pc, n_pm, n_maxGen, n_maxFe);
}

GA::~GA()
{

    delete[] population;
    delete[] offspring;
    delete[] selectionIndex;
}

void GA::init(int n_ell, int n_nInitial, int n_selectionPressure, double n_pc,
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

    population = new Chromosome[nInitial];
    offspring = new Chromosome[nInitial];
    selectionIndex = new int[nInitial];

    for (i = 0; i < nInitial; i++)
    {
        population[i].init(ell);
        offspring[i].init(ell);
    }

    initializePopulation();
}

void GA::initializePopulation()
{
    int i, j;
    double p = 0.5;

    for (i = 0; i < nInitial; i++)
        for (j = 0; j < ell; j++)
            if (myRand.uniform() > p)
                population[i].setVal(j, 1);
            else
                population[i].setVal(j, 0);
}

// For now, assuming fixed population size
int GA::getNextPopulation()
{
    return nCurrent;
}

void GA::selection()
{
    //rwSelection ();
    tournamentSelection();
}

// Roulette wheel selection
// This is a O(n^2) implementation
// You can achieve O(nlogn) by using binary search
void GA::rwSelection()
{
    int i, j;

    // Adjusting population size
    nNextGeneration = getNextPopulation();

    if (nNextGeneration != nCurrent)
    {
        delete[] selectionIndex;
        delete[] offspring;
        selectionIndex = new int[nNextGeneration];
        offspring = new Chromosome[nNextGeneration];

        for (i = 0; i < nNextGeneration; i++)
            offspring[i].init(ell);
    }

    double totalFitness = 0.0;
    for (i = 0; i < nCurrent; i++)
        totalFitness += population[i].getFitness();

    for (i = 0; i < nNextGeneration; i++)
    {
        double pointer = totalFitness * myRand.uniform();
        int index = -1;
        double partialSum = 0.0;
        for (j = 0; j < nCurrent; j++)
        {
            partialSum += population[j].getFitness();
            if (partialSum >= pointer)
            {
                index = j;
                break;
            }
        }
        if (index == -1)
            index = nCurrent - 1;

        selectionIndex[i] = index;
    }
}

// tournamentSelection without replacement
void GA::tournamentSelection()
{
    int i, j;

    // Adjusting population size
    nNextGeneration = getNextPopulation();

    if (nNextGeneration != nCurrent)
    {
        delete[] selectionIndex;
        delete[] offspring;
        selectionIndex = new int[nNextGeneration];
        offspring = new Chromosome[nNextGeneration];

        for (i = 0; i < nNextGeneration; i++)
            offspring[i].init(ell);
    }

    int randArray[selectionPressure * nNextGeneration];

    int q = (selectionPressure * nNextGeneration) / nCurrent;
    int r = (selectionPressure * nNextGeneration) % nCurrent;

    for (i = 0; i < q; i++)
        myRand.uniformArray(randArray + (i * nCurrent), nCurrent, 0, nCurrent - 1);

    myRand.uniformArray(randArray + (q * nCurrent), r, 0, nCurrent - 1);

    for (i = 0; i < nNextGeneration; i++)
    {

        int winner = 0;
        double winnerFitness = -DBL_MAX;

        for (j = 0; j < selectionPressure; j++)
        {
            int challenger = randArray[selectionPressure * i + j];
            double challengerFitness = population[challenger].getFitness();

            if (challengerFitness > winnerFitness)
            {
                winner = challenger;
                winnerFitness = challengerFitness;
            }
        }
        selectionIndex[i] = winner;
    }
}

void GA::crossover()
{
    int i;

    if ((nNextGeneration & 0x1) == 0)
    {
        // nNextGeneration is even

        for (i = 0; i < nNextGeneration; i += 2)
            pairwiseXO(population[selectionIndex[i]], population[selectionIndex[i + 1]],
                       offspring[i], offspring[i + 1]);
    }
    else
    {
        for (i = 0; i < nNextGeneration - 1; i += 2)
        {
            pairwiseXO(population[selectionIndex[i]], population[selectionIndex[i + 1]],
                       offspring[i], offspring[i + 1]);
        }
        offspring[nNextGeneration - 1] =
            population[selectionIndex[nNextGeneration - 1]];
    }
}

void GA::pairwiseXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{
    if (myRand.uniform() < pc)
    {
        onePointXO(p1, p2, c1, c2);
        //      uniformXO (p1, p2, c1, c2, 0.5);
    }
    else
    {
        c1 = p1;
        c2 = p2;
    }
}

void GA::onePointXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{
    int i;
    int crossSite = myRand.uniformInt(1, ell - 1);

    for (i = 0; i < crossSite; i++)
    {
        c1.setVal(i, p1.getVal(i));
        c2.setVal(i, p2.getVal(i));
    }

    for (i = crossSite; i < ell; i++)
    {
        c1.setVal(i, p2.getVal(i));
        c2.setVal(i, p1.getVal(i));
    }
}

void GA::nPointXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)

{

    int i, j;

    int * crossSite = new int[nCurrent + 1];

    crossSite[0] = 0;

    for (i = 1; i <= nCurrent; i++)
    {

        crossSite[i] = myRand.uniformInt(1, ell - 1);
    }
        std::sort(crossSite + 1, crossSite + nCurrent+1)      

        for (j = 0; j < nCurrent - 2; j+=2)
        {

            if (j % 2 == 0)
            {

                for (i = crossSite[j]; i <= crossSite[j + 1]; i++)
                {

                    c1.setVal (i, p1.getVal(i));

                    c2.setVal (i, p2.getVal(i));
                }
            }
            else
            {

                for (i = crossSite[j]; i <= crossSite[j + 1]; i++)
                {

                    c1.setVal (i, p2.getVal(i));

                    c2.setVal (i, p1.getVal(i));
                }
            }
        }
}

void GA::arithmaticalXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{

    double alpha = myRand.uniform();
    for (int i = 0; i < ell; i++)
    {
        c1.setVal(i, alpha * p1.getVal(i) + (1 - alpha) * p2.getVal(i));
    }
    alpha = myRand.uniform();
    for (int i = 0; i < ell; i++)
    {
        c2.setVal(i, alpha * p1.getVal(i) + (1 - alpha) * p2.getVal(i));
    }
}
void GA::extendedXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{

    double alpha = myRand.uniform(-0.25, 1.25);
    for (int i = 0; i < ell; i++)
    {
        c1.setVal(i, alpha * p1.getVal(i) + (1 - alpha) * p2.getVal(i));
    }
    alpha = myRand.uniform(-0.25, 1.25);
    for (int i = 0; i < ell; i++)
    {
        c2.setVal(i, alpha * p1.getVal(i) + (1 - alpha) * p2.getVal(i));
    }
}
void GA::uniformXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2, double prob)
{
    int i;

    for (i = 0; i < ell; i++)
    {
        if (myRand.flip(prob))
        {
            c1.setVal(i, p1.getVal(i));
            c2.setVal(i, p2.getVal(i));
        }
        else
        {
            c1.setVal(i, p2.getVal(i));
            c2.setVal(i, p1.getVal(i));
        }
    }
}
void GA::flatXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{
    for (int i = 0; i < ell; i++)
    {
        c1.setVal(i, myRand.uniform(p1.getVal(i), p2.getVal(i)));
        c2.setVal(i, myRand.uniform(p1.getVal(i), p2.getVal(i)));
    }
}
void GA::BLXXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{
    float cmin, cmax, I;
    for (int i = 0; i < ell; i++)
    {
        cmin = (p1.getVal(i) < p2.getVal(i)) ? p1.getVal(i) : p2.getVal(i);
        cmax = (p1.getVal(i) > p2.getVal(i)) ? p1.getVal(i) : p2.getVal(i);
        I = cmax - cmin;
        c1.setVal(i, myRand.uniform(cmin - 0.5 * I, cmax + 0.5 * I));
        c2.setVal(i, myRand.uniform(cmin - 0.5 * I, cmax + 0.5 * I));
    }
}
void GA::SPXXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{
    double r = 0.0;
    int epsilon = sqrt(ell + 2);
    for (int i = 0; i < ell; i++)
    {
        r += pow(p1.getVal(i) - p2.getVal(i), 2)
    }
    r = sqrt(r);
    float cmin = 0.0, cmax = 0.0;
    for (int i = 0; i < ell; i++)
    {
        cmin = (p1.getVal(i) < p2.getVal(i)) ? p1.getVal(i) : p2.getVal(i);
        cmax = (p1.getVal(i) > p2.getVal(i)) ? p1.getVal(i) : p2.getVal(i);
        c1.setVal(i, myRand.uniform(cmin - epsilon * r, cmax + epsilon * r));
        c2.setVal(i, myRand.uniform(cmin - epsilon * r, cmax + epsilon * r));
    }
}
int GA::isPrime(int n)
{
    for (int i = 2; i <= sqrt(n); i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
    return 1;
}
void GA::primeXO(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2)
{
    for (int i = 0; i < ell; i++)
    {
        if (isPrime(i))
        {
            c1.setVal(i, p2.getVal(i));
            c2.setVal(i, p1.getVal(i));
        }
        else
        {
            c1.setVal(i, p1.getVal(i));
            c2.setVal(i, p2.getVal(i));
        }
    }
   
    void GA::simplifycfgXO_p1(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2){
        vector<int>p1_XOpoint_List=new int[1];
        
        for (int  = 0; i < ell; i++)
        {
            if(p1.getValue(i)==82){
                //會放入list的代表到i之前的block要保留，從i+1的點開始切
                p1_XOpoint_List.push_back(i+1);
            }
           
        }
        //判斷用p1還是用p2的gene，0用p1;1用p2
        int geneSrc=0;
        //設定children的gene,index:0~p1_XOpoint_List[0]
         for (int i = 0; i < p1_XOpoint_List[0]; i++)
         {
            if(geneSrc==1){
                    c1.setVal(i, p2.getVal(i));
                    c2.setVal(i, p1.getVal(i));
                    geneSrc=0;
                }else{
                    c1.setVal(i, p1.getVal(i));
                    c2.setVal(i, p2.getVal(i));
                    geneSrc=1;
                }
         }
         //設定children剩下的gene
        for (int l = 1; l <p1_XOpoint_List.size(); l++)
        {        
             for (int i = p1_XOpoint_List[l-1]; i <=p1_XOpoint_List[l]-1; i++){
                    if(geneSrc==1){
                    c1.setVal(i, p2.getVal(i));
                    c2.setVal(i, p1.getVal(i));
                    geneSrc=0;
                }else{
                    c1.setVal(i, p1.getVal(i));
                    c2.setVal(i, p2.getVal(i));
                    geneSrc=1;
                }
         
                }
           
          
        }
        
    } 
    void GA::simplifycfgXO_p2(const Chromosome &p1, const Chromosome &p2, Chromosome &c1, Chromosome &c2){
        vector<inti>p2_XOpoint_List=new int[1];
        for (int  = 0; i < ell; i++)
        {
            if(p2.getValue(i)==82){
                //會放入list的代表到i之前的block要保留，從i+1的點開始不一樣
                p2_XOpoint_List.push_back(i+1);
            }
        }
        int geneSrc=0;
     //設定children的gene,index:0~p1_XOpoint_List[0]
         for (int i = 0; i < p2_XOpoint_List[0]; i++)
         {
            if(geneSrc==1){
                    c1.setVal(i, p2.getVal(i));
                    c2.setVal(i, p1.getVal(i));
                    geneSrc=0;
                }else{
                    c1.setVal(i, p1.getVal(i));
                    c2.setVal(i, p2.getVal(i));
                    geneSrc=1;
                }
         }
        for (int l = 1; l <p2_XOpoint_List.size(); l++)
        {        
             for (int i = p2_XOpoint_List[l-1]; i <p2_XOpoint_List[l]; i++)
             {
                   if(geneSrc==1){
                    c1.setVal(i, p2.getVal(i));
                    c2.setVal(i, p1.getVal(i));
                    geneSrc=0;
                }else{
                    c1.setVal(i, p1.getVal(i));
                    c2.setVal(i, p2.getVal(i));
                    geneSrc=1;
                }
         
                }    
        }
        
    } 
    void GA::mutation()
    {
        //simpleMutation ();
        mutationClock();
    }

    void GA::simpleMutation()
    {
        int i, j;

        for (i = 0; i < nNextGeneration; i++)
            for (j = 0; j < ell; j++)
                if (myRand.flip(pm))
                {
                    int val = offspring[i].getVal(j);
                    offspring[i].setVal(j, 1 - val);
                }
    }
    void GA::randomMutation()
    {
        if (pm <= 1e-6)
            return; // can't deal with too small pm

        int pointer = (int)(log(1 - myRand.uniform()) / log(1 - pm) + 1);

        while (pointer < nNextGeneration * ell)
        {

            int q = pointer / ell;
            int r = pointer % ell;

            // 選擇flag的範圍
            int random_num = myRand.uniformInt(1, 100);
            offspring[q].setVal(r, random_num);

            // Compute next mutation clock
            pointer += (int)(log(1 - myRand.uniform()) / log(1 - pm) + 1);
        };
    }
    void GA::randomMutation2 .0()
    {
        if (myRand.flip(pm))
        {
            //選擇染色體(在array內)
            int random_chromosome = myRand.uniformInt(0, nNextGeneration - 1);
            int random_gene = myRand.uniformInt(0, ell - 1); //選擇bit位置
            int random_number = myRand.uniformInt(1, 100);   //隨機選擇一個flag
            offspring[random_chromosome].setVal(random_gene, random_number);
        }
    }
    void GA::newMutation ()

    {

        if
             (pm <= 1e-6) return;
          // can't deal with too small pm

            int pointer = (int) (log(1 - myRand.uniform()) / log(1 - pm) + 1);

        while
             (pointer < nNextGeneration * ell) 
            {

                int q = pointer / ell;

                int r = pointer % ell;

                // 選擇flag的範圍

                double p = myRand.uniform(0, 1);

                if (p <= pm && offspring[q].getVal(r) == 0)
                {

                    int random_num = myRand.uniformInt(1, 100);

                    offspring[q].setVal(r, random_num);
                }

                else if (p <= pm)
                {

                    offspring[q].setVal(r, 0);
                }

                // Compute next mutation clock

                pointer += (int) (log(1 - myRand.uniform()) / log(1 - pm) + 1);
            };
    }
    void GA::mutationClock()
    {
        if (pm <= 1e-6)
            return; // can't deal with too small pm

        int pointer = (int)(log(1 - myRand.uniform()) / log(1 - pm) + 1);

        while (pointer < nNextGeneration * ell)
        {

            int q = pointer / ell;
            int r = pointer % ell;

            int val = offspring[q].getVal(r);
            offspring[q].setVal(r, 1 - val);

            // Compute next mutation clock
            pointer += (int)(log(1 - myRand.uniform()) / log(1 - pm) + 1);
        };
    }

    void GA::showStatistics()
    {

        printf("Gen:%d  Fitness:(Max/Mean/Min):%f/%f/%f Chromsome Length:%d\n",
               generation, stFitness.getMax(), stFitness.getMean(),
               stFitness.getMin(), population[0].getLength());
        printf("best chromosome:");
        population[bestIndex].printf();
        printf("\n");
    }

    void GA::replacePopulation()
    {
        int i;

        if (nNextGeneration != nCurrent)
        {
            delete[] population;
            population = new Chromosome[nNextGeneration];
        }

        for (i = 0; i < nNextGeneration; i++)
            population[i] = offspring[i];

        nCurrent = nNextGeneration;
    }

    void GA::oneRun(bool output)
    {
        int i;

        selection();
        crossover();
        mutation();
        replacePopulation();

        double max = -DBL_MAX;
        stFitness.reset();
        for (i = 0; i < nCurrent; i++)
        {
            double fitness = population[i].getFitness();
            if (fitness > max)
            {
                max = fitness;
                bestIndex = i;
            }
            stFitness.record(fitness);
        }

        if (output)
            showStatistics();

        generation++;
    }

    int GA::doIt(bool output)
    {
        generation = 0;

        while (!shouldTerminate())
        {
            oneRun(output);
        }
        return generation;
    }

    bool GA::shouldTerminate()
    {
        bool termination = false;

        // Reach maximal # of function evaluations
        if (maxFe != -1)
        {
            if (fe > maxFe)
                termination = true;
        }

        // Reach maximal # of generations
        if (maxGen != -1)
        {
            if (generation > maxGen)
                termination = true;
        }

        // Found a satisfactory solution
        if (stFitness.getMax() >= population[0].getMaxFitness())
            termination = true;

        // The population loses diversity
        if (stFitness.getMax() - 1e-6 < stFitness.getMean())
            termination = true;

        return termination;
    }
