---
title: "Features"
permalink: /docs/knowledge/features/
excerpt: "Basic knowledge of the toolkits technical domain."
last_modified_at: 2017-08-04T12:37:48-04:00
redirect_from:
   - /theme-setup/
sidebar:
   nav: "knowledgedocs"
---
{% include toc %}
{% include editme %}


### Lemmatize

A lemma is the canonical form, dictionary form, or citation form of a set of words.
In English, for example, run, runs, ran and running are forms of the same lexeme, with run as the lemma.
This functionality is covered with operator Lemmatizer. 

### Part-of-speech tagging

Part-of-speech tagging (aka POS tagging, POST, grammatical tagging) is the process of marking up a word in a text (corpus) as corresponding to a particular part of speech, based on both its definition and its context.
This functionality is part of operator Lemmatizer. 

### Stop Word removal

Stop Words are words which do not contain important significance to be used in Search Queries such as the, and, ...
This functionality is part of operator DictionaryFilter.

### Dictionary Filter

In the opposite to stop word removal it is sometimes useful to only keep the words from a text that are in a dictionary.
This functionality covered with operator DictionaryFilter.

### TF-IDF

Tf-idf is a statistical measure used to evaluate how important a word is to a document in a collection or corpus. It needs a trained model.
The training could run in Streams using operator IdfCorpusBuilder. But the trained model could also come from external sources.
This TF-IDF functionality is covered with operator TfIdfWeight. 

### N-Grams

N-Grams of texts are a set of co-occuring words within a given window.
When N=1, this is referred to as unigrams and this is essentially the individual words in a sentence.
When N=2, this is called bigrams and 
when N=3 this is called trigrams.
When N>3 this is usually referred to as four grams or five grams and so on. 
This functionality is covered with operator NgramBasic.

### Rule-based Text Annotation

Support text annotation and analysis using Apache UIMA Ruta scripts or UIMA Analysis Engines coming in a .pear file.
This functionality is covered with operators UimaText, UimaCase, RutaText and RutaCas.
For the Ruta operators, the .pear file does not need to contain the analysis engine (and thus does not need to validate).
Please, find in toolkit dir ./doc/UIMA_workbench.pdf a detailed sample description of Ruta .pear creation.

### Classification

Classification is the problem of identifying to which of a set of categories (sub-populations) a new observation belongs, on the basis of a training set of data containing observations (or instances) whose category membership is known. An example would be assigning a given email into "spam" or "non-spam" classes.
The training could run in Streams using operator LinearClassificationModelBuilder. But the trained model could also come from external sources.
The classification is covered with operator LinearClassification.

### Content Ranking

Content Ranking tries to figure out the intend of a text in relation to a field of interest. This requires to train a model before.
The training could run in Streams using operator ContentRankingModelBuilder. But the trained model could also come from external sources.
The content ranking is covered with operator ContentRanking.

