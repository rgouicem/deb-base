#! /usr/bin/perl

use strict;
use File::Basename;
if (scalar @ARGV !=2){
    my$name=basename($0);
    print STDERR "Incorrect number of arguments\n";
    print STDERR "Usage : $name <config_file> <makefile_dir>\n";
    exit 1;
}

my$in=shift;
$/=undef;
my $dirname=shift;
$dirname=$1 if ($dirname=~/^(.+)\/$/);

open IN, $in;
my$config = <IN>;

# remove comments
$config =~ s/#.*\n//g;
# rearrange BEFOREBUILD and AFTERBUILD lists
$config =~ /BEFOREBUILD\s*?:\s*?(.*?)\n\n/s;
my$res = $1;
$res =~ s/([^\s])\n/$1;\n/sg;
$config =~ s/(BEFOREBUILD\s*?:\s*?)(.*?)(\n\n)/$1$res$3/s;
$config =~ /AFTERBUILD\s*?:\s*?(.*?)\n\n/s;
my$res = $1;
$res =~ s/([^\s])\n/$1;\n/sg;
$config =~ s/(AFTERBUILD\s*?:\s*?)(.*?)(\n\n)/$1$res$3/s;
# rearrange lists
$config =~ s/\n\s*-//g;
$config =~ s/\s*- / /g;
# remove empty lines
$config =~ s/\n\n/\n/g;
# substitute : by = and put quotes
$config =~ s/\s*:[^\S\n]*/="/g;
# add export, end of line quotes and ;
$config =~ s/([^\s][^\n]+=[^\n]+)\n/export $1";\n/g;
# fix URLs
$config =~ s#="//#://#g;

#Remove useless spaces
$config =~ s/\n{2,}/\n/g;
$config =~ s/^\n(.+)$/$1/sg;

#Adding comas in BUILDEPENDS, BINRUNDEPENDS, LIBRUNDEPENDS, INDRUNDEPENDS
$config =~ /BUILDDEPENDS=\"(.*?)\";/;
$res=$1;
$res=~s/ (?=[^\d\(\s])/, /g, $1;
$config =~ s/BUILDDEPENDS=\"(.*?)\";/BUILDDEPENDS=\"$res\";/;

$config =~ /BINRUNDEPENDS=\"(.*?)\";/;
$res=$1;
$res=~s/ (?=[^\d\(\s])/, /g, $1;
$config =~ s/BINRUNDEPENDS=\"(.*?)\";/BINRUNDEPENDS=\"$res\";/;

$config =~ /LIBRUNDEPENDS=\"(.*?)\";/;
$res=$1;
$res=~s/ (?=[^\d\(\s])/, /g, $1;
$config =~ s/LIBRUNDEPENDS=\"(.*?)\";/LIBRUNDEPENDS=\"$res\";/;

$config =~ /INDRUNDEPENDS=\"(.*?)\";/;
$res=$1;
$res=~s/ (?=[^\d\(\s])/, /g, $1;
$config =~ s/INDRUNDEPENDS=\"(.*?)\";/INDRUNDEPENDS=\"$res\";/;

#Checking version
$config =~ /VERSION=\"(.*?)\";/;
exit 4 if ($1 eq "");

#Checking package type
$config =~ /PACKAGETYPE=\"(.*?)\";/;
exit 2 if ($1 eq "");
my@types=split / /, $1;
#exit 3 if ($types[0] ne "s" && $types[0] ne "l");
#exit 3 if (($types[1] ne "") && ($types[1] ne "s") && ($types[1] ne "l"));
foreach my$t (@types) {
    exit 3 if ($t ne "s" && $t ne "l" && $t ne "i");
}

#Checking BINPACKAGENAME, LIBPACKAGENAME and INDPACKAGENAME
$config=~/BINPACKAGENAME=\"(.*?)\";/;
exit 5 if ($1 eq "" && ($types[0] eq "s" || $types[1] eq "s" 
			|| $types[2] eq "s"));

$config=~/LIBPACKAGENAME=\"(.*?)\";/;
exit 6 if ($1 eq "" && ($types[0] eq "l" || $types[1] eq "l"
			|| $types[2] eq "l"));

$config=~/INDPACKAGENAME=\"(.*?)\";/;
exit 11 if ($1 eq "" && ($types[0] eq "i" || $types[1] eq "i"
			|| $types[2] eq "i"));

#Checking COPYRIGHT and DEVS variables
$config =~ /DEVS=\"(.*?)\";/;
my$devs=$1;

$config =~ /COPYRIGHT=\"(.*?)\";/;
my$cop=$1;

#Checking if user wants a generated copyright
my$isvalue;
if (($cop eq "gpl") || ($cop eq "gpl2") || ($cop eq "gpl3") || ($cop eq "lgpl")
    || ($cop eq "lgpl2") || ($cop eq "lgpl3") || ($cop eq "artistic") 
    || ($cop eq "apache") || ($cop eq "bsd") || ($cop eq "mit")){
    $isvalue=1
}
#if not, checking file if given, or standard files COPYING and LICENSE
if ($isvalue!=1) {
    exit 7 if ($cop ne "" && (! (-f "$dirname/$cop")));
    if ($cop eq "") {
	if (-f "$dirname/COPYING") {
	    $config =~ s/(COPYRIGHT=\")(\";)/$1COPYING$2/;
	}
	elsif (-f "$dirname/LICENSE") {
	    $config =~ s/(COPYRIGHT=\")(\";)/$1LICENSE$2/;
	}
    }
}
#if generated copyright, check for devs to edit copyright file
if ($isvalue==1) {
    if ($devs eq "") {
	if (-f "$dirname/AUTHORS") {
	    open AUTHORS, "$dirname/AUTHORS";
	    my$authors = <AUTHORS>;
	    $authors =~ s/\n/;/sg;
	    $config =~ s/(DEVS=\")(\";)/$1$authors$2/;
	}
	else {
	    exit 8;
	}
    }
    else {
	$config =~ /DEVS=(.+)/;
	my$subst = $1;
	$subst =~ s/>/>;/g;
	$config =~ s/(DEVS=).+/$1$subst/;
    }
}

#Checking if BINARYNAMES is empty
$config =~ /BINARYNAMES=\"(.*?)\";/;
my$bins=$1;
exit 9 if ($bins eq "" && ($types[0] eq "s" || $types[1] eq "s"
			   || $types[2] eq "s"));

#Checking if LIBNAMES or HEADERNAMES is empty
$config =~ /LIBNAMES=\"(.*?)\";/;
my$libs=$1;
$config =~ /HEADERNAMES=\"(.*?)\";/;
my$headers=$1;
exit 10 if (($types[0] eq "l" || $types[1] eq "l" || $types[2] eq "l")
	    && $libs eq "" && $headers eq "");

#Checking if INDNAMES is empty
$config =~ /INDNAMES=\"(.*?)\";/;
my$inds=$1;
exit 12 if (($types[0] eq "i" || $types[1] eq "i" || $types[2] eq "i")
	    && $inds eq "");

print $config;
close IN;
