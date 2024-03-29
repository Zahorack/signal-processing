//
// Created by zahorack on 11/17/19.
//

// Digital signal processing
// Method: FIR Filter - finite impulse response
// Usage:  Frequency filtering low-pass, band-pass, band-stop

//#include "lp1500_256.cof"             // Low-pass filter frequency 1500 Hz
//#include "bs790.cof"                  // Band-stop filter frequency 790 Hz
//#include "BP1750.COF"                 // Band-pass filter frequency 1750 Hz
//#include "sp_cv09-FIR/BS2700.COF"       // Band-stop filter frequency 2700 Hz

#include "circular.h"

//Example Band-stop 2700 Hz sampling frequency 8000Hz

#define N 89            	//number of coefficients

const short coeffs[N] = {-14,23,-9,-6,0,8,16,-58,50,44,-147,119,67,-245,
                        200,72,-312,257,53,-299,239,20,-165,88,0,105,
                        -236,33,490,-740,158,932,-1380,392,1348,-2070,
                        724,1650,-2690,1104,1776,-3122,1458,1704,29491,
                        1704,1458,-3122,1776,1104,-2690,1650,724,-2070,
                        1348,392,-1380,932,158,-740,490,33,-236,105,0,
                        88,-165,20,239,-299,53,257,-312,72,200,-245,67,
                        119,-147,44,50,-58,16,8,0,-6,-9,23,-14
                        };

circularBuffer_t data;

static int finite_impulse_response(int input){
    int output = 0;
    int temp;
    int sample = input;

    if(buf.count == N) {
        circular_buffer_pop(&buf, &temp);
    }
    circular_buffer_push(&buf, &sample);

    for(int i = 0; i< N; i++) {
        circular_buffer_value_at(&buf, &temp, N-1-i);
        output += (coeffs[i] * temp);
    }
    //output = output >> 15;            //explicit conversion from 32bit to 16bit

    return output;
}


static void periodic_interrupt()	   	//ISR - 8kHz
{
    output_sample(finite_impulse_response(input_sample()));
}

void main()
{
    circular_buffer_init(&data, N, sizeof(int));

    while(1);                  	//infinite loop

    circular_buffer_deinit(&data);
}


/*
 *  Problem with old version of FIR filter implementation is unefficient data ordering.
 *  This issue should solve circular buffer
 */
static int old_fir_filter_implementation(int input) {
    int output = 0;				                //initialize filter's output
    static int dly[N] = {0};

    dly[0] = input;		                        //input newest sample
    //initialize filter's output
    for (int i = 0; i< N; i++)
        output += (coeffs[i] * dly[i]);  	    //y(n) += h(i)* x(n-i)
    for (int i = N-1; i > 0; i--)  	            //starting @ end of buffer
        dly[i] = dly[i-1];      	            //update delays with data move
    //yn=yn >> 15;                              //explicit conversion from 32bit to 16bit

    return output;
}

