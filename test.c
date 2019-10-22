#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "btree.h"

typedef clock_t clock_t;

double insert_assessment(BP_Tree b, int cap);
double find_assessment(BP_Tree b, int cap);

int main()
{
    BP_Tree b = BPT_Create();
    assert(b);

    // Used to determine the number of inserts and return the time taken to perform them
    printf("%f\n\n", insert_assessment(b, 5000000));
    // Used to determine the number of finds and return the time taken to perform them
    printf("%f", find_assessment(b, 5000000));

    return 0;
}

double insert_assessment(BP_Tree b, int cap)
{
    clock_t clk_t;
    clk_t = clock();

    for (int i = 0; i < cap; i++)
    {
        // assert(find(b, i) == 0);
        insert(b, i);
        // assert(find(b, i) == 1);
    }
    
    clk_t = clock() - clk_t;
    double time_taken = ((double)clk_t) / CLOCKS_PER_SEC;
    return time_taken;
}

double find_assessment(BP_Tree b, int cap)
{
    clock_t clk_t;
    clk_t = clock();

    for (int i = 0; i < cap; i++)
    {
        find(b, i);
    }

    clk_t = clock() - clk_t;
    double time_taken = ((double)clk_t) / CLOCKS_PER_SEC;
    return time_taken;
}