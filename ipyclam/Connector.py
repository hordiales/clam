In = "In"
Out = "Out"
Port = "Port"
Control = "Control"

class Connector(object):
	def __init__(self, networkProxy, host, kind=Port, direction=In, name = "Inport1"):
		self._proxy = networkProxy
		self.__dict__["host"] = host
		self.__dict__["name"] = name
		self.__dict__["kind"] = kind
		self.__dict__["direction"] = direction
	@property
	def name(self):
		"""The name of the port"""
		return self.__dict__["name"]
	@property
	def kind(self):
		"""The kind of the port"""
		return self.__dict__["kind"]
	@property
	def direction(self):
		"""The direction of the port"""
		return self.__dict__["direction"]
	@property
	def index(self):
		"""The index of the port"""
		return self._proxy.connectorIndex(self.__dict__["host"], self.__dict__["kind"], self.__dict__["direction"], self.__dict__["name"])
	@property                                                                                                                                
	def type(self):                                                                                                                          
		"""The type of the port"""                                                                                                       
		return self._proxy.connectorType(self.__dict__["host"], self.__dict__["kind"], self.__dict__["direction"], self.__dict__["name"])
	@property
	def host(self):
		import Processing
		return Processing.Processing(
			self.__dict__["host"],
			self._proxy,
			)
	@property
	def peers(self):
		from PeerConnectors import PeerConnectors
		return PeerConnectors(self._proxy, self.__dict__["host"], self.__dict__["kind"], self.__dict__['direction'], self.__dict__["name"])

	def connect(self, connectorDestiny):
		self._proxy.connect(self.__dict__["host"], self.__dict__["name"], self.__dict__["kind"], self.__dict__["direction"],
					connectorDestiny.host.name, connectorDestiny.name, connectorDestiny.kind, connectorDestiny.direction)

import unittest
import TestFixtures

class ConnectorTests(unittest.TestCase):
	def test_name(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertEqual(port.name, "InPort1")
	def test_name_isReadOnly(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertRaises(AttributeError, setattr, port, "name", 'InPort2')
	def test_kind_whenPort(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertEqual(port.kind, "Port")
	def test_kind_whenControl(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Control, direction=In, name="InPort1")
		self.assertEqual(port.kind, "Control")
	def test_kind_isReadOnly(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertRaises(AttributeError, setattr, port, "kind", Control)
	def test_direction_whenIn(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertEqual(port.direction, "In")
	def test_direction_whenOut(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=Out, name="InPort1")
		self.assertEqual(port.direction, "Out")
	def test_direction_isReadOnly(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertRaises(AttributeError, setattr, port, "direction", Out)
	def test_index(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertEqual(port.index, 0)
	def test_index_isReadOnly(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertRaises(AttributeError, setattr, port, "index", 2)
	def test_type(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertEqual(port.type, "type1")
	def test_type_isReadOnly(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="InPort1")
		self.assertRaises(AttributeError, setattr, port, "type", "tipus2")
	def test_host(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=In, name="OutPort1")
		self.assertEqual(port.host.name, "Processing1")
		self.assertEqual(port.host.type, "AudioSource")
	# TODO: is host read only?
	def test_peers_forInPorts(self):
		port = Connector(TestFixtures.proxy(), "Processing2", kind=Port, direction=In, name="Inport2")
		listPeers = [ connector.name for connector in port.peers ]
		self.assertEqual(['OutPort1', 'OutPort2'], listPeers)
	def test_peers_forInPorts_whenEmpty(self):
		port = Connector(TestFixtures.proxy(), "Processing2", kind=Port, direction=In, name="Inport1")
		listPeers = [ connector.name for connector in port.peers ]
		self.assertEqual([], listPeers)
	def test_peers_forOutPorts(self):
		port = Connector(TestFixtures.proxy(), "Processing1", kind=Port, direction=Out, name="OutPort1")
		listPeers = [ connector.name for connector in port.peers ]
		self.assertEqual(['Inport2'], listPeers)
	def test_connectTwoPorts(self) :
		proxy = TestFixtures.proxy()
		port = Connector(proxy, "Processing1", kind=Port, direction=Out, name="OutPort1")
		port2 = Connector(proxy, "Processing2", kind=Port, direction=In, name="Inport1")
		port.connect(port2)
		listPeersPort = [ connector.name for connector in port.peers ]
		self.assertEqual(['Inport2', 'Inport1'], listPeersPort)
		listPeersPort2 = [ connector.name for connector in port2.peers ]
		self.assertEqual(['OutPort1'], listPeersPort2)

if __name__ == '__main__':
	unittest.main()



