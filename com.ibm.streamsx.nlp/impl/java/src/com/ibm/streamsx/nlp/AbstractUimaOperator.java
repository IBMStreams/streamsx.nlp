//
// *******************************************************************************
// * Copyright (C)2016, International Business Machines Corporation *
// * All rights reserved. *
// *******************************************************************************
//
package com.ibm.streamsx.nlp;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.uima.UIMAFramework;
import org.apache.uima.analysis_engine.AnalysisEngine;
import org.apache.uima.cas.BooleanArrayFS;
import org.apache.uima.cas.ByteArrayFS;
import org.apache.uima.cas.CAS;
import org.apache.uima.cas.DoubleArrayFS;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.cas.Feature;
import org.apache.uima.cas.FeatureStructure;
import org.apache.uima.cas.FloatArrayFS;
import org.apache.uima.cas.IntArrayFS;
import org.apache.uima.cas.LongArrayFS;
import org.apache.uima.cas.ShortArrayFS;
import org.apache.uima.cas.StringArrayFS;
import org.apache.uima.cas.Type;
import org.apache.uima.cas.TypeSystem;
import org.apache.uima.resource.ResourceManager;
import org.apache.uima.resource.ResourceSpecifier;
import org.apache.uima.ruta.engine.Ruta;
import org.apache.uima.ruta.engine.RutaEngine;
import org.apache.uima.util.XMLInputSource;
import org.apache.uima.cas.impl.XmiCasDeserializer;
import org.apache.uima.cas.impl.XmiCasSerializer;
import org.apache.uima.cas.text.AnnotationFS;
import org.apache.uima.pear.tools.PackageBrowser;
import org.apache.uima.pear.tools.PackageInstaller;
import org.apache.uima.util.XMLSerializer;

import org.apache.log4j.Logger;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.logging.LoggerNames;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.meta.TupleType;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPortSet.WindowMode;
import com.ibm.streams.operator.model.InputPortSet.WindowPunctuationInputMode;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.Libraries;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.types.RString;

/**
 * Abstract class for the UIMA operators.
 * <P>
 * The following event methods from the Operator interface can be called:
 * </p>
 * <ul>
 * <li><code>initialize()</code> to perform operator initialization</li>
 * <li>allPortsReady() notification indicates the operator's ports are ready to process and submit tuples</li> 
 * <li>process() handles a tuple arriving on an input port 
 * <li>processPuncuation() handles a punctuation mark arriving on an input port 
 * <li>shutdown() to shutdown the operator. A shutdown request may occur at any time, 
 * such as a request to stop a PE or cancel a job. 
 * Thus the shutdown() may occur while the operator is processing tuples, punctuation marks, 
 * or even during port ready notification.</li>
 * </ul>
 * <p>With the exception of operator initialization, all the other events may occur concurrently with each other, 
 * which lead to these methods being called concurrently by different threads.</p> 
 */
@Libraries({"impl/lib/apache-uima/uima-core.jar"})
public abstract class AbstractUimaOperator extends AbstractOperator {
	
	private static Logger trace = Logger.getLogger(AbstractUimaOperator.class.getName());

	protected static boolean controlPortDefined = false;
	
	/**
	 * filename for update/reload on control stream
	 */
	protected static final String CONTROL_STREAM_FILE_ATTR = "filename";
	
	// parameters
	protected String inputDoc;
	protected String casIn;
	protected String casOut = null;
	protected String pearFileParam = null;
	protected String languageCode = "en";
	protected String languageCodeAttribute = null;
	protected String viewParam = null;
	protected String view = CAS.NAME_DEFAULT_SOFA;
	protected boolean trimInputDoc = true;

	protected List<String> outputAttributes = null;
	protected List<String> outputViews = null;
	protected List<String> outputTypes = null;

	private static final String quoteReserved = "protectreserved_";
	
	/**
	 * lock to synchronize PEAR file update and document processing
	 */
	protected Object updateLock = new Object();

	private Set<String> splReserved;

	/**
	 * Directory of installed PEAR Files
	 */
	private File installDir = null;
	/**
	 * PEAR file with absolute path
	 */
	private File pearFile = null;
	/**
	 * PEAR file with absolute path if install succeeded, set in initialize()
	 * This is the fallback file if PEAR update fails
	 */
	private File initialPearFile = null;
	/**
	 * The data directory of the application
	 */
	protected File dataDir = null;
	/**
	 * The base directory of the application
	 */	
	protected File baseDir = null;
	
	// UIMA objects
	protected AnalysisEngine ae = null;
	protected CAS cas = null;
	protected PackageBrowser instPear = null;
	
	protected boolean doPearVerification = true;
	
	/**
	 * If this parameter is set to true, then additional information about the execution of a rule
	 * script is added to the CAS.
	 */
	protected boolean debugMode = false;
	
	private final String INPUT_ENCODING = "UTF-8";
	
	private static final String UIMA_STREAMS_COVERED_TEXT_ATTRIBUTE = "text";
	private static final String UIMA_STREAMS_COVERED_TYPE_DESC_ATTRIBUTE = "typeDescription";
	
	private static final String PARAMETER_NAME_PEARFILE = "pearFile";
	private static final String PARAMETER_NAME_LANGUAGE_CODE = "languageCode";
	private static final String PARAMETER_NAME_LANGUAGE_CODE_ATTRIBUTE = "languageCodeAttribute";
	private static final String PARAMETER_NAME_VIEW = "view";
	private static final String PARAMETER_NAME_CAS_OUT = "casOut";
	private static final String PARAMETER_NAME_OUTPUT_ATTRIBUTES = "outputAttributes";
	private static final String PARAMETER_NAME_OUTPUT_VIEWS = "outputViews";
	private static final String PARAMETER_NAME_OUTPUT_TYPES = "outputTypes";
	
	@Parameter(name=PARAMETER_NAME_PEARFILE, description="This parameter specifies the PEAR file to be installed. The file should be stored in etc directory and can be specified using absolute paths or relative paths. If relative paths, then the PEAR file is relative to the root of the application directory.", optional=false)
	public void setPearFile(String pearFile) {
		this.pearFileParam = pearFile;
	}

	@Parameter(name=PARAMETER_NAME_LANGUAGE_CODE, description="This optional parameter specifies the ISO language code to be used by UIMA. The default value is `en` for English.", optional=true)
	public void setLanguageCode(String languageCode) {
		this.languageCode = languageCode;
	}

	@Parameter(name=PARAMETER_NAME_LANGUAGE_CODE_ATTRIBUTE, description="This optional parameter enables the language to be specified on a tuple-by-tuple basis. It specifies the name of the attribute that contains the language code.", optional=true)
	public void setLanguageCodeAttribute(String languageCodeAttribute) {
		this.languageCodeAttribute = languageCodeAttribute;
	}

	@Parameter(name=PARAMETER_NAME_VIEW, description="This parameter specifies the view of the CAS.", optional=true)
	public void setView(String view) {
		this.viewParam = view;
	}

	@Parameter(name=PARAMETER_NAME_CAS_OUT, description="This parameter specifies the attribute of the output tuples that contains the UIMA CAS as serialized XMI string. The output attribute is of type rstring. If this parameter is not specified, the operator expects that the parameter `outputAttributes` is set.", optional=true)
	public void setOutputAttribute(String casOut) {
		this.casOut = casOut;
	}

	@Parameter(name=PARAMETER_NAME_OUTPUT_ATTRIBUTES, description="This parameter specifies the name of tuple attributes on the output port for the annotations. This parameter can be specified more than once. The operator assumes that the views from the parameter `outputViews` are in the same order as the attribute names in this parameter. If this parameter is not specified, the operator expects that the parameter `casOut` is set. The attribute must a list type.", optional=true)
	public void setOutputAttributes(String[] outputAttributes) {
		this.outputAttributes = new ArrayList<String>();
		for (String s : outputAttributes) {
			this.outputAttributes.add(s);
		}
	}

	@Parameter(name=PARAMETER_NAME_OUTPUT_VIEWS, description="This optional parameter specifies the fully qualified view names to output. This parameter can be specified more than once. The operator assumes that the output tuple attribute names from the parameter `outputAttributes` are in the same order as the views in this parameter. If this parameter is not specified, the operator expects that the parameter `outputAttributes` contains a single output tuple attribute only.", optional=true)
	public void setOutputViews(String[] outputViews) {
		this.outputViews = new ArrayList<String>();
		for (String s : outputViews) {
			this.outputViews.add(s);
		}
	}

	@Parameter(name=PARAMETER_NAME_OUTPUT_TYPES, description="This optional parameter specifies the fully qualified type names to filter the output for a set of types. This parameter can be specified more than once. The output attributes, that are set with the parameter `outputAttributes`, contain annotation of these types only.", optional=true)
	public void setOutputTypes(String[] outputTypes) {
		this.outputTypes = new ArrayList<String>();
		for (String s : outputTypes) {
			this.outputTypes.add(s);
		}
	}

	@ContextCheck(compile = true)
	public static void checkParameters(OperatorContextChecker checker) {

		OperatorContext context = checker.getOperatorContext();
		int numInpoutPorts = context.getNumberOfStreamingInputs();

		// more than 2 input ports is not possible since validated by the operator model
		if (numInpoutPorts == 2) {
			StreamingInput<Tuple> port1 = context.getStreamingInputs().get(1);

			if (!checker.checkRequiredAttributes(port1, CONTROL_STREAM_FILE_ATTR)){
				checker.setInvalidContext();
			}
			controlPortDefined = true;
		} else {
			controlPortDefined = false;
		}
		// Either casOut or outputAttributes must be set
		if ((!context.getParameterNames().contains(PARAMETER_NAME_CAS_OUT)) && (!context.getParameterNames().contains(PARAMETER_NAME_OUTPUT_ATTRIBUTES)) ) {
			checker.setInvalidContext("Either "+PARAMETER_NAME_CAS_OUT+" or "+PARAMETER_NAME_OUTPUT_ATTRIBUTES+" must be set.", null);
		}
		if (context.getParameterNames().contains(PARAMETER_NAME_OUTPUT_VIEWS)) {
			if (!context.getParameterNames().contains(PARAMETER_NAME_OUTPUT_ATTRIBUTES)) {
				checker.setInvalidContext("The parameter "+PARAMETER_NAME_OUTPUT_ATTRIBUTES+" must be specified, if the parameter "+PARAMETER_NAME_OUTPUT_VIEWS+" is set.", null);
			}
		}
	}

	@ContextCheck(compile = false)
	public static void checkRuntimeParameters(OperatorContextChecker checker) {
		OperatorContext context = checker.getOperatorContext();
		
		// Check if the names of the outputAttributes parameter are valid attributes on output port 0
		if (context.getParameterNames().contains(PARAMETER_NAME_OUTPUT_ATTRIBUTES)) {
			List<String> names = context.getParameterValues(PARAMETER_NAME_OUTPUT_ATTRIBUTES);
			
			for (String n : names) {
				boolean isAttrOnOutputPort = false;
				for (String att : context.getStreamingOutputs().get(0).getStreamSchema().getAttributeNames()) {
					if (n.equals(att)) {
						isAttrOnOutputPort = true;
						break;
					}
				}
				if (!isAttrOnOutputPort) {
					checker.setInvalidContext("The parameter "+PARAMETER_NAME_OUTPUT_ATTRIBUTES+" has a value that does not match an attribute name of the output port 0: {0}", new Object[] {n});
				}
				else {
					// check the type of the output attribute, list<tuple> is required
					Attribute attr = context.getStreamingOutputs().get(0).getStreamSchema().getAttribute(n);
					if (null != attr) {
						if (attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.LIST) {
							checker.setInvalidContext("The output attribute {0} must be of type list<tuple>.", new Object[] {n});
						}
					}
				}
			}
			
			if (context.getParameterNames().contains(PARAMETER_NAME_OUTPUT_VIEWS)) {
				List<String> viewnames = context.getParameterValues(PARAMETER_NAME_OUTPUT_VIEWS);
				if (names.size() != viewnames.size()) {
					checker.setInvalidContext("The number of view attributes (parameter "+PARAMETER_NAME_OUTPUT_ATTRIBUTES+") does not match the number of view names (parameter "+PARAMETER_NAME_OUTPUT_VIEWS+").", null);
				}
			}
			
		}
		// Check if the name of the casOut parameter is a valid attribute on output port 0
		if (context.getParameterNames().contains(PARAMETER_NAME_CAS_OUT)) {
			String value = context.getParameterValues(PARAMETER_NAME_CAS_OUT).get(0);
			if (!context.getStreamingOutputs().get(0).getStreamSchema().getAttributeNames().contains(value)) {
				checker.setInvalidContext("The parameter "+PARAMETER_NAME_CAS_OUT+" has a value that does not match an attribute name of the output port 0: {0}", new Object[] {value});
			}
		}
	}

	/**
	 * The pear file is extracted and optional validated with the UIMA PackageInstaller.
	 * 
	 * @throws Exception
	 */
	protected void installPearFile() throws Exception {
		// install PEAR package
		instPear = PackageInstaller.installPackage(installDir, pearFile, doPearVerification);

		if (trace.isInfoEnabled()) {
			// retrieve installed PEAR data
			// PEAR package classpath
			String classpath = instPear.buildComponentClassPath();
			// PEAR package datapath
			String datapath = instPear.getComponentDataPath();
			// PEAR package main component descriptor
			String mainComponentDescriptor = instPear.getInstallationDescriptor().getMainComponentDesc();
			// PEAR package component ID
			String mainComponentID = instPear.getInstallationDescriptor().getMainComponentId();
			// PEAR package pear descriptor
			String pearDescPath = instPear.getComponentPearDescPath();

			trace.info("PEAR package class path: " + classpath);
			trace.info("PEAR package datapath: " + datapath);
			trace.info("PEAR package mainComponentDescriptor: " + mainComponentDescriptor);
			trace.info("PEAR package mainComponentID: " + mainComponentID);
			trace.info("PEAR package specifier path: " + pearDescPath);
		}
	}

	/**
	 * Removes the directory of the extracted pear file.
	 */
	protected void uninstallPearFile() {
		if (null != installDir) {
			// delete installed pears
			deleteDir(installDir);
		}
	}

	/**
	 * Converts the path to absolute path.
	 */
	protected String makeAbsolute(File rootForRelative, String path)
			throws IOException {
		File pathFile = new File(path);
		if (pathFile.isAbsolute()) {
			return pathFile.getCanonicalPath();
		} else {
			File abs = new File(rootForRelative.getAbsolutePath()
					+ File.separator + path);
			return abs.getCanonicalPath();
		}
	}

	/**
	 * Recursive directory delete
	 * @param dir
	 * @return
	 */
	private boolean deleteDir(File dir) {
		if (dir.isDirectory()) {
			String[] f = dir.list();
			for (int i = 0; i < f.length; i++) {
				boolean success = deleteDir(new File(dir, f[i]));
				if (!success) {
					return false;
				}
			}
		}
		return dir.delete();
	}

	/**
	 * Create AnalysisEngine and CAS with UIMAFramework
	 * This method is called by UimaText and UimaCas operators.
	 * 
	 * @throws Exception
	 */
	protected void prepareAnalysisEngine() throws Exception {
		if (null != instPear) {
			// Create a default resource manager
			ResourceManager rsrcMgr = UIMAFramework.newDefaultResourceManager();

			// Create analysis engine from the installed PEAR package using
			// the created PEAR specifier
			XMLInputSource in = new XMLInputSource(instPear.getComponentPearDescPath());
			ResourceSpecifier specifier = UIMAFramework.getXMLParser().parseResourceSpecifier(in);
			ae = UIMAFramework.produceAnalysisEngine(specifier, rsrcMgr, null);

			// Create a CAS
			cas = ae.newCAS();
		}
	}

	/**
	 * Create AnalysisEngine and CAS with Ruta Framework
	 * This method is called by RutaText and RutaCas operators.
	 * 
	 * @throws Exception
	 */	
	protected void prepareRutaAnalysisEngine(String view, boolean removeBasics) throws Exception {
		if (null != instPear) {
			File descriptor = new File(instPear.getInstallationDescriptor().getMainComponentDesc());
			// change paths in descriptor xml file
			File rootDir = new File(instPear.getComponentPearDescPath());
			rootDir = new File(rootDir.getParentFile().getAbsolutePath());
			trace.debug("rootDir: "+rootDir.toString());		
			String pattern = "<string>(.*" + instPear.getInstallationDescriptor().getMainComponentId() + "/)";
			String tmp = org.apache.uima.util.FileUtils.file2String(descriptor);
			tmp = tmp.replaceAll(pattern, "<string>"+rootDir+"/");
			org.apache.uima.util.FileUtils.saveString2File(tmp, descriptor);

			ae = Ruta.wrapAnalysisEngine(descriptor.toURI().toURL(), view, true, INPUT_ENCODING);

			if (debugMode) {
				ae.setConfigParameterValue(RutaEngine.PARAM_DEBUG, true);
				ae.setConfigParameterValue(RutaEngine.PARAM_DEBUG_WITH_MATCHES, true);
				ae.setConfigParameterValue(RutaEngine.PARAM_PROFILE, true);
				ae.setConfigParameterValue(RutaEngine.PARAM_STATISTICS, true);
				ae.setConfigParameterValue(RutaEngine.PARAM_CREATED_BY, true);
			}
			if (removeBasics) {
				ae.setConfigParameterValue(RutaEngine.PARAM_REMOVE_BASICS, true);
			}
			else {
				ae.setConfigParameterValue(RutaEngine.PARAM_REMOVE_BASICS, false);
			}

			ae.reconfigure();
			cas = ae.newCAS();
		}
	}

	protected void selectView() {
		if (viewParam != null) {
			boolean found = false;
			Iterator<CAS> viewIterator = cas.getViewIterator();
			while (viewIterator.hasNext()) {
				CAS each = (CAS) viewIterator.next();
				String viewName = each.getViewName();
				if (viewName.equals(view)) {
					cas = cas.getView(view);
					found = true;
					trace.debug("Select CAS view " + view);
					break;
				}
			}
			if (!found) {
				trace.debug("Create CAS view " + view);
				cas = cas.createView(view);
			}
		}
	}

	protected File getPearFile() {
		return pearFile;
	}
	
	protected void fallbackPearFile() {
		pearFile = initialPearFile;
	}

	protected void setPearFilename(File rootForRelative, String filename) throws Exception {
		pearFile = new File(makeAbsolute(rootForRelative, filename));
	}

	protected File getInstallDir() {
		return installDir;
	}

	protected String getXmiString() throws Exception {
		// create xmi output
		StringWriter strOut = new StringWriter();
		XmiCasSerializer ser = new XmiCasSerializer(cas.getTypeSystem());
		XMLSerializer xmlSer = new XMLSerializer(strOut, false);
		ser.serialize(cas, xmlSer.getContentHandler());
		return strOut.toString();
	}
	
	protected void createCasFromXmiString(String xmi) throws Exception {
		InputStream stream = new ByteArrayInputStream(xmi.getBytes("UTF-8"));
		XmiCasDeserializer.deserialize(stream, cas);
	}

	/**
	 * Initialize this operator. Called once before any tuples are processed.
	 * @param context OperatorContext for this operator.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	public synchronized void initialize(OperatorContext context)
			throws Exception {
		// Must call super.initialize(context) to correctly setup an operator.
		super.initialize(context);
		trace.info("Operator " + context.getName() + " initializing in PE: " + context.getPE().getPEId() + " in Job: " + context.getPE().getJobId() );
		
		trace.debug("pearFile: " + pearFileParam);
		baseDir = context.getPE().getApplicationDirectory();
		if (true == new File(baseDir + File.separator + "etc" + File.separator + pearFileParam).isFile()) {
			// compatibility:
			// in earlier versions it was sufficient to apply the filename only,
			// if the PEAR file is located in applications etc dir.
			trace.debug("pearFile base is ApplicationDirectory/etc");
			baseDir = new File(baseDir + File.separator + "etc");
		}
		
		try {
			dataDir = context.getPE().getDataDirectory();
		} catch (Exception e) {
			trace.warn("Unable to get the data directory. Install PEAR file in /tmp ...");
			dataDir = new File("/tmp");
		}
		// pear files are installed in data directory
		installDir = new File(dataDir + File.separator + new File("installedPears") + context.getName());
		if (null != pearFileParam) {
			setPearFilename(baseDir, pearFileParam);
			installPearFile();
			if (controlPortDefined) {
				initialPearFile = new File(makeAbsolute(baseDir, pearFileParam));
			}
		}
		if (viewParam != null) {
			view = viewParam;
		}
		initSplReservedSet();

		//StreamSchema sSchema = context.getStreamingOutputs().get(0).getStreamSchema();
		//for (Attribute attr1 : sSchema) {
			//System.out.println("attrName="+attr1.getName());
			//System.out.println("attrType="+attr1.getType());
		//}
	}

	private void initSplReservedSet() {
		splReserved = new HashSet<String>();
		splReserved.add("namespace");
		splReserved.add("use");
		splReserved.add("public");
		splReserved.add("composite");
		splReserved.add("input");
		splReserved.add("output");
		splReserved.add("stream");
		splReserved.add("param");
		splReserved.add("type");
		splReserved.add("graph");
		splReserved.add("config");
		splReserved.add("logic");
		splReserved.add("window");
		splReserved.add("onTuple");
		splReserved.add("onPunct");
		splReserved.add("state");
		splReserved.add("public");
		splReserved.add("stateful");
		splReserved.add("mutable");
		splReserved.add("any");
		splReserved.add("collection");
		splReserved.add("complex");
		splReserved.add("composite");
		splReserved.add("decimal");
		splReserved.add("enum");
		splReserved.add("float");
		splReserved.add("floatingpoint");
		splReserved.add("integral");
		splReserved.add("list");
		splReserved.add("map");
		splReserved.add("numeric");
		splReserved.add("ordered");
		splReserved.add("primitive");
		splReserved.add("set");
		splReserved.add("string");
		splReserved.add("tuple");
		splReserved.add("if");
		splReserved.add("for");
		splReserved.add("while");
		splReserved.add("break");
		splReserved.add("continue");
		splReserved.add("return");
		splReserved.add("true");
		splReserved.add("false");
		splReserved.add("static");
		splReserved.add("attribute");
		splReserved.add("expression");
		splReserved.add("function");
		splReserved.add("operator");
		splReserved.add("void");
		splReserved.add("enum");
		splReserved.add("int8");
		splReserved.add("int16");
		splReserved.add("int32");
		splReserved.add("int64");
		splReserved.add("int128");
		splReserved.add("uint8");
		splReserved.add("uint16");
		splReserved.add("uint32");
		splReserved.add("uint64");
		splReserved.add("uint128");
		splReserved.add("float32");
		splReserved.add("float64");
		splReserved.add("decimal32");
		splReserved.add("decimal64");
		splReserved.add("decimal128");
		splReserved.add("complex32");
		splReserved.add("complex64");
		splReserved.add("timestamp");
		splReserved.add("blob");
		splReserved.add("ustring");
		splReserved.add("rstring");
		splReserved.add("list");
		splReserved.add("map");
		splReserved.add("set");
	}

	/**
	 * Notification that initialization is complete and all input and output ports 
	 * are connected and ready to receive and submit tuples.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	public synchronized void allPortsReady() throws Exception {
		// This method is commonly used by source operators. 
		// Operators that process incoming tuples generally do not need this notification. 
		OperatorContext context = getOperatorContext();
		trace.info("Operator " + context.getName() + " all ports are ready in PE: " + context.getPE().getPEId() + " in Job: " + context.getPE().getJobId() );
	}

	/**
	 * Process an incoming tuple that arrived on the specified port.
	 * <P>
	 * Copy the incoming tuple to a new output tuple and submit to the output port. 
	 * </P>
	 * @param inputStream Port the tuple is arriving on.
	 * @param tuple Object representing the incoming tuple.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	public final void process(StreamingInput<Tuple> inputStream, Tuple tuple)
			throws Exception {

		if (inputStream.getPortNumber() == 0) {
			if (controlPortDefined) {
				synchronized (updateLock) {
					handleInputOnPort0(tuple);
				}
			}
			else {
				handleInputOnPort0(tuple);
			}
		}
		else if (inputStream.getPortNumber() == 1){
			setPearFilename(dataDir, tuple.getString(CONTROL_STREAM_FILE_ATTR));
		}
	}

	/**
	 * Process an incoming punctuation that arrived on the specified port.
	 * @param stream Port the punctuation is arriving on.
	 * @param mark The punctuation mark
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	public void processPunctuation(StreamingInput<Tuple> stream,
			Punctuation punct) throws Exception {
		int portNumber = stream.getPortNumber();
		if (portNumber == 1 && punct == Punctuation.WINDOW_MARKER) {
			synchronized (updateLock) {
				processWindowPunctuationPort1();
			}
		}
		else {
			super.processPunctuation(stream, punct);
		}
	}

	/**
	 * Process the tuples arrived on the port 0.
	 */
	protected abstract void handleInputOnPort0(Tuple tuple) throws Exception;

	/**
	 * Process the incoming window punctuation that arrived on the port 1.
	 */
	protected abstract void processWindowPunctuationPort1() throws Exception;

	/**
	 * Deletes the AnalysisEngine resources
	 */
	protected void destroyAnalysisEngine() {
		// release UIMA resources
		if (null != ae) {
			ae.destroy();
			ae = null;
		}
	}

	/**
	 * Shutdown this operator.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	public synchronized void shutdown() throws Exception {
		OperatorContext context = getOperatorContext();
		trace.info("Operator " + context.getName() + " shutting down in PE: " + context.getPE().getPEId() + " in Job: " + context.getPE().getJobId() );

		destroyAnalysisEngine();

		uninstallPearFile();

		// Must call super.shutdown()
		super.shutdown();
	}
	
	/**
	 * Creates the output tuple, forwards input attributes, retrieves annotations from the CAS and submits the tuple.
	 *  
	 * @param inputTuple
	 * @throws Exception
	 */
	public void createAndSubmitOutputTuple(Tuple inputTuple) throws Exception {
		// Create a new tuple for output port 0
		StreamingOutput<OutputTuple> outStream = getOutput(0);
		OutputTuple outTuple = outStream.newTuple();

		// Copy across all matching attributes.
		outTuple.assign(inputTuple);
		
		if (null != casOut) {
			outTuple.setString(casOut, getXmiString());
		}
		if (null != outputAttributes) {
			StreamSchema outputSchema = outStream.getStreamSchema();
			for (Attribute attr : outputSchema) {
				String attributeName = attr.getName();
				//System.out.println("attributeName="+attributeName);
				//System.out.println("type="+attr.getType());
				if (outputAttributes.contains(attributeName)) {
					
					CollectionType streamsListType = (CollectionType) attr.getType();
					// Get the type of the streams list element.
					StreamSchema sSchema = ((TupleType) streamsListType.getElementType()).getTupleSchema();
					
					// key is the attribute name in lower case for the compare with UIMA feature name
					// The value is the origin attribute name
					Map<String, String> attributeNamesMap = new HashMap<String, String>();
					for (Attribute attr1 : sSchema) {
						attributeNamesMap.put(attr1.getName().toLowerCase(), attr1.getName());
					}
					
					if ((outputViews != null) && (outputViews.size() == outputAttributes.size())) {
						String viewname = outputViews.get(outputAttributes.indexOf(attributeName));
						trace.debug("Select CAS view " + viewname + " for attribute "+attributeName);
						cas = cas.getView(viewname);
					}
					
					List<Tuple> listTuple = new ArrayList<Tuple>();
					fillListTupleWithAnnotations(cas, sSchema, attributeNamesMap, listTuple);
					// set the list for the output attribute to the outTuple
					outTuple.setList(attributeName, listTuple);
				}
			}
		}

		// Submit new tuple to output port 0
		outStream.submit(outTuple);
	}

	/**
	 * Checks if the parameter list outputTypes contains the annotation type 
	 * @param type
	 * @return true if the type should be part of the output attribute
	 */
	private boolean includeAnnotationType (String type) {
		boolean result = true;
		if (null != outputTypes) {
			if (outputTypes.contains(type)) {
				result = true;
			}
			else {
				result = false;
			}
		}
		return result;
	}

	/**
	 * Iterate over annotations and fill the map of attributes for list tuple type.
	 * 
	 * @param aCAS (input)
	 * @param listTupleType (input)
	 * @param attributeNamesMap (input)
	 * @param listTuple (output)
	 */
	private void fillListTupleWithAnnotations(CAS aCAS, StreamSchema listTupleType, Map<String, String> attributeNamesMap, List<Tuple> listTuple) {
		// This map contains the attributes of the list tuple type
		// The key is the attribute name and the value is the tuple type specific object
		Map<String, Object> tupleMap = new HashMap<String, Object>();

		// get iterator over annotations
		FSIterator iter = aCAS.getAnnotationIndex().iterator();

		// iterate
		while (iter.isValid()) {
			FeatureStructure fs = iter.get();
			// check if the type is part of the output filter
			trace.debug("FeatureStructure type="+fs.getType().getName());
			if (includeAnnotationType(fs.getType().getName())) {
				StringBuilder sofaID = new StringBuilder();
				// check feature structure
				processFS(fs, aCAS, listTupleType, attributeNamesMap, tupleMap, sofaID);

				trace.debug("viewname="+sofaID.toString());
				if ((null == outputViews) || ((outputViews != null) && (outputViews.contains(sofaID.toString())))) {
					// add attributes from the map to the list tuple
					listTuple.add(listTupleType.getTuple(tupleMap));
				}
			}
			iter.moveToNext();
			tupleMap.clear(); // reset attributes for next annotation
		}
	}

	/**
	 * Iterate over all features and fill the tupleMap if attribute names and type match the feature name and type
	 * 
	 * @param aFS (input)
	 * @param aCAS (input)
	 * @param listTupleType (input)
	 * @param attributeNamesMap (input)
	 * @param tupleMap (output)
	 * @param sofaID (output)
	 */
	private void processFS(FeatureStructure aFS, CAS aCAS, StreamSchema listTupleType, Map<String, String> attributeNamesMap, Map<String, Object> tupleMap, StringBuilder sofaID) {
		Type stringType = aCAS.getTypeSystem().getType(CAS.TYPE_NAME_STRING);

		if (aFS instanceof AnnotationFS) {
			// check if covered text attribute needs to be filled
			if (attributeNamesMap.containsKey(UIMA_STREAMS_COVERED_TEXT_ATTRIBUTE.toLowerCase())) {
				if (com.ibm.streams.operator.Type.MetaType.RSTRING == listTupleType.getAttribute(UIMA_STREAMS_COVERED_TEXT_ATTRIBUTE).getType().getMetaType()) {
					AnnotationFS annot = (AnnotationFS) aFS;
					tupleMap.put(UIMA_STREAMS_COVERED_TEXT_ATTRIBUTE, new RString(annot.getCoveredText()));
				}
				else {
					trace.error("Expected type is rstring for attribute ["+ listTupleType.getAttribute(UIMA_STREAMS_COVERED_TEXT_ATTRIBUTE).getType() + " " + UIMA_STREAMS_COVERED_TEXT_ATTRIBUTE + "]");
				}
			}
			// check if type attribute needs to be filled
			if (attributeNamesMap.containsKey(UIMA_STREAMS_COVERED_TYPE_DESC_ATTRIBUTE.toLowerCase())) {
				if (com.ibm.streams.operator.Type.MetaType.RSTRING == listTupleType.getAttribute(UIMA_STREAMS_COVERED_TYPE_DESC_ATTRIBUTE).getType().getMetaType()) {
					tupleMap.put(UIMA_STREAMS_COVERED_TYPE_DESC_ATTRIBUTE, new RString(aFS.getType().getName()));
				}
				else {
					trace.error("Expected type is rstring for attribute ["+ listTupleType.getAttribute(UIMA_STREAMS_COVERED_TYPE_DESC_ATTRIBUTE).getType() + " " + UIMA_STREAMS_COVERED_TYPE_DESC_ATTRIBUTE + "]");
				}
			}
		}

		// iterate over all features
		List aFeatures = aFS.getType().getFeatures();
		Iterator iter = aFeatures.iterator();
		while (iter.hasNext()) {
			Feature feat = (Feature) iter.next();

			// feature value (how we get this depends on feature's range type)
			String rangeTypeName = feat.getRange().getName();

			trace.debug("featureShortName=" + feat.getShortName() + " type="+rangeTypeName);

			String attrName = ""; // if not empty and attribute type matches, put the value to the tupleMap
			// check the feature name
			String featName = feat.getShortName().toLowerCase();
			if (attributeNamesMap.containsKey(featName)) { // compare the lower case name
				attrName = attributeNamesMap.get(featName); // retrieve the case sensitive name
			}
			else if (splReserved.contains(featName)) {
				if (attributeNamesMap.containsKey(quoteReserved + featName)) {
					attrName = attributeNamesMap.get(quoteReserved + featName); // retrieve the case sensitive name
				}
			}

			if (aCAS.getTypeSystem().subsumes(stringType, feat.getRange())) {
				// STRING VALUE
				if ("" != attrName) {
					String str = aFS.getStringValue(feat);
					if (com.ibm.streams.operator.Type.MetaType.RSTRING == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						if (str == null) {
							tupleMap.put(attrName, new RString(""));
						} else {
							tupleMap.put(attrName, new RString(str));
						}
					}
					else if (com.ibm.streams.operator.Type.MetaType.USTRING == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						if (str == null) {
							tupleMap.put(attrName, new String(""));
						} else {
							tupleMap.put(attrName, str);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is STRING");
					}
				}
				else if (feat.getName().equals(CAS.FEATURE_FULL_NAME_SOFAID)) {
					sofaID.append(aFS.getStringValue(feat));
				}
			} else if (CAS.TYPE_NAME_INTEGER.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.INT32 == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getIntValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_INTEGER);
					}
				}
			} else if (CAS.TYPE_NAME_BOOLEAN.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.BOOLEAN == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getBooleanValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_BOOLEAN);
					}
				}
			} else if (CAS.TYPE_NAME_FLOAT.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.FLOAT32 == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getFloatValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_FLOAT);
					}
				}
			} else if (CAS.TYPE_NAME_BYTE.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.INT8 == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getByteValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_BYTE);
					}
				}
			} else if (CAS.TYPE_NAME_SHORT.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.INT16 == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getShortValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_SHORT);
					}
				}
			} else if (CAS.TYPE_NAME_LONG.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.INT64 == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getLongValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_LONG);
					}
				}
			} else if (CAS.TYPE_NAME_DOUBLE.equals(rangeTypeName)) {
				if ("" != attrName) {				
					if (com.ibm.streams.operator.Type.MetaType.FLOAT64 == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						tupleMap.put(attrName, aFS.getDoubleValue(feat));
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_DOUBLE);
					}
				}
			} else if (CAS.TYPE_NAME_STRING_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.RSTRING == streamsListType.getElementType().getMetaType()) {
							StringArrayFS arrayFS = (StringArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								List<RString> rstringList = new ArrayList<RString>(0); // null
								tupleMap.put(attrName, rstringList);
							} else {
								String[] vals = arrayFS.toArray();
								List<RString> rstringList = new ArrayList<RString>(vals.length);
								for (String s : vals) {
									rstringList.add(new RString(s));
								}
								tupleMap.put(attrName, rstringList);
							}
						}
						else if (com.ibm.streams.operator.Type.MetaType.USTRING == streamsListType.getElementType().getMetaType()) {
							StringArrayFS arrayFS = (StringArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								// null
								String[] vals = new String[0];
								tupleMap.put(attrName, vals);
							} else {
								String[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_STRING_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_STRING_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_INTEGER_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.INT32 == streamsListType.getElementType().getMetaType()) {
							IntArrayFS arrayFS = (IntArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new int[0]); // null
							} else {
								int[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_INTEGER_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_INTEGER_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_FLOAT_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.FLOAT32 == streamsListType.getElementType().getMetaType()) {
							FloatArrayFS arrayFS = (FloatArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new float[0]); // null
							} else {
								float[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_FLOAT_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_FLOAT_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_BOOLEAN_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.BOOLEAN == streamsListType.getElementType().getMetaType()) {
							BooleanArrayFS arrayFS = (BooleanArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new boolean[0]); // null
							} else {
								boolean[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_BOOLEAN_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_BOOLEAN_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_BYTE_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.INT8 == streamsListType.getElementType().getMetaType()) {
							ByteArrayFS arrayFS = (ByteArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new byte[0]); // null
							} else {
								byte[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_BYTE_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_BYTE_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_SHORT_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.INT16 == streamsListType.getElementType().getMetaType()) {
							ShortArrayFS arrayFS = (ShortArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new short[0]); // null
							} else {
								short[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_SHORT_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_SHORT_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_LONG_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.INT64 == streamsListType.getElementType().getMetaType()) {
							LongArrayFS arrayFS = (LongArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new long[0]); // null
							} else {
								long[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_LONG_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_LONG_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_DOUBLE_ARRAY.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.FLOAT64 == streamsListType.getElementType().getMetaType()) {
							DoubleArrayFS arrayFS = (DoubleArrayFS) aFS.getFeatureValue(feat);
							if (arrayFS == null) {
								tupleMap.put(attrName, new double[0]); // null
							} else {
								double[] vals = arrayFS.toArray();
								tupleMap.put(attrName, vals);
							}
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_DOUBLE_ARRAY);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_DOUBLE_ARRAY);
					}
				}
			} else if (CAS.TYPE_NAME_EMPTY_FS_LIST.equals(rangeTypeName)) {
				// nothing to do here
			} else if ((CAS.TYPE_NAME_NON_EMPTY_INTEGER_LIST.equals(rangeTypeName)) || (CAS.TYPE_NAME_INTEGER_LIST.equals(rangeTypeName))) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.INT32 == streamsListType.getElementType().getMetaType()) {
							FeatureStructure val = aFS.getFeatureValue(feat);
							int[] vals = (int[]) integerListToArray (val);
							tupleMap.put(attrName, vals);
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_NON_EMPTY_INTEGER_LIST);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_NON_EMPTY_INTEGER_LIST);
					}
				}
			} else if (CAS.TYPE_NAME_EMPTY_INTEGER_LIST.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.INT32 == streamsListType.getElementType().getMetaType()) {
							// nothing to do here
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_EMPTY_INTEGER_LIST);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_EMPTY_INTEGER_LIST);
					}
				}
			} else if ((CAS.TYPE_NAME_NON_EMPTY_FLOAT_LIST.equals(rangeTypeName)) || (CAS.TYPE_NAME_FLOAT_LIST.equals(rangeTypeName))) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.FLOAT32 == streamsListType.getElementType().getMetaType()) {
							FeatureStructure val = aFS.getFeatureValue(feat);
							float[] vals = (float[]) floatListToArray (val);
							tupleMap.put(attrName, vals);
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_NON_EMPTY_FLOAT_LIST);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_NON_EMPTY_FLOAT_LIST);
					}
				}
			} else if (CAS.TYPE_NAME_EMPTY_FLOAT_LIST.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.FLOAT32 == streamsListType.getElementType().getMetaType()) {
							// nothing to do here
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_EMPTY_FLOAT_LIST);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_EMPTY_FLOAT_LIST);
					}
				}
			} else if ((CAS.TYPE_NAME_NON_EMPTY_STRING_LIST.equals(rangeTypeName)) || (CAS.TYPE_NAME_STRING_LIST.equals(rangeTypeName))) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.RSTRING == streamsListType.getElementType().getMetaType()) {
							FeatureStructure val = aFS.getFeatureValue(feat);
							String[] vals = (String[]) stringListToArray (val);
							List<RString> rstringList = new ArrayList<RString>(vals.length);
							for (String s : vals) {
								rstringList.add(new RString(s));
							}
							tupleMap.put(attrName, rstringList);
						}
						else if (com.ibm.streams.operator.Type.MetaType.USTRING == streamsListType.getElementType().getMetaType()) {
							FeatureStructure val = aFS.getFeatureValue(feat);
							String[] vals = (String[]) stringListToArray (val);
							tupleMap.put(attrName, vals);
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_NON_EMPTY_STRING_LIST);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_NON_EMPTY_STRING_LIST);
					}
				}
			} else if (CAS.TYPE_NAME_EMPTY_STRING_LIST.equals(rangeTypeName)) {
				if ("" != attrName) {
					if (com.ibm.streams.operator.Type.MetaType.LIST == listTupleType.getAttribute(attrName).getType().getMetaType()) {
						CollectionType streamsListType = (CollectionType) listTupleType.getAttribute(attrName).getType();
						if (com.ibm.streams.operator.Type.MetaType.RSTRING == streamsListType.getElementType().getMetaType()) {
							List<RString> rstringList = new ArrayList<RString>(0);
							tupleMap.put(attrName, rstringList);
						}
						else if (com.ibm.streams.operator.Type.MetaType.USTRING == streamsListType.getElementType().getMetaType()) {
							String[] vals = new String[0];
							tupleMap.put(attrName, vals);
						}
						else {
							trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_EMPTY_STRING_LIST);
						}
					}
					else {
						trace.error("Type mismatch for attribute ["+ listTupleType.getAttribute(attrName).getType() + " " + attrName + "], feature " + feat.getShortName() + " type is " + CAS.TYPE_NAME_EMPTY_STRING_LIST);
					}
				}
			// ignore CAS.TYPE_NAME_NON_EMPTY_FS_LIST and CAS.TYPE_NAME_FS_ARRAY types
			} else { // non-primitive type
				FeatureStructure val = aFS.getFeatureValue(feat);
				if (val == null) {
					// null
				} else {
					processFS(val, aCAS, listTupleType, attributeNamesMap, tupleMap, sofaID);
				}
			}
		}
	}
	
	private Object floatListToArray(FeatureStructure fs) {
		List<Float> list = new ArrayList<Float>();

		TypeSystem ts = fs.getCAS().getTypeSystem();
		Type emptyFSList = ts.getType("uima.cas.EmptyFloatList");
		Feature headFeature = ts.getFeatureByFullName("uima.cas.NonEmptyFloatList:head");
		Feature tailFeature = ts.getFeatureByFullName("uima.cas.NonEmptyFloatList:tail");
		Set<FeatureStructure> alreadySeen = new HashSet<FeatureStructure>();
		FeatureStructure nextFs;
		for (FeatureStructure currentFs = fs; currentFs.getType() != emptyFSList; currentFs = nextFs) {
			list.add(Float.valueOf(currentFs.getFloatValue(headFeature)));
			nextFs = currentFs.getFeatureValue(tailFeature);
			if (alreadySeen.contains(nextFs)) {
				return loopInList(list);
			}
			alreadySeen.add(nextFs);
		}
		float[] floatArray = new float[list.size()];
		for (int i = 0; i < floatArray.length; i++) {
			floatArray[i] = list.get(i).floatValue();
		}
		return floatArray;
	}

	private Object integerListToArray(FeatureStructure fs) {
		List<Integer> list = new ArrayList<Integer>();
		TypeSystem ts = fs.getCAS().getTypeSystem();
		Type emptyFSList = ts.getType("uima.cas.EmptyIntegerList");
		Feature headFeature = ts.getFeatureByFullName("uima.cas.NonEmptyIntegerList:head");
		Feature tailFeature = ts.getFeatureByFullName("uima.cas.NonEmptyIntegerList:tail");

		Set<FeatureStructure> alreadySeen = new HashSet<FeatureStructure>();
		FeatureStructure nextFs;
		for (FeatureStructure currentFs = fs; currentFs.getType() != emptyFSList; currentFs = nextFs) {
			list.add(Integer.valueOf(currentFs.getIntValue(headFeature)));
			nextFs = currentFs.getFeatureValue(tailFeature);
			if (alreadySeen.contains(nextFs)) {
				return loopInList(list);
			}
			alreadySeen.add(nextFs);
		}
		int[] intArray = new int[list.size()];
		for (int i = 0; i < intArray.length; i++) {
			intArray[i] = list.get(i).intValue();
		}
		return intArray;
	}

	private Object stringListToArray(FeatureStructure fs) {
		List<String> list = new ArrayList<String>();
		TypeSystem ts = fs.getCAS().getTypeSystem();
		Type emptyFSList = ts.getType("uima.cas.EmptyStringList");
		Feature headFeature = ts.getFeatureByFullName("uima.cas.NonEmptyStringList:head");
		Feature tailFeature = ts.getFeatureByFullName("uima.cas.NonEmptyStringList:tail");

		Set<FeatureStructure> alreadySeen = new HashSet<FeatureStructure>();
		FeatureStructure nextFs;
		for (FeatureStructure currentFs = fs; currentFs.getType() != emptyFSList; currentFs = nextFs) {
			list.add(currentFs.getStringValue(headFeature));
			nextFs = currentFs.getFeatureValue(tailFeature);
			if (alreadySeen.contains(nextFs)) {
				return loopInList(list);
			}
			alreadySeen.add(nextFs);
		}
		return list.toArray(new String[list.size()]);
	}

	private Object loopInList(List<?> list) {
		Object[] array = new Object[list.size() + 1];
		for (int i = 0; i < list.size(); i++) {
			Object v = list.get(i);
			array[i] = (v instanceof Integer) ? ((Integer) v).toString()
					: (v instanceof Float) ? ((Float) v).toString() : list.get(i);
		}
		array[list.size()] = "... loop in list";
		return array;
	}

}
