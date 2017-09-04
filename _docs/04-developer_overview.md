---
title: "Toolkit Development overview"
permalink: /docs/development/overview/
excerpt: "Contributing to this toolkits development."
last_modified_at: 2017-08-04T12:37:48-04:00
redirect_from:
   - /theme-setup/
sidebar:
   nav: "developerdocs"
---
{% include toc %}
{% include editme %}

Download and build the toolkit yourself from the source.

*Directory structure*

1. `com.ibm.streamsx.nlp` - the toolkit directory containing the operators, functions and libraries
2. `samples` - Sample projects demonstrating the usage of each toolkit operator and function
3. `setup` - File for the installation procedure of required modules
4. `test` - Test suits for regression and performance tests.

## Building the toolkit

The toolkit needs to be build before you can use it in a SPL application.

To build the toolkit libraries, `cd` to the `com.ibm.streamsx.nlp` folder and run the make command:

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

Call `make make-doc` in the `com.ibm.streamsx.nlp` directory to generate the toolkit documentation pages.
Open the `index.html` file with your browser in the `com.ibm.streamsx.nlp/doc/spldoc/html` folder to view the toolkit reference documentation.

## Running the test cases

Call `./runtests.py -f alltests.txt` in the `test` directory to perform the regression test.
This compiles and launches each sample application and verifies the resulting output.
At the end of the test run, the result is printed to the terminal output.

