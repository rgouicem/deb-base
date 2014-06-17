deb-base
========

##Summary

Here's what you can find on this README :

1. [Quick introduction to Debian Packages](#introdeb)
2. [How to use the scripts to build packages](#use)
    1. [Requirements](#req)
    2. [The configuration file](#conf)
    3. [The Makefile](#makefile)
3. [Appendix](#appendix)
    1. [How to install docco](#installdocco)

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
an error if it cannot find your makefile. Also, you cannot suppose anything 
about environment variables; this script may edit some of them (like CFLAGS).
To prevent any inconsistency, do not use `-e` option in make and always redefine 
the environment variables you are using to proper values.

If you meet the requirements, you may proceed to the next step.

###<a name='conf'></a>The configuration file

Before you can create your packages, you have to fill the configuration file
named [build_config.yml](./scripts/build_config.yml). Keep in mind that this 
file must be in the same directory as the scripts `build_deb.sh` and 
`config_parser.pl`. Here's a quick explanation of the meaning of all the values
 you can find in the configuration file :

-    **DIRECTORYNAME** : It's the directory where your makefile is (which is 
usually the source code directory). It MUST be filled, and if it doesn't lead 
to an existing directory, the script will exit without building the packages 
you want. Keep in mind that all the paths we require are relative to this 
directory.
-    **BINPACKAGENAME** : It's the name you want your binary package to have. 
It'll look something like that : `<BINPACKAGENAME>_<VERSION>_<archi>.deb`. This
can only be empty if you don't want to build a binary package. If this field is
empty and you've indicated that you want to build a binary package, the script
`build_deb.sh` will produce an error. 
-    **LIBPACKAGENAME** : It's the name you want to give to your library 
package. Two packages are actually produced, if you have indicated that you want
to build a library package. The first one is name according to this pattern : 
`<LIBPACKAGENAME>_<VERSION>_<archi>.deb`. This is the package that will contain
the libraries. The second one is named according to the following pattern :
`<LIBPACKAGENAME>-dev_<VERSION>_<archi>.deb`. This is the package where you'll
find the header files you wanted to include 
-    **VERSION** : The current version of your program. It must not be empty.
-    **COPYRIGHT** : This field must be one of the following :
    -    `gpl`, `gpl2`, `gpl3`, `lgpl`, `lgpl2`, `lgpl3`, `artistic`,
`apache`, `bsd` or `mit` if you want to have a generated copyright file. In
this case, unless there's a file `AUTHORS` in **DIRECTORYNAME**, you have to put
the names of all the devellopers in **DEVS**.
    -	 A path to your own copyright file. If the path is wrong, the script 
produce an error. You can leave this field empty if your copyright file is named
`COPYING` or `LICENSE`
    -    Or you can just leave it empty. In that case, if there's a file named
`COPYING` or `LICENSE` in **DIRECTORYNAME**, the script will consider that this
is the copyright file. If there isn't any, the copyright file will be empty.
-    **DEVS** : The only case where this field must not be empty is when you
don't have a file `AUTHORS`, and you want to have a generated coypright file
(i.e. **COPYRIGHT** contains one of the following values : `gpl`, `gpl2`, 
`gpl3`, `lgpl`, `lgpl2`, `lgpl3`, `artistic`, `apache`, `bsd` or `mit`).
-    **PACKAGETYPE** : This field allow you to indicate what type of packages
you want to build. If you want to build a binary package, you have to put a 's'
in it. If you want to build library packages, you have to put an 'l' in it. 
You have to separate each type with a space. If you put anything other than 'l'
or/and 's', the script will not proceed. If you put an 's', and the field 
**BINPACKAGENAME** is not filled, the script will also produce an error. It 
goes the same way with 'l' and **LIBPACKAGENAME**.
-    **BUILDDEPENDS** : List of packages that must be installed in order to 
build your program. You must put all of them, even the ones that might seem 
obvious to you.
-    **PACKAGEDEPENDS** : List of packages that must be installed in order to 
run your program correctly.
-    **BINARYNAMES** : Path of all the binaries you want to include in the 
binary package. The path is relative to the directory **DIRECTORYNAME**.
You can leave this field empty if you have specified that you don't want to 
build a binary package. Otherwise, the script will produce an error.
-    **LIBNAMES** : Path of all the libraries you want to include in your
library package. Paths are relatives to **DIRECTORYNAME**.
-    **HEADERNAMES** : Path of all the headers you want to include in your
library package. Paths are relative to **DIRECTORYNAME**.
-    **BINPACKAGEDESCFILE** : Path of the description file for the binary
package. It should be in the following format : The first line must be shorter
than 60 chars, and it must the a short description of the package. The other 
lines represents a long description of your package. This field can be empty.
-    **LIBPACKAGEDESCFILE** : Path of the description file for the library 
packages. This file must be in the following format : The first line is a
short description (up to 60 chars) for the package that will contain the 
libraries. Then you can put a long description for the same package. After those
 you have to put a blank line. After that blank line, you have to write a line
(up to 60 chars) that will be the description for the package that will contain
the header files. Following that, you can write the long description of that
package. 
-    **DEBFULLNAME** : Name of the package maintainer.
-    **DEBEMAIL** : Email of the package maintainer.
-    **HOMEPAGE** : Homepage of your project.
-    **LOGFILE** : The name of the logfile, that will contain a log of what the
script did.

###<a name='makefile'></a> The Makefile

In this repository, there's a makefile which will help you build packages. In
order for this makefile to work, you have to fill three fields :
-    **SCRIPTS_DIR** : Directory were the scripts are (path are relative to
were the makefile is)
-    **DOCS_DIR** : Directory where you want the documentation to be.
-    **RESOURCE_DIR** : Directory where the resources are (i.e. where the files
[docco.css](./resources/docco.css) and 
[languages.json](./resources/languages.json) are located).

Once you've filled everything, you can build your packages. The makefile
consists of three targets :
-    **build_packages** : Allows you to build packages.
-    **build_docs** : Create documentation ([docco](#installdocco) must be 
installed).
-    **clean_docs** : Remove documentation.

##<a name='appendix'*></a>Appendix

###<a name='installdocco'></a>How to install docco

In order to install docco, you need Node.js installed. You shoudn't try
to install it via apt-get, because it doesn't work. You have to download it 
[here](http://nodejs.org/download/) and copy the file where they should be 
copied. Once you've done that, you just have to type the following command line 
: `sudo npm install -g docco`.
