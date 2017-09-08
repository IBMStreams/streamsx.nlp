---
title: "Toolkit technical background overview"
permalink: /docs/knowledge/overview/
excerpt: "Basic knowledge of the toolkits technical domain."
last_modified_at: 2017-08-04T12:37:48-04:00
redirect_from:
   - /theme-setup/
sidebar:
   nav: "knowledgedocs"
---
{% include toc %}
{% include editme %}

## IBM Natural Language Processing Toolkit

The streamsx.nlp Toolkit integrates the Text Analytics component of Apache UIMA, which provides a system for extracting information from text data.

This version of the toolkit is intended for use with IBM Streams release 3.2 and later.

Instead of the operator com.ibm.streamsx.nlp::RutaText (UIMA Ruta rules based), you can also use the operator com.ibm.streams.text.analytics::TextExtract (AQL rules based) from the com.ibm.streams.text toolkit.

In addition to the com.ibm.streams.text toolkit the streamsx.nlp toolkit covers features that are useful in the natural language analysis pipeline with functionality: lemmatization, part-of-speech tagging, stop-word-removal, dictionary-filter, n-gram, tf-idf (determine the meaning of a word to the text), classification (which category a text belongs to), content ranking (figure out the intend of a text).

The toolkit supports the following features:


