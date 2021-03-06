#!/usr/bin/python
# -*- coding: UTF-8 -*-

import unittest
import clam

class NetworkTest(unittest.TestCase):

	def setUp(self):
		self.network = clam.Network()
	#setUp()

	def tearDown(self):
		pass

	def testAddAProcessingToTheNetwork(self):
		reader = self.network.AddProcessing( "MonoAudioFileReader" )
		self.assertEqual( "MonoAudioFileReader", reader, "Error adding a processing to the network." )
	#testAddAProcessingToTheNetwork()

	def testConfigureProcessingFromTheNetwork(self):
		reader = self.network.AddProcessing( "MonoAudioFileReader" )
		config = clam.MonoAudioFileReaderConfig()
		self.assertEqual( False, self.network.ConfigureProcessing(reader,clam.toProcessingConfig(config)) )
		config.SetSourceFile( "trumpet.mp3" )
		self.assertEqual( True, self.network.ConfigureProcessing(reader,clam.toProcessingConfig(config)) )
	#testConfigureProcessingFromTheNetwork()

#class NetworkTest

if __name__ == '__main__':
	unittest.main()
