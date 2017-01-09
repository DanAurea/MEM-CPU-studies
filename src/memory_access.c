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
 * Will demonstrate how loop unrolling can exploit CPU pipelines alot 
 * better (could be wrong with modern CPU / compilers).
 * Should take account IPC of your CPU (if possible).
 */
static void pipelineImpact(){

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
 * There is an improvement of 17% to 19% in execution time between both 
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

	printf("Start loop 1 with increments on same memory location: \n");
	
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

	gain = (1.0 - t2 / t1) * 100;

	printf("\nLoop 1 took %0.4f seconds to execute.\n", t1);
	printf("Loop 2 took %0.4f seconds to execute.\n", t2);
	printf("\nImprovement of %0.2f %% between both loops.\n", gain);
}

int main(void){
	
	/** Parallelism  */
	printf("\n=====================================================================\n");
	printf("\t Demo of parallelism on CPU pipelines:");
	printf("\n=====================================================================\n\n");
	parallelismDemo();

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