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
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.uima.UIMAFramework;
import org.apache.uima.analysis_engine.AnalysisEngine;
import org.apache.uima.cas.CAS;
import org.apache.uima.resource.ResourceManager;
import org.apache.uima.resource.ResourceSpecifier;
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
 * Class for the UimaText operator that receives a tuple with document, processes the UIMA AnalysisEngine of the deployed PEAR file. The output stream tuple either contains the serialized UIMA CAS (rstring) or a custom attribute defined with the outputAttributes parameter.
 */
@PrimitiveOperator(name="UimaText", namespace="com.ibm.streamsx.nlp",
description="The Java Operator UimaText uses a Apache UIMA Analysis Engine to annotate incoming tuple with text of type rstring. It creates a UIMA CAS from the text, processes the Analysis Engine, serializes the resulting CAS to .xmi and then submits a tuple of type rstring. The Analysis Engine, resources and CAS types are expected in a UIMA .pear file. The .pear file gets loaded on operator initialization and reloaded when a window punctuation is received on the second input port. The .pear file is installed in the data directory under 'installedPears<OPERATOR_NAME>'. If data directory is not set, then /tmp is used for installation. Please, find in toolkit dir ./doc/UIMA_sample.pdf a detailed sample description of UIMA .pear creation. If this operator is used in the Streaming Analytics service (IBM Cloud), then the data directory needs to be set to '/tmp'.")
@InputPorts({@InputPortSet(id="0", description="Port that ingests tuples", cardinality=1, optional=false, windowingMode=WindowMode.NonWindowed, windowPunctuationInputMode=WindowPunctuationInputMode.Oblivious), @InputPortSet(id="1", description="Optional control port", cardinality=1, optional=true, controlPort=true, windowingMode=WindowMode.NonWindowed, windowPunctuationInputMode=WindowPunctuationInputMode.Oblivious)})
@OutputPorts({@OutputPortSet(description="Port that produces tuples", cardinality=1, optional=false, windowPunctuationOutputMode=WindowPunctuationOutputMode.Preserving, windowPunctuationInputPort="0")})
public class UimaText extends AbstractUimaOperator {
	
	private static Logger trace = Logger.getLogger(UimaText.class.getName());
	
	private static final String PARAMETER_NAME_INPUT_DOC = "inputDoc";
	private static final String PARAMETER_NAME_TRIM_INPUT_DOC = "trimInputDoc";

	@Parameter(name=PARAMETER_NAME_INPUT_DOC, description="This optional parameter specifies the attribute of the input tuples that is passed to the Analytics Engine of UIMA. If there is only one attribute on the input tuple, this parameter is not required.", optional=true)
	public void setInputDoc(String inputDoc) {
		this.inputDoc = inputDoc;
	}

	@Parameter(name=PARAMETER_NAME_TRIM_INPUT_DOC, description="If this optional parameter is set to false, then trim function is not applied on the input document and leading whitespace characters are not removed. The default value of this parameter is set to true.", optional=true)
	public void setTrimInputDoc(boolean trimInputDoc) {
		this.trimInputDoc = trimInputDoc;
	}
	
	@ContextCheck(compile = true)
	public static void checkParameterAndSchema(OperatorContextChecker checker) {
		OperatorContext context = checker.getOperatorContext();
		if (!context.getParameterNames().contains(PARAMETER_NAME_INPUT_DOC)) {
			if (1 < context.getStreamingInputs().get(0).getStreamSchema().getAttributeCount()) {
				checker.setInvalidContext("The parameter "+PARAMETER_NAME_INPUT_DOC+" must be set if the input stream has more than one attribute.", null);
			}
			else {
				Attribute attr = context.getStreamingInputs().get(0).getStreamSchema().getAttribute(0);
				if (null != attr) {
					if ((attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.RSTRING) &&
						    (attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.USTRING)) {
							checker.setInvalidContext("The input stream attribute must be of type rstring or ustring.", null);
					}
				}
			}
		}
	}
	
	@ContextCheck(compile = false)
	public static void checkRuntimeParameterValues(OperatorContextChecker checker) {
		OperatorContext context = checker.getOperatorContext();
		if (context.getParameterNames().contains(PARAMETER_NAME_INPUT_DOC)) {
			String value = context.getParameterValues(PARAMETER_NAME_INPUT_DOC).get(0); // getParameterValues() is runtime only
			// Check if the name of the inputDoc parameter is a valid attribute on input port 0
			if (!context.getStreamingInputs().get(0).getStreamSchema().getAttributeNames().contains(value)) {
				checker.setInvalidContext("The parameter "+PARAMETER_NAME_INPUT_DOC+" has a value that does not match an attribute name of the input port 0: {0}", new Object[] {value});
			}
			// check the type of the input stream attribute, that is set with the inputDoc parameter
			Attribute attr = context.getStreamingInputs().get(0).getStreamSchema().getAttribute(value);
			if (null != attr) {
				if ((attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.RSTRING) &&
				    (attr.getType().getMetaType() != com.ibm.streams.operator.Type.MetaType.USTRING)) {
					checker.setInvalidContext("The parameter "+PARAMETER_NAME_INPUT_DOC+" must be of type rstring or ustring.", null);
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
		// Must call super.initialize(context) to correctly setup an operator.
		super.initialize(context);
		
		if (inputDoc == null) {
			StreamSchema inputSchema = context.getStreamingInputs().get(0).getStreamSchema();
			// when inputDoc isn't specified, the first attribute is the input document.
			Attribute attr = inputSchema.getAttribute(0);
			inputDoc = attr.getName();
		}

		prepareAnalysisEngine(); // creates ae and cas
	}

	/**
	 * Process an incoming tuple that arrived on the port 0.
	 * @param tuple Object representing the incoming tuple.
	 * @throws Exception Operator failure, will cause the enclosing PE to terminate.
	 */
	@Override
	protected void handleInputOnPort0(com.ibm.streams.operator.Tuple tuple) throws Exception{
		selectView(); // must be called per tuple since cas.reset() destroys the view

		String document = tuple.getString(inputDoc);
		if (trimInputDoc) {
			document = document.trim();
		}
		// put document text in CAS
		cas.setDocumentText(document);

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
			prepareAnalysisEngine(); // creates ae and cas
		} catch (Exception e) {
			trace.error("Error loading PEAR file: "+getPearFile().toString(),e);
			fallbackPearFile(); // load initial PEAR file
			trace.error("Fallback to initial PEAR file: "+getPearFile().toString());
			installPearFile();
			prepareAnalysisEngine(); // creates ae and cas
		}
	}
}
