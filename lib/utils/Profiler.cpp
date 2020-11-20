/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <utils/Profiler.hpp>

using std::cout;
using std::endl;

namespace Profiler {

double getCurrentTime(){
	double res = 0;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	res += tv.tv_sec;
	res += (tv.tv_usec/1000000.0);

	return res;
}

int64_t getRandomNum(int64_t max){
  	FILE *urand = fopen("/dev/urandom", "r");
  	if(urand == NULL){
		cout<<"Could not open device file /dev/urandom"<<endl;
		exit(1);
	}

	uint64_t seed;
	for(int i=0; i<8; i++){
		i[(unsigned char*)&seed] = fgetc(urand);
	}
	fclose(urand);

	return (seed % ((uint64_t)max));
}

void printSet(std::vector<int64_t> *s){
	cout<<"[ Size = "<<s->size()<<" ] [ ";
	for(size_t i=0; i<s->size(); i++){
		cout<<s->at(i)<<" ";
	}
	cout<<"]"<<endl;
}

void generateRandomSet(int64_t size, std::vector<int64_t> *s){
	do{
		s->push_back(getRandomNum(RAND_MAX));
	} while (s->size() < (size_t)size);
}

}
