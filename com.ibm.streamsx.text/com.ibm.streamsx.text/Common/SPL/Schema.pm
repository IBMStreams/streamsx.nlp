# begin_generated_IBM_copyright_prolog                            
#                                                                 
# This is an automatically generated copyright prolog.            
# After initializing,  DO NOT MODIFY OR MOVE                      
# ****************************************************************
# Licensed Materials - Property of IBM                            
# 5724-Y95                                                        
# (C) Copyright IBM Corp.  2011, 2016    All Rights Reserved.     
# US Government Users Restricted Rights - Use, duplication or     
# disclosure restricted by GSA ADP Schedule Contract with         
# IBM Corp.                                                       
#                                                                 
# end_generated_IBM_copyright_prolog                              
# -----------------------------------------------------------------------------
# The SPL Extension Package
# -----------------------------------------------------------------------------
# Functions, which are required to evaluate SPL type definitions easier.
# -----------------------------------------------------------------------------

package SPL::Schema;

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

use strict;
use Data::Dumper;
use XML::Simple;
use Debug;

# -----------------------------------------------------------------------------
# The constructor creates the class. It evaluates optional parameters.
# @param model specifies the operator model
# @param debug specifies the debug level
# -----------------------------------------------------------------------------
sub new
{
	my ($class, %parameters) = @_;
	# Create the class with default values.
	my $self =
	{
		# The tracer object.
		_tracer => new Debug(0),
		# The operator model
		_model => undef,
	};
	bless($self, $class);
	# Evaluate parameters.
	foreach my $key (keys %parameters)
	{
		if ($key eq "debug")
		{
			$self->{_tracer}->enable($parameters{$key});
		}
		elsif ($key eq "model")
		{
			$self->{_model} = $parameters{$key};
		}
		else
		{
			SPL::CodeGen::exitln("internal error in function '%s': invalid parameter '%s=%s'", (caller(0))[3], $key, $parameters{$key});
		}
	}
	# Trace the configuration.
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->info("model=" . (defined $self->{_string} ? "defined" : "undef"), "debug=" . $self->{_tracer}->level());
	$self->{_tracer}->pop();
	return $self;
}

# -----------------------------------------------------------------------------
# Get the port's schema and build a XML tree, which contains all names and type
# information. It is important to keep the order, therefore, ForceArray is used
# -----------------------------------------------------------------------------
sub _getPortSchemaAsTree($$)
{
	my ($self, $port, $rootName) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("port=" . (defined $port ? $port->getIndex() : "undef"), "rootName=" . (defined $rootName ? $rootName : "undef"));
	my $schema = $port->getSchema($rootName);
	my $xml = new XML::Simple;
	my $result = $xml->XMLin($schema, ForceArray => [ 'tt:attr' ], KeyAttr => []);
	$self->{_tracer}->leave();
	$self->{_tracer}->pop();
	return $result;
}

# -----------------------------------------------------------------------------
# Create a string from an XML tree.
# -----------------------------------------------------------------------------
sub _buildSchemaString($$)
{
	my ($self, $tree, $rootName) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("rootName=" . (defined $rootName ? $rootName : "undef"));
	my $xml = new XML::Simple;
	my $result = $xml->XMLout($tree, RootName => $rootName, NoIndent => 1);
	$self->{_tracer}->leave("result=$result");
	$self->{_tracer}->pop();
	return $result;
}

# -----------------------------------------------------------------------------
# Read the type definition's header file.
# -----------------------------------------------------------------------------
sub _readHeaderFile($)
{
	my ($self, $node) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("node=" . $node->{name});

	# Get the output directory.
	my $outputDirectory = $self->{_model}->getContext()->getOutputDirectory();

	# Cut the C++ type and get the name without namespace.
	my $cppType = $node->{cppType};
	$cppType =~ s/^.+:://;

	# Build the filename.
	my $file = $outputDirectory . "/src/type/" . $cppType . ".h";
	$self->{_tracer}->info("file=$file");

	# Read the file content, type definitions only.
	open FILE, "$file" or die $! . ", file $file";
	my @content = grep { m/ typedef .*_type;/ } <FILE>;
	chomp @content;
	close FILE;
#	$self->{_tracer}->info("content={'" . join("', '", @content) . "'}");

	# Build hash from type definitions.
	# Samples
	# 1) typedef SPL::rstring inner_test_type;
	#            ^^^^^^^^^^^^ ^^^^^^^^^^
	# 2) typedef SPL::BeJwrMcw0NM3NLy6Jz8zLSy2KL0ktLgEATZYHDS inner_outer_test_type;
	#            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^
	my %attributesAndTypes;
	foreach (@content)
	{
		if (m/^\s*typedef\s+(.+)\s+(\w+)_type;\s*$/)
		{
#			$self->{_tracer}->info("typedef: '$2' --> '$1'");
			$attributesAndTypes{$2} = $1;
		}
	}
	$self->{_tracer}->leave("result={" . join(", ", map { $attributesAndTypes{$_} . " " . $_ } sort keys %attributesAndTypes) . "}");
	$self->{_tracer}->pop();
	return \%attributesAndTypes;
}

# -----------------------------------------------------------------------------
# Iterate through the first level of the schema. The C++ type information is
# available for each attribute. Only the C++ type information for attributes
# below sub containers is not available and requires - today - parsing of the
# generated C++ header files.
# -----------------------------------------------------------------------------
sub _processSchemaTree($$$)
{
	my ($self, $port, $tree) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("port=" . (defined $port ? $port->getIndex() : "undef"));
	# Provide a tree, which is identical for the root and the sub elements.
	# The root begins with 'attr' instead of 'tuple'. Therefore, insert the
	# 'tuple' level. Provide also a cppType for the root element.
	$tree = { name => "*root*", cppType => $port->getCppTupleType(), "tt:tuple" => $tree };
	foreach my $node (@{$tree->{"tt:tuple"}->{"tt:attr"}})
	{
		my $attr = $port->getAttributeByName($node->{name});
		$node->{cppType} = $attr->getCppType();
		$self->{_tracer}->info("name=" . $node->{name}, "cppType=" . $node->{cppType});
		# Check for containers
		$self->_processContainer($node);
	}
	$self->{_tracer}->leave();
	$self->{_tracer}->pop();
	return $tree;
}

# -----------------------------------------------------------------------------
# Check whether the node is a container type. If it is, process the container.
# -----------------------------------------------------------------------------
sub _processContainer($)
{
	my ($self, $node) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("name=" . $node->{name}, "cppType=" . $node->{cppType}, "type=" . (exists $node->{type} ? $node->{type} : "container-type"));

	my $result = "container";
	# Process containers.
	if (exists $node->{"tt:tuple"})
	{
		$self->_processSchemaTuple($node);
	}
	elsif (exists $node->{"tt:list"})
	{
		$self->_processSchemaListOrSet($node, "list");
	}
	elsif (exists $node->{"tt:set"})
	{
		$self->_processSchemaListOrSet($node, "set");
	}
	elsif (exists $node->{"tt:map"})
	{
		$self->_processSchemaMap($node);
	}
	else
	{
		$result = "primitive";
	}

	$self->{_tracer}->leave("result=$result");
	$self->{_tracer}->pop();
}

# -----------------------------------------------------------------------------
# Process a tuple type i.e. read the type's C++ header file and process the
# tuple type's attributes.
# -----------------------------------------------------------------------------
sub _processSchemaTuple($)
{
	my ($self, $node) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("name=" . $node->{name}, "cppType=" . $node->{cppType});

	# Assertions.
	my $expected = "tuple";
	die sprintf("%s: internal error: passed node is not a $expected definition (%s)", (caller(0))[3], join(", ", map { $_ . "=" . $node->{$_} } sort keys %{$node})) unless exists $node->{"tt:$expected"};

	my $attributesAndTypes = $self->_readHeaderFile($node);

	# Add types to node's children (do it recursive).
	foreach my $child (@{$node->{"tt:$expected"}->{"tt:attr"}})
	{
		my $attr = $attributesAndTypes->{$child->{name}};
		$child->{cppType} = $attr;
		$self->{_tracer}->info("name=" . $child->{name}, "cppType=" . $child->{cppType});
		# Check for containers
		$self->_processContainer($child);
	}

	$self->{_tracer}->leave();
	$self->{_tracer}->pop();
}

# -----------------------------------------------------------------------------
# Process a list or set type i.e. extract the element type before continuing
# with the sub types.,
# -----------------------------------------------------------------------------
sub _processSchemaListOrSet($$)
{
	my ($self, $node, $expected) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("name=" . $node->{name}, "cppType=" . $node->{cppType}, "expected=$expected");

	# Assertions.
	die sprintf("%s: internal error: passed node is not a $expected definition (%s)", (caller(0))[3], join(", ", map { $_ . "=" . $node->{$_} } sort keys %{$node})) unless exists $node->{"tt:$expected"};
	die sprintf("%s: internal error: cannot extract element type from $expected's C++ type '%s'", (caller(0))[3], $node->{cppType}) if ($node->{cppType} !~ m/^SPL::$expected<(.+?)\s*>$/);
	$node->{"tt:$expected"}->{cppType} = $1;
	$self->_processContainer($node->{"tt:$expected"});

	$self->{_tracer}->leave();
	$self->{_tracer}->pop();
}

# -----------------------------------------------------------------------------
# Process a map type i.e. extract the key and value types before continuing
# with the sub types.
# -----------------------------------------------------------------------------
sub _processSchemaMap($$)
{
	my ($self, $node) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("name=" . $node->{name}, "cppType=" . $node->{cppType});

	# Assertions.
	my $expected = "map";
	die sprintf("%s: internal error: passed node is not a $expected definition (%s)", (caller(0))[3], join(", ", map { $_ . "=" . $node->{$_} } sort keys %{$node})) unless exists $node->{"tt:$expected"};
	die sprintf("%s: internal error: cannot extract key and value types from $expected's C++ type '%s'", (caller(0))[3], $node->{cppType}) if ($node->{cppType} !~ m/^SPL::$expected<(.+?)\s*,\s*(.+?)\s*>$/);

	# In case of primitive key/value type, there are a keyType/valueType key
	# only. In case of container types, there is a tt:key/tt:value key.
	# In both cases, add the cppType below the tt:XXX, but only in case the
	# tt:XXX key existed before, process the container. In case of primitive
	# types, add also the primitive type to the created tt:XXX.
	my $primitiveKey = (exists $node->{"tt:$expected"}->{keyType});
	my $primitiveValue = (exists $node->{"tt:$expected"}->{valueType});
	$node->{"tt:$expected"}->{"tt:key"}->{cppType} = $1;
	$node->{"tt:$expected"}->{"tt:value"}->{cppType} = $2;
	$node->{"tt:$expected"}->{"tt:key"}->{type} = $node->{"tt:$expected"}->{keyType} if $primitiveKey;
	$node->{"tt:$expected"}->{"tt:value"}->{type} = $node->{"tt:$expected"}->{valueType} if $primitiveValue;
	$self->_processContainer($node->{"tt:$expected"}->{"tt:key"}) unless $primitiveKey;
	$self->_processContainer($node->{"tt:$expected"}->{"tt:value"}) unless $primitiveValue;

	$self->{_tracer}->leave();
	$self->{_tracer}->pop();
}

# -----------------------------------------------------------------------------
# Get the port's schema and build a XML tree, which contains all names and type
# information. It is important to keep the order, therefore, ForceArray is used
# -----------------------------------------------------------------------------
sub getSchema($$)
{
	my ($self, $port, $rootName) = @_;
	$self->{_tracer}->push((caller(0))[3]);
	$self->{_tracer}->enter("port=" . (defined $port ? $port->getIndex() : "undef"), "rootName=" . (defined $rootName ? $rootName : "undef"));

	# Assertions.
	die sprintf("%s: internal error: no port", (caller(0))[3]) unless defined $port;
	die sprintf("%s: internal error: no root", (caller(0))[3]) unless defined $rootName;

	# Create a tree from the port's schema (getSchema()).
	my $tree = $self->_getPortSchemaAsTree($port, $rootName);

	# Add the C++ type information.
	$tree = $self->_processSchemaTree($port, $tree);

	# Create a string to be compatible to the port's getSchema().
	my $result = $self->_buildSchemaString($tree, $rootName);

	$self->{_tracer}->leave("result=$result");
	$self->{_tracer}->pop();
	return $result;
}

1;
