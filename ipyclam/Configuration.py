
class Configuration(object):
	def __init__(self, proxy):
		object.__setattr__(self, "_proxy", proxy)
	def __getitem__(self, name):
		return self._proxy[name]
	def __setitem__(self, name, value):
		self._proxy[name] = value
	def __getattr__(self, name):
		try:
			return self.__getitem__(name)
		except KeyError, e:
			raise AttributeError(e.args[0])
	def __setattr__(self, name, value):
		try:		
			self.__setitem__(name, value)
		except KeyError, e:
			raise AttributeError(e.args[0])
	def __dir__(self):
		return self._proxy.keys()

import operator
import unittest
import TestFixtures
import Dummy_ConfigurationProxy

class ConfigurationTests(unittest.TestCase):

	def stringParametersConfig(self) :
		return Dummy_ConfigurationProxy.Dummy_ConfigurationProxy(
			TestFixtures.dummyConfigWithStrings())

	def test_getItem(self):
		c = Configuration(self.stringParametersConfig())
		self.assertEqual(c["ConfigParam1"], "Param1")
		self.assertEqual(c["ConfigParam2"], "Param2")

	def test_setItem(self):
		c = Configuration(self.stringParametersConfig())
		c["ConfigParam1"] = 'newvalue'
		self.assertEqual(c["ConfigParam1"], "newvalue")

	def test_getAttr(self):
		c = Configuration(self.stringParametersConfig())
		self.assertEqual(c.ConfigParam1, "Param1")
		self.assertEqual(c.ConfigParam2, "Param2")

	def test_setAttr(self):
		c = Configuration(self.stringParametersConfig())
		c.ConfigParam1 = 'newvalue'
		self.assertEqual(c.ConfigParam1, "newvalue")

	def test_setAttr_affects_getItem(self):
		c = Configuration(self.stringParametersConfig())
		c.ConfigParam1 = 'newvalue'
		self.assertEqual(c["ConfigParam1"], "newvalue")

	def test_setItem_affects_getAttr(self):
		c = Configuration(self.stringParametersConfig())
		c['ConfigParam1'] = 'newvalue'
		self.assertEqual(c.ConfigParam1, "newvalue")

	def test_setAttr_wrongName(self):
		c = Configuration(self.stringParametersConfig())
		try:
			c.WrongParam1 = "ParamValue"
			self.fail("Exception expected")
		except AttributeError, e:
			self.assertEquals("WrongParam1", e.args[0])

	def test_getAttr_wrongName(self):
		c = Configuration(self.stringParametersConfig())
		try:
			param = c.WrongParam1
			self.fail("Exception expected")
		except AttributeError, e:
			self.assertEquals("WrongParam1", e.args[0])

	def test_setAttr_wrongType(self):
		c = Configuration(self.stringParametersConfig())
		try:
			c.ConfigParam1 = 1
			self.fail("Exception expected")
		except TypeError, e:
			self.assertEquals("str value expected, got int", e.args[0])

	def test_getItem_wrongName(self):
		c = Configuration(self.stringParametersConfig())
		try:
			param = c["WrongParam1"]
			self.fail("Exception expected")
		except KeyError, e:
			self.assertEquals("WrongParam1", e.args[0])

	def test_setitem_wrongName(self):
		c = Configuration(self.stringParametersConfig())
		try:
			c["WrongParam1"] = "value"
			self.fail("Exception expected")
		except KeyError, e:
			self.assertEquals("WrongParam1", e.args[0])

	def test_setItem_wrongType(self):
		c = Configuration(self.stringParametersConfig())
		try:
			c["ConfigParam1"] = 1
			self.fail("Exception expected")
		except TypeError, e:
			self.assertEquals("str value expected, got int", e.args[0])

	def test_dirFunction(self):
		c = Configuration(self.stringParametersConfig())
		self.assertEquals(["ConfigParam1", "ConfigParam2", "ConfigParam3"], dir(c))


if __name__ == '__main__':
	unittest.main()

