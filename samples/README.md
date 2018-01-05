# com.ibm.streamsx.nlp Toolkit Samples

The samples in this directory are prepared for the Streaming Analytics service in the IBM Cloud.
They can also be used in your local Streams installations, for example IBM Streams Quick Start Edition (QSE).

You can find sample projects for older releases (Streams 3.2 - 4.1) in `streamsx.nlp/test/feature` directory.

## Building the sample projects

To build the toolkit samples, run the make command in this directory:

    make all

Generate the documentation for the sample projects:

    make make-doc

Clean the toolkit samples:

    make clean

Delete the generated documentation for the sample projects:

    make clean-doc

## Streams Studio

* [Add com.ibm.streamsx.nlp toolkit](https://www.ibm.com/support/knowledgecenter/SSCRJU_4.2.1/com.ibm.streams.studio.doc/doc/tusing-working-with-toolkits-adding-toolkit-locations.html)
* Import "Existing Projects into Workspace and import to Streams Studio".
* Build the project
