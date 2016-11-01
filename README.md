# IBM Streams Text Toolkit

The Text Toolkit includes operators to extract information from text data and provides operations for text analysis, like lemmatization and text annotation with UIMA Ruta scripts or existing project specific UIMA pear files.

The Text Toolkit integrates the Text Analytics component of Apache UIMA, which provides a system for extracting information from text data.

This version of the toolkit is intended for use with IBM Streams release 3.2 and later.

Instead of the operator com.ibm.streamsx.text::RutaText (UIMA Ruta rules based), you can also use the operator com.ibm.streams.text.analytics::TextExtract (AQL rules based) from the com.ibm.streams.text toolkit.

In addition to the com.ibm.streams.text toolkit the streamsx.text toolkit covers features that are useful in the native language analysis pipeline with functionality: lemmatization, part-of-speach tagging, stop-word-removal, dictionary-filter, n-gram, tf-idf (determine the meaning of a word to the text), classification (which category a text belongs to), content ranking (figure out the intend of a text).

# Overview

Project Overview at: http://ibmstreams.github.io/streamsx.text/

Operators, functions & types are described using SPLDOC at

http://ibmstreams.github.io/streamsx.text/com.ibm.streamsx.text/doc/spldoc/html/index.html

# Features

https://github.com/IBMStreams/streamsx.text/wiki/Features

# Getting Started

https://github.com/IBMStreams/streamsx.text/wiki/Getting-Started

# Troubleshooting

Please enter an issue on GitHub for defects and other problems. 

# Releases

The toolkit version 1.0.0 has been tested and released with
* IBM Streams
    + 3.2
    + 4.0
    + 4.1
    + 4.2
* Apache UIMA
    + Core 2.8.1.
    + Ruta 2.4.0,
    + uimaFIT 2.1.0
* Python 2.7

We will make releases after major features have been added. If you wish to request a release, please open an issue.

# Learn more about Streams:

* [IBM Streams on Github](http://ibmstreams.github.io/)
* [StreamsDev](https://developer.ibm.com/streamsdev/)
