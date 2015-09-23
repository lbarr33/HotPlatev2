#include <iostream>
#include <cmath>
#include <sys/time.h>
#include <omp.h>
#include<pthread.h>
#include<cstdlib>
#include<stdlib.h>
#define SIZE 16384
#define NUM_THREADS 24

using namespace std;

struct INFO
{
        int id;
        bool useold;
};

static float oldplate[SIZE][SIZE] = {{0}};
static float newplate[SIZE][SIZE] = {{0}};
static int fixedCells[SIZE][SIZE] = {{0}};
static bool fiftyArray[SIZE][SIZE] = {{true}};
bool checkState(float newplate[SIZE][SIZE], int fixedCells[SIZE][SIZE])
{
        for(int i=0; i<SIZE; i++)
        {
                for(int j=0; j<SIZE; j++)
                {
                        if(fixedCells[i][j]==0)
                        {
                           float average = (newplate[i+1][j] + newplate[i-1][j] + newplate[i][j+1] + newplate[i][j-1])/4;
                           if(abs(newplate[i][j]-average)> .1) return false;
                        }
                }
        }
        return true;
}

void *Initialize(void *struct_in)
{
        INFO* info = (INFO*) struct_in;
         for(int i=info->id; i<SIZE; i+=24)
        {
                for(int j=0;j<SIZE;j++)
                {
                oldplate[i][j]= 50;
                newplate[i][j] = 50;
                fixedCells[i][j] = 0;
                if(i==SIZE-1)
                {
                        oldplate[i][j] = 100;
                        newplate[i][j] = 100;
                        fixedCells[i][j] = 1;
                }
                if(i==0 || j==SIZE-1 || j==0)
                {
                        oldplate[i][j] = 0;
                        newplate[i][j] = 0;
                        fixedCells[i][j] = 1;
                }

                if(i==400 && j<331)
                {
                        oldplate[i][j] = 100;
                        newplate[i][j] = 100;
                        fixedCells[i][j] = 1;
                }
                if(i==200 && j==500)
                {
                        oldplate[i][j]= 100;
                        newplate[i][j]= 100;
                        fixedCells[i][j] = 1;

                }
                if(i%20==0)
                {
                        oldplate[i][j] = 100;
                        newplate[i][j] = 100;
                        fixedCells[i][j] = 1;
                }
                if(j%20==0)
                {
                        oldplate[i][j] = 0;
                        newplate[i][j] = 0;
                        fixedCells[i][j] = 1;
                }

        }
   }
         pthread_exit(NULL);


}

void *Update(void *struct_in)
{
        INFO* info = (INFO*) struct_in;
        int* idVal = &info->id;
        int count = *idVal;
        for(int i=count; i<SIZE; i+=24)
        {
                for(int j=0; j<SIZE; j++)
                {
                        if(fixedCells[i][j]==0)
                        {
                                 if(info->useold) newplate[i][j] = (oldplate[i+1][j] + oldplate[i-1][j] + oldplate[i][j+1] + oldplate[i][j-1] + (4*oldplate[i][j]))/8;
                                else oldplate[i][j] = (newplate[i+1][j] + newplate[i-1][j] + newplate[i][j+1] + newplate[i][j-1] + (4*newplate[i][j]))/8;

                        }
                }
        }

        pthread_exit(NULL);
}

void *Count(void *struct_in)
{
         INFO* info = (INFO*) struct_in;
        int* idVal = &info->id;
        int count = *idVal;
        for(int i=count; i<SIZE; i+=24)
        {
                for(int j=0; j<SIZE; j++)
                {
                        if(info->useold){
                                if(newplate[i][j]>50) fiftyArray[i][j] = true;
                                else fiftyArray[i][j] = false;
                        }
                        else{
                                if(oldplate[i][j]>50) fiftyArray[i][j] = true;
                                else fiftyArray[i][j] = false;
                        }
                }
        }

        pthread_exit(NULL);
}

double When()
{
struct timeval tp;
gettimeofday(&tp, NULL);
return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

int main()
{
   double time = When();
   pthread_t *threads;
   threads = new pthread_t[24];
   int fifties[97];

   int rc;
   for(int i=0;i<24;i++)
   {
        INFO*  my_struct = new INFO();
        my_struct->id = i;
        pthread_create(&threads[i],NULL, Initialize, (void *)my_struct);

   }
   for(int i=0; i<24; i++)
   {
        pthread_join(threads[i],NULL);
   }
   bool steady = false;
   int cycles = 0;
   bool useold = true;
   while(!steady)
   {
        for(int i=0; i<24;i++)
        {
                //cout<<"On iteration "<<i<<endl;
                INFO* my_struct = new INFO();
                if(useold) my_struct->useold = true;
                else my_struct->useold = false;
                my_struct->id = i;
                rc = pthread_create(&threads[i],NULL, &Update, (void *)my_struct);
                if(rc)
                {
                        cout<<"ERROR"<<endl;
                        exit(-1);
                }
        }
         for(int i=0; i<24; i++)
        {
                //cout<<"join number "<<i<<endl;
                pthread_join(threads[i],NULL);
        }

         for(int i=0; i<24;i++)
                {
                        INFO* my_struct = new INFO();
                        if(useold) my_struct->useold = true;
                        else my_struct->useold = false;
                        my_struct->id = i;
                        pthread_create(&threads[i],NULL,&Count, (void *)my_struct);

                }
                for(int i=0;i<24;i++)
                {
                        pthread_join(threads[i],NULL);
                }
                int fiftyCount = 0;
                for(int i=0;i<SIZE;i++)
                {
                        for(int j=0; j<SIZE;j++)
                       {
                                if(fiftyArray[i][j]) fiftyCount++;
                        }
                }
                fifties[cycles] = fiftyCount;
                cycles++;

        if(useold)
        {
                useold= false;
                if(checkState(newplate,fixedCells)) steady = true;
        }
        else
        {
                useold = true;
                if(checkState(oldplate, fixedCells)) steady = true;
        }
   }
        double endtime = When();
        double elapsed = endtime-time;
        cout<<elapsed<<" seconds elapsed"<<endl;
        for(int i=0;i<97;i++)
        {
        //      cout<<i+1<<":  "<<fifties[i]<<endl;
        }


}
