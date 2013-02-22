import Connector
from collections import OrderedDict as odict

_connectorKindNames = [
	(Connector.Port, Connector.In, 'inports'),
	(Connector.Port, Connector.Out, 'outports'),
	(Connector.Control, Connector.In, 'incontrols'),
	(Connector.Control, Connector.Out, 'outcontrols'),
	]
_kind2Name = dict([((k,d),n) for k,d,n in _connectorKindNames])

def kind2Name(k,d) :
	return _kind2Name[(k,d)]

_dummyPrototypes = dict(
	DummyTypeProcessing1 = dict(
		type = "DummyTypeProcessing1",
		config = odict(
			ConfigParam1 = "default1",
			ConfigParam2 = "default2",
		),
		inports = [
			["InPort1", "f"],
			["InPort2", "f"],
			['InPort3', 'f'],
			['InPort4', 'f'],
		],
		outports = [
			["OutPort1", "f"],
			["OutPort2", "f"]
		],
		incontrols = [
			["InControl1", "f"],
			["InControl2", "f"]
		],
		outcontrols = [
			["OutControl1", "f"],
			["OutControl2", "f"]
		],
	),
	DummyTypeProcessing2 = dict(
		type = "DummyTypeProcessing2",
		config = odict(
			ConfigParam1 = "default1",
			ConfigParam2 = "default2",
		),
		inports = [
			["Inport1", "f"],
			["Inport2", "f"]
		],
		outports = [
			["Outport1", "f"],
			["Outport2", "f"]
		],
		incontrols = [
			["Incontrol1", "f"],
			["Incontrol2", "f"],
			["Incontrol3", "i"]
		],
		outcontrols = [
			["Outcontrol1", "f"],
			["Outcontrol2", "f"]
		],
	),
	Minimal = dict(
		type = "Minimal",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [],
		outcontrols = []
	),
	DummyPortSink = dict(
		type = "DummyPortSink",
		config = odict(),
		inports = [['InPort1', 'DataType']],
		outports = [],
		incontrols = [],
		outcontrols = []
	),
	DummyPortSource = dict(
		type = "DummyPortSource",
		config = odict(),
		inports = [],
		outports = [['OutPort1', 'DataType']],
		incontrols = [],
		outcontrols = []
	),
	DummyControlSink = dict(
		type = "DummyControlSink",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [['InControl1', 'ControlType']],
		outcontrols = []
	),
	DummyControlSource = dict(
		type = "DummyControlSource",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [],
		outcontrols = [
			['OutControl1', 'ControlType'],
			]
	),
	OtherControlSink = dict(
		type = "OtherControlSink",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [
			['InControl1', 'OtherControlType'],
			],
		outcontrols = []
	),
	SeveralInPortsProcessing = dict(
		type = "SeveralInPortsProcessing",
		config = odict(),
		inports = [
			['InPort1', 'DataType'],
			['InPort2', 'DataType'],
			['InPort3', 'DataType'],
			['InPort4', 'DataType'],
			],
		outports = [],
		incontrols = [],
		outcontrols = []
	),
	SeveralInControlsProcessing = dict(
		type = "SeveralInControlsProcessing",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [
			['InControl1', 'ControlType'],
			['InControl2', 'ControlType'],
			['InControl3', 'ControlType'],
			['InControl4', 'ControlType'],
			],
		outcontrols = []
	),
	ProcessingWithNameSpacedPorts = dict(
		type = "ProcessingWithNameSpacedPorts",
		config = odict(),
		inports = [['An inport', 'DataType']],
		outports = [['An outport', 'DataType']],
		incontrols = [],
		outcontrols = []
	),
	ProcessingWithNameSpacedControls = dict(
		type = "ProcessingWithNameSpacedControls",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [['An incontrol', 'ControlType']],
		outcontrols = [['An outcontrol', 'ControlType']]
	),
	ProcessingWithPortsAndControls = dict(
		type = "ProcessingWithPortsAndControls",
		config = odict(),
		inports = [
			['InPort1', 'DataType'],
			['InPort2', 'DataType'],
			['InPort3', 'DataType'],
			],
		outports = [
			['OutPort1', 'DataType'],
			['OutPort2', 'DataType'],
			['OutPort3', 'DataType'],
			['OutPort4', 'DataType'],
			],
		incontrols = [
			['InControl1', 'ControlType'],
			['InControl2', 'ControlType'],
			['InControl3', 'ControlType'],
			['InControl4', 'ControlType'],
			],
		outcontrols = [
			['OutControl1', 'ControlType'],
			['OutControl2', 'ControlType'],
			['OutControl3', 'ControlType'],
			]
	),
	ProcessingWithNumericPorts = dict(
		type = "ProcessingWithNumericPorts",
		config = odict(),
		inports = [
			['1', 'DataType'],
			],
		outports = [
			['1', 'DataType'],
			['2', 'DataType'],
			],
		incontrols = [],
		outcontrols = []
	),
	ProcessingWithNumericControls = dict(
		type = "ProcessingWithNumericControls",
		config = odict(),
		inports = [],
		outports = [],
		incontrols = [
			['1', 'ControlType'],
			['2', 'ControlType'],
			],
		outcontrols = [
			['1', 'ControlType'],
			]
	),
	ProcessingWithConfig = dict(
		type = "ProcessingWithConfig",
		config = odict([
			("ConfigParam1", "default1"),
			("ConfigParam2", "default2"),
		]),
		inports = [],
		outports = [],
		incontrols = [
			['1', 'ControlType'],
			['2', 'ControlType'],
			],
		outcontrols = [
			['1', 'ControlType'],
			]
	),
	DummyProcessingWithStringConfiguration = dict(
		type = "DummyProcessingWithStringConfiguration",
		config = odict([
			("AString", "DefaultValue"),
			("OtherString", "Another default value"),
		]),
		inports = [],
		outports = [],
		incontrols = [],
		outcontrols = []
	),
	Dummy2IOPortsControls = dict(
		type = "Dummy2IOPortsControls",
		config = odict(),
		inports = [
			['inport1', 'DataType'],
			['inport2', 'DataType'],
			],
		outports = [
			['outport1', 'DataType'],
			['outport2', 'DataType'],
			],
		incontrols = [
			['incontrol1', 'DataType'],
			['incontrol2', 'DataType'],
			],
		outcontrols = [
			['outcontrol1', 'DataType'],
			['outcontrol2', 'DataType'],
			]
	),
	Dummy6IOPorts = dict(
		type = "Dummy6IOPorts",
		config = odict(),
		inports = [
			['inport1', 'DataType'],
			['inport2', 'DataType'],
			['inport3', 'DataType'],
			['inport4', 'DataType'],
			['inport5', 'DataType'],
			['inport6', 'DataType'],
			],
		outports = [
			['outport1', 'DataType'],
			['outport2', 'DataType'],
			['outport3', 'DataType'],
			['outport4', 'DataType'],
			['outport5', 'DataType'],
			['outport6', 'DataType'],
			],
		incontrols = [
			],
		outcontrols = [
			]
	),
	Dummy6IOControls = dict(
		type = "Dummy6IOControls",
		config = odict(),
		inports = [
			],
		outports = [
			],
		incontrols = [
			['incontrol1', 'DataType'],
			['incontrol2', 'DataType'],
			['incontrol3', 'DataType'],
			['incontrol4', 'DataType'],
			['incontrol5', 'DataType'],
			['incontrol6', 'DataType'],
			],
		outcontrols = [
			['outcontrol1', 'DataType'],
			['outcontrol2', 'DataType'],
			['outcontrol3', 'DataType'],
			['outcontrol4', 'DataType'],
			['outcontrol5', 'DataType'],
			['outcontrol6', 'DataType'],
			]
	),
)

from Exceptions import *

class Dummy_NetworkProxy :

	def __init__(self, processings=[], portConnections=[], controlConnections=[], description="", types=_dummyPrototypes) :
		self._processings = dict()
		for process in processings:
			self._processings[process['name']] = process
		self._portConnections = portConnections or []
		self._controlConnections = controlConnections or []
		self._types = types
		self._description = description

	def processingType(self, name) :
		try :
			return self._processings[name]["type"]
		except KeyError :
			raise ProcessingNotFound(name)

	def processingConfig(self, name) :
		import Dummy_ConfigurationProxy
		return Dummy_ConfigurationProxy.Dummy_ConfigurationProxy(self._processings[name]["config"])

	def renameProcessing(self, oldName, newName):
		if oldName not in self._processings:
			raise ProcessingNotFound(oldName)
		if newName in self._processings:
			raise NameAlreadyExists(newName)
		self._processings[newName] = self._processings[oldName]
		del self._processings[oldName]

	def processingConnectors(self, name, kind, direction) :
		if name not in self._processings :
			raise ProcessingNotFound(name)
		connectorKindName = kind2Name(kind,direction)
		return [name for name,type in self._processings[name][connectorKindName]]

	def connectorPeers(self, processingName, kind, direction, portName):
		connections = self._portConnections if kind == Connector.Port else self._controlConnections
		if direction == Connector.In :
			return [x[0:2] for x in connections if (processingName, portName)==x[2:4] ]
		else :
			return [x[2:4] for x in connections if (processingName, portName)==x[0:2] ]

	def connectorIndex(self, processingName, kind, direction, connectorName) :
		connectorKindName = kind2Name(kind,direction)
		for i, (name, type) in enumerate(self._processings[processingName][connectorKindName]):
			if name == connectorName: return i
		return -1

	def connectorType(self, processingName, kind, direction, connectorName) :
		self._assertHasProcessing(processingName)
		connectorKindName = kind2Name(kind,direction)
		for name, type in self._processings[processingName][connectorKindName]:
			if name == connectorName: return type
		raise ConnectorNotFound(processingName, kind, direction, connectorName)

	def hasProcessing(self, processingName) :
		return processingName in self._processings

	def processingNames(self) :
		return [ processingName for processingName in self._processings]

	def addProcessing(self, type, name) :
		if self.hasProcessing(name):
			raise NameAlreadyExists(name)
		if type not in self._types.keys():
			raise BadProcessingType(type)
		self._processings[name] = self._types[type].copy()
		self._processings[name]['config'] = self._processings[name]['config'].copy()

	def processingHasConnector(self, processingName, kind, direction, connectorName):
		return connectorName in self.processingConnectors(processingName, kind, direction)

	def areConnectable(self, kind, fromProcessing, fromConnector, toProcessing, toConnector) :
		return self.connectorType(fromProcessing, kind, Connector.Out, fromConnector) == self.connectorType(toProcessing, kind, Connector.In, toConnector)

	def connect(self, kind, fromProcessing, fromConnector, toProcessing, toConnector) :
		self._assertHasConnector(fromProcessing, kind, Connector.Out, fromConnector)
		self._assertHasConnector(toProcessing, kind, Connector.In, toConnector)

		assert self.areConnectable(kind,
			fromProcessing, fromConnector, toProcessing, toConnector
			), "%s.%s and %s.%s have incompatible types"%(
			fromProcessing, fromConnector, toProcessing, toConnector)
		assert not self.connectionExists(kind, fromProcessing, fromConnector, toProcessing, toConnector), "%s.%s and %s.%s already connected"%(fromProcessing, fromConnector, toProcessing, toConnector)
		connections = self._controlConnections if kind == Connector.Control else self._portConnections
		connections.append((fromProcessing, fromConnector, toProcessing, toConnector))
		return True

	def portConnections(self) :
		return self._portConnections

	def controlConnections(self) :
		return self._controlConnections

	def availableTypes(self) :
		return self._types.keys()

	def connectionExists(self, kind,
			fromProcessing, fromConnector,
			toProcessing, toConnector) :
		toFind = (fromProcessing, fromConnector, toProcessing, toConnector)
		connections = self._controlConnections if kind == Connector.Control else self._portConnections
		return toFind in connections

	def disconnect(self, kind, fromProcessing, fromConnector, toProcessing, toConnector) :
		if kind == Connector.Port:
			self._portConnections.remove((fromProcessing, fromConnector, toProcessing, toConnector))
		else:
			self._controlConnections.remove((fromProcessing, fromConnector, toProcessing, toConnector))
		return True

	def getDescription(self):
		return self._description

	def setDescription(self, description):
		self._description = description

	def deleteProcessing(self, processingName):
		self._assertHasProcessing(processingName)
		del self._processings[processingName]

	def _assertHasProcessing(self, name) :
		if not self.hasProcessing(name) :
			raise ProcessingNotFound(name)

	def _assertHasConnector(self, proc, kind, direction, connector) :
		self._assertHasProcessing(proc)
		if not self.processingHasConnector(proc, kind, direction, connector) :
			raise ConnectorNotFound(proc, kind, direction, connector)

