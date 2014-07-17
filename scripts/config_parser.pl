#! /usr/bin/perl

use strict;
use YAML::Tiny;
use File::Basename;

#The script needs two arguments : the path leading to the configuration file
#and the path leading to the directory that contains the makefile.
if (scalar @ARGV !=2){
    my$name=basename($0);
    print STDERR "Incorrect number of arguments\n";
    print STDERR "Usage : $name <config_file> <makefile_dir>\n";
    exit 1;
}


my$configfile=shift;
my $dirname=shift;

#Removing the final '/' of the makefile directory's path
$dirname=$1 if ($dirname=~/^(.+)\/$/);

#Create YAML parser
my $yaml=YAML::Tiny->new;

#Reading the configuration file
$yaml=YAML::Tiny->read($configfile);

#Retrieving the data structure corresponding to the configuration file
$yaml=$yaml->[0];

#Converting that data structure into something that looks like 
#'export NAME="VALUE";'.
my $res = "";
foreach my $key (keys $yaml){
    if (defined $yaml->{$key}){
        if(ref($yaml->{$key}) eq 'ARRAY'){
	    my @array=@{$yaml->{$key}};
	    if($key eq "BEFOREBUILD" || $key eq "AFTERBUILD"){
		my $commands="";
		foreach my $cmd (@array){
		    $commands=$commands . "$cmd; ";
		}
		$commands=$1 if ($commands=~/^(.+); $/);
		$res = $res . "export $key=\"$commands\";\n";
	    }elsif ($key eq "BUILDDEPENDS" || $key eq "BINRUNDEPENDS" ||
		    $key eq "LIBRUNDEPENDS" || $key eq "INDRUNDEPENDS"){
		my $dependencies="";
		foreach my $dep (@array){
		    $dependencies=$dependencies . "$dep, ";
		}
		$dependencies=$1 if ($dependencies=~/^(.+), $/);
		$res = $res . "export $key=\"$dependencies\";\n";
	    }elsif($key eq "DEVS"){
		my $devs = "";
		foreach my $dev (@array){
		    $devs = $devs . "$dev; ";
		}
		$devs=$1 if ($devs =~ /^(.+;) $/);
		$res = $res . "export $key=\"$devs\"\n";
	    }else{
		my $elems="";
		foreach my $elem (@array) {
		    $elems=$elems . "$elem ";
		}
		$elems=$1 if ($elems=~/^(.+) $/);
		$res = $res . "export $key=\"$elems\";\n";
	    }
	}else{
	    if($key eq "BUILDDEPENDS" || $key eq "BINRUNDEPENDS" ||
	       $key eq "LIBRUNDEPENDS" || $key eq "INDRUNDEPENDS"){
		my $dependencies = $yaml->{$key};
		$dependencies=~s/ (?=[^\d\(\s])/, /g;
		$res = $res . "export $key=\"$dependencies\";\n";
	    }else{
		$res = $res. "export $key=\"$yaml->{$key}\";\n";
	    }
	}
    }else{
	$res = $res . "export $key=\"\";\n";
    }
}

#Checking if PACKAGETYPE isn't empty
$res =~ /PACKAGETYPE=\"(.*?)\";/;
exit 2 if ($1 eq "");

#Checking if PACKAGETYPE contain valid types
my@types=split / /, $1;
foreach my$t (@types) {
     exit 3 if ($t ne "s" && $t ne "l" && $t ne "i");
}

#Checking if BINPACKAGENAME should contain a value
$res=~/BINPACKAGENAME=\"(.*?)\";/;
exit 4 if ($1 eq "" && ($types[0] eq "s" || $types[1] eq "s" 
			|| $types[2] eq "s"));
#Checking if LIBPACKAGENAME should contain a value
$res=~/LIBPACKAGENAME=\"(.*?)\";/;
exit 5 if ($1 eq "" && ($types[0] eq "l" || $types[1] eq "l"
 			|| $types[2] eq "l"));
#Checking if INDPACKAGENAME should contain a value
$res=~/INDPACKAGENAME=\"(.*?)\";/;
exit 6 if ($1 eq "" && ($types[0] eq "i" || $types[1] eq "i"
 			|| $types[2] eq "i"));

#Retreiving the values of COPYRIGHT and DEVS variables
$res =~ /DEVS=\"(.*?)\";/;
my$devs=$1;

$res =~ /COPYRIGHT=\"(.*?)\";/;
my$cop=$1;

#Checking if user wants a generated copyright file
my$isvalue;
if (($cop eq "gpl") || ($cop eq "gpl2") || ($cop eq "gpl3") || ($cop eq "lgpl")
    || ($cop eq "lgpl2") || ($cop eq "lgpl3") || ($cop eq "artistic") 
    || ($cop eq "apache") || ($cop eq "bsd") || ($cop eq "mit")){
    $isvalue=1
}

#If the user doesn't want a generated copyright file, we check if COPYRGIHT 
#contains a valid path to a file. If COPYRIGHT is empty, we check if there's a
#file named COPYING or LICENSE.
if ($isvalue!=1) {
    exit 7 if ($cop ne "" && (! (-f "$dirname/$cop")));
    if ($cop eq "") {
	if (-f "$dirname/COPYING") {
	    $res =~ s/(COPYRIGHT=\")(\";)/$1COPYING$2/;
	}
	elsif (-f "$dirname/LICENSE") {
	    $res =~ s/(COPYRIGHT=\")(\";)/$1LICENSE$2/;
	}
    }
}

#If the user wants a generated copyright file, then we have to check if
#the field DEVS is empty. If it is, then we check if there's a file named AUTHORS
if ($isvalue==1) {
    if ($devs eq "") {
	if (-f "$dirname/AUTHORS") {
	    open AUTHORS, "$dirname/AUTHORS";
	    my$authors = <AUTHORS>;
	    $authors =~ s/\n/;/sg;
	    $res =~ s/(DEVS=\")(\";)/$1$authors$2/;
	}
	else {
	    exit 8;
	}
    }
}

#Checking if BINARYNAMES is empty
$res =~ /BINARYNAMES=\"(.*?)\";/;
my$bins=$1;
exit 9 if ($bins eq "" && ($types[0] eq "s" || $types[1] eq "s"
 			   || $types[2] eq "s"));

#Checking if LIBNAMES or HEADERNAMES is empty
$res =~ /LIBNAMES=\"(.*?)\";/;
my$libs=$1;
$res =~ /HEADERNAMES=\"(.*?)\";/;
my$headers=$1;
exit 10 if (($types[0] eq "l" || $types[1] eq "l" || $types[2] eq "l")
	    && $libs eq "" && $headers eq "");

# Checking if INDNAMES is empty
$res =~ /INDNAMES=\"(.*?)\";/;
my$inds=$1;
exit 11 if (($types[0] eq "i" || $types[1] eq "i" || $types[2] eq "i")
 	    && $inds eq "");

print $res;
