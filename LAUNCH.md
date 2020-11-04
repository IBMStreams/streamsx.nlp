# Launch Microservices

## Before launching the microservice

Ensure that you have Python 3.6 installed.

Ensure that the bin directory is added to the PATH environment variable. If necessary, add the bin directory by entering the following command on the command line:

    export PATH="~/anaconda3/bin:$PATH"

Ensure that you have set the following environment variables:

* `STREAMING_ANALYTICS_SERVICE_NAME` - name of your Streaming Analytics service
* `VCAP_SERVICES` - [VCAP](https://console.bluemix.net/docs/services/StreamingAnalytics/r_vcap_services.html#r_vcap_services) information in JSON format or a JSON file

Install the latest streamsx package with pip, a package manager for Python, by entering the following command on the command line:

    pip install --user --upgrade streamsx

### Starting a Streaming Analytics service

Make sure that your Streaming Analytics service is running.

* If you have a Streaming Analytics service in IBM Cloud, make sure that it is started and running.
* To create a new [IBM Streaming Analytics](https://console.bluemix.net/docs/services/StreamingAnalytics/index.html#gettingstarted) service:
	* Go to the IBM Cloud web portal and sign in (or sign up for a free IBM Cloud account).
	* Click Catalog, browse for the Streaming Analytics service, and then click it.
	* Enter the service name and then click Create to set up your service. The service dashboard opens and your service starts automatically. The service name appears as the title of the service dashboard.

## Launch UimaService to the Streaming Analytics service

From command line you could launch the application with [streamsx-runner](http://ibmstreams.github.io/streamsx.topology/doc/pythondoc/scripts/runner.html) to the Streaming Analytics service:

    streamsx-runner --service-name $STREAMING_ANALYTICS_SERVICE_NAME --main-composite com.ibm.streamsx.nlp.services::UimaService --toolkits com.ibm.streamsx.nlp

