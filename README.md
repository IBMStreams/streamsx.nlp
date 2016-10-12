# IBM Streams Text Toolkit

The Text Toolkit includes operators to extract information from text data and provides operations for text analysis, like lemmatization and text annotation with Uima Ruta scripts or existing project specific Uima pear files.

The Text Toolkit integrates the Text Analytics component of Apache Uima, which provides a system for extracting information from text data.

This version of the toolkit is intended for use with IBM Streams release 3.2 and later.


# Overview

The toolkit supports the following features:

**Lemmatize**

A lemma is the canonical form, dictionary form, or citation form of a set of words.
In English, for example, run, runs, ran and running are forms of the same lexeme, with run as the lemma.
This functionality is covered with operator Lemmatizer. 

**Part-of-speech tagging**

Part-of-speech tagging (aka POS tagging, POST, grammatical tagging) is the process of marking up a word in a text (corpus) as corresponding to a particular part of speech, based on both its definition and its context.
This functionality is part of operator Lemmatizer. 

**Stop Word removal**

Stop Words are words which do not contain important significance to be used in Search Queries such as the, and, ...
This functionality is part of operator DictionaryFilter.

**Dictionary Filter**

In the opposite to stop word removal it is sometimes useful to only keep the words from a text that are in a dictionary.
This functionality covered with operator DictionaryFilter.

**TF-IDF**

Tf-idf is a statistical measure used to evaluate how important a word is to a document in a collection or corpus. It needs a trained model.
The training could run in Streams using operator IdfCorpusBuilder. But the trained model could also come from external sources.
This TF-IDF functionality is covered with operator TfIdfWeight. 

**N-Grams**

N-Grams of texts are a set of co-occuring words within a given window.
When N=1, this is referred to as unigrams and this is essentially the individual words in a sentence.
When N=2, this is called bigrams and 
when N=3 this is called trigrams.
When N>3 this is usually referred to as four grams or five grams and so on. 
This functionality is covered with operator NgramBasic.

**Rule-based Text Annotation**

Support text annotation and analysis using Apache UIMA Ruta scripts or UIMA Analysis Engines coming in a .pear file.
This functionality is covered with operators UimaText, UimaCase, RutaText and RutaCas.
For the Ruta operators, the .pear file does not need to contain the analysis engine (and thus does not need to validate).
Please, find in toolkit dir ./doc/UIMA_workbench.pdf a detailed sample description of Ruta .pear creation.

**Classification**

Classification is the problem of identifying to which of a set of categories (sub-populations) a new observation belongs, on the basis of a training set of data containing observations (or instances) whose category membership is known. An example would be assigning a given email into "spam" or "non-spam" classes.
The training could run in Streams using operator LinearClassificationModelBuilder. But the trained model could also come from external sources.
The classification is covered with operator LinearClassification.

**Content Ranking**

Content Ranking tries to figure out the intend of a text in relation to a field of interest. This requires to train a model before.
The training could run in Streams using operator ContentRankingModelBuilder. But the trained model could also come from external sources.
The content ranking is covered with operator ContentRanking.

# Getting Started

Download and build the toolkit yourself from the source.

*Directory structure*

1. `com.ibm.streamsx.text` - the toolkit directory containing the operators, functions and libraries
2. `samples` - Sample projects demonstrating the usage of each toolkit operator and function
3. `setup` - File for the installation procedure of required modules
4. `test` - Test suits for regression and performance tests.

## Building the toolkit

The toolkit needs to be build before you can use it in a SPL application.

To build the toolkit libraries, `cd` to the `com.ibm.streamsx.text` folder and run the make command:

    make all

## Setup

The toolkit operators ContentClassification and LinearClassification require Python 2.7 be installed.

To install the required and verified extra modules, `cd` to the `setup` folder and run the install command:

    (sudo) pip install -r requirements_2.7.txt

In the IBM Streams 4.2 Quick Start Edition VM, `cd` to the `setup` folder and run the install command:

    ./setup_streamsV4.2-qse.sh

## Running the sample applications

To build and launch a sample application, `cd` to the `samples/<SAMPLE_APP>` folder and run the commands:

    make all
    output/bin/standalone
    
*\<SAMPLE_APP\> is one of the sub-directories of the samples folder, e.g. UimaTextSample.*

## Generate the documentation

Call `make make-doc` in the `com.ibm.streamsx.text` directory to generate the toolkit documentation pages.
Open the `index.html` file with your browser in the `com.ibm.streamsx.text/doc/spldoc/html` folder to view the toolkit reference documentation.

## Running the test cases

Call `./runtests.py -f alltests.txt` in the `test/feature` directory to perform the regression test.
This compiles and launches each sample application and verifies the resulting output.
At the end of the test run, the result is printed to the terminal output.

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

Project Overview at: http://ibmstreams.github.io/streamsx.text/

Operators, functions & types are described using SPLDOC at

http://ibmstreams.github.io/streamsx.text/com.ibm.streamsx.text/doc/spldoc/html/index.html

# Learn more about Streams:

* [IBM Streams on Github](http://ibmstreams.github.io/)
* [StreamsDev](https://developer.ibm.com/streamsdev/)
