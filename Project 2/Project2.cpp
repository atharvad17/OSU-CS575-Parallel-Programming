#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

int NowYear;        // 2023 - 2028
int NowMonth;       // 0 - 11

float   NowPrecip;      // inches of rain per month
float   NowTemp;        // temperature this month
float   NowHeight;      // rye grass height in inches
int NowNumRabbits;      // number of rabbits in the current population
int NowNumSnake;
const float RYEGRASS_GROWS_PER_MONTH =      20.0;
const float ONE_RABBITS_EATS_PER_MONTH =     1.0;

const float AVG_PRECIP_PER_MONTH =         12.0;    // average
const float AMP_PRECIP_PER_MONTH =      4.0;    // plus or minus
const float RANDOM_PRECIP =         2.0;    // plus or minus noise

const float AVG_TEMP =              60.0;   // average
const float AMP_TEMP =              20.0;   // plus or minus
const float RANDOM_TEMP =           10.0;   // plus or minus noise

const float MIDTEMP =               60.0;
const float MIDPRECIP =             14.0;

unsigned int seed = 0;


float
Sqr( float x )
{
    return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX
    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

void RyeGrass() {
 //   cout << "In grass" <<endl;
    while (NowYear < 2029)
    {
 //       cout << "In grass while" <<endl;
        float tempFactor = exp(-Sqr((NowTemp - MIDTEMP) / 10.));
        float precipFactor = exp(-Sqr((NowPrecip - MIDPRECIP) / 10.));

        float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;
        if( nextHeight < 0. ) nextHeight = 0.;
        //DoneComputing barrier
        #pragma omp barrier
        //WaitBarrier();
        NowHeight = nextHeight;

        //DoneAssigning barrier
        #pragma omp barrier
        //WaitBarrier();
        //DonePrinting barrier
        #pragma omp barrier
        //WaitBarrier();
    }
}

void Rabbits() {
    //cout << "In rabbit" <<endl;
    while (NowYear < 2029)
        {
 //           cout << "In rabbit while" <<endl;
            int nextNumRabbits = NowNumRabbits;
            int carryingCapacity = (int)( NowHeight );
            if( nextNumRabbits < carryingCapacity )
                nextNumRabbits++;
            else
                if( nextNumRabbits > carryingCapacity)
                    nextNumRabbits--;

            if( nextNumRabbits < 0 )
                    nextNumRabbits = 0;


            //DoneComputing barrier
            //WaitBarrier();
            #pragma omp barrier
            NowNumRabbits = nextNumRabbits;

            //DoneAssigning barrier
            //WaitBarrier();
            #pragma omp barrier

            //DonePrinting barrier
            #pragma omp barrier
            //WaitBarrier();
        }
}

void Watcher() {
    //cout << "In watcher" <<endl;
    while(NowYear < 2029){
        #pragma omp barrier
        #pragma omp barrier
        //WaitBarrier();
        //WaitBarrier();
        float ang = (30.*(float)NowMonth + 15.) * (M_PI / 180.);
        float temp = AVG_TEMP - AMP_TEMP * cos(ang);

        NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);
        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
        NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);

        if(NowPrecip < 0.){
             NowPrecip = 0.;
        }
        NowMonth++;

        if (NowMonth == 12){
            NowMonth = 0;
            NowYear++;
        }
        float C_temperature = (5. / 9.) * (NowTemp - 32);
        float P_Height = NowPrecip * 2.54;
        float G_Height = NowHeight * 2.54;
        cout << NowYear << "," << NowMonth <<  "," << NowNumSnake << ","
        << NowNumRabbits << "," << G_Height << "," << C_temperature
        << "," << P_Height << endl;

        //WaitBarrier();
        #pragma omp barrier
    }
}

void Snake(){
    while(NowYear < 2029)
    {
        int nextNumSnake = NowNumSnake;
        if (nextNumSnake >= (NowNumRabbits / 5) ){
            nextNumSnake--;
        }
        else if (nextNumSnake <  (NowNumRabbits / 5)){
            nextNumSnake = nextNumSnake + 2;
        }
        if (nextNumSnake < 0) nextNumSnake = 0;

        #pragma omp barrier
        NowNumSnake = nextNumSnake;
        #pragma omp barrier
        #pragma omp barrier

    }

}


int main(){
    NowMonth = 0;
    NowYear  = 2023;

    NowNumRabbits = 1;
    NowHeight =  5.;
    NowNumSnake = 7;
    float ang = (30.*(float)NowMonth + 15.) * (M_PI / 180.);
    float temp = AVG_TEMP - AMP_TEMP * cos(ang);

    NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
    NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);

    if(NowPrecip < 0.) NowPrecip = 0.;
    omp_set_num_threads(4);   // same as # of sections
    //InitBarrier(3);
    cout << "Year,Month,NumSnake,NumRabbits,G_Height,Temperature,P_Height" << endl;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Rabbits();
        }
        #pragma omp section
        {
            RyeGrass();
        }
        #pragma omp section
        {
            Watcher();
        }
        #pragma omp section
        {
            Snake();
        }
    }

    return 0;

}

