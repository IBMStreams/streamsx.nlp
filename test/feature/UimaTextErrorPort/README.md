## UimaTextErrorPort

This sample demonstrates how you can use the UimaText operator from the com.ibm.streamsx.nlp toolkit with the optional second output port for error tuples.

## Use

Build standalone application:

`make`

Run:

`./output/bin/standalone`

You can examine the input `./data/input.txt` and output `./data/out.txt`.

Compare output with expected data:

`diff ./data/expected.txt ./data/out.txt`

There should be no difference between the files.

Clean:

`make clean`

You can also build a distributed application with the following command (required for Streams releases lower than 4.2 only):

`make distributed`

then submit your job to a running Streams instance.
