## ContentRankingSample

This sample demonstrates how you can use the ContentRankingModelBuilder operator and the ContentRanking operator from the com.ibm.streamsx.nlp toolkit.

## Use

Build standalone application:

`make`

Run:

`./output/ModelBuilder/bin/standalone`

You can also change the python command if you need to select a specific python version.

`./output/ModelBuilder/bin/standalone pythonCommand=python3`

After building the model files you can start the ContentRankingSample.

`./output/bin/standalone`

You can examine the input `./data/in.txt` and output `./data/out.txt`.

Compare output with expected data:

`diff ./data/expected2.txt ./data/out.txt`

There should be no difference between the files.

Clean:

`make clean`

You can also build a distributed application with the following command (required for Streams releases lower than 4.2 only):

`make distributed`

then submit your job to a running Streams instance.
