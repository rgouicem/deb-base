#How To Use This Script
#----------------------

#The purpose of this script is to create Debian packages. As you may know
#there are differents types of packages. This script allows you to create two
#differents type of packages. The first one is the binary package which contains
#binary files. The other one is the library package. When you want to create 
#a library package, two packages are actually created. Here is a quick
#description of those : 
#1. &lt;package\_name&gt;\_&lt;version&gt;.deb : This is the package containing
#the libraries.
#2. &lt;package\_name&gt;-dev\_&lt;version&gt; : This package will contain the
#header files.

#In order for this script to work correctly, some scripts and files are needed. 
#Thoses files can be found in the same repository where you've found this
#one. Here is a list of them :
#1. build_config.yml : This file contains all the informations this script
#need to build the Debian packages you want. <span class='gras'>You must
#read it, and fill it with correct informations.</span>
#2. config_parser.pl : This perl script parses the configuration file (see 
#above), and produces an output that look something like "export NAME=VALUE;".
#3. correct_lintian.pl : Perl script that correct some of the mistakes that
#lintian (command that check the conformity of packages) detects. Although this
#script is not necessary to produce a package, it's highly recommended
#to have it, in order to build a package that complies with the Debian Policy.
#4. Your project needs a makefile if you want to build a binary package. Debian
#tools need one to create binaries, in order to include them into the package.

#How this script works
#---------------------

#Here are the main steps of the script :
#1. <a href='#conf'>Reading the configuration file</a>
#2. <a href='#makefile'>Looking for the Makefile</a>
#3. <a href='#tarball'>Creating Tarballs</a>
#4. <a href='#copyright'>The copyright file</a>
#5. <a href='#lib'>Building the library packages</a>
#  1. <a href='#libdeb'>Creation of the Debian files</a>
#  2. <a href='#libmod'>Modification of the Debial files</a>
#  3. <a href='#libsource'>Creation of the source package</a>
#  4. <a href='#libbuild'> Building the package</a>
#6. <a href='#bin'>Building the binary package</a>
#  1. <a href='#bindeb'>Creation of the Debian files</a>
#  2. <a href='#binmod'>Modification of the Debial files</a>
#  3. <a href='#binsource'>Creation of the source package</a>
#  4. <a href='#binbuild'> Building the package</a>


#<a id='conf'>Reading configuration file.</a>
#--------------------------------------------

#In order for this script to run correctly, you need to complete the file named
#'_build\_config.yml_'. This file defines a certain number of variable (like the
#name of the packages, the version of the program, and so on ...) that this
#script will then use to build the packages the user want. <span class='gras'>
#The configuration file MUST be in the same directory as this script.</span>

#So, firstly, we check if there's a configuration file. If there isn't
#any, we exit the script.

if [ ! -f "build_config.yml" ] 
then
    echo "build_config.yml is missing" >&2
    exit 1;
fi

#Same for the script _config\_parser.pl_.

if [ ! -f "config_parser.pl" ] 
then
    echo "config_parser.pl is missing" >&2
    exit 2;
fi

#Then, we parse the configuration file. This is done by the perl script 
#_config\_parser.pl_. This script must also be in the same directory as this
#script. This works because the script that parses the file produces an output
#that looks like "export NAME=VALUE;", which is actually shell commands that
#can be passed to the shell command eval.

eval `./config_parser.pl build_config.yml`;

#<a id='makefile'>Looking for the Makefile</a>
#---------------------------------------------

#Here, we are checking the presence of a Makefile.
#If there is no Makefile, the script quits immediatly.
#Debian tools need that in order to build your program.

cd $DIRECTORYNAME;
if [ ! -f Makefile ] 
then
    echo "Makefile is missing" >&2
    exit 3;
fi

#<a id='tarball'>Creating Tarballs</a>
#--------------------------------------

#We check if the user wants to build a library package by looking 
#at the PACKAGETYPE variable. If there is a "l" in this string, it means that 
#the user want to build a library package. To do so, we have to create an 
#adequately named tarball. In order to be adequatly named, a tarball must 
#have a name that matches the following pattern : 
#&lt;package\_name&gt;\_&lt;version&gt;.orig.tar.gz.

#<span class='gras'>N.B. :</span> package\_name must be the name of the 
#library package you want to build.

#In the same way, if there is a "s" in PACKAGETYPE, it means that we 
#have to create a binary package. We must also create a tarball that is named
#adequately (i.e. it must follow the previous pattern).

for var in $PACKAGETYPE; do
    if [ $var = "l" ]
    then
	LIBTARBALL="$LIBPACKAGENAME""_$VERSION.orig.tar.gz";
	tar czf "$LIBTARBALL" .;
	mv $LIBTARBALL ..
    fi
    if [ $var = "s" ]
    then
	BINTARBALL="$BINPACKAGENAME""_$VERSION.orig.tar.gz";
	tar czf "$BINTARBALL" .;
	mv $BINTARBALL ..
    fi
done;

#<a id='copyright'>The copyright file</a>
#----------------------------------------

#The Debian Policy tells us that there need to be a file named '_copyright_'
#in the debian directory. Although this is not mandatory, it's recommended to
#do so, if you want your package to be compliant with the Debian Policy. There's
#a variable concerning this in the configuration file.

#In this part of the script, we check if the variable COPYRIGHT is empty, a file
#the user wants to include or a predefined value.
 
if [ $COPYRIGHT = "gpl" ] || [ $COPYRIGHT = "gpl2" ] || [ $COPYRIGHT = "gpl3" ]\
    || [ $COPYRIGHT = "lgpl" ] || [ $COPYRIGHT = "lgpl2" ] \
    || [ $COPYRIGHT = "lgpl3" ] || [ $COPYRIGHT = "artistic" ] || \
    [ $COPYRIGHT = "apache" ] || [ $COPYRIGHT = "bsd" ] || \
    [ $COPYRIGHT = "mit" ]
then
    COPYRIGHTTYPE=predefined;
elif [ -f $COPYRIGHT ]
then
    COPYRIGHTTYPE=file;
else
    COPYRIGHTTYPE=empty;
fi

#<a id='lib'>Building the library packages</a>
#----------------------------------------------

#This is the part of the script where we're going to build the packages that
#will contain the library and the header files. This is also where we're
#going to build the binary package, if it happens that the user wants to build
#both type of packages. Indeed, if the user wants to build both type of packages
#, it is much simpler to do it in this part of the script, because this is
#where there are less modification to do in the debian files.

if [ ! -z $LIBTARBALL ]
then

#<a id='libdeb'>Creation of the Debian files</a>
#-----------------------------------------------

#In order to build packages, debuild (the utilitary that will create the 
#package) needs some files that must be put in a directory 
#named _debian_, in the source code directory (variable DIRECTORYNAME in 
#the configuration file).

#The files that debuild needs can be created automatically by the command 
#dh_make. We call dh_make with those options :
#1. -l : It means that we want to create library packages.
#2. -c : It tells dh_make we want a pre-filled copyright file.
#3. -p : Forces the package name to be what is after that option. It's quite
#usefull because dh_make normally checks if the source code directory is named
#according to the following pattern : &lt;packagename&gt;-&lt;version&gt;. 
#This option allows you to name your directory whatever name you want.

#We remove de debian directory, because if there is one, dh_make will not make
#any changes, and that's not what we want.
    rm -rf debian/

#We create a temporary file that only contains a blank line. We do that because
#dh_make waits an input from the user
    env echo -e -n "\n" > debtmpfile

#We use the -c option of dh_make if the user specified that the copyright file
#must be generated by dh_make. You shouldn't really do that because the
#copright files that are generated by dh_make needs to be filled with
#correct information
    if [ $COPYRIGHTTYPE = "predefined" ]
    then
	dh_make -l -c $COPYRIGHT -p "$LIBPACKAGENAME""_$VERSION" < debtmpfile
    else
	dh_make -l -p "$LIBPACKAGENAME""_$VERSION" < debtmpfile
    fi
    
#If the user has written a copyright file, we copy it in the debian directory
    if [ $COPYRIGHTTYPE = "file" ] 
    then
	cp $COPYRIGHT debian/copyright
    fi

#We then remove the temporary file we have created.
    rm -f debtmpfile
    

#<a id='libmod'>Modification of the debian files</a>
#----------------------------------------------------

#dh\_make creates the debian directory, and all the files in it. Therefore, we
#must modify some of them, because some of them are not complete.

#First, we need to modify the changelog file, because if we don't, the name
#of the package will be something like : &lt;packagename&gt;\_&lt;version&gt;-1
#\_&lt;archi&gt;.deb.    This is not what we want. We want something like :
#&lt;packagename&gt;\_&lt;version&gt;\_&lt;archi&gt;.deb.

    perl -pi -e 's/\(((\d+\.?)+)-\d+\)/($1)/' debian/changelog

#Some of the files generated by dh_make are just examples of what you should 
#provide. This is not interesting to us, therefore we have to suppress them.

    rm -rf debian/*.EX debian/*.ex debian/README*

#We then have to rename the dirs files and the install files that have been
#created by dh\_make. .dirs files tells _debuild_ what are the directories
#that must created in the package(s). .install files tells the _debuild_ 
#command what files must be put in the package(s). You can create up to one
#.dirs file and one .install file for each package you want to build.
#We need to rename those files because the generated files are named according
#to those patterns : &lt;packagename&gt;1.dirs and &lt;packagename&gt;1.install
#. This is not 
#correct, because those files must be named after the package name. Thus, 
#we must rename them according to the following pattern : 
#&lt;packagename&gt;.dirs and &lt;packagename&gt;.install.

    mv debian/"$LIBPACKAGENAME"1.dirs debian/"$LIBPACKAGENAME".dirs
    mv debian/"$LIBPACKAGENAME"1.install debian/"$LIBPACKAGENAME".install

#In the control file, the name of the package is &lt;packagename&gt;BROKEN.
#This is not the package name we want, we must therefore change it to
#&lt;packagename&gt;.

    sed -i -r -e s/"$LIBPACKAGENAME"BROKEN/"$LIBPACKAGENAME"/g debian/control

#Then, we have to fill the .install and .dirs files with correct information.
#In order to do that, we use the variables LIBNAMES (which contains the
#libraries you want to include in the package) and HEADERNAMES (which contains
#the name of the header files you want to include in the package).

    rm -f debian/$LIBPACKAGENAME.install
    for library in $LIBNAMES
    do
	echo "$library usr/lib" >> debian/$LIBPACKAGENAME.install
    done;

    if [ ! -z $HEADERNAMES ] ; then
	rm -f debian/$LIBPACKAGENAME-dev.install;
	for header in $HEADERNAMES
	do
	    echo "$header usr/include/" >> debian/$LIBPACKAGENAME-dev.install
	done;
    else
	perl -0777 -pi -e "s/Package: $LIBPACKAGENAME-dev.+?\n\n//s" debian/control
    fi
   
#If the user wants to build both types of packages, we create a .install file
#and a .dirs file containing the right informations. 
    if [ ! -z $BINTARBALL ] 
    then

	for binary in $BINARYNAMES
	do
	    echo "$binary usr/bin" >> debian/$BINPACKAGENAME.install
	done;
	echo "usr/bin" >> debian/$BINPACKAGENAME.dirs

#We then have to add information in the control file. We have to tell _debuild_
#that there's another package to create.

	env echo -e -n "\n" >> debian/control
	echo "Package: $BINPACKAGENAME" >> debian/control
	echo "Architecture: any" >> debian/control
	echo 'Depends: ${shlibs:Depends}, ${misc:Depends}' >> debian/control
	echo 'Description: <insert up to 60 chars description>' >>debian/control
	echo ' <insert long description, indented with spaces>' >>debian/control
    fi

#Now, we have to add the dependencies of the packages, in the control file.

    if [ ! -z $PACKAGEDEPENDS ]
    then
	perl -pi -e 's/^(Depends:.+)$/$1, '"$PACKAGEDEPENDS/" debian/control 
    fi   
    if [ ! -z $BUILDDEPENDS ]
    then
	perl -pi -e 's/(Build-Depends:.+)$/$1, '"$BUILDDEPENDS/" debian/control
    fi

#<a id='libsource'>Creation of the source package</a>
#----------------------------------------------------

#Now, we are building a source package that contains the source code 
#of your program. When debuild has created that package, it runs a program 
#called _lintian_, that checks if the package complies with the debian policy.
#The log of this run can be found in the file whose name matches the following
#pattern : &lt;packagename&gt;\_&lt;version&gt;\_source.build. Some of the
#errors detected by _lintian_ can be corrected automatically. This is why we've
#created a script called _correct\_lintian.pl_, that corrects some of thoses
#errors. 

    debuild -S -uc -us
    ../correct_lintian.pl  ../"$LIBPACKAGENAME""_$VERSION""_source.build"
   
#<a id='libbuild'>Building the package</a>
#-----------------------------------------

#Now, we can finally build the package. _debuild_ does pretty much all the work.
#-us and -uc options means that we don't sign the packages.
    debuild -us -uc;

#We now have to remove the debian directory, the tarballs, and some files
#created by _debuild_.
    #rm -rf debian/ && cd ..;
    #rm -f "$BINTARBALL" "$LIBTARBALL";
    #rm -f "$LIBPACKAGENAME""_$VERSION.debian.tar.gz";
    #rm -f "$LIBPACKAGENAME""_$VERSION.dsc";
    #rm -f *.build *.changes;

#If the user wanted to create both type of packages, we can exit the script
#because we've done that.
    if [ ! -z $BINTARBALL ]
    then
	exit 0;
    fi
#<a id'=bin'>Building the binary package</a>
#-------------------------------------------
elif [ ! -z $BINTARBALL ] 
then
#<a id='bindeb'>Creation of the debian files</a>
#-----------------------------------------------

#In this part of the script, we create the files that _debuild_ needs to build
#the package. We do exactly the same things as above, so you should read that
#part of the script.

#Removing the debian directory, if it exists
    rm -rf debian/

#Creation of the debian files 
#The only difference between this call to _dh\_make_ and the one above is
#the -s option. This option tells _dh\_make_ that we want to create a binary
#package.

    env echo -e -n "\n" >debtmpfile
    if [ $COPYRIGHTTYPE = "predefined" ]
    then
	dh_make -s -c $COPYRIGHT -p "$BINPACKAGENAME""_$VERSION" < debtmpfile
    else
	dh_make -s -p "$BINPACKAGENAME""_$VERSION" < debtmpfile
    fi

#Copying the copyright file, if there is one
    if [ $COPYRIGHTTYPE = "file" ] 
    then
	cp $COPYRIGHT debian/copyright
    fi

    rm -f debtmpfile

#<a id='debmod'>Modification of the debian files</a>
#----------------------------------------------------

#Modifying the changelog file

    perl -pi -e 's/\(((\d+\.?)+)-\d+\)/($1)/' debian/changelog

#Removing useless files

    rm -rf debian/*.EX debian/*.ex debian/README*

#Creating .install file
    for binary in $BINARYNAMES
    do
	echo "$binary usr/bin" >> debian/$BINPACKAGENAME.install
    done;

#Creating .dirs file
    echo "usr/bin" >> debian/$BINPACKAGENAME.dirs

#Adding dependencies, if there are any
    if [ ! -z $PACKAGEDEPENDS ]
    then
	perl -pi -e 's/^(Depends:.+)$/$1, '"$PACKAGEDEPENDS/" debian/control 
    fi

    if [ ! -z $BUILDDEPENDS ]
    then
	perl -pi -e 's/^(Build-Depends:.+)$/$1, '"$BUILDDEPENDS/" debian/control
    fi

#<a id='binsource'>Creation of the source package</a>
#----------------------------------------------------

#We build the source package, and then we correct some of the mistakes
#that _lintian_detected
    debuild -S -uc -us
    ../correct_lintian.pl  ../"$BINPACKAGENAME""_$VERSION""_source.build"
    

#<a id='libbuild'>Building the package</a>
#-----------------------------------------

#Building the package
    debuild -us -uc;

#Removing useless files
    rm -rf debian/ && cd ..;
    rm -f "$BINTARBALL" "$LIBTARBALL";
    rm -f "$BINPACKAGENAME""_$VERSION.debian.tar.gz";
    rm -f "$BINPACKAGENAME""_$VERSION.dsc";
    rm -f *.build *.changes;
fi