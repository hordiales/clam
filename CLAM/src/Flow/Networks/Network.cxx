/*
 * Copyright (c) 2001-2007 MUSIC TECHNOLOGY GROUP (MTG)
 *                         UNIVERSITAT POMPEU FABRA
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include "Network.hxx"
#include "NaiveFlowControl.hxx"
#include "NetworkPlayer.hxx"
#include <algorithm>
#include "ProcessingDefinitionAdapter.hxx"
#include "ConnectionDefinitionAdapter.hxx"
#include "CommentAdapter.hxx"
#include "ProcessingFactory.hxx"
#include "XmlStorageErr.hxx"
#include "ControlSink.hxx"
#include "ControlSource.hxx"
#include "CLAMVersion.hxx"

#include <sstream>
#include <algorithm>
#include <iterator>


namespace CLAM
{	
	typedef std::pair<double, Processing *> ProcessingAndGeometry;

	Network::Network() :
		_name("Unnamed Network"),
		_flowControl(0),
		_player(0),
		_inPasteMode(false)
	{
		AddFlowControl( new NaiveFlowControl );
	}
	
	Network::~Network()
	{
		Clear();
		if (_flowControl) delete _flowControl;
		if (_player) delete _player;
	}

	void Network::StoreOn( Storage & storage) const
	{
		XMLAdapter<std::string> strAdapter( _name, "id");
		storage.Store(strAdapter);

		std::string version = CLAM::GetVersion();
		XMLAdapter<std::string> versionAdapter( version, "clamVersion");
		storage.Store(versionAdapter);
		
		if (not _description.empty())
		{
			XMLAdapter<Text> descriptionAdapter(_description, "description", true);
			storage.Store(descriptionAdapter);
		}
		
		ProcessingsMap::const_iterator it;
		for(it=BeginProcessings();it!=EndProcessings();it++)
		{
			Processing * proc = it->second;
			const std::string & name = it->first;
			if (SelectionAndDoesNotContain(name))
				continue;
			std::string processingPosition;
			std::string processingSize;
			// if exists canvas geometries, store them
			if (!_processingsGeometries.empty())
			{
				Geometry & geometry=_processingsGeometries.find(name)->second;
				processingPosition=IntsToString (geometry.x, geometry.y);
				processingSize=IntsToString (geometry.width, geometry.height);
			}
			ProcessingDefinitionAdapter procDefinition(proc, name, processingPosition, processingSize);
			XMLComponentAdapter xmlAdapter(procDefinition, "processing", true);
			storage.Store(xmlAdapter);
		}

		// second iteration to store ports. 
		// XR: maybe it should be all in one iteration but this way
		// the xml file is clearer.

		for(it=BeginProcessings();it!=EndProcessings();it++)
		{
			const std::string & name = it->first;
			Processing * proc = it->second;

			if (SelectionAndDoesNotContain(name))
				continue;

			unsigned nOutPorts = proc->GetNOutPorts();
			for (unsigned i = 0; i<nOutPorts; i++)
			{
				OutPortBase & outport = proc->GetOutPort(i);
				if (!outport.HasConnections())
					continue;
	
				std::string outPortName = name + "." + outport.GetName();
				NamesList namesInPorts = GetInPortsConnectedTo(outPortName);
				NamesList::iterator namesIterator;
				for(namesIterator=namesInPorts.begin();
				    namesIterator!=namesInPorts.end();
				    namesIterator++)
				{
					if (SelectionAndDoesNotContain(GetProcessingIdentifier(*namesIterator)))
						continue;
					ConnectionDefinitionAdapter connectionDefinition( outPortName, *namesIterator );
					XMLComponentAdapter xmlAdapter(connectionDefinition, "port_connection", true);
					storage.Store(xmlAdapter);
				}
			}
		}

		for(it=BeginProcessings();it!=EndProcessings();it++)
		{
			const std::string & name = it->first;
			Processing * proc = it->second;

			if (SelectionAndDoesNotContain(name))
				continue;

			unsigned nOutControls = proc->GetNOutControls();
			for (unsigned i = 0; i<nOutControls; i++)
			{
				OutControlBase & outControl = proc->GetOutControl(i);
				std::string outControlName = name+ "." + outControl.GetName();
				NamesList namesInControls = GetInControlsConnectedTo(outControlName);
				NamesList::iterator namesIterator;
				for(namesIterator=namesInControls.begin();
				    namesIterator!=namesInControls.end();
				    namesIterator++)
				{
					if (SelectionAndDoesNotContain(GetProcessingIdentifier(*namesIterator)))
						continue;
					ConnectionDefinitionAdapter connectionDefinition( outControlName, *namesIterator );
					XMLComponentAdapter xmlAdapter(connectionDefinition, "control_connection", true);
					storage.Store(xmlAdapter);
				}
			}
		}
		Comments::const_iterator ibIt;
		for(ibIt=BeginComments();ibIt!=EndComments();ibIt++)
		{
			CommentAdapter infoTextDefinition((*ibIt)->x, (*ibIt)->y, (*ibIt)->text);
			XMLComponentAdapter xmlAdapter(infoTextDefinition, "information", true);
			storage.Store(xmlAdapter);
		}

		_selectedProcessings.clear();
		_processingsGeometries.clear();
	}

	void Network::LoadFrom( Storage & storage)
	{
		typedef std::map <std::string, std::string> NamesMap;
		NamesMap namesMap;
		if (!_inPasteMode) Clear();
		XMLAdapter<std::string> strAdapter( _name, "id");
		storage.Load(strAdapter);
		_processingsGeometries.clear();

		XMLAdapter<Text> descriptionAdapter(_description, "description", true);
		if(not storage.Load(descriptionAdapter)) _description="";
	
		while(1)
		{
			ProcessingDefinitionAdapter procDefinition;
			XMLComponentAdapter xmlAdapter(procDefinition, "processing", true);
			if (not storage.Load(xmlAdapter)) break;

			const std::string & definitionName = procDefinition.GetName();
			CLAM::Processing * processing = procDefinition.GetProcessing();
			std::string finalName = definitionName;
			if (_inPasteMode)
			{
				finalName = GetUnusedName(definitionName, true);
				namesMap.insert(std::make_pair(definitionName,finalName));
			}
			// if exists canvas geometries, restore them
			if (procDefinition.GetPosition()!="" && procDefinition.GetSize()!="")
			{
				Geometry geometry;
				StringPairToInts(procDefinition.GetPosition(),geometry.x,geometry.y);
				StringPairToInts(procDefinition.GetSize(),geometry.width,geometry.height);
				_processingsGeometries.insert(ProcessingsGeometriesMap::value_type(finalName,geometry));
			}
			AddProcessing(finalName, processing, procDefinition.GetConfiguration());
		}

		while(1)
		{
			ConnectionDefinitionAdapter connectionDefinition;
			XMLComponentAdapter xmlAdapter(connectionDefinition, "port_connection", true);
			if (not storage.Load(xmlAdapter)) break;
			std::string fullOut = connectionDefinition.GetOutName();
			std::string fullIn = connectionDefinition.GetInName();

			if (_inPasteMode)
			{
				fullOut = namesMap.find(GetProcessingIdentifier(fullOut))->second
					+"."+GetConnectorIdentifier(fullOut);
				fullIn = namesMap.find(GetProcessingIdentifier(fullIn))->second
					+"."+GetConnectorIdentifier(fullIn);
			}

			if (CheckForBrokenConnection(fullOut, fullIn, PORT_CONNECTION))
				continue;

			if (not ConnectPorts( fullOut, fullIn ))
			{
				throw XmlStorageErr(std::string("Unable to connect ports '")+fullOut+"->"+fullIn+".");
			}
		}

		while(1)
		{
			ConnectionDefinitionAdapter connectionDefinition;
			XMLComponentAdapter xmlAdapter(connectionDefinition, "control_connection", true);
			if (!storage.Load(xmlAdapter)) break;
			std::string fullOut = connectionDefinition.GetOutName();
			std::string fullIn = connectionDefinition.GetInName();
			if (_inPasteMode)
			{
				fullOut = namesMap.find(GetProcessingIdentifier(fullOut))->second
					+"."+GetConnectorIdentifier(fullOut);
				fullIn = namesMap.find(GetProcessingIdentifier(fullIn))->second
					+"."+GetConnectorIdentifier(fullIn);
			}
			if (CheckForBrokenConnection(fullOut, fullIn, CONTROL_CONNECTION))
				continue;
			if (not ConnectControls( fullOut, fullIn ))
				throw XmlStorageErr(std::string("Unable to connect controls '")+fullOut+"->"+fullIn+".");
		}

		while(true and not _inPasteMode)
		{
			CommentAdapter infoTextDefinition;
			XMLComponentAdapter xmlAdapter(infoTextDefinition, "information", true);
			if (not storage.Load(xmlAdapter)) break;
			
			Comment *myComment=new Comment();
			myComment->x=infoTextDefinition.GetCoordX();
			myComment->y=infoTextDefinition.GetCoordY();
			myComment->text=infoTextDefinition.GetText();			

			_informationTexts.push_back(myComment);
		}

		_inPasteMode=false;
	}

	bool Network::UpdateSelections (const NamesList & processingsNamesList)
	{
		if (!_selectedProcessings.empty() || processingsNamesList.empty())
		{
			_selectedProcessings.clear();
			return true;
		}
		NamesList::const_iterator namesIterator;
		for (namesIterator=processingsNamesList.begin();namesIterator!=processingsNamesList.end();namesIterator++)
			_selectedProcessings.insert(*namesIterator);
		return false;
	}

	bool Network::SelectionAndDoesNotContain(const std::string & name) const
	{
		if (_selectedProcessings.empty()) return false;
		return _selectedProcessings.count(name)==0;
	}


	const Network::Geometry & Network::getProcessingGeometry(const std::string & processingName) const
	{
		ProcessingsGeometriesMap::const_iterator it =
			_processingsGeometries.find(processingName);
		if (it!=_processingsGeometries.end())
			return it->second;
		static Geometry nullGeometry={0,10000,0,0};
		return nullGeometry;
	}

	const Network::Processings Network::getOrderedProcessings(const std::string & type, bool horizontalOrder) const
	{
		std::list <ProcessingAndGeometry> processingsAndGeometries;
		for (ProcessingsMap::const_iterator it=_processings.begin(); it!=_processings.end(); it++)
		{
			Processing * proc = it->second;
			const std::string className = proc->GetClassName();
			if (className!=type) continue;
			const Geometry & geometry = getProcessingGeometry(it->first);
			int position = horizontalOrder ? geometry.x : geometry.y;
			processingsAndGeometries.push_back(std::make_pair(position, proc));
		}
		processingsAndGeometries.sort();

		Processings orderedProcessings;
		for (std::list<ProcessingAndGeometry>::const_iterator it=processingsAndGeometries.begin();
			it!=processingsAndGeometries.end();it++)
		{
			orderedProcessings.push_back( it->second );
		}

		return orderedProcessings;
	}

	const Network::Processings Network::getOrderedProcessingsByAttribute(const std::string & attribute, bool horizontalOrder) const
	{
		ProcessingFactory & factory = ProcessingFactory::GetInstance();
		std::list <ProcessingAndGeometry> processingsAndGeometries;
		for (ProcessingsMap::const_iterator it=_processings.begin(); it!=_processings.end(); it++)
		{
			Processing * proc = it->second;
			const std::string className = proc->GetClassName();
			if (!factory.AttributeExists(className,attribute)) continue;
			const Geometry & geometry = getProcessingGeometry(it->first);
			int position = horizontalOrder? geometry.x : geometry.y;
			processingsAndGeometries.push_back(std::make_pair(position, proc));
		}
		processingsAndGeometries.sort();

		Processings orderedProcessings;
		for (std::list<ProcessingAndGeometry>::const_iterator it=processingsAndGeometries.begin();
			it!=processingsAndGeometries.end();it++)
		{
			orderedProcessings.push_back( it->second );
		}
		return orderedProcessings;
	}

	const Network::ControlSinks Network::getOrderedControlSinks() const
	{
		std::list <ProcessingAndGeometry> processingsAndGeometries;
		for (ProcessingsMap::const_iterator it=_processings.begin(); it!=_processings.end(); it++)
		{
			Processing * proc = it->second;
			const std::string className = proc->GetClassName();
			if (className!="ControlSink") continue;
			const Geometry & geometry = getProcessingGeometry(it->first);
			int position = geometry.x;
			processingsAndGeometries.push_back(std::make_pair(position, proc));
		}
		processingsAndGeometries.sort();

		ControlSinks orderedControlSinks;
		for (std::list<ProcessingAndGeometry>::const_iterator it=processingsAndGeometries.begin();
			it!=processingsAndGeometries.end();it++)
		{
			ControlSink* controlSink = dynamic_cast<ControlSink*>(it->second);
			CLAM_ASSERT(controlSink, "Expected an AudioSink");
			orderedControlSinks.push_back( controlSink );
		}
		return orderedControlSinks;
	}
	
	const Network::ControlSources Network::getOrderedControlSources() const
	{
		std::list <ProcessingAndGeometry> processingsAndGeometries;
		for (ProcessingsMap::const_iterator it=_processings.begin(); it!=_processings.end(); it++)
		{
			Processing * proc = it->second;
			const std::string className = proc->GetClassName();
			if (className!="ControlSource") continue;
			const Geometry & geometry = getProcessingGeometry(it->first);
			int position = geometry.x;
			processingsAndGeometries.push_back(std::make_pair(position, proc));
		}
		processingsAndGeometries.sort();

		ControlSources orderedControlSources;
		for (std::list<ProcessingAndGeometry>::const_iterator it=processingsAndGeometries.begin();
			it!=processingsAndGeometries.end();it++)
		{
			ControlSource* controlSource = dynamic_cast<ControlSource*>(it->second);
			CLAM_ASSERT(controlSource, "Expected an AudioSink");
			orderedControlSources.push_back( controlSource );
		}
		return orderedControlSources;
	}

	bool Network::SetProcessingsGeometries (const ProcessingsGeometriesMap & processingsGeometries)
	{
		_processingsGeometries.clear(); 
		if (processingsGeometries.empty())
			return true;
		_processingsGeometries=processingsGeometries;
		return false;
	}

	const Network::ProcessingsGeometriesMap Network::GetAndClearGeometries()
	{
		const ProcessingsGeometriesMap copyProcessingsGeometry(_processingsGeometries);
		_processingsGeometries.clear();
		return copyProcessingsGeometry;
	}

	void Network::StringPairToInts(const std::string & geometryInString, int & a, int & b)
	{
		a=atoi(geometryInString.substr(0,geometryInString.find(",")).c_str());
		b=atoi(geometryInString.substr(geometryInString.find(",")+1,geometryInString.length()).c_str());
	}

	const std::string Network::IntsToString (const int & a, const int & b) const
	{
		std::ostringstream stream;
		stream<<a<<","<<b;
		return stream.str();
	}

	// flow and player related methods
	void Network::AddFlowControl(FlowControl* flowControl)
	{
		if (_flowControl) delete _flowControl;
		_flowControl = flowControl;
		_flowControl->AttachToNetwork(this);
	}
	void Network::SetPlayer(NetworkPlayer* player)
	{
		if (_player) delete _player;
		_player = player;
		_player->SetNetworkBackLink(*(CLAM::Network*)this);
		_player->Init();
	}
	unsigned Network::BackendBufferSize()
	{
		if (!_player) return 512;
		return _player->BackendBufferSize();
	}
	unsigned Network::BackendSampleRate()
	{
		if (!_player) return 44100;
		return _player->BackendSampleRate();
	}


	Processing& Network::GetProcessing( const std::string & name ) const
	{
		CLAM_ASSERT( HasProcessing(name), 
			("No processing in the network has the name '"+name+"'.").c_str());

		ProcessingsMap::const_iterator it = _processings.find( name );
		return *it->second;
	}
	std::string Network::GetProcessingName( Processing & processing ) const
	{
		for (ProcessingsMap::const_iterator it=_processings.begin(); it!=_processings.end(); it++)
		{
			if (it->second == &processing)
				return it->first;
		}
		CLAM_ASSERT(false, "Network::GetProcessingName the processing provided as argument is not in the network");
		return "";
	}

	void Network::AddProcessing( const std::string & name, Processing* proc, const ProcessingConfig * config)
	{
		if (!IsStopped()) Stop();

		if (!_processings.insert( ProcessingsMap::value_type( name, proc ) ).second )
			CLAM_ASSERT(false, 
			("Network::AddProcessing() Trying to add a processing with a repeated name (key): '"+name+"'." ).c_str() );
		proc->SetNetworkBackLink((CLAM::Network*)this);
		proc->Configure(config ? *config : proc->GetConfig());
		_flowControl->ProcessingAddedToNetwork(*proc);
	}

	Processing & Network::AddProcessing( const std::string & name, const std::string & factoryKey )
	{
		Processing * proc = ProcessingFactory::GetInstance().CreateSafe( factoryKey  );
		AddProcessing(name, proc);
		return *proc;
	}

	std::string Network::AddProcessing( const std::string & factoryKey )
	{
		std::string name = GetUnusedName( factoryKey  ); 
		AddProcessing(name, factoryKey );
		return name;
	}

	std::string Network::GetUnusedName( const std::string& prefix, const bool cutOnLastSeparator, const std::string separator ) const
	{
		std::string name;
		std::string newPrefix=prefix;
		if (cutOnLastSeparator==true)
		{
			size_t lastSeparatorPos=prefix.rfind(separator);
			if (lastSeparatorPos!=std::string::npos)
				newPrefix=prefix.substr(0,lastSeparatorPos);
		}

		for ( int i = 0; i<9999999; i++ )
		{
			std::stringstream tmp;
			tmp << i;
			name = i? newPrefix + separator + tmp.str() : newPrefix;
			if (!this->HasProcessing( name ) ) return name;
		}
		CLAM_ASSERT(false, "All valid id's for given prefix are exhausted");
		return "";
	}

	void Network::RemoveProcessing ( const std::string & name)
	{
		CLAM_ASSERT( _flowControl, 
			     "Network::RemoveProcessing() - Network should have an attached flow control at this state.");

		ProcessingsMap::const_iterator i = _processings.find( name );
		if(i==_processings.end())
		{
			std::string msg("Network::RemoveProcessing() Trying to remove a processing that is not included in the network:");
			msg += name;
			CLAM_ASSERT(false, msg.c_str() );
		}
		if ( !IsStopped() ) Stop(); 
		Processing * proc = i->second;
		_processings.erase( name );

		_flowControl->ProcessingRemovedFromNetwork(*proc);
		delete proc;		
	}

	bool Network::HasProcessing( const std::string& name ) const
	{
		ProcessingsMap::const_iterator i = _processings.find( name );
		return i!=_processings.end();
	}
	
	bool Network::ConfigureProcessing( const std::string & name, const ProcessingConfig & newConfig )	
	{
		ProcessingsMap::iterator it = _processings.find( name );
		CLAM_ASSERT(it!=_processings.end(),"Wrong processing name to configure in a network");
		Processing * proc = it->second;
		if ( !IsStopped() ) Stop();
		bool ok = proc->Configure( newConfig );
		_flowControl->ProcessingConfigured(*proc);
		return ok;
	}

	void Network::ReconfigureAllProcessings()
	{
		ProcessingsMap::iterator it;
		for( it=_processings.begin(); it!=_processings.end(); it++)
		{
			Processing* proc = it->second;
			proc->Configure( proc->GetConfig() );
		}
	}

	bool Network::ConnectPorts( const std::string & producer, const std::string & consumer )
	{
		_flowControl->NetworkTopologyChanged();

		OutPortBase & outport = GetOutPortByCompleteName(producer);
		InPortBase & inport = GetInPortByCompleteName(consumer);

		if ( outport.IsVisuallyConnectedTo(inport) ) 
			return false;
			
		if ( !outport.IsConnectableTo(inport) ) //they have different type
			return false;

		if( inport.GetVisuallyConnectedOutPort())
			return false;

		if (!IsStopped()) Stop();

		outport.ConnectToIn( inport );
		return true;
	}

	bool Network::ConnectControls( const std::string & producer, const std::string & consumer )
	{
		OutControlBase & outcontrol = GetOutControlByCompleteName(producer);
		InControlBase & incontrol = GetInControlByCompleteName(consumer);

		if ( outcontrol.IsConnectedTo(incontrol) ) 
			return false;

		if ( !outcontrol.IsLinkable(incontrol) ) //they have different type
			return false;

		if (!IsStopped()) Stop();

		outcontrol.AddLink( incontrol );
		return true;
	}


	bool Network::DisconnectPorts( const std::string & producer, const std::string & consumer)
	{
		_flowControl->NetworkTopologyChanged();

		OutPortBase & outport = GetOutPortByCompleteName(producer);
		InPortBase & inport = GetInPortByCompleteName(consumer);

		if ( !outport.IsVisuallyConnectedTo(inport))
			return false;

		if (!IsStopped()) Stop();

		outport.DisconnectFromIn( inport );
		return true;
	}

	bool Network::DisconnectControls( const std::string & producer, const std::string & consumer)
	{
		OutControlBase & outcontrol = GetOutControlByCompleteName(producer);
		InControlBase & incontrol = GetInControlByCompleteName(consumer);

		if ( !outcontrol.IsConnectedTo( incontrol )) 
			return false;

		if (!IsStopped()) Stop();

		outcontrol.RemoveLink( incontrol );
		return true;
	}

	std::string Network::GetConnectorIdentifier( const std::string& str ) const
	{
		return str.substr( PositionOfLastIdentifier(str)+1 );
	}

	std::string Network::GetProcessingIdentifier( const std::string& str ) const
	{
		std::size_t length = PositionOfLastIdentifier(str)  - PositionOfProcessingIdentifier(str);
		return str.substr( PositionOfProcessingIdentifier(str), length);
	}

	InPortBase & Network::GetInPortByCompleteName( const std::string & name ) const
	{
		Processing& proc = GetProcessing( GetProcessingIdentifier(name) );
		return proc.GetInPort( GetConnectorIdentifier(name) );
	}

	OutPortBase & Network::GetOutPortByCompleteName( const std::string & name ) const
	{
		Processing& proc = GetProcessing( GetProcessingIdentifier(name) );
		return proc.GetOutPort( GetConnectorIdentifier(name) );
	}

	InControlBase & Network::GetInControlByCompleteName( const std::string & name ) const
	{
		Processing& proc = GetProcessing( GetProcessingIdentifier(name) );
		return proc.GetInControl( GetConnectorIdentifier(name) );
	}

	OutControlBase & Network::GetOutControlByCompleteName( const std::string & name ) const
	{
		Processing& proc = GetProcessing( GetProcessingIdentifier(name) );
		return proc.GetOutControl( GetConnectorIdentifier(name) );
	}

	bool Network::IsStopped() const
	{
		if (! _player) 
			return true;
		return _player->IsStopped();
	}

	bool Network::IsPlaying() const
	{
		if (! _player) 
			return false;
		return _player->IsPlaying();
	}

	bool Network::IsPaused() const
	{
		if (! _player) 
			return false;
		return _player->IsPaused();
	}

	bool Network::IsRealTime() const
	{
		if (! _player)
			return false;
		return _player->IsRealTime();
	}

	void Network::Start()
	{
		ProcessingsMap::iterator it;
		for (it=BeginProcessings(); it!=EndProcessings(); it++)
		{
			if (it->second->IsRunning()) 
			{
				continue;
			}
			if (it->second->IsConfigured())
			{
				it->second->Start();
			}
			else
			{	
				std::cerr << "Warning: could not start processing for not being Configured: '" 
						<< it->first<< "' of class " << it->second->GetClassName() << std::endl;
			}
		}
		if (_player) 
			_player->Start();
	}
	
	void Network::Stop()
	{
		if (_player) _player->Stop();
		ProcessingsMap::iterator it;
		for (it=BeginProcessings(); it!=EndProcessings(); it++)
			if (it->second->IsRunning())
				it->second->Stop();
	}
	void Network::Pause()
	{
		if (_player) _player->Pause();
	}
	
	void Network::Do()
	{
		_flowControl->Do();
	}

	void Network::Clear()
	{
		if ( !IsStopped() ) Stop(); 
		
		while( !_processings.empty() )
		{
			//std::cerr << "REMOVING <"<<_processings.begin()->first<<">"<<std::endl;
			RemoveProcessing( _processings.begin()->first );
		}
		for(unsigned i=0;i<_informationTexts.size();i++)
		{
			removeComment(_informationTexts[i]);
		}
		_informationTexts.clear();
	}

	Network::ProcessingsMap::iterator Network::BeginProcessings()
	{
		return _processings.begin();
	}

	Network::ProcessingsMap::iterator Network::EndProcessings()
	{
		return _processings.end();
	}
	Network::ProcessingsMap::const_iterator Network::BeginProcessings() const
	{
		return _processings.begin();
	}

	Network::ProcessingsMap::const_iterator Network::EndProcessings() const
	{
		return _processings.end();
	}

	// accessors to text boxes

	void Network::addComment(Comment * informationText)
	{
		_informationTexts.push_back(informationText);
	}

	void Network::removeComment(Comment * informationText)
	{
		Comments::iterator it = find(BeginComments(), EndComments(), informationText);
	
		if(it!=EndComments())
			_informationTexts.erase(it);
		else
			std::cerr << "Warning: Information Text Box does not exist.";
	}

	Network::Comments::iterator Network::BeginComments()
	{
		return _informationTexts.begin();
	}
	
	Network::Comments::iterator Network::EndComments()
	{
		return _informationTexts.end();
	}
	
	Network::Comments::const_iterator Network::BeginComments() const
	{
		return _informationTexts.begin();
	}
	
	Network::Comments::const_iterator Network::EndComments() const
	{
		return _informationTexts.end();
	}


	Network::NamesList  Network::GetInPortsConnectedTo( const std::string & producer ) const
	{		
		OutPortBase & out = GetOutPortByCompleteName( producer );
		NamesList consumers;

		if(!out.HasConnections())
			return consumers;

		OutPortBase::InPortsList::iterator it;
		for(it=out.BeginVisuallyConnectedInPorts(); it!=out.EndVisuallyConnectedInPorts(); it++)
		{
			std::string completeName(GetNetworkId((*it)->GetProcessing()));
			completeName += ".";
			completeName += (*it)->GetName();
			consumers.push_back(completeName);
		}
		return consumers;
	}

	Network::NamesList  Network::GetInControlsConnectedTo( const std::string & producer ) const
	{		
		OutControlBase & out = GetOutControlByCompleteName( producer );
		NamesList consumers;

		std::list<InControlBase*>::iterator it;
		for(it=out.BeginInControlsConnected();
		    it!=out.EndInControlsConnected();
		    it++)
		{
			std::string completeName(GetNetworkId((*it)->GetProcessing()));
			completeName += ".";
			completeName += (*it)->GetName();
			consumers.push_back(completeName);
		}
		return consumers;
	}

	Network::NamesList  Network::GetOutControlsConnectedTo( const std::string & producer ) const
	{		
		InControlBase & in = GetInControlByCompleteName( producer );
		NamesList consumers;

		std::list<OutControlBase*>::iterator it;
		for(it=in.BeginOutControlsConnected();
		    it!=in.EndOutControlsConnected();
		    it++)
		{
			std::string completeName(GetNetworkId((*it)->GetProcessing()));
			completeName += ".";
			completeName += (*it)->GetName();
			consumers.push_back(completeName);
		}
		return consumers;
	}

	Network::InPortsList Network::GetInPortsConnectedTo( OutPortBase & producer ) const
	{		
		InPortsList consumers;
		OutPortBase::InPortsList::iterator it;
		for(it=producer.BeginVisuallyConnectedInPorts(); it!=producer.EndVisuallyConnectedInPorts(); it++)
			consumers.push_back(*it);
		return consumers;
	}

	const std::string &  Network::GetNetworkId(const Processing * proc) const
	{
		ProcessingsMap::const_iterator it;
		for(it=BeginProcessings(); it!=EndProcessings(); it++)
			if(it->second == proc )
				return it->first;

		CLAM_ASSERT(false, "Trying to get a network id from a processing not present in it");
		throw 0; // To avoid warning message
	}

	bool Network::RenameProcessing( const std::string & oldName, const std::string & newName )
	{
		if (oldName==newName) return true;
		if( _processings.find( newName ) != _processings.end() ) // newName is being used
			return false;
		ProcessingsMap::iterator it = _processings.find( oldName );
		Processing * proc = it->second;
		_processings.erase( it );
		_processings.insert( ProcessingsMap::value_type( newName, proc ) );
		return true;
	}

	bool Network::IsReady() const
	{
		if (IsEmpty()) return false;
		if (HasMisconfiguredProcessings()) return false;
		if (HasUnconnectedInPorts()) return false;
		return true;
	}

	bool Network::IsEmpty() const
	{
		return _processings.empty();
	}

	bool Network::HasMisconfiguredProcessings() const
	{
		ProcessingsMap::const_iterator it;
		for(it=BeginProcessings(); it!=EndProcessings(); it++)
			if(!it->second->IsConfigured())
				return true;
		return false;
	}

	bool Network::HasUnconnectedInPorts() const
	{
		for (ProcessingsMap::const_iterator it=BeginProcessings(); it!=EndProcessings(); it++)
		{
			Processing * proc = it->second;
			unsigned nInPorts = proc->GetNInPorts();
			for (unsigned i = 0; i<nInPorts; i++)
			{
				InPortBase & inPort = proc->GetInPort(i);
				if (not inPort.GetVisuallyConnectedOutPort())
					return true;
			}
		}
		return false;
	}
	std::string Network::GetUnconnectedInPorts() const
	{
		std::string result;
		for (ProcessingsMap::const_iterator it=BeginProcessings(); it!=EndProcessings(); it++)
		{
			Processing * proc = it->second;
			unsigned nInPorts = proc->GetNInPorts();
			for (unsigned i = 0; i<nInPorts; i++)
			{
				InPortBase & inPort = proc->GetInPort(i);
				if (not inPort.GetVisuallyConnectedOutPort())
					result+= it->first + "." + inPort.GetName() + "\n";
			}
		}
		return result;
	}

	bool Network::HasSyncSource() const
	{
		ProcessingsMap::const_iterator it;
		for(it=BeginProcessings(); it!=EndProcessings(); it++)
			if(it->second->IsSyncSource())
				return true;
		return false;
	}

	bool Network::SupportsVariableAudioSize() const
	{
	    ProcessingsMap::const_iterator it;
		for(it=BeginProcessings(); it!=EndProcessings(); it++)
			if(!it->second->SupportsVariableAudioSize())
				return false;
		return true;
    }

	// TODO: Test GetConfigurationErrors
	std::string Network::GetConfigurationErrors() const
	{
		std::ostringstream errorMessage;
		ProcessingsMap::const_iterator it;
		for(it=BeginProcessings(); it!=EndProcessings(); it++)
		{
			if(it->second->IsConfigured()) continue;
			errorMessage << "* Processing '" <<  it->first  << "' is misconfigured:\n";
			errorMessage << it->second->GetConfigErrorMessage() << std::endl;
		}

		return errorMessage.str();
	}

	bool Network::CheckForBrokenConnection( const std::string & producer, const std::string & consumer, ConnectionType const connectionType )
	{
		bool brokenConnection = false;

		//std::cout << "Checking port and control connections, producer=" << producer << " consumer=" << consumer << std::endl;

		Processing& prod = GetProcessing( GetProcessingIdentifier(producer) );
		Processing& cons = GetProcessing( GetProcessingIdentifier(consumer) );

		if (connectionType == PORT_CONNECTION) 
		{
			if (!prod.HasOutPort(GetConnectorIdentifier(producer)))
				 brokenConnection = true;

			if (!cons.HasInPort(GetConnectorIdentifier(consumer)))
				 brokenConnection = true;
		}
		if (connectionType == CONTROL_CONNECTION) 
		{
			if (!prod.HasOutControl(GetConnectorIdentifier(producer)))
				 brokenConnection = true;
			if (!cons.HasInControl(GetConnectorIdentifier(consumer)))
				 brokenConnection = true;
		}

		if (brokenConnection)
		{
			_brokenConnections.push_back(producer + " -> " + consumer);
		}

		return brokenConnection;
	} 

	Network::ConnectionState Network::GetConnectionReport() const
	{
		std::ostringstream os;
		std::copy(_brokenConnections.begin(), _brokenConnections.end(), 
				  std::ostream_iterator<std::string> (os, "<br/> "));

		return std::make_pair(_brokenConnections.size() != 0, os.str());
	}

	void Network::ResetConnectionReport()
	{
		_brokenConnections.clear();
	}
}

