# streamsx.nlp tests

This test suite is intended for feature tests of the nlp toolkit using the applications in the `feature` directory.

Supported Streams Releases:

* InfoSphereStreams 3.2
* InfoSphereStreams 4.0
* InfoSphereStreams 4.1
* IBM Streams 4.2

## Directory structure

* cloud-test - python unit test using TopologyTester to launch and validate the sample applications in the Streaming Analytics service.
* feature - sample applications used by the test scenario scripts
* issues - test applications for manual tests, e.g. memory leak test
* performance - applications to measure and compares the throughput of AQL, UIMA and Ruta applications
* testcases - python scripts containing the test scenario


# Run the tests

The test scripts require **python 2.x** for python unittest.

```
ant test
```

To run a single test, select one of the test applications, for example:
```
./runtests.py -t feature/Lemmatizer
```

# Clean-up

Delete generated files of test suites.
```
ant clean
```
