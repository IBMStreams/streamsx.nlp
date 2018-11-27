---
title: "UIMA pear file usage"
permalink: /docs/user/uima/
excerpt: "How to integrate pear files."
last_modified_at: 2018-11-27T12:37:48-04:00
redirect_from:
   - /theme-setup/
sidebar:
   nav: "userdocs"
---
{% include toc %}
{%include editme %}


## UIMA PEARs

[Getting started with UIMA PEAR files](https://uima.apache.org/doc-uima-pears.html)


## How to integrate UIMA PEAR files

This guide helps you in the scenario, that you have a UIMA PEAR file and you need to know the supported annotation types before writing an application with the UimaText operator.

The script `createTypes.pl` in `com.ibm.streamsx.nlp/bin` directory generates a file containing the annotation types as SPL types from the metadata of an UIMA pear file.

The following instructions show you, how to generate SPL files with the script `createTypes.pl` from command line:

### Prepare the project directory

Clone the repository and set the path to the streamsx.nlp repository.

     export NLP_HOME=<path_to_the_streamsx.nlp_repository>

Create a project directory

     mkdir uimapear
     cd uimapear
     mkdir etc
     mkdir data

Place your UIMA PEAR file into the etc directory.
In this guide we select the PEAR file and a sample input file from a sample project.

     cp $NLP_HOME/samples/UimaTextGeneratedTypes/etc/troubleful9.pear etc/
     cp $NLP_HOME/samples/UimaTextGeneratedTypes/opt/input.txt data/

### Generate

     $NLP_HOME/com.ibm.streamsx.nlp/bin/createTypes.pl --pearFile etc/troubleful9.pear --outputfile Types.spl --main Main

The script has generated two files in your project directory:

* Main.spl (sample application using the com.ibm.streamsx.nlp::UimaText operator)
* Types.spl (types generated out the pear file)

### Build the SPL application

     sc -M Main -t $NLP_HOME/com.ibm.streamsx.nlp --data-directory data

### Launch the application

     output/bin/standalone

The application processes the input.txt files and creates a out.txt file in the data directory.

     cat data/out.txt


