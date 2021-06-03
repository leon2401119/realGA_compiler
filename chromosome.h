/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#ifndef _CHROMOSOME_H
#define _CHROMOSOME_H


class Chromosome
{
    public:
        Chromosome ();
        Chromosome (int n_ell);

        ~Chromosome ();

        Chromosome& operator= (const Chromosome & c);

        void init (int n_ell);

        int getVal (int index) const;
        void setVal (int index, int val);

        double getFitness (int worker_id = 1, int repeat = 1);

        /** real evaluator */
        double evaluate (int worker_id, int repeat);

        double oneMax () const;

        double eval_flag(int worker_id, int repeat) const;

        bool isEvaluated () const;

        void printf () const;

        int getLength () const;

        double getMaxFitness () const;

    protected:
        int *gene;
        int length;
        double fitness;
        bool evaluated;
};
#endif
