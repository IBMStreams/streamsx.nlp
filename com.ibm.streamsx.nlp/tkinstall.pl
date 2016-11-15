#!/usr/bin/perl
# Copyright (C)2016, International Business Machines Corporation
# All rights reserved. 

use strict;
use Cwd qw(abs_path cwd);
use Env qw(USER);
use File::Basename;

my $STREAMS_INSTALL=$ENV{STREAMS_INSTALL};
$ENV{STREAMS_INSTALL} or die "Error: STREAMS_INSTALL not set. Please source streamsprofile.sh";

my $ST=$STREAMS_INSTALL."/bin/streamtool";

my $dirname = dirname(abs_path($0));

sub getStreamsVersion()
{
	return substr(`$ST version | grep Version`, 8, 3,);
}

#print getStreamsVersion();

my $replaceText = "SPL::Functions::Utility::getToolkitDirectory(\"com.ibm.streamsx.nlp\")";
$replaceText = "\"$dirname\"" if (getStreamsVersion() < 4.2);

my $templateFile=$dirname."/impl/include/libtextutils/UtilsFunctions.template";
my $headerFile=$dirname."/impl/include/libtextutils/UtilsFunctions.h";
open(FILE, "<$templateFile") || die "File not found";
my @lines = <FILE>;
close(FILE);

my @newlines;
foreach(@lines) {
   $_ =~ s/<TK_DIR>/$replaceText/g;
   push(@newlines,$_);
}

open(FILE, ">$headerFile") || die "File not found";
print FILE @newlines;
close(FILE);
