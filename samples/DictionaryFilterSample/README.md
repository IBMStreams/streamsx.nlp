## DictionaryFilterSample

This sample demonstrates how you can use the DictionaryFilter operator from the com.ibm.streamsx.nlp toolkit.
The operator reads the list of stopwords from the `./etc/stopwords.txt` file on start-up to build up the dictionary.
Each line of the input file represents a document. Each term of the document that matches a word in the dictionary is removed in the output.

## Use

Build the application:

`make`

Run:

`./output/bin/standalone`

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/nlp.sample.DictionaryFilterSample.sab from your file system.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output

Clean:

`make clean`
