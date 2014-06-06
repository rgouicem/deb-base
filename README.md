deb-base
========

##Summary

Here's what you can find on this README :

1. [Quick introduction to Debian Packages](#introdeb)
2. [How to use the scripts to build packages](#use)
    1. [Requirements](#req)
    2. [The configuration file](#conf)
    3. [The Makefile](#usemakefile)
3. [How the scripts works](#howitworks)
    1. [build_deb.sh](#builddeb)
    2. [config_parser.pl](#configparser)
    3. [correct_lintian.pl](#correctlintian)

##<a name='introdeb'></a>1. Quick introduction to Debian Packages

As you might know, Debian packages are ***ar*** archives that may contain 
binary files, libraries or a lot of other things. The purpose of the files that
 can be found here is to create packages that will contain binaries (we will 
call them binary packages), and others that will  contain libraries and 
header files (we will call them library packages). Library packages are divided
 into two categories :

1. The first category concerns packages that only contain library files. Those
 packages are named something like : `<package_name>_<archi>.deb`.
2. The second category consists of all the packages that only contain header 
files. They are named like that : `<package_name>-dev_<archi>.deb`.

Usually, a package that contains libraries won't have headers in it (and vice
versa). When you install a package, its content is copied in your directories 
(/usr/bin for binaries, /usr/lib for libraries, ...).


##<a name='use'></a> 2. How to use the scripts to build packages

First, you have to make sure that you meet the requirements before continuing.

###<a name='req'></a>Requirements

In order for the scripts to work, you need to have some software installed.
Here is the list of packages you must install :

-    dh-make
-    devscripts
-    build-essential
-    debhelper
-    perl
-    make
-    sed
-    docco (In order to create the documentation. Click [here](#installdocco)
if you want to know how to install docco).

(or you can use the following command line : `sudo apt-get install dh-make
devscripts build-essential debhelper perl make sed`).

Your project also have to respect some rules. It must have a makefile that build
 your program. That makefile must have at least two targets : clean and install.
The target install is used by `debuild` (the tool that will create the
package) in order to build your program. The script `build_deb.sh` will produce
an error if it cannot find your makefile.

If you meet the requirements, you may proceed to the next step.

###<a name='conf'></a>The configuration file

Before you can create your packages, you have to fill the configuration file
named [build_config.yml](./scripts/build_config.yml). Keep in mind that this 
file must be in the same directory as the scripts `build_deb.sh` and 
`config_parser.pl`. Here's a quick explanation of the meaning of all the values
 you can find in the configuration file :

-    DIRECTORYNAME : It's the directory where your makefile is (which is usually
the source code directory). It MUST be filled, and if it doesn't lead to an
existing directory, the script will exit without building the packages you want.
Keep in mind that all the paths we require are relative to this directory.
-    BINPACKAGENAME : It's the name you want your binary package to have. It'll
look something like that : `<BINPACKAGENAME>_<VERSION>_<archi>.deb`. This can 
only be empty if you don't want to build a binary package. If this field is
empty and you've indicated that you want to build a binary package, the script
`build_deb.sh` will produce an error. 
-    LIBPACKAGENAME : It's the name you want to give to your library package. 
Two packages are actually produced, if you have indicated that you want to 
build a library package. The first one is name according to this pattern : 
`<LIBPACKAGENAME>_<VERSION>_<archi>.deb`. This is the package that will contain
the libraries. The second one is named according to the following pattern :
`<LIBPACKAGENAME>-dev_<VERSION>_<archi>.deb`. This is the package where you'll
find the header files you wanted to include 
