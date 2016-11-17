#!/usr/bin/perl

# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.                             


# extract metrics from the output of streamtool capturestate

use strict;
use warnings;
use SimpleDom;
use Data::Dumper;

my $STREAMS_INSTALL=$ENV{STREAMS_INSTALL};
$ENV{STREAMS_INSTALL} or die "Internal Error: STREAMS_INSTALL not set. Please source streamsprofile.sh";

my $STREAMS_BIN=$STREAMS_INSTALL."/bin";
my $STREAMTOOL_CMD=$STREAMS_BIN."/streamtool";

my $xmlFile = "/tmp/st.$$";

sub captureStats() {
        my $res;
	system($STREAMTOOL_CMD." capturestate --jobnames LemmatizerMemTest --select jobs=metrics -f $xmlFile");

	# load xml file
	my $metrics = new SimpleDom($xmlFile);
	unlink $xmlFile;

	# collect metrics from pes/operators
	my $pes = $metrics->getElementsByPath("/instance/job/pe");
	foreach my $pe (@$pes)
	{
		my ($peId,$processId) = $pe->getAttributeValues("id","processId");
		my $cpu = $pe->getElementsByPath('./metric[@name=nCpuMilliseconds]/metricValue')->[0]->getAttribute("value");
		my $mem = $pe->getElementsByPath('./metric[@name=nMemoryConsumption]/metricValue')->[0]->getAttribute("value");
		print "pe:$peId pid:$processId cpu:$cpu mem:$mem \n";
		$res = $mem;
	}
	return $res;
}

sub dumpToFile($$) {
	my ($file, $data) = @_;
	open FH, ">$file" or die ("ERR_FILE_OPEN_ERROR",$file,$!);
	print FH $data;
	close(FH);
}

# submit the job
system($STREAMTOOL_CMD." submitjob output/LemmatizerTest.sab --jobname LemmatizerMemTest");

sleep(10);

my $memoryConsumption;
my $filename="mem0";
my $i = 0;
for($i = 0; $i < 15; $i++) {
	$memoryConsumption = captureStats();
	dumpToFile($filename,$memoryConsumption) if (2 == $i);
	sleep(10);
}

$filename="mem1";
$memoryConsumption = captureStats();
dumpToFile($filename,$memoryConsumption);

# cancel the job
system($STREAMTOOL_CMD." canceljob --jobnames LemmatizerMemTest");






