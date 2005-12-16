from SCons.Action import *
from buildtools import *
import sys

def handle_preinclude ( env ):
	if sys.platform != 'win32' :
		env.Append(CCFLAGS='-include CLAM/%s'%env['preinclude'])
	else:
		env.Append(CCFLAGS='/FICLAM/%s'%env['preinclude'])
	return

def posix_lib_rules( name, version, headers, source_files, install_dirs, env) :
	lib_descriptor = env.File( 'clam_'+name+'.pc' )

	# We expect a version like " X.Y-possibleextrachars "
	versionnumbers = version.split('.')

	if len(versionnumbers) > 2:
		print " ERROR in buildtools.posix_lib_rules: version name does not follow CLAM standard "
		print "   Check the variable 'version' in the main SConstruct"
		sys.exit(1)

	if sys.platform == 'linux2' :
		soname = 'libclam_'+name+'.so.%s' % versionnumbers[0]
		linker_name = 'libclam_'+name+'.so'
		env.Append(SHLINKFLAGS=['-Wl,-soname,%s'%soname ] )
		lib = env.SharedLibrary( 'clam_' + name, source_files, SHLIBSUFFIX='.so.%s'%version )
		#NOT needed now
		#soname_lib = env.SonameLink( soname, lib )			# lib***.so.X -> lib***.so.X.Y
		#linkername_lib = env.LinkerNameLink( linker_name, soname_lib)	# lib***.so -> lib***.so.X
		linkername_lib = env.SoNameLink( soname, lib )			# lib***.so.X -> lib***.so
	else : #darwin
		soname = 'libclam_'+name+'.%s.dylib' % versionnumbers[0]
		#soname = 'libclam_'+name+'.%s.dylib' % (versionnumbers[0], versionnumbers[1])
		linker_name = 'libclam_'+name+'.dylib'
		env.Append( CCFLAGS=['-fno-common'] )
		env.Append( SHLINKFLAGS=['-dynamic',
				'-Wl,-install_name,%s'%(install_dirs.lib + '/' + 'libclam_' + name + '.%s.dylib'%(version))] )
		lib = env.SharedLibrary( 'clam_' + name, source_files, SHLIBSUFFIX='.%s.dylib'%version )
		#soname_lib = env.LinkerNameLink( soname, lib )			# lib***.X.dylib -> lib***.X.Y.dylib
		#linkername_lib = env.LinkerNameLink( linker_name, soname_lib)	# lib***.dylib -> lib***.X.Y.dylib
		linkername_lib = env.LinkerNameLink( soname, lib)		# lib***.X.dylib -> lib***.dylib
	tgt = env.Alias( name, linkername_lib )

	install_headers = env.Install( install_dirs.inc+'/CLAM', headers )
	env.AddPostAction( install_headers, "chmod 644 $TARGET" )

	install_lib = env.Install( install_dirs.lib, lib)
	env.AddPostAction( install_lib, Action(make_lib_names, make_lib_names_message ) )

	install_descriptor = env.Install( install_dirs.lib+'/pkgconfig', lib_descriptor )

	install_tgt = env.Alias( 'install_' + name, [install_headers, install_lib, install_descriptor] )

	runtime_lib = env.Install( install_dirs.lib, lib )
	runtime_soname = env.SonameLink( install_dirs.lib + '/' + soname, runtime_lib )

	env.Alias( 'install_'+name+'_runtime', [runtime_lib, runtime_soname] )

	static_lib = env.Library( 'clam_'+name, source_files )
	install_static = env.Install( install_dirs.lib, static_lib )

	dev_linkername =  env.LinkerNameLink( install_dirs.lib+'/'+linker_name, install_dirs.lib+'/'+soname) 
	env.Alias( 'install_'+name+'_dev', [install_headers,dev_linkername, install_descriptor, install_static] )
	
	return tgt, install_tgt

def win32_lib_rules( name, version, headers, source_files, install_dirs, env ) :
	static_lib = env.Library( 'clam_' + name, source_files )
	tgt = env.Alias(name, static_lib)
	lib_descriptor = env.File( 'clam_'+name+'.pc' )
	install_static = env.Install( install_dirs.lib, static_lib )
	install_descriptor = env.Install( install_dirs.lib+'/pkgconfig', lib_descriptor )
	install_headers = env.Install( install_dirs.inc+'/CLAM', headers )
	env.Alias('install_'+name, [install_headers,install_static,install_descriptor])
	return tgt, install_static
