# Test the RutaText operator for a long duration

Build the test application:

`make all`

Run the test (launch the job, capture memoryConsumption metric and cancel the job):

`runTest.sh`

Check the mem.<pid> file for the resident memory consumption over the time.
