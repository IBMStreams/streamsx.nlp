## UimaTextGenerated

This sample demonstrates how you can use the UimaText operator from the com.ibm.streamsx.nlp toolkit.
During build process the script `createTypes.pl` generates the SPL types for the given PEAR file and generates the sample SPL application.

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
