## RutaTextSample

This sample demonstrates how you can use the RutaText operator from the com.ibm.streamsx.nlp toolkit.
The application reads 51 files, that are part of the application bundle in the opt folder.
One file is one document for which the names and titles are annotated according the Ruta rule.
You can choose with the loop parameter how often these files are read. The default parameter value is -1 for an infinite loop.

## Use

Build the application:

`make`

Run:

`./output/bin/standalone`

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/nlp.sample.RutaTextSample.sab from your file system.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output

Clean:

`make clean`


