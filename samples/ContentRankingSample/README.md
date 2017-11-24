## ContentRankingSample

This sample demonstrates how you can use the ContentRankingModelBuilder operator and the ContentRanking operator from the com.ibm.streamsx.nlp toolkit.

## Use

Build the application:

`make`

Run:

`./output/bin/standalone`

You can also change the python command if you need to select a specific python version.

`./output/bin/standalone pythonCommand=python3`

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/nlp.sample.LinearClassificationSample.sab from your file system and 
apply `$PYTHONHOME/bin/python3` as value for the submission-time parameter `pythonCommand`.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output

Clean:

`make clean`
