import unittest

from streamsx.topology.topology import *
from streamsx.topology.tester import Tester
import streamsx.spl.op as op
import streamsx.spl.toolkit as tk
import os
import streamsx.rest as sr

class TestCloud(unittest.TestCase):
    """ Test invocations of composite operators in Streaming Analytics Service """

    @classmethod
    def setUpClass(self):
        self.service_name = os.environ.get('STREAMING_ANALYTICS_SERVICE_NAME')

    def setUp(self):
        Tester.setup_streaming_analytics(self, force_remote_build=False)

    def _add_toolkits(self, topo):
        tk.add_toolkit(topo, '../../samples/ContentRankingSample')
        tk.add_toolkit(topo, '../../samples/DictionaryFilterSample')
        tk.add_toolkit(topo, '../../samples/FunctionsSample')
        tk.add_toolkit(topo, '../../samples/LemmatizerSample')
        tk.add_toolkit(topo, '../../samples/LinearClassificationSample')
        tk.add_toolkit(topo, '../../samples/LinearClassificationSplPy')
        tk.add_toolkit(topo, '../../samples/NgramBasicSample')
        tk.add_toolkit(topo, '../../samples/NgramSample')
        tk.add_toolkit(topo, '../../samples/RutaTextSample')
        tk.add_toolkit(topo, '../../samples/TfIdfSample')
        tk.add_toolkit(topo, '../../samples/UimaTextGeneratedTypes')
        tk.add_toolkit(topo, '../../samples/UimaTextSample')

        tk.add_toolkit(topo, '../../com.ibm.streamsx.nlp')

    def _build_launch_validate(self, name, composite_name, parameters):
        topo = Topology(name)
        self._add_toolkits(topo)
	
        params = parameters
        # Call the test composite
        test_op = op.Source(topo, composite_name, 'tuple<rstring result>', params=params)

        tester = Tester(topo)
        tester.tuple_count(test_op.stream, 1, exact=True)
        tester.contents(test_op.stream, [{'result':'ok'}] )

        tester.test(self.test_ctxtype, self.test_config)

# missing python modules
#    def test_content_ranking(self):
#        self._build_launch_validate("test_content_ranking", "nlp.sample::ContentRankingComp", {'pythonCommand':'$PYTHONHOME/bin/python3'})

    def test_dictionary_filter(self):
        self._build_launch_validate("test_dictionary_filter", "nlp.sample::DictionaryFilterComp", {})

    def test_functions(self):
        self._build_launch_validate("test_functions", "nlp.sample::FunctionsComp", {})

    def test_lemmatizer(self):
        self._build_launch_validate("test_lemmatizer", "nlp.sample::LemmatizerComp", {})

    def test_linear_classification(self):
        self._build_launch_validate("test_linear_classification", "nlp.sample::LinearClassificationComp", {'pythonCommand':'$PYTHONHOME/bin/python3'})

    def test_linear_classification_py(self):
        self._build_launch_validate("test_linear_classification_py", "nlp.sample::LinearClassificationPyComp", {})

    def test_ngram_basic(self):
        self._build_launch_validate("test_ngram_basic", "nlp.sample::NgramBasicComp", {})

    def test_ngram(self):
        self._build_launch_validate("test_ngram", "nlp.sample::NgramComp", {})

    def test_ruta_text(self):
        self._build_launch_validate("test_ruta_text", "nlp.sample::RutaTextComp", {'loop':1})

    def test_tfidf(self):
        self._build_launch_validate("test_tfidf", "nlp.sample::TfIdfComp", {})

    def test_uima_text_gen_types(self):
        self._build_launch_validate("test_uima_text_gen_types", "nlp.sample::UimaTextGeneratedTypesComp", {})

    def test_uima_text(self):
        self._build_launch_validate("test_uima_text", "nlp.sample::UimaTextComp", {})



