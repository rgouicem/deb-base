#! /usr/bin/perl

use strict;

#print "# perl script begins\n";

my$in=shift;
$/=undef;

open IN, $in;
my$config = <IN>;

# remove comments
$config =~ s/#.*\n//g;
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

#Adding comas in BUILDEPEND and PACKAGEDEPENDS
$config =~ /BUILDDEPENDS=\"(.*?)\";/;
my$res=$1;
$res=~s/ (?=[^\d\(\s])/, /g, $1;
$config =~ s/BUILDDEPENDS=\"(.*?)\";/BUILDDEPENDS=\"$res\";/;

$config =~ /PACKAGEDEPENDS=\"(.*?)\";/;
$res=$1;
$res=~s/ (?=[^\d\(\s])/, /g, $1;
$config =~ s/PACKAGEDEPENDS=\"(.*?)\";/PACKAGEDEPENDS=\"$res\";/;

#Now we're going to check if DIRECTORYNAME contains a valid directory
$config =~ /DIRECTORYNAME=\"(.*?)\";/;
my$dirname=$1;
exit 1 if (!(-d $dirname));

#Checking version
$config =~ /VERSION=\"(.*?)\";/;
exit 4 if ($1 eq "");

#Checking package type
$config =~ /PACKAGETYPE=\"(.*?)\";/;
exit 2 if ($1 eq "");
my@types=split / /, $1;
exit 3 if ($types[0] ne "s" && $types[0] ne "l");
exit 3 if (($types[1] ne "") && ($types[1] ne "s") && ($types[1] ne "l"));

#Checking BINPACKAGENAME and LIBPACKAGENAME
$config=~/BINPACKAGENAME=\"(.*?)\";/;
exit 5 if ($1 eq "" && ($types[0] eq "s" || $types[1] eq "s"));

$config=~/LIBPACKAGENAME=\"(.*?)\";/;
exit 6 if ($1 eq "" && ($types[0] eq "l" || $types[1] eq "l"));

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
exit 9 if ($bins eq "" && ($types[0] eq "s" || $types[1] eq "s"));

#Checking if LIBNAMES or HEADERNAMES is empty
$config =~ /LIBNAMES=\"(.*?)\";/;
my$libs=$1;
$config =~ /HEADERNAMES=\"(.*?)\";/;
my$headers=$1;
exit 10 if (($types[0] eq "l" || $types[1] eq "l") && $libs eq "" 
	    && $headers eq "");
print $config;
close IN;
