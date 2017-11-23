## LinearClassificationSample

This sample demonstrates how you can use the LinearClassification operator from the com.ibm.streamsx.nlp toolkit.

The LinearClassification operator has been developed for older Streams releases (e.g. Streams 3.2) where SPL Python primitive support was not provided. It uses a **Shell operator** to invoke Python scripts.

When using a Streams releases 4.2 or later it is recommended to invoke Python classes or functions like in the *LinearClassificationSplPy* sample that creates a *SPL Python primitive operator*.

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
