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
package Debug;

use strict;
use Data::Dumper;

#------------------------------------------------
# Constructor
#------------------------------------------------
#------------------------------------------------
sub new
#------------------------------------------------
{
	my ($class, $level, %args) = @_;
	$level = 0 unless (defined $level);
	my $self =
	{
		level => $level,
		stack => [],
		standalone => 0,
	};
	bless($self, $class);
	foreach my $arg (sort keys %args)
	{
		if ($arg eq "standalone")
		{
			$self->{standalone} = 1;
		}
	}
	return $self;
}

#------------------------------------------------
sub push($$)
#------------------------------------------------
{
	my $self = shift;
	my $method = shift;
	push @{$self->{stack}}, $method;
}

#------------------------------------------------
sub pop($$)
#------------------------------------------------
{
	my $self = shift;
	pop @{$self->{stack}};
}

#------------------------------------------------
sub method($)
#------------------------------------------------
{
	my $self = shift;
	return $self->{stack}->[-1];
}

#------------------------------------------------
sub enter
#------------------------------------------------
{
	my $self = shift;
	if ($self->{level})
	{
		my $text = sprintf
		(
			"--> %s%s",
			$self->method(),
			(scalar @_ > 0 ? "(" . join(", ", @_) . ")" : ""),
		);
		if ($self->{standalone})
		{
			print $text, "\n";
		}
		else
		{
			SPL::CodeGen::println($text);
		}
	}
}

#------------------------------------------------
sub leave
#------------------------------------------------
{
	my $self = shift;
	if ($self->{level})
	{
		my $text = sprintf
		(
			"<-- %s%s",
			$self->method(),
			(scalar @_ > 0 ? ": " . join(", ", @_) : ""),
		);
		if ($self->{standalone})
		{
			print $text, "\n";
		}
		else
		{
			SPL::CodeGen::println($text);
		}
	}
}

#------------------------------------------------
sub info
#------------------------------------------------
{
	my $self = shift;
	if ($self->{level})
	{
		my $text = sprintf
		(
			"    %s%s",
			$self->method(),
			(scalar @_ > 0 ? ": " . join(", ", @_) : ""),
		);
		if ($self->{standalone})
		{
			print $text, "\n";
		}
		else
		{
			SPL::CodeGen::println($text);
		}
	}
}

#------------------------------------------------
sub error
#------------------------------------------------
{
	my $self = shift;
#	if ($self->{level})
	{
		my $text = sprintf
		(
			"ERR %s%s",
			$self->method(),
			(scalar @_ > 0 ? ": " . join(", ", @_) : ""),
		);
		if ($self->{standalone})
		{
			print $text, "\n";
		}
		else
		{
			SPL::CodeGen::errorln($text);
		}
	}
}

#------------------------------------------------
sub warning
#------------------------------------------------
{
	my $self = shift;
#	if ($self->{level})
	{
		my $text = sprintf
		(
			"WRN %s%s",
			$self->method(),
			(scalar @_ > 0 ? ": " . join(", ", @_) : ""),
		);
		if ($self->{standalone})
		{
			print $text, "\n";
		}
		else
		{
			SPL::CodeGen::warnln($text);
		}
	}
}

#------------------------------------------------
sub enable
#------------------------------------------------
{
	my $self = shift;
	my $level = (scalar @_ > 0 ? shift : 1);
	$self->{level} = $level;
}

#------------------------------------------------
sub disable
#------------------------------------------------
{
	my $self = shift;
	$self->{level} = 0;
}

#------------------------------------------------
sub level
#------------------------------------------------
{
	my $self = shift;
	return $self->{level};
}

#------------------------------------------------
sub standalone
#------------------------------------------------
{
	my $self = shift;
	$self->{standalone} = 1;
#	print STDERR "standalone enabled\n";
}

1;
