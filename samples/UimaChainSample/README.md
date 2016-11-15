## UimaChainSample

This sample demonstrates how you can use the UimaText operator and the UimaCas from the com.ibm.streamsx.nlp toolkit.
The UimaCas operator has as input the output of the UimaText operator. Both operators annotate the document with its own UIMA analyis engines.

## Use

Build standalone application:

`make`

Run:

`./output/bin/standalone`

You can examine the input `./data/input.txt` and the output `./data/output_2_operators`.

Clean:

`make clean`

You can also build a distributed application with the following command (required for Streams releases lower than 4.2 only):

`make distributed`

then submit your job to a running Streams instance.
