#include <boost/python.hpp>
#include <CLAM/Network.hxx>
#include <CLAM/ProcessingConfig.hxx>
#include <CLAM/ProcessingFactory.hxx>
#include <CLAM/XMLStorage.hxx>
#include "ConfigurationProxy.hxx"

namespace py = boost::python;

void throwPythonException(PyObject * type, std::string errorString)
{
	PyErr_SetString(type, errorString.c_str() );
	py::throw_error_already_set();
}

int getAttributeIndex(const ConfigurationProxy & config, const std::string & attribute)
{
	for(unsigned int i = 0; i < config.nAttributes(); ++i)
	{
		if ( attribute == config.attributeName(i) )
			return (int) i;
	}
	return -1;
}

ConfigurationProxy * createConfigurationProxy(const std::string & type)
{
	CLAM::Processing * proc = CLAM::ProcessingFactory::GetInstance().CreateSafe( type );
	const CLAM::ProcessingConfig & config = proc->GetConfig();
	ConfigurationProxy * configurationProxy = new ConfigurationProxy(config);
	return configurationProxy;
}

ConfigurationProxy * createConfigurationProxyWithProc(const std::string & type)
{
	CLAM::Processing * proc = CLAM::ProcessingFactory::GetInstance().CreateSafe( type );
	ConfigurationProxy * configurationProxy = new ConfigurationProxy(*proc);
	return configurationProxy;
}

std::string Dump(ConfigurationProxy & config)
{
	std::ostringstream str;
	CLAM::XMLStorage::Dump(*config._processingConfig, "Configuration", str);
	return str.str();
}

py::object getAttribute(ConfigurationProxy & config, const std::string & attribute)
{
	int index = getAttributeIndex(config, attribute);
	if (index == -1)
		throwPythonException(PyExc_KeyError, attribute.c_str());
	
	if ( !config.HasAttribute(index) )
		return py::object();

	ConfigurationProxyPlugin & plugin = ConfigurationProxyPlugin::GetPlugin(config, index);
	return plugin.getAttribute(config, index);
}

void setAttribute(ConfigurationProxy & config, const std::string & attribute, py::object value)
{
	int index = getAttributeIndex(config, attribute);
	if (index == -1)
		throwPythonException(PyExc_KeyError, attribute.c_str());

	if (value.ptr() == Py_None)
	{
		config.RemoveAttribute(index);
		return;
	}

	if ( !config.HasAttribute(index) )
		config.AddAttribute(index);

	ConfigurationProxyPlugin & plugin = ConfigurationProxyPlugin::GetPlugin(config, index);
	plugin.setAttribute(config, index, value);
}

py::list keys(ConfigurationProxy & config)
{
	py::list keys;
	for(unsigned int i = 0; i < config.nAttributes(); ++i)
		keys.append(config.attributeName(i));
	return keys;
}

bool nonDefault(ConfigurationProxy & config, const std::string & attribute)
{
	int index = getAttributeIndex(config, attribute);
	if (index == -1)
		throwPythonException(PyExc_KeyError, attribute.c_str());

	ConfigurationProxyPlugin & plugin = ConfigurationProxyPlugin::GetPlugin(config, index);
	return plugin.nonDefault(config, index);
}

void hold(ConfigurationProxy & config)
{
	config.hold();
}

void apply(ConfigurationProxy & config)
{
	config.apply();
}

ConfigurationProxy * cloneConfig(ConfigurationProxy & config)
{
	return config.cloneConfig();
}

void copyConfig(ConfigurationProxy & config, ConfigurationProxy & configuration)
{
	if (configuration._processingConfig)
		config._processing->Configure(*configuration._processingConfig);
	else
		config._processing->Configure(configuration._processing->GetConfig());
}

BOOST_PYTHON_MODULE(Clam_ConfigurationProxy)
{
	using namespace boost::python;

	def("createConfigurationProxy",
		createConfigurationProxy,
		return_value_policy<manage_new_object>()
		);

	def("createConfigurationProxyWithProc",
		createConfigurationProxyWithProc,
		return_value_policy<manage_new_object>()
		);

	class_<ConfigurationProxy>("Clam_ConfigurationProxy")
		.def("xml",
			Dump,
			"Returns the xml representation"
			)
		.def("__getitem__",
			getAttribute,
			"Returns the attribute value"
			)
		.def("__setitem__",
			setAttribute,
			"Sets a new value for the attribute"
			)
		.def("keys",
			keys,
			"Returns a list with the keys of the proxy"
			)
		.def("nonDefault",
			nonDefault,
			"Returns true if the attribute has been changed"
			)
		.def("hold",
			hold,
			"Tells the proxy that the changes should not be applied yet"
			)
		.def("apply",
			apply,
			"Applies all the changes done since hold"
			)
		.def("clone",
			cloneConfig,
			return_value_policy<manage_new_object>(),
			"Returns a new proxy thats a clone of this one"
			)
		.def("copyConfig",
			copyConfig,
			"Copies the values from the config passed to itself"
			)
		;
}
