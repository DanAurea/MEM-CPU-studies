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
 * 				 FYI: All examples were tested on a laptop with I5 6300HQ CPU
 * 				 and results could differ from others CPU.
 * 	                 
 *	@author		 DanAurea										  													  
 * 																		  
 * =============================================================================
 * 					============================================
 *							  Developers: DanAurea
 * 					============================================
 *								Copyright © 2016	
 */

 
/**											
 * 					============================================
 * 					||                Headers                 ||
 * 					============================================ 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/**											
 * 					============================================
 * 					||            Global variables            ||
 * 					============================================
 */


/**											
 * 					============================================
 * 					||                Functions               ||
 * 					============================================
 */


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
	
	printf("\nDemo of parallelism on CPU pipelines: \n\n");
	parallelismDemo();

	return 0;
}