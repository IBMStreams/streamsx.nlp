## NgramBasicSample

This sample demonstrates how you can use the NgramBasic operator from the com.ibm.streamsx.nlp toolkit.
In this sample a document is converted to unigrams, bigrams and trigrams.

## Use

Build the application:

`make`

Run:

`./output/bin/standalone`

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/nlp.sample.NgramBasicSample.sab from your file system.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output

Clean:

`make clean`
