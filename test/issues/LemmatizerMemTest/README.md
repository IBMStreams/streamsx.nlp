# Test the Lemmatizer operator for a long duration

Build the test application:

`make all`

Run the test (launch the job, capture memoryConsumption metric and cancel the job):

`runTest.sh`

Compare output (mem0 file is the first memoryConsumption snapshot and mem1 is the last):

`diff ./mem0 ./mem1`

It is expected that the files not differ.
