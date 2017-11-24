## RutaChainSample

This sample demonstrates how you can use the RutaText operator and the RutaCas from the com.ibm.streamsx.nlp toolkit.
The RutaCas operator has as input the output of the RutaText operator. Both operators annotate the document with its own Ruta script.

## Use

Build standalone application:

`make`

Run:

`./output/bin/standalone`

You can examine the input `./data/input.txt` and the output `./data/out.txt`.

Compare output with expected data:

`diff ./data/expected.txt ./data/out.txt`

There should be no difference between the files.

Clean:

`make clean`

You can also build a distributed application with the following command (required for Streams releases lower than 4.2 only):

`make distributed`

then submit your job to a running Streams instance.
