# LinearClassificationSplPy sample

This sample demonstrates the use of Linear Classification algorithm from the Python sklearn module.

The example requires the **'streams.topology'** toolkit at least version **v1.5.11**.

The recommended Streams version is 4.2.
 
The LinearClassification Operator is defined in python module 'mysamples.py' in directory opt/python/streams.
This sample reads training data from a file 'training.csv' and the data to classify from file 'in.txt'.
When no training is ongoing, the data tuples are passed directly to the LinearClassification Operator.
During the training, data tuples are not transfered to the LinearClassification Operator but they are stored in a queue.
When the training has finished, theses tuples are passed from the queue to the phyton operator.
When a new training file is moved into the data directory (atomic move), then the operator will be re-trained.
Classification results are printed.
Submission parameter verbose=True produces diagnostics output.

## Building the sample project

You must set the environment STREAMS_TOPOLOGY_TOOLKIT to the location of the topology toolkit.

To build this toolkit sample, run the make command in this directory:

`make`

The info.xml is generated during the build.

Clean the toolkit sample:

`make clean`

## Run the sample project

Please set PYTHONHOME to a valid Python 3.5 install.

`./output/bin/standalone`

## Building the sample project for Streaming Analytics service on Bluemix

The sample input and training files needs to be located in the Application Bundle file.
To build this toolkit sample for Streaming Analytics service on Bluemix, run the following command in this directory:

`make bluemix`

## Run the sample project in Streaming Analytics service on Bluemix

In the Streaming Analytics service, click LAUNCH to open the Streams Console, where you can submit and manage your jobs.
Upload the application bundle file ./output/myapplication.Main.sab from your file system.

In the Streaming Analytics service, go to the Log Viewer and Click on the PE's Console Log to view output
