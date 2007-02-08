import os
import sys
from buildtools.include_rename import update_includes_without_db
import shelve

def generate_copy_files( target, source, env ) :
	if sys.platform == 'win32' :
		copyCmd = 'copy'
	else :
		copyCmd = 'cp'
	command = "%s %s %s"%(copyCmd, source[0], target[0])
	os.system( command )
	update_includes_without_db(str(target[0]))
	return None

def generate_copy_files_message( target, source, env ) :
	return "== Copying and updating includes : %s" % target[0]
	

def generate_so_name( target, source, env ) :
	source_dir = os.path.dirname( str(source[0]) )
	source_file = os.path.basename( str(source[0]) )
	cwd = os.getcwd()
	os.chdir( source_dir )
	if sys.platform == 'linux2' :
		os.system( "/sbin/ldconfig -n ." )
	os.chdir(cwd)
	return None

def generate_so_name_message( target, source, env ) :
	return "generating %s with /sbin/ldconfig -n"%target[0]

def generate_linker_name( target, source, env ) :
	source_file = os.path.basename( str(source[0]) )
	os.system( "ln -sf %s %s"%(source_file,str(target[0]) ))
	return None

def generate_linker_name_message( target, source, env ) :
	return "Linking linker name %s to %s"%(target[0], source[0])

