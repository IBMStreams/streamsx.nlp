# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.                             
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
