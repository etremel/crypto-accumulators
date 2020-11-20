/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <bilinear/Scalar_DCLXVI.hpp>
#include <bilinear/G1_DCLXVI.hpp>
#include <bilinear/G2_DCLXVI.hpp>
#include <bilinear/GT_DCLXVI.hpp>

#include <utils/Profiler.hpp>

#include <algorithms/ASCAlgorithms.h>

typedef struct{
	ASCAlgorithms *asc;
	vector<vector<G *> *> *pubkey;
	vector<Scalar *> *coeffs;
	G *witness;
} witnessThreadArgT;

static void *computeWitness(void *arg){
	witnessThreadArgT *pT = (witnessThreadArgT *)arg;

	if(pT->coeffs->size()){
		if(pT->witness->getSize() == 384){
			pT->asc->accumulateSetFromCoeffs(pT->coeffs, pT->pubkey, pT->witness, false);
		} else {
			pT->asc->accumulateSetFromCoeffs(pT->coeffs, pT->pubkey, pT->witness, true);
		}
	} else {
		pT->witness->becomeIdentity();
	}
	delete pT->coeffs;
	delete pT;
	pthread_exit(NULL);
}

typedef struct{
	ASCAlgorithms *asc;
	vector<vector<G *> *> *pubkey;
	vector< vector<Scalar *> *> *polyCoeffs;
	vector<G *> *witnesses;

	int type;		// 0:subset, 1:completeness
	double time;	// how long
} witnessesThreadArgT;

static void *computeWitnesses(void *arg){
	G *pG;
	double t1;
	witnessesThreadArgT *pT = (witnessesThreadArgT *)arg;

	t1 = Profiler::getInstance()->getCurrentTime();
	pthread_t *workers = new pthread_t[pT->polyCoeffs->size()];
	for(size_t i=0; i<pT->polyCoeffs->size(); i++){
		if(pT->type == 0){
			pG = new G1DCLXVI();
		} else {
			pG = new G2DCLXVI();
		}
		pT->witnesses->push_back(pG);

		witnessThreadArgT *threadData = new witnessThreadArgT();
		threadData->asc = pT->asc;
		threadData->pubkey = (vector<vector<G *> *> *)pT->pubkey;
		threadData->coeffs = pT->polyCoeffs->at(i);
		threadData->witness = pT->witnesses->at(i);

		pthread_create(&workers[i], NULL, (void*(*)(void*))computeWitness, (void*)threadData);
	}

	for(size_t i=0; i<pT->polyCoeffs->size(); i++){
		pthread_join(workers[i], NULL);
	}

	delete workers;
	delete pT->polyCoeffs;

	pT->time = Profiler::getInstance()->getCurrentTime() - t1;

	pthread_exit(NULL);
}

typedef struct{
	ASCAlgorithms *asc;
	vector<vector<G *> *> *pubkey;
	vector<int64_t> *intersection;
	vector<NTL::ZZ_p *> *coeffs;
	G *acc;
	double time;	// how long
} iSectThreadArgT;

static void *computeISectData(void *arg){
	double t1;
	iSectThreadArgT *pT = (iSectThreadArgT *)arg;

	// Coefficients
	t1 = Profiler::getInstance()->getCurrentTime();

	MPZUtils::getInstance()->computeCoeffViaFFT(pT->intersection, pT->coeffs);

	// Accumulator
	vector<Scalar *> *myCoeffs = new vector<Scalar *>();
	for(size_t i=0; i<pT->coeffs->size(); i++){
		Scalar *pScalar = new ScalarDCLXVI();
		pScalar->importNTLObject(*(pT->coeffs->at(i)));
		myCoeffs->push_back(pScalar);
	}
	pT->asc->accumulateSetFromCoeffs(myCoeffs, pT->pubkey, pT->acc, true);

	delete myCoeffs;

	pT->time = Profiler::getInstance()->getCurrentTime() - t1;

	pthread_exit(NULL);
}

void ASCAlgorithms::intersectionQuery(const vector<vector<G *> *> *pk, const vector<vector<int64_t> *> *sets, const vector<int64_t> *intersection, QueryProof *proof, bool printTiming){
	double t1, pqComp, total;

	// Compute P(s) & Q(s) coeffs
	t1 = Profiler::getInstance()->getCurrentTime();
	vector< vector<Scalar *> *> *pCoeffs = new vector< vector<Scalar *> *>();
	vector< vector<Scalar *> *> *qCoeffs = new vector< vector<Scalar *> *>();
	MPZUtils::getInstance()->computeIntersectionWitnessesData(sets, intersection, pCoeffs, qCoeffs);
	pqComp = Profiler::getInstance()->getCurrentTime() - t1;
	if(printTiming){
		cout<<fixed<<setprecision(6)<<pqComp<<" ";
	}

	// Intersection data: accumulator & coefficients
	pthread_t iSectThread;
	iSectThreadArgT *iSectThreadData = new iSectThreadArgT();
	iSectThreadData->asc = this;
	iSectThreadData->pubkey = (vector<vector<G *> *> *)pk;
	iSectThreadData->intersection = (vector<int64_t> *)intersection;
	iSectThreadData->coeffs = proof->getCoeffs();
	iSectThreadData->acc = proof->getIntersectionAccumulator();
	pthread_create(&iSectThread, NULL, (void*(*)(void*))computeISectData, (void*)iSectThreadData);

	// Subset witnesses
	pthread_t subsetThread;
	witnessesThreadArgT *subsetThreadData = new witnessesThreadArgT();
	subsetThreadData->type = 0;
	subsetThreadData->asc = this;
	subsetThreadData->pubkey = (vector<vector<G *> *> *)pk;
	subsetThreadData->polyCoeffs = pCoeffs;
	subsetThreadData->witnesses = proof->getWitnesses1();;
	pthread_create(&subsetThread, NULL, (void*(*)(void*))computeWitnesses, (void*)subsetThreadData);

	// Complete witnesses
	pthread_t completenessThread;
	witnessesThreadArgT *completenessThreadData = new witnessesThreadArgT();
	completenessThreadData->type = 1;
	completenessThreadData->asc = this;
	completenessThreadData->pubkey = (vector<vector<G *> *> *)pk;
	completenessThreadData->polyCoeffs = qCoeffs;
	completenessThreadData->witnesses = proof->getWitnesses2();;
	pthread_create(&completenessThread, NULL, (void*(*)(void*))computeWitnesses, (void*)completenessThreadData);

	pthread_join(iSectThread, NULL);
	pthread_join(subsetThread, NULL);
	pthread_join(completenessThread, NULL);

	total = pqComp + max(iSectThreadData->time, max(subsetThreadData->time, completenessThreadData->time));

	if(printTiming){
		cout<<fixed<<setprecision(6)<<iSectThreadData->time<<" ";
		cout<<fixed<<setprecision(6)<<subsetThreadData->time<<" ";
		cout<<fixed<<setprecision(6)<<completenessThreadData->time<<" ";
		cout<<fixed<<setprecision(6)<<total<<" ";
	}

	delete iSectThreadData;
	delete subsetThreadData;
	delete completenessThreadData;
}
