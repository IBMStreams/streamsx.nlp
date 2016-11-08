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
# This Perl module provides a set of commonly used functions to support the
# code generation process of the different operators in this toolkit.
# -----------------------------------------------------------------------------

package CommonPerlFunctions;

use strict;
use warnings;

use Data::Dumper;
use XML::Simple;
use Debug;
use SPL::Schema;

# -----------------------------------------------------------------------------
# Get the port's schema and build a XML tree, which contains all names and type
# information.
# -----------------------------------------------------------------------------
sub getPortSchemaAsTree($$)
{
	my ($model, $port) = @_;
	my $spl = new SPL::Schema(model => $model); # , debug => 1);
	my $schema = $spl->getSchema($port, "schema");
	$schema =~ s/tt://g;
	my $xml = new XML::Simple;
	my $result = $xml->XMLin($schema, ForceArray => [ 'attr' ], KeyAttr => []);
#	print STDERR Dumper($result);
	return $result;
}

1;
