#!/usr/bin/python
# -*- coding: utf-8 -*-
# main
import SCons.Script

import os
import sys
import platform

env = Environment()
env.Alias('all', ['.'])

scons_root = os.getcwd()
#TBD a folder named scons maybe used in the future sys.path.append(os.path.join(scons_root, "scons"))

#absolute paths are depre
bridge = 'tick42blp'
payload = 'tick42blpmsg'

#TBD check here OS. Currently hard code to SConscript
#TBD maybe in the future the host architecture should be checked too  'HOST_ARCH': 'x86_64',     
if env['HOST_OS'] == 'win32':
	SConsript = 'SConscript.win'
else:
	SConsript = 'SConscript'

# Default values for variables
blp_prefix = ''
boost_prefix = ''
boost_inc_prefix = ''
boost_lib_prefix = ''
openmama_src_prefix = ''
openmama_lib_prefix = ''
uuid_prefix = ''
dump_variables = 'no'
lib_prefix = os.path.abspath( os.path.join('.', 'lib'));

projects =[]

# 1. default values for variables to existing environment variables or by known existing folders
if 'TICK42_BLPAPI_CPP' in os.environ:
	blp_prefix = os.environ['TICK42_BLPAPI_CPP']
if 'TICK42_BOOST' in os.environ:
	boost_prefix = os.environ['TICK42_BOOST']
	if env['HOST_OS'] == 'win32':
		boost_lib_prefix = os.path.join(boost_prefix, "stage\lib")
		boost_inc_prefix = boost_prefix
	else:
		boost_lib_prefix = os.path.join(boost_prefix, "lib")
		boost_inc_prefix = os.path.join(boost_prefix, "include")
		
if 'TICK42_OPENMAMA' in os.environ:
	openmama_src_prefix= os.environ['TICK42_OPENMAMA']
if env['HOST_OS'] != 'win32' and os.path.exists("/opt/openmama/lib"):
	openmama_lib_prefix = "/opt/openmama/lib"
if 'TICK42_UUID' in os.environ:
	uuid_prefix= os.environ['TICK42_UUID']


# 2. Missing argument and flags handlers defaults
if 'target' not in ARGUMENTS:
	ARGUMENTS['target']='all'

if 'build' not in ARGUMENTS:
	ARGUMENTS['build']='release'

# 3. Then override all values that where defaulted to environment variable to to existing argument values 
if 'blp_prefix' in ARGUMENTS:
	blp_prefix = ARGUMENTS['blp_prefix']
if 'boost_prefix' in ARGUMENTS:
	boost_prefix = ARGUMENTS['boost_prefix']
	if env['HOST_OS'] == 'win32':
		boost_inc_prefix = boost_prefix
		boost_lib_prefix = os.path.join(boost_prefix, "state\lib")
	else:
		boost_lib_prefix = os.path.join(boost_prefix, "lib")
		boost_inc_prefix = os.path.join(boost_prefix, "include")
		
if 'boost_lib_prefix' in ARGUMENTS:
	boost_lib_prefix= ARGUMENTS['boost_lib_prefix']
if 'boost_inc_prefix' in ARGUMENTS:
	boost_inc_prefix= ARGUMENTS['boost_inc_prefix']
if 'openmama_src_prefix' in ARGUMENTS:
	openmama_src_prefix= ARGUMENTS['openmama_src_prefix']
if 'openmama_lib_prefix' in ARGUMENTS:
	openmama_lib_prefix= ARGUMENTS['openmama_lib_prefix']
if 'uuid_prefix' in ARGUMENTS:
	uuid_prefix= ARGUMENTS['uuid_prefix']
if 'dump_variables' in ARGUMENTS:
	dump_variables= ARGUMENTS['dump_variables']
if 'lib_prefix' in ARGUMENTS:
	lib_prefix = ARGUMENTS['lib_prefix']
else:
	lib_prefix = os.path.abspath( os.path.join('.', 'lib', ARGUMENTS['build'])); #this string might be wrong because of wrong build name, but the build is checked right away.

# Handle defaults and errors
if 'target' not in ARGUMENTS:
	projects = [bridge, payload]
elif ARGUMENTS['target'] == 'all':
	projects = [bridge, payload]
elif ARGUMENTS['target'] == 'bridge':
	projects = [bridge]
elif ARGUMENTS['target'] == 'payload':
	projects = [bridge]
elif ARGUMENTS['target'] == 'none':
	projects =[]
if ARGUMENTS['build'] != 'debug' and ARGUMENTS['build'] != 'release':
	print "Wrong build %s!" % ARGUMENTS['build']
	Exit(-1);

if blp_prefix == "":
	print "Wrong blp_prefix!"
	Exit(-1);
if boost_prefix == "":
	print "Wrong boost_prefix!"
	Exit(-1);
if boost_lib_prefix == "":
	print "Wrong boost_lib_prefix!"
	Exit(-1);
if boost_inc_prefix == "":
	print "Wrong boost_inc_prefix!"
	Exit(-1);
if openmama_src_prefix == "":
	print "Wrong openmama_src_prefix!"
	Exit(-1);
if openmama_lib_prefix == "" and env['HOST_OS'] != 'win32':
	print "Wrong openmama_lib_prefix!"
	Exit(-1);
	
if (dump_variables=='yes'):
    print 'target=%s' % (ARGUMENTS['target'])
    print 'build=%s' % (ARGUMENTS['build'])
    print 'blp_prefix=%s' % (blp_prefix)
    print 'boost_prefix=%s' % (boost_prefix)
    print 'boost_inc_prefix=%s' % (boost_inc_prefix)
    print 'boost_lib_prefix=%s' % (boost_lib_prefix)
    print 'openmama_src_prefix=%s' % (openmama_src_prefix)
    print 'openmama_lib_prefix=%s' % (openmama_lib_prefix)
    if env['HOST_OS'] == 'win32':
        print 'uuid_prefix is ignored on windows build' 
    else:
        print 'uuid_prefix=%s' % (uuid_prefix)
    print 'lib_prefix=%s' %(lib_prefix)
    print 'bridge=%s' % (bridge)
    print 'payload=%s' % (payload)
# Initialize Help 
Help("""
Help:

Supported Platfroms and Architectures:
Linux x64
                  	
Build variables are written as scons arguments followed by an equal mark. 
For example:
$ scons target=payload build=debug 
will compile a debug target of the payload project.
	   
Build Variables:
Each option is followed by a path or an optional value from the brackets on its 
right.
	   
    target: [all|bridge|payload|none] Default: all - Chosen Targets
    build: [debug|release] Default: release - Targets configuration
    blp_prefix: Default: $TICK42_BLPAPI_CPP - Path to Bloomberg API root
    boost_prefix: Default: $TICK42_BOOST - Path to Boost root
    boost_inc_prefix: Default: $TICK42_BOOST/include - Path to Boost 'include' 
    boost_lib_prefix: Default: $TICK42_BOOST/lib - Path to Boost libraries, 
        static and shared objects.
    openmama_src_prefix: Default: $TICK42_OPENMAMA - Path to OpenMAMA sources 
        root 
    openmama_lib_prefix: Default: /opt/openmama/lib - Path to OpenMAMA shared 
        objects
    uuid_prefix: Default $TICK42_UUID - Path to uuid libraries (should be used 
        mostly in Linux environments and ignored on Windows)
    dump_variables: [yes|no] Default: no - Should dump variable. 'yes' is case 
        sensitive and all other values are considered as 'no'
    lib_prefix: Default: ./lib[debug|release] - libraries target. For any folder 
        which is not local (the same level) as SConstruct needs to add the 
        'install' option
    install: installing the libraries into a specific folder (in levels above 
        the SConstruct)
       """)
#TBD When Windows is supported then add under (in the Help above)
#1. 
#          boost_inc_prefix: Default: 
#			Linux: $TICK42_BOOST/include 
#			Windows: $TICK42_BOOST 
#		- Path to Boost include folder  
#
#          openmama_lib_prefix: Default: 
#                       Linux: /opt/openmama/lib - Path to OpenMAMA shared objects
#                       Windows: $TICK42_OPENMAMA/[Release/Debug depends on the build]
#
# Exports to sub-projects

build = ARGUMENTS['build']
Export('env build blp_prefix boost_prefix boost_lib_prefix boost_inc_prefix openmama_src_prefix openmama_lib_prefix uuid_prefix dump_variables lib_prefix')

VariantDir('build', 'src', duplicate=0)

# Set working sub-projects
if env['HOST_OS'] == 'win32':
	print "Windows is not supported"
# Currently we expect a middleware consist of only a payload and a bridge
elif (len(projects) >= 1): 
	if bridge in projects:
		SConscript(os.path.join(bridge, SConsript), variant_dir=os.path.join('build',  bridge, build ))
	if payload in projects:
		SConscript(os.path.join(payload, SConsript), variant_dir=os.path.join('build',  payload, build ))
