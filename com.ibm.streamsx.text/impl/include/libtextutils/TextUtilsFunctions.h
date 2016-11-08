#ifndef TEXT_UTILS_FUNCTIONS_H_
#define TEXT_UTILS_FUNCTIONS_H_

#include <SPL/Runtime/Function/SPLFunctions.h>

namespace com { namespace ibm { namespace streamsx { namespace text { namespace utils {

// initializeLemmatizer - initialize the tagger
// returns true if initialized successfully
// never throws
SPL::boolean initializeLemmatizer();

// initializeLemmatizer - initialize the tagger
// returns true if initialized successfully
// never throws
SPL::boolean initializeLemmatizer(SPL::rstring const & directory);

// lemmatize the text
// never throws
void lemmatize(SPL::rstring const text, SPL::list<SPL::rstring>& words, SPL::list<SPL::rstring>& pos, SPL::list<SPL::rstring>& lemmas);

// lemmatize the text
// never throws
void lemmatize(SPL::rstring const text, SPL::list<SPL::rstring>& lemmas);

// Generate n-gram terms
// never throws
void generateNgram(SPL::list<SPL::rstring> const & words, SPL::uint32 size, SPL::list<SPL::rstring>& terms);

// Generate n-gram terms
// never throws
void generateNgram(SPL::list<SPL::rstring> const & words, SPL::uint32 minSize, SPL::uint32 maxSize, SPL::list<SPL::rstring>& terms);

}}}}} //namespace

#endif /* TEXT_UTILS_FUNCTIONS_H_ */
