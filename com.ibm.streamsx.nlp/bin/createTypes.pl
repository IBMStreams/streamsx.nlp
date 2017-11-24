#!/usr/bin/perl
# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.                           

use Getopt::Long;
use strict;
use Cwd qw(abs_path cwd);
use File::Basename;
use File::Spec;
use File::Temp;
use FindBin;
use lib $FindBin::Bin;

my $executeRC;

my $pid = $$;
my $tmpDir = "/tmp/createTypes".$pid;

my $STREAMS_INSTALL=$ENV{STREAMS_INSTALL};
$ENV{STREAMS_INSTALL} or die "Internal Error: STREAMS_INSTALL not set. Please source streamsprofile.sh";

my $JAR_BIN=$STREAMS_INSTALL."/java/bin/jar";


sub usage($;$) {
    my ($retCode,$message) = @_;

    if (defined $message) {
        print STDERR $message;
    }
    my $usageString=<<END;

Options:
		--pearFile <filename>					Specifies the PEAR file to be loaded.
		--outputfile <filename>         		Filename as the output file name.
		--namespace <namespace>         		SPL namespace of the generated SPL files (optional)
		--main <composite>						Name of the main composite to be generated (optional). File <composite>.spl will be created.
		--help                          		Print this message.
END
if ($retCode == 0) {
    print STDOUT $usageString;
    exit(0);
} else {
    print STDERR $usageString;
    exit(1);
}
}

sub executeAndCapture($) {
    my ($cmd) = (@_);
    my $rc = `$cmd`;
    $executeRC = $?;
    chomp $rc;
    return $rc;
}

# Internal method to run a system command and capture the output
sub _runCmdNoDieCapture($;$) { # logs starting and completion; returns exit code (processed $?)
    my ($cmd, $captureStdErr) = @_;

    $captureStdErr = 1 if !defined($captureStdErr);

    my $ec;
    my $output = $captureStdErr == 1 ? `$cmd 2>&1` : `$cmd`; # Redirect stderr to stdout so we can capture it
    if ($? < 0) { # cmd didn't run
        $ec = -1;
    } else {
        # return exit code... -1 if it died.
        my $low = $? & 0xff; # non-0 if died
        my $hi = $? >> 8; # exit code if exit() was called
        $ec = $low ? -1 : $hi;
    }

    return ($ec, $output);
}

sub _extractPearFile($$) {
    my ($pearFile, $xsltScriptDir) = @_;
    my $cmd = $JAR_BIN." xvf ".$pearFile;
    my ($rc, $output) = _runCmdNoDieCapture($cmd, 0);
    my $cmd = "cp ".$xsltScriptDir."/*.x* ".$tmpDir;
    my ($rc, $output) = _runCmdNoDieCapture($cmd, 0);

    return ($rc);
}

sub _createTypes($) {
    my ($outfilename) = @_;
    my $cmd = "xsltproc -o ".$outfilename." getSPLTypes.xsl metadata/install.xml";
    #print $cmd."\n";
    my ($rc, $output) = _runCmdNoDieCapture($cmd, 0);
    return ($rc);
}

sub  trim ($){ 
    my $str = shift; 
    $str =~ s/^\s+|\s+$//g; 
    return $str;
};

sub getFirstAnnoType($) {
    my ($file) = @_;
    open(TYPES, $file) or die("Could not open  file.");
    my $line;
    my $result = "uima_tcas_Annotation_type";
    foreach $line (<TYPES>)  {
        $line = trim($line);
        if ($line =~ /^type (.*)_type =/) {
            $result = $1."_type";
            last;
        }
    }
    close(TYPES);
    return $result;
}

sub generateComposite($$$) {

    my ($compositeName, $pearFile, $annoType) = @_;
    $pearFile = basename($pearFile);
    my $prolog=<<END;
composite $compositeName
{
	graph 

 		stream<rstring text> TextStream = FileSource() {
			param
				format : line;
				file : "input.txt";
		}

		stream<rstring text, list<$annoType> annotations> TextAnnotated = com.ibm.streamsx.nlp::UimaText(TextStream as I) {
			param
				pearFile: "$pearFile"; // relative to etc dir
				outputAttributes: "annotations"; // this attribute must be of type list<tuple>
		}

		() as SinkTextAnnotated = FileSink(TextAnnotated) {
 			param
				file:  "out.txt";
				format: txt;
				flushOnPunctuation: true;
		}
}
END
    return $prolog;
}

sub addNamespace($) {

    my ($namespace) = @_;
    my $prolog=<<END;
namespace $namespace;

END
    return $prolog;
}

sub main() {
    my $pearFile;
    my $outfilename;
    my $namespace;
    my $needHelp;
    my $outfilename="TypesGenerated.spl";
    my $createMain = 0;
    my $compositeName="MainGenerated";
    my $mainParam;
        
    GetOptions ("pearFile=s" => \$pearFile,
                "outputfile=s", \$outfilename,
                "namespace=s", \$namespace,
                "main=s",\$mainParam,
                "help|h|?",\$needHelp,
                ) or usage(1);
    
    if (($needHelp) || (!defined $pearFile)) {
        usage(0);
    }

    if (defined $mainParam) {
        $createMain = 1;
        $compositeName = $mainParam;
    }

    my $dirname = dirname(abs_path($0));
    #print $dirname."\n";
    my $xsltScriptDir = $dirname."/../com.ibm.streamsx.nlp/Common/xslt";

    $pearFile = abs_path($pearFile);

    # create temporary directory for the extraced PEAR file
    executeAndCapture("mkdir $tmpDir");
    my $pwd = cwd();
    chdir($tmpDir);
    # unpack the PEAR file
    _extractPearFile($pearFile, $xsltScriptDir);
    # create types.spl
    _createTypes($outfilename);
    # copy output to original working dir
    chdir($pwd);
    executeAndCapture("cp ".$tmpDir."/".$outfilename." .");
    #clean-up
    executeAndCapture("rm -rf ".$tmpDir);
    
    # generate the SPL Main composite
    if ($createMain) {
        my $annoType=getFirstAnnoType($outfilename);
        open (MAINFILE,">".$compositeName.".spl") or die "Could not create file ".$compositeName.".spl";
        if (defined $namespace) {
        	print MAINFILE addNamespace($namespace);
        }        
        print MAINFILE generateComposite($compositeName,$pearFile,$annoType);
        close MAINFILE;
    }
    
    if (defined $namespace) {
    	executeAndCapture("mkdir -p $namespace");
    	if ($createMain) {
			my $mainFile=$compositeName.".spl";
    	   	executeAndCapture("mv $mainFile $namespace");
    	}
		open (DATA, "<".$outfilename) || die "could not open $outfilename\n";
		my @body=<DATA>;
		close(DATA);
		# add namespace to top of the file
		open(TYPESFILE,">".$namespace."/".$outfilename);
		print TYPESFILE addNamespace($namespace);
		print TYPESFILE @body;
		close(TYPESFILE);    	
    	executeAndCapture("rm -f $outfilename");    	
    }
}

main();
    
