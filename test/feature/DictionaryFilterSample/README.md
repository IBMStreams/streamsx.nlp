## DictionaryFilterSample

This sample demonstrates how you can use the DictionaryFilter operator from the com.ibm.streamsx.nlp toolkit.
The operator reads the list of stopwords from the `./etc/stopwords.txt` file on start-up to build up the dictionary.
Each line of the input file represents a document. Each term of the document that matches a word in the dictionary is removed in the output.

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
