<xsl:stylesheet version='1.0' 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:fn="http://www.w3.org/2005/xpath-functions"
	xmlns:xsd="http://www.w3.org/2001/XMLSchema"
	xmlns:res="http://uima.apache.org/resourceSpecifier"
    xmlns:str="http://exslt.org/strings"
    xmlns:exsl="http://exslt.org/common"
    extension-element-prefixes="str">
<!-- Copyright (C)2016, International Business Machines Corporation -->
<!-- All rights reserved.                                -->
	<xsl:output method="text" media-type="text/plain" indent="no"/>
	<xsl:strip-space elements="*"/>


<!-- load typeconversion data and SPL reserved words -->
<!-- located in the root of extracted pear file -->
<xsl:variable name="typeConversion" select="document('uima2spltypes.xml')" />
<xsl:variable name="splReservedWords" select="document('splReserved.xml')" />

<!-- install.xml -->

<!-- following templates are for processing the install.xml file    -->
<!-- This is the xml file for which the xsl is called.              -->
<!-- This file holds the path to the engine xml file from           -->
<!-- which we get the type information as well as other type files  -->
<!-- path information to be included                                -->
<!-- we expect to be called everytime with $pear_root_dir/metadata/instal.xml -->
<!--no namespace is used there so we just use tag in XPATH -->
<xsl:template match="/COMPONENT_INSTALLATION_DESCRIPTOR">
	<xsl:apply-templates/>
</xsl:template> 



<!-- get the engine xml file and process it -->
<xsl:template match="SUBMITTED_COMPONENT">
	<xsl:variable 	name="v_installComponent" 
					select="DESC/text()" />
	<xsl:variable 	name="v_installComponentRelPath" 
					select="substring-after($v_installComponent,'$main_root/')" />
	<!-- just stuff for getting the path to the component as string-->
	<xsl:variable 	name="v_onlyPath">
		<xsl:apply-templates 
			mode="pathExtract" 
			select="exsl:node-set(str:tokenize($v_installComponentRelPath,'/'))"/>
	</xsl:variable>
	<!-- end getting path-->
// ------------------------------------------------------------
//	component file: <xsl:value-of select="$v_installComponentRelPath"/>
// ------------------------------------------------------------
	<xsl:apply-templates select="document($v_installComponentRelPath)" >
		<xsl:with-param name="p_includersRelPath" select="$v_onlyPath"/>
	</xsl:apply-templates>
	<xsl:text>

// ------------------------------------------------------------
// basic type definitions
// ------------------------------------------------------------
	</xsl:text>
	<xsl:apply-templates select="document('uimaBasicTypes.xml')">
		<xsl:with-param name="p_includersRelPath" select="'./'"/>
	</xsl:apply-templates> 
</xsl:template> 



<!-- suppress all text() output for default templates not matching own templates -->
<xsl:template match="text()"/>



<!-- process the node-set created by tokenizing the path -->
<!-- exclude the last node (this is the original filename -->
<xsl:template mode="pathExtract" match="token">
	<xsl:if test="position() != last()">
		<xsl:value-of select="text()"/><xsl:text>/</xsl:text>
	</xsl:if>
</xsl:template>




<!-- any engine or type file -->

<!-- following templates are for processing <typeSystemDescription> in -->
<!-- any of those files containing typeSystem definitions               -->
<!-- the type definition files use a namespace     -->
<!-- so we need to use the namespace in XPATH also -->
<!-- handle <imports> and <types> separately       -->
<xsl:template match="res:typeSystemDescription">
	<xsl:param name="p_includersRelPath"/>
	<xsl:apply-templates select="res:types"/> 

	<!-- just use the relPath parameter for include -->
	<xsl:apply-templates select="res:imports">
		<xsl:with-param name="p_relPath" select="$p_includersRelPath"/>
	</xsl:apply-templates>
<!--	<xsl:apply-templates select="res:types"/> -->
</xsl:template>  


<!-- <imports/import> -->
<xsl:template match="res:import">
	<xsl:param name="p_relPath" select="'./'"/>
	<xsl:variable name="v_importFile">
		<!-- location may conatin relative path related to including xml -->
		<!-- but xslt needs relative path to xsl location which we       -->
		<!-- set to pear package root directory                         -->
		<xsl:choose>
			<xsl:when test="starts-with(@location,'$main_root')">
				<xsl:value-of select="substring-after($v_installComponent,'$main_root/')"/>
			</xsl:when>
			<xsl:otherwise>			
				<xsl:value-of select="concat($p_relPath,@location)"/>
			</xsl:otherwise>			
		</xsl:choose>
	</xsl:variable>
/******************************************************/
/*	import file: <xsl:value-of select="$v_importFile"/> */
/******************************************************/
	<xsl:variable 	name="v_onlyPath">
		<xsl:apply-templates 
			mode="pathExtract" 
			select="exsl:node-set(str:tokenize($v_importFile,'/'))"/>
	</xsl:variable>

	<!-- load the import file and apply all templates on it -->
	<!-- this is done recursively too -->
	<xsl:apply-templates select="document($v_importFile)">
		<xsl:with-param name="p_includersRelPath" select="$v_onlyPath"/>
	</xsl:apply-templates> 

</xsl:template>  

<!-- <types/type> -->
<xsl:template match="res:typeDescription">

	<!-- determine supertypeName -->
	<xsl:variable name="v_supertypeName" select="res:supertypeName/text()"/>
	<!-- get filtered featuires with SPL type resolution -->
	<xsl:variable name="v_features"> 
		<xsl:apply-templates mode="getFeatures" select="res:features"/>
	</xsl:variable>

type <xsl:value-of select="concat(translate(res:name/text(),'.','_'),'_type')"/> = 
	tuple &lt;
		<xsl:if test="$v_supertypeName">
			<xsl:value-of select="concat(translate($v_supertypeName,'.','_'),'_type')"/>
			<xsl:if test="exsl:node-set($v_features)/res:featureDescription">
				<xsl:text>,
		</xsl:text>
			</xsl:if>
		</xsl:if>

		<xsl:if test="exsl:node-set($v_features)/res:featureDescription">
		tuple &lt;
		<xsl:apply-templates select="exsl:node-set($v_features)"/>
	    &gt; 
		</xsl:if>
    &gt; ;
</xsl:template>  


<!-- <featureDescription> -->
<!-- for each feature check if the type is in spl conversion table -->
<!--  if not ignore it by not generating an attribute              -->
<xsl:template match="res:featureDescription">
	<xsl:variable name="v_name" select="res:name/text()"/>
	<xsl:variable name="v_type" select="res:rangeTypeName/text()"/>
	<xsl:variable name="v_spltype" select="$typeConversion/uimatypes/uimatype[@name = $v_type]"/>
	<!-- check if feature name is reserved wors and prefix it if so -->
	<xsl:variable name="v_checkedName">	
		<xsl:choose>
			<xsl:when test="$splReservedWords/splReserved/reserved[@name=$v_name]">
				<xsl:text>protectreserved_</xsl:text><xsl:value-of select="$v_name"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$v_name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<!-- ignore features which are not in SPL conversion xml -->
	<xsl:if test="$v_spltype">
		<xsl:value-of select="' '"/><xsl:value-of select="$v_spltype"/> <xsl:value-of select="' '"/> <xsl:value-of select="$v_checkedName"/> 
		<xsl:if test="position() != last()">
			<xsl:text>,
		</xsl:text>
		</xsl:if>
	</xsl:if>
</xsl:template>

<!-- FILTER ON featureDescription> -->
<!-- get cleaned featureDescription, no unsupported features anymore -->
<!-- for each feature check if the type is in spl conversion table -->
<!-- if not ignore don't copy to result set                        -->
<xsl:template mode="getFeatures" match="res:features/res:featureDescription">
	<xsl:variable name="v_name" select="res:name/text()"/>
	<xsl:variable name="v_type" select="res:rangeTypeName/text()"/>
	<xsl:variable name="v_spltype" select="$typeConversion/uimatypes/uimatype[@name = $v_type]"/>
	<xsl:if test="$v_spltype">
	    <xsl:copy-of select="."/>
	</xsl:if>
</xsl:template>

</xsl:stylesheet>
