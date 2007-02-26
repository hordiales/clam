import re, os, glob
import sys

class FileRetriever :
	__hdr_extensions = ['h', 'H', 'hxx', 'hpp']
	__src_extensions = ['c', 'C', 'cxx', 'cpp', r'c\+\+', 'cc']
	if sys.platform == 'win32' :
		__copy_cmd = 'copy'
	else :
		__copy_cmd = 'cp'


	def __init__(self, basedir, folders, blacklist  ) :
		self.out_inc = 'include/CLAM'
		self.out_src = 'src'
		self.headerREs = []
		
		for ext in self.__hdr_extensions :
			self.headerREs.append( re.compile( r'^.+\.%s$'%ext ) )

		self.sourceREs = [] 

		for ext in self.__src_extensions : 
			self.sourceREs.append( re.compile( r'^.+\.%s$'%ext  ) )
	
		self.scantargets = [ basedir+'/'+folder for folder in folders ]

		self.blacklisted = []

		for entry in blacklist :
			self.blacklisted.append( re.compile( entry ) )

		self.headers = []
		self.sources = []
		self.origTargetHeaders = []
		self.origTargetSources = []

	def __setup_file( self, src, tgt, echo = True ) :
		norm_src = os.path.normcase( src )
		norm_tgt = os.path.normcase( tgt )
		command = "%s %s %s"%(self.__copy_cmd, norm_src, norm_tgt)
		if echo : print command
		os.system( command )

	def is_blacklisted( self, filename ) :
		for entry in self.blacklisted :
			if entry.search(filename) is not None :
				return True		

		return False

	def is_header( self, filename ) :
		for regexp in self.headerREs :
			if regexp.search( filename ) is not None :
				return True
		return False

	def is_source( self, filename ) :
		for regexp in self.sourceREs :
			if regexp.search( os.path.basename(filename) ) is not None :
				return True
		return False

	def scan_without_copy( self ) :
		for target in self.scantargets :
			if not os.path.isdir( target ) : # is a file
				base = os.path.dirname(target)
				for file in glob.glob(target ) :
					if self.is_blacklisted(file) :
						continue
					if self.is_header(file) :
						self.origTargetHeaders.append((file, '%s/%s'%(self.out_inc,os.path.basename(file))))
					if self.is_source(file) :
						self.origTargetSources.append((file, '%s/%s'%(self.out_src,os.path.basename(file))))
			else : # is a dir
				for file in os.listdir(target) :
					if self.is_blacklisted(file) :
						continue
					if self.is_header(file) :
						self.origTargetHeaders.append((target+'/'+file, '%s/%s'%(self.out_inc,file)))
					if self.is_source(file) :
						self.origTargetSources.append((target+'/'+file, '%s/%s'%(self.out_src,file)))

	
