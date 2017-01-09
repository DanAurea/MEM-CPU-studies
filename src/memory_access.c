/**											
 * =============================================================================
 * 																		 
 * 	Filename: 	 
 * 	Date: 		 08/01/2017 								  
 * 	@description This file serve as an educational purpose only
 * 	             it shows how memory access and CPU architecture could
 * 	             imply some performances issues in software.
 * 	             You will see some simple examples on how parallelism 
 * 	             instruction could speed up execution time, how memory 
 * 	             (spatial) locality can mess up your algorithm and 
 * 	             contradict your expectations about some speed improvements.
 * 	             
 * 	             This is just a collection of some searches on internet
 * 	             about optimization, CPU architecture and how CPU cache
 * 	             works.
 * 	             So some portions of code there could be found on web
 * 	             and are either written by someone else and simply rewrited
 * 	             in C or are just interpretations of what I understood from
 * 	             different articles I had read.
 * 	             I will do my best for documenting every examples present in
 * 	             this file.
 *
 *               FYI: All examples were tested on a laptop with I5 6300HQ CPU
 *               and results could differ from others CPU.
 * 	                 
 *	@author		 DanAurea										  													  
 * 																		  
 * =============================================================================
 *                  ============================================
 *                            Developers: DanAurea
 *                  ============================================
 *                              Copyright © 2016	
 */

 
/**											
 *                  ============================================
 *                  ||                Headers                 ||
 *                  ============================================ 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


/**											
 *                  ============================================
 *                  ||            Global variables            ||
 *                  ============================================
 */


/**											
 *                  ============================================
 *                  ||                Functions               ||
 *                  ============================================
 */

/**
 * Will demonstrate how invalidation in cache could greatly 
 * mess up multithreaded application.
 */
static void cacheLineBouncing(){

}

/**
 * Will demonstrate how access time differs from L1 / L2 / L3
 * and main memory.
 */
static void cacheMissImpact(){

}

/**
 * Will demonstrate how it's important to respect spatial locality
 * such as in matrix exploration for a better use of CPU caches.
 */
static void localityImpact(){

} 

/**
 * Unroll a loop which show how pipeline and L1i cache could speed up
 * your execution time. This is an optimization done by most compilers
 * on O2 level if there is enough informations for compiler to unroll
 * it. So pipeline could have less impact on some CPU/compilers
 * because theoretically loop process could be translated to SIMD 
 * instruction set (3DNow, AltiVec, SSE).
 * 
 * For better results, float array is used, floats are not directly
 * processed by ALUs on CPU, so loop unrolling should show greater
 * gain.
 *
 * Look followings url for more informations:
 * lwn.net/Articles/255364/
 * azillionmonkeys.com/qed/optimize.html
 *
 * There is an improvement of roughly 95% between first loop and the shortest loop 
 * on my laptop.
 * 
 * *TODO* Should take account IPC of your CPU (if possible) through compiling
 * constant (IPC increase is a consequence of pipelining).
 */
static void pipelineImpact(){
	clock_t tClock;
	float * array;
	float sum = 0, sum0, sum1, sum2, sum3;
	double t1, t2, t3, t4;
	double temp, min, gain;
	int i, arraySize = 64 * 1024 * 1024;

	array = malloc( arraySize * sizeof(float));

	/** Error during allocation */
	if(array == NULL){
		return;
	}

	memset(array, 1.0, arraySize);


	/** Loop 1 */

	printf("\nStart loop 1 with basic use of pipeline: \n");

	tClock = clock();
	sum = 0;
	for(i = 0; i < arraySize; i++){
		sum += array[i];
	}
	tClock = clock() - tClock;
	
	printf("End loop 1: \n");
	t1  = (double) ((double)tClock)/CLOCKS_PER_SEC;

	/** 
	 *  Unrolling loop permit a n-cycle latency fully
	 *  pipelined FADD unit.
	 *  We are now exploiting advanced parallelism previously
	 *  demonstrated with memory access from CPU on same and
	 *  different memory location -> parallelismDemo().
	 *  Those n operations should be done simultaneously on
	 *  a CPU with IPC equal to n or more.
	 *  
	 *  Gains come from a better use of pipelines and L1i cache
	 *  so even a CPU with low IPC could earn alot of execution
	 *  time by doing loop unrolling.
	 */
	

	/** Loop 2 */

	printf("\nStart loop 2 with use of pipelines (2 cycles pipeline): \n");

	tClock = clock();
	sum = sum0 = sum1 = 0;
	for(i = 0; i < arraySize; i+=2){
		sum0 += array[i];
		sum1 += array[i+1];
	}
	sum = (sum0 + sum1);
	tClock = clock() - tClock;

	printf("End loop 2: \n");
	t2  = (double) ((double)tClock)/CLOCKS_PER_SEC;

	/** Loop 3 */

	printf("\nStart loop 3 with use of pipelines (3 cycles pipeline): \n");

	tClock = clock();
	sum = sum0 = sum1 = sum2 = 0;
	for(i = 0; i < arraySize; i+=3){
		sum0 += array[i];
		sum1 += array[i+1];
		sum2 += array[i+2];
	}
	sum = (sum0 + sum1) + sum2;
	tClock = clock() - tClock;

	printf("End loop 3: \n");
	t3  = (double) ((double)tClock)/CLOCKS_PER_SEC;

	/** Loop 4 */

	printf("\nStart loop 4 with use of pipelines (4 cycles pipeline): \n");

	tClock = clock();
	sum = sum0 = sum1 = sum2 = sum3 = 0;
	for(i = 0; i < arraySize; i+=4){
		sum0 += array[i];
		sum1 += array[i+1];
		sum2 += array[i+2];
		sum3 += array[i+3];
	}
	sum = (sum0 + sum1) + (sum2 + sum3);
	tClock = clock() - tClock;

	printf("End loop 4: \n");
	t4  = (double) ((double)tClock)/CLOCKS_PER_SEC;

	/** Compute minimun value between each loop unrolling  */
	temp = (t2 < t3)    ? t2 : t3;
    min =  (t4 < temp) ? t4 : temp;

	gain = (1.0 - t1 / min) * 100;

	printf("\nCPU took %f ms to process floats sum on loop 1.\n", t1 * 1000 );
	printf("CPU took %f ms to process floats sum on loop 2.\n", t2 * 1000 );
	printf("CPU took %f ms to process floats sum on loop 3.\n", t3 * 1000 );
	printf("CPU took %f ms to process floats sum on loop 4.\n", t4 * 1000 );
	printf("\nImprovement of %0.2f %% in execution time (greatest gain between all loops).\n", fabs(gain));

	free(array);
}

/**
 * Perform a simple operation on all values in an array and jump
 * from a fixed step between array values to show how memory access
 * is done, demonstrating that CPU don't access memory byte by byte but
 * fetch it from a chunk of variable size depending on your CPU (32/64/128 bytes).
 * Of course this will work only on CPU with cache like x86 architecture
 * and so including cache lines.
 * You can find some useful informations on following url: lwn.net/Articles/255364/
 * about how CPU read datas stored in memory / cache.
 *
 * On our modern CPU / compiler it seems this affirmation is not true now, on my
 * laptop I had those results:
 * 
 * step(1)   = 341.63ms
 * step(2)   = 228.85ms
 * step(4)   = 166.83ms
 * step(8)   = 135.72ms
 * step(16)  = 132.86ms
 * step(32)  = 127.78ms
 * step(64)  = 119.73ms
 * step(128) = 112.56ms
 * step(256) = 109.73ms 
 *
 * So newer CPU seems to have a different fetching method resulting in less overhead 
 * than in past when skipping a cache line could divide update time by 2.
 *
 * *TODO* Take account Cache line size from machine's CPU to show when step should be 
 * enough to result in a great boost.
 * 
 * @param step Update step
 */
static void cacheLineImpact(int step){
	clock_t tClock;
	double updateTime;
	int arraySize = 64 * 1024 * 1024;
	int i, * array = NULL;

	array = malloc( arraySize * sizeof(int));

	memset(array, 1, arraySize);

	/** Error during allocation */
	if(array == NULL){
		return;
	}

	tClock = clock();

	/** 
	 * Update k-values in an array with an update 
	 * step specified by user, showing how CPU 
	 * access memory (chunk read).
	 */
	for(i = 0; i < arraySize; i += step){
		array[i] *= 3;
	}

	tClock = clock() - tClock;
	updateTime = (double) ((double)tClock)/CLOCKS_PER_SEC;

	printf("CPU took %f ms to execute update loop with a step of %i.\n", updateTime * 1000, step);

	free(array); /** Free allocated memory from array */

}

/**
 * Perform increments on some values of an array to demonstrate
 * how parallelism on CPU pipeline could speed up programs.
 * parallelism shouldn't be considered as a form of concurrency 
 * on system, there is still only one job doing work.
 * This example is retrieved from igoro.com website at following url: 
 * igoro.com/archive/gallery-of-processor-cache-effects/
 *
 * There is an improvement of 25% to 28% in execution time between both 
 * loops on my machine although they are doing same operations.
 */
static void parallelismDemo(){
	clock_t tClock;
	double t1, t2, gain;

	int array[2] = {0};
	int steps = 512 * 1024 * 1024, i;

	/**  
	 * This loop will execute on CPU as a sequential list of 
	 * instructions:
	 * x = a[0] -> x++ -> a[0] = x -> y = a[0] -> y++ -> a[0] = y
	 * To perform less instruction we could simply increment from
	 * a step of 2 but some compilers seems to not implement this
	 * kind of optimization.
	 */

	printf("\nStart loop 1 with increments on same memory location: \n");
	
	tClock = clock();
	
	for (i=0; i<steps; i++) { 
		array[0]++; 
		array[0]++; 
	}
	
	tClock = clock() - tClock;
	
	printf("End loop 1: \n");
	t1 = (double) ((double)tClock)/CLOCKS_PER_SEC ;
	
	/**  
	 * This loop will execute on CPU as a parallel list
	 * of instructions, exploiting capacities of CPU pipelines
	 * alot better, it could be seen as:
	 * x = a[0] -> x++ -> a[0] = x
	 * y = a[1] -> y++ -> a[1] = y
	 */
	
	printf("\nStart loop 2 with increments on different memory locations: \n");
	
	tClock = clock();
	
	for (i=0; i<steps; i++) { 
		array[0]++; 
		array[1]++; 
	}
	
	tClock = clock() - tClock;
	
	printf("End loop 2: \n");
	t2 = (double) ((double)tClock)/CLOCKS_PER_SEC ;

	gain = (1.0 - t1 / t2) * 100;

	printf("\nLoop 1 took %0.4f seconds to execute.\n", t1);
	printf("Loop 2 took %0.4f seconds to execute.\n", t2);
	printf("\nImprovement of %0.2f %% in execution time between both loops.\n", fabs(gain));
}

int main(void){
	
	/** Parallelism  */
	printf("\n=====================================================================\n");
	printf("\t Demo of parallelism on CPU pipelines:");
	printf("\n=====================================================================\n");
	parallelismDemo();

	/** Pipeline impact  */

	printf("\n=====================================================================\n");
	printf("\t Demo of pipelines impact and L1i cache:");
	printf("\n=====================================================================\n");

	pipelineImpact();

	/** Cache lines  */

	printf("\n=====================================================================\n");
	printf("\t Demo of cache lines impact with an increasing step:");
	printf("\n=====================================================================\n");
	

	printf("\nStep(1): \n");
	cacheLineImpact(1);

	printf("\nStep(2): \n");
	cacheLineImpact(2);

	printf("\nStep(4): \n");
	cacheLineImpact(4);

	printf("\nStep(8): \n");
	cacheLineImpact(8);

	printf("\nStep(16): \n");
	cacheLineImpact(16);

	printf("\nStep(32): \n");
	cacheLineImpact(32);

	printf("\nStep(64): \n");
	cacheLineImpact(64);

	printf("\nStep(128): \n");
	cacheLineImpact(128);

	printf("\nStep(256): \n");
	cacheLineImpact(256);

	return 0;
}