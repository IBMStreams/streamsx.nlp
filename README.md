# IBM Natural Language Processing Toolkit

The Natural Language Processing (NLP) Toolkit includes operators to extract information from text data and provides operations for text analysis, like lemmatization and text annotation with UIMA Ruta scripts or existing project specific UIMA pear files.

The streamsx.nlp Toolkit integrates the Text Analytics component of Apache UIMA, which provides a system for extracting information from text data.

This version of the toolkit is intended for use with IBM Streams release 3.2 and later.

Instead of the operator com.ibm.streamsx.nlp::RutaText (UIMA Ruta rules based), you can also use the operator com.ibm.streams.text.analytics::TextExtract (AQL rules based) from the com.ibm.streams.text toolkit.

In addition to the com.ibm.streams.text toolkit the streamsx.nlp toolkit covers features that are useful in the natural language analysis pipeline with functionality: lemmatization, part-of-speech tagging, stop-word-removal, dictionary-filter, n-gram, tf-idf (determine the meaning of a word to the text), classification (which category a text belongs to), content ranking (figure out the intend of a text).

# Overview

Project Overview at: http://ibmstreams.github.io/streamsx.nlp/

Operators, functions & types are described using SPLDOC at

https://ibmstreams.github.io/streamsx.nlp/doc/spldoc/html/tk$com.ibm.streamsx.nlp/tk$com.ibm.streamsx.nlp.html

# Troubleshooting

Please enter an issue on GitHub for defects and other problems. 

# Releases

https://github.com/IBMStreams/streamsx.nlp/releases

We will make releases after major features have been added. If you wish to request a release, please open an issue.

# Learn more about Streams:

* [IBM Streams on Github](http://ibmstreams.github.io/)
* [StreamsDev](https://developer.ibm.com/streamsdev/)
