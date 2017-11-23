## FunctionsSample

This sample demonstrates how you can use the com.ibm.streamsx.nlp.utils::lemmatize() function and the com.ibm.streamsx.nlp.utils::generateNgram function from the com.ibm.streamsx.nlp toolkit.
These functions are called in a sequence in a Custom operator to create n-grams of lemmas for a document. Each line of the input file represents a document.

## Use

Build the application:

`make`

Run:

`./output/bin/standalone`

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/nlp.sample.FunctionsSample.sab from your file system.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output

Clean:

`make clean`
