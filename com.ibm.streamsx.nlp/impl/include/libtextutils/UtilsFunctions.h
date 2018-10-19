#ifndef UTILS_FUNCTIONS_H_
#define UTILS_FUNCTIONS_H_
//
// *******************************************************************************
// * Copyright (C)2016, International Business Machines Corporation *
// * All rights reserved. *
// *******************************************************************************
//
#include <SPL/Runtime/Function/SPLFunctions.h>

namespace com { namespace ibm { namespace streamsx { namespace nlp { namespace utils {


// Workaround for Streams 3.2, because SPL::Functions::Utility::getToolkitDirectory() is not supported
// Streams 4.x SPL::Functions::Utility::getToolkitDirectory() returns the toolkit dir of the extracted sab file at runtime
// Streams 3.2 Absolute path of the toolkit location is generated in make all of the toolkit.
// returns toolkit dir
// never throws
inline SPL::rstring getToolkitPath() {
	return SPL::Functions::Utility::getToolkitDirectory("com.ibm.streamsx.nlp");
}


}}}}} //namespace

#endif /* UTILS_FUNCTIONS_H_ */
