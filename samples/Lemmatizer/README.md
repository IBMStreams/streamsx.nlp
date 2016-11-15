## LemmatizerSample

This sample demonstrates how you can use the Lemmatizer operator from the com.ibm.streamsx.nlp toolkit.
It also shows the use of the com.ibm.streamsx.nlp.utils::lemmatize() function in a Custom operator.

## Use

Build standalone application:

`make`

Run:

`./output/bin/standalone`

You can examine the input `./data/SourceData.txt` and output `./data/out.txt`.

Compare output with expected data:

`diff ./data/expected.txt ./data/out.txt`

There should be no difference between the files.

Clean:

`make clean`

You can also build a distributed application with the following command (required for Streams releases lower than 4.2 only):

`make distributed`

then submit your job to a running Streams instance.
