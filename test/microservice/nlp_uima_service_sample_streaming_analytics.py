
#Imports
from streamsx.topology.topology import *
from streamsx.topology.context import *
from streamsx.topology.schema import CommonSchema, StreamSchema
from streamsx.topology import context
import streamsx.spl.op as op


def _launch(main):
    cfg = {}
    cfg[streamsx.topology.context.ConfigParams.SSL_VERIFY] = False
    rc = streamsx.topology.context.submit('STREAMING_ANALYTICS_SERVICE', main, cfg)

def uima_service():
    json_toolkit_location = None # use remote toolkit from build-service
    streams_install = os.environ.get('STREAMS_INSTALL')
    if streams_install is not None:
        json_toolkit_location = streams_install+'/toolkits/com.ibm.streamsx.json'

    topo = Topology('UimaService')
    nlp_toolkit = '../../com.ibm.streamsx.nlp'
    streamsx.spl.toolkit.add_toolkit(topo, nlp_toolkit)
    if json_toolkit_location is not None:
        streamsx.spl.toolkit.add_toolkit(topo, json_toolkit_location)
        toolkits=[nlp_toolkit, json_toolkit_location]
    else:
        toolkits=[nlp_toolkit]
    r = op.main_composite(kind='com.ibm.streamsx.nlp.services::UimaService', toolkits=toolkits)
    _launch(r[0])


class StringData(object):
    def __init__(self, count, delay=True):
        self.count = count
        self.delay = delay
    def __call__(self):
        if self.delay:
            time.sleep(10)
        doc = 'Text Sample\n'+'April 4, 2019 Distillery Lunch Seminar UIMA and its Metadata 12:00PM-1:00PM in HAW GN-K35 \n'+'April 16, 2019 KM & I Department Tea \n'+'Title: An Eclipse-based TAE Configurator Tool \n'+'3:00PM-4:30PM in HAW GN-K35 \n'+'May 11, 2019 UIMA Tutorial \n'+'9:00AM-5:00PM in HAW GN-K35 \n'
        for i in range(self.count):
            yield doc + ' - doc_' + str(i)

def sample():
    topo = Topology("NLPSample")
    s = topo.source(StringData(1000)).as_string()
    s.publish("streamsx/nlp/documents", schema=CommonSchema.String)
    
    ts = topo.subscribe("streamsx/nlp/annotations", schema=CommonSchema.Json)
    ts.print()
    ts.isolate()

    _launch(topo)


# launch microservice
uima_service()

# launch sample app
sample()

