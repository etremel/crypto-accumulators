/* 
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <iostream>

#include <vector>
#include <algorithm>

#include <stdio.h>

#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <sys/time.h>

using std::vector;

namespace Profiler{

		double getCurrentTime();
		int64_t getRandomNum(int64_t max);
		void printSet(vector<int64_t> *s);
		void generateRandomSet(int64_t size, vector<int64_t> *s);
		
};

#endif /* _PROFILER_H_ */
