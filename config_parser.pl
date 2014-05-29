#! /usr/bin/perl

#print "# perl script begins\n";

$in=shift;
$/=undef;

open IN, $in;
$config = <IN>;

# remove comments
$config =~ s/#.*\n//g;
# rearrange lists
$config =~ s/\n\s*-/ /g;
$config =~ s/\s*- / /g;
# remove empty lines
$config =~ s/\n\n/\n/g;
# substitute : by = and put quotes
$config =~ s/\s*:[^\S\n]*/="/g;
# end end of line quotes
$config =~ s/([^\s])\n/$1"\n/g;
# add export and ;
$config =~ s/([^\s][^\n]+=[^\n]+)\n/export $1;\n/g;
# fix URLs
$config =~ s#="//#://#g;

print $config;

close IN;

