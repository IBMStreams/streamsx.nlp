## RutaDocumentAnnotationsSample

This sample demonstrates how you can use the RutaText operator from the com.ibm.streamsx.nlp toolkit.
The PEAR file in this sample creates document annotations.

## Use

Build the application:

`make`

Run:

`./output/bin/standalone`

You can examine the input `./opt/test.txt`.

Output is printed on console and written to files (out.txt and out{id}.xmi) in the data directory.
The data directory is set to `/tmp`.

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/nlp.sample.RutaTextSample.sab from your file system.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output


Clean:

`make clean`

