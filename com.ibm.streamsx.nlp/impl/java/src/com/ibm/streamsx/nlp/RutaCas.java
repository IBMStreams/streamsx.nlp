//
// *******************************************************************************
// * Copyright (C)2016, International Business Machines Corporation *
// * All rights reserved. *
// *******************************************************************************
//
package com.ibm.streamsx.nlp;

import java.io.File;
import java.io.IOException;
import java.io.StringWriter;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.uima.UIMAFramework;
import org.apache.uima.analysis_engine.AnalysisEngine;
import org.apache.uima.analysis_engine.AnalysisEngineDescription;
import org.apache.uima.cas.CAS;
import org.apache.uima.resource.ResourceManager;
import org.apache.uima.resource.ResourceSpecifier;
import org.apache.uima.resource.metadata.TypeSystemDescription;
import org.apache.uima.resource.metadata.impl.TypeSystemDescription_impl;
import org.apache.uima.ruta.engine.Ruta;
import org.apache.uima.ruta.engine.RutaEngine;
import org.apache.uima.util.CasCreationUtils;
import org.apache.uima.util.XMLInputSource;
import org.apache.uima.cas.impl.XmiCasSerializer;
import org.apache.uima.pear.tools.PackageBrowser;
import org.apache.uima.pear.tools.PackageInstaller;
import org.apache.uima.pear.tools.PackageInstallerException;
import org.apache.uima.util.XMLSerializer;

import org.apache.log4j.Logger;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.logging.LoggerNames;
import com.ibm.streams.operator.logging.TraceLevel;
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

/**
 * Class for the RutaCas operator that receives a tuple with serialized UIMA CAS (rstring), applies the RUTA script to the CAS and then submits a tuple containing the serialized UIMA CAS (rstring).
 */
@PrimitiveOperator(name="RutaCas", namespace="com.ibm.streamsx.nlp",
description="The Java Operator RutaCas uses the Apache UIMA Ruta rules to annotate incoming tuple with serialized UIMA CAS xmi of type rstring. It deserializes the incoming .xmi to an internal UIMA CAS representation, applies the Ruta script to the CAS, serializes the resulting CAS to .xmi and then submits a tuple of type rstring. The RUTA rules, resources and CAS types are expected in a UIMA .pear file. The CAS types in the .pear need to contain the CAS types of the incoming tuple. The .pear file gets loaded on operator initialization and are reloaded when a window punctuation is received on the second input port. The .pear file is installed in the data directory under 'installedPears<OPERATOR_NAME>'. If data directory is not set, then /tmp is used for installation. Please, find in toolkit dir ./doc/UIMA_workbench.pdf a detailed sample description of Ruta .pear creation. If this operator is used in the Streaming Analytics service (IBM Cloud), then the data directory needs to be set to '/tmp'.")
@InputPorts({@InputPortSet(id="0", description="Port that ingests tuples", cardinality=1, optional=false, windowingMode=WindowMode.NonWindowed, windowPunctuationInputMode=WindowPunctuationInputMode.Oblivious), @InputPortSet(id="1", description="Optional control port", cardinality=1, optional=true, controlPort=true, windowingMode=WindowMode.NonWindowed, windowPunctuationInputMode=WindowPunctuationInputMode.Oblivious)})
@OutputPorts({@OutputPortSet(description="Port that produces tuples", cardinality=1, optional=false, windowPunctuationOutputMode=WindowPunctuationOutputMode.Preserving, windowPunctuationInputPort="0")})
@Libraries({"impl/lib/apache-uima/uima-core.jar","impl/lib/apache-uima/ruta-core-2.4.0.jar","impl/lib/apache-uima/uimafit-core-2.1.0.jar","impl/lib/apache-uima/antlr-runtime-3.5.2.jar","impl/lib/apache-uima/spring-asm-3.1.2.RELEASE.jar","impl/lib/apache-uima/spring-beans-3.1.2.RELEASE.jar","impl/lib/apache-uima/spring-context-3.1.2.RELEASE.jar","impl/lib/apache-uima/spring-core-3.1.2.RELEASE.jar","impl/lib/apache-uima/spring-expression-3.1.2.RELEASE.jar","impl/lib/apache-uima/commons-lang-2.6.jar","impl/lib/apache-uima/commons-logging-1.1.1.jar","impl/lib/apache-uima/commons-lang3-3.1.jar","impl/lib/apache-uima/commons-collections-3.2.1.jar","impl/lib/apache-uima/commons-io-2.4.jar","impl/lib/apache-uima/commons-math3-3.0.jar"})
public class RutaCas extends AbstractUimaOperator {
	
	private static Logger trace = Logger.getLogger(RutaCas.class.getName());
	
	private static final String PARAMETER_NAME_CAS_IN = "casIn";
	private static final String PARAMETER_NAME_DEBUG_MODE = "debugMode";
	private static final String PARAMETER_NAME_REMOVE_BASICS = "removeBasics";
	
	/**
	 * Configuration parameters of the UIMA Ruta Analysis Engine 
	 * Option to remove all inference annotations after execution of the rule script.
	 */
	private boolean removeBasics = false;

	@Parameter(name=PARAMETER_NAME_CAS_IN, description="This optional parameter specifies the attribute of the input tuples that contains the UIMA CAS as serialized XMI string. If there is only one attribute on the input tuple, this parameter is not required.", optional=true)
	public void setInputDoc(String casIn) {
		this.casIn = casIn;
	}
	
	@Parameter(name=PARAMETER_NAME_DEBUG_MODE, description="If this parameter is set to true, then additional information about the execution of a rule script is added to the CAS. The default value of this parameter is set to false.", optional=true)
	public void setDebugMode(boolean debugMode) {
		this.debugMode = debugMode;
	}

	@Parameter(name=PARAMETER_NAME_REMOVE_BASICS, description="If this parameter is set to true, then all inference annotations are removed and the CAS xmi output does not contain these basic annotations. The default value of this parameter is set to false. This parameter needs to be set only, if the parameter casOut is set.", optional=true)
	public void setRemoveBasics(boolean removeBasics) {
		this.removeBasics = removeBasics;
	}

	@ContextCheck(compile = true)
	public static void checkParameterAndSchema(OperatorContextChecker checker) {
		OperatorContext context = checker.getOperatorContext();
		if (!context.getParameterNames().contains(PARAMETER_NAME_CAS_IN)) {
			if (1 < context.getStreamingInputs().get(0).getStreamSchema().getAttributeCount()) {
				checker.setInvalidContext("The parameter "+PARAMETER_NAME_CAS_IN+" must be set if the input stream has more than one attribute.", null);
			}
			else {
				Attribute attr = context.getStreamingInputs().get(0).getStreamSchema().getAttribute(0);
				if (null != attr) {
					if (attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.RSTRING) {
						checker.setInvalidContext("The input stream attribute must be of type rstring.", null);
					}
				}
			}
		}
	}
	
	@ContextCheck(compile = false)
	public static void checkRuntimeParameterValues(OperatorContextChecker checker) {
		OperatorContext context = checker.getOperatorContext();
		if (context.getParameterNames().contains(PARAMETER_NAME_CAS_IN)) {
			String value = context.getParameterValues(PARAMETER_NAME_CAS_IN).get(0); // getParameterValues() is runtime only
			// Check if the name of the casIn parameter is a valid attribute on input port 0
			if (!context.getStreamingInputs().get(0).getStreamSchema().getAttributeNames().contains(value)) {
				checker.setInvalidContext("The parameter "+PARAMETER_NAME_CAS_IN+" has a value that does not match an attribute name of the input port 0: {0}", new Object[] {value});
			}
			// check the type of the input stream attribute, that is set with the casIn parameter
			Attribute attr = context.getStreamingInputs().get(0).getStreamSchema().getAttribute(value);
			if (null != attr) {
				if (attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.RSTRING) {
					checker.setInvalidContext("The parameter "+PARAMETER_NAME_CAS_IN+" must be of type rstring.", null);
				}
			}
		}
	}
	
	/**
	 * Initialize this operator. Called once before any tuples are processed.
	 * @param context OperatorContext for this operator.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	public synchronized void initialize(OperatorContext context)
			throws Exception {
		doPearVerification = false; // disable PEAR file verification
		// Must call super.initialize(context) to correctly setup an operator.
		super.initialize(context);

		if (casIn == null) {
			StreamSchema inputSchema = context.getStreamingInputs().get(0).getStreamSchema();
			// when casIn isn't specified, the first attribute is the input CAS.
			Attribute attr = inputSchema.getAttribute(0);
			casIn = attr.getName();
		}

		prepareRutaAnalysisEngine(view, removeBasics); // creates ae and cas
	}

	/**
	 * Process an incoming tuple that arrived on the port 0.
	 * @param tuple Object representing the incoming tuple.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	protected void handleInputOnPort0(com.ibm.streams.operator.Tuple tuple) throws Exception{
		selectView(); // must be called per tuple since cas.reset() destroys the view

		// deserialize the XMI string
		createCasFromXmiString(tuple.getString(casIn));

		// Set the language code. Use the default unless we allow tuple-by-tuple language code use.
		if (languageCodeAttribute != null) {
			languageCode = tuple.getString(languageCodeAttribute);
		}
		cas.setDocumentLanguage(languageCode);

		// process
		ae.process(cas);

		createAndSubmitOutputTuple(tuple);

		// reset the CAS to prepare it for processing the next document
		cas.reset();
	}

	@Override
	public void processWindowPunctuationPort1() throws Exception {
		trace.info("reload PEAR file " + getPearFile().toString());
		destroyAnalysisEngine();
		uninstallPearFile();
		try {
			// load new PEAR file
			installPearFile();
			prepareRutaAnalysisEngine(view, removeBasics); // creates ae and cas
		} catch (Exception e) {
			trace.error("Error loading PEAR file: "+getPearFile().toString(),e);
			fallbackPearFile(); // load initial PEAR file
			trace.error("Fallback to initial PEAR file: "+getPearFile().toString());
			installPearFile();
			prepareRutaAnalysisEngine(view, removeBasics); // creates ae and cas
		}
	}
}
