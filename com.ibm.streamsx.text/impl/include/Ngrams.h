/*
 * Ngrams.h
 *
 *  Created on: Nov 27, 2011
 *      Author: leonid.gorelik
 */

#ifndef NGRAMS_H_
#define NGRAMS_H_

#include <vector>
#include <tr1/unordered_map>
#include "sys/time.h"
// Define ngrams types and functions.
#include "ngramhashing/rabinkarphash.h"
// Define SPL types and functions.
#include "SPL/Runtime/Function/SPLFunctions.h"

using std::tr1::unordered_map;
using std::vector;
using SPL::rstring;

namespace ngrams {

	// uses rolling hash to calculate ngrams count, returns vector of counts.
	inline vector<unsigned> countNgrams(rstring const& data, unsigned n, unsigned M = 19) {
		vector<unsigned> datavec(data.size()+1-n, 1);
		unordered_map<unsigned,unsigned> ngrams(data.size()+1-n);

		KarpRabinHash hf(n,M);

		for(unsigned k = 0; k < n; ++k) {
		 hf.eat(data[k]);
		}

		ngrams[hf.hashvalue] = 0;
		for(unsigned k = n; k<data.size();++k) {
			hf.update(data[k-n],data[k]);
			if(ngrams.count(hf.hashvalue) > 0) {
				datavec[ngrams[hf.hashvalue]]++;
				datavec[k+1-n] = 0;
			}
			else {
				ngrams[hf.hashvalue] = k+1-n;
			}
		}
		return datavec;
	}

	// Returns map <ngram,count>, based on results of countNgrams function
	inline unordered_map<rstring,unsigned> getNgrams(rstring const& data, unsigned n) {

		unordered_map<rstring,unsigned> ngrams;
		vector<unsigned> ngCounts = countNgrams(data, n);

		for(int i = 0; i < ngCounts.size(); i++) {
			if(ngCounts[i] > 0) {
				ngrams[data.substr(i,n)] = ngCounts[i];
			}
		}

		return ngrams;
	}
}

#endif /* NGRAMS_H_ */
