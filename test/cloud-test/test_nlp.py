import unittest

from streamsx.topology.topology import *
from streamsx.topology.tester import Tester
import streamsx.spl.op as op
import streamsx.spl.toolkit as tk
import os
import streamsx.rest as sr

import streamsx.topology.context
import requests
from urllib.parse import urlparse

class TestCloud(unittest.TestCase):
    """ Test invocations of composite operators in Streaming Analytics Service """

    @classmethod
    def setUpClass(self):
        self.service_name = os.environ.get('STREAMING_ANALYTICS_SERVICE_NAME')
        # start streams service
        connection = sr.StreamingAnalyticsConnection()
        service = connection.get_streaming_analytics()
        result = service.start_instance()

    def setUp(self):
        Tester.setup_streaming_analytics(self, force_remote_build=True)

    def _add_toolkits(self, topo, toolkit_name):
        tk.add_toolkit(topo, toolkit_name)
        tk.add_toolkit(topo, '../../com.ibm.streamsx.nlp')

    def _build_launch_validate(self, name, composite_name, parameters, toolkit_name):
        print ("------ "+name+" ------")        
        topo = Topology(name)
        self._add_toolkits(topo, toolkit_name)
        #if name == 'test_content_ranking':
        #    topo.add_pip_package('Theano==0.8.2')
        #    topo.add_pip_package('Keras==1.0.7')
	
        params = parameters
        # Call the test composite
        test_op = op.Source(topo, composite_name, 'tuple<rstring result>', params=params)

        tester = Tester(topo)
        tester.tuple_count(test_op.stream, 1, exact=True)
        tester.contents(test_op.stream, [{'result':'ok'}] )

        cfg = {}
  
        # change trace level
        job_config = streamsx.topology.context.JobConfig(tracing='info')
        job_config.add(cfg)

        if ("TestICP" in str(self)):
            cfg[streamsx.topology.context.ConfigParams.SSL_VERIFY] = False

        tester.test(self.test_ctxtype, cfg)
        print (str(tester.result))

#    def test_content_ranking(self):
#        self._build_launch_validate("test_content_ranking", "nlp.sample::ContentRankingComp", {'pythonCommand':'$PYTHONHOME/bin/python3'}, '../../samples/ContentRankingSample')

    def test_dictionary_filter(self):
        self._build_launch_validate("test_dictionary_filter", "nlp.sample::DictionaryFilterComp", {}, '../../samples/DictionaryFilterSample')

    def test_functions(self):
        self._build_launch_validate("test_functions", "nlp.sample::FunctionsComp", {}, '../../samples/FunctionsSample')

    def test_lemmatizer(self):
        self._build_launch_validate("test_lemmatizer", "nlp.sample::LemmatizerComp", {}, '../../samples/LemmatizerSample')

    def test_linear_classification(self):
        self._build_launch_validate("test_linear_classification", "nlp.sample::LinearClassificationComp", {'pythonCommand':'$PYTHONHOME/bin/python3'},'../../samples/LinearClassificationSample')

    def test_linear_classification_py(self):
        self._build_launch_validate("test_linear_classification_py", "nlp.sample::LinearClassificationPyComp", {}, '../../samples/LinearClassificationSplPy')

    def test_ngram_basic(self):
        self._build_launch_validate("test_ngram_basic", "nlp.sample::NgramBasicComp", {},'../../samples/NgramBasicSample')

    def test_ngram(self):
        self._build_launch_validate("test_ngram", "nlp.sample::NgramComp", {}, '../../samples/NgramSample')

    def test_ruta_text(self):
        self._build_launch_validate("test_ruta_text", "nlp.sample::RutaTextComp", {'loop':1}, '../../samples/RutaTextSample')

    def test_tfidf(self):
        self._build_launch_validate("test_tfidf", "nlp.sample::TfIdfComp", {}, '../../samples/TfIdfSample')

    def test_uima_text_gen_types(self):
        self._build_launch_validate("test_uima_text_gen_types", "nlp.sample::UimaTextGeneratedTypesComp", {}, '../../samples/UimaTextGeneratedTypes')

    def test_uima_text(self):
        self._build_launch_validate("test_uima_text", "nlp.sample::UimaTextComp", {}, '../../samples/UimaTextSample')


class TestICP(TestCloud):
    """ Test invocations of composite operators in remote Streams instance using local toolkit """

    @classmethod
    def setUpClass(self):
        print (str(self))
        env_chk = True
        try:
            print("STREAMS_REST_URL="+str(os.environ['STREAMS_REST_URL']))
        except KeyError:
            env_chk = False
        assert env_chk, "STREAMS_REST_URL environment variable must be set"     

    def setUp(self):
        Tester.setup_distributed(self)



