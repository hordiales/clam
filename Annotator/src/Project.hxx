/*
 * Copyright (c) 2001-2006 MUSIC TECHNOLOGY GROUP (MTG)
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

#ifndef _Project_hxx_
#define _Project_hxx_

#include "Schema.hxx"
#include <CLAM/DynamicType.hxx>
#include "SongFiles.hxx"
#include "InstantView.hxx"
#include <CLAM/Filename.hxx>
#include <CLAM/DescriptionScheme.hxx>
#include <vector>
#include <QtGui/QInputDialog>

#include <QtCore/QTemporaryFile>
#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#include <CLAM/XMLStorage.hxx>

class TaskRunner;

namespace CLAM_Annotator
{
class Schema;

class Extractor : public CLAM::DynamicType
{
	DYNAMIC_TYPE(Extractor,5);
	DYN_ATTRIBUTE(0, public, std::string, Name);
	DYN_ATTRIBUTE(1, public, CLAM::Filename, Extractor);
	DYN_ATTRIBUTE(2, public, CLAM::Text, Configuration);
	DYN_ATTRIBUTE(3, public, CLAM::Filename, Schema);
	DYN_ATTRIBUTE(4, public, std::string, PoolSuffix);
	void DefaultInit()
	{
		AddAll();
		UpdateData();
	}
	void CopyInit(const Extractor & copied)
	{
		_schema = copied._schema;
	}
private:
	Schema _schema;
	bool generateSchema(const QString & configFile, const QString schemaFile);
	void loadSchemaFile(const std::string & schemaFile);
public:
	bool generateSchema(QWidget * window);
	bool isRunnable(QWidget * window);
	bool computeDescriptors(QWidget * window, const QString & wavefile);
	const Schema & schema() const;
};

class AttributeMap : public CLAM::DynamicType
{
	DYNAMIC_TYPE(AttributeMap,5);
	DYN_ATTRIBUTE(0, public, std::string, TargetScope);
	DYN_ATTRIBUTE(1, public, std::string, TargetAttribute);
	DYN_ATTRIBUTE(2, public, std::string, Source);
	DYN_ATTRIBUTE(3, public, std::string, SourceScope);
	DYN_ATTRIBUTE(4, public, std::string, SourceAttribute);
	void DefaultInit()
	{
		AddAll();
		UpdateData();
	}
};

class Project : public CLAM::DynamicType
{
	DYNAMIC_TYPE(Project,9);
	DYN_ATTRIBUTE(0, public, CLAM::Text, Description);
	DYN_ATTRIBUTE(1, public, CLAM::Filename, Schema);
	DYN_ATTRIBUTE(2, public, CLAM::Filename, Extractor);
	DYN_ATTRIBUTE(3, public, std::string, PoolSuffix);
	DYN_ATTRIBUTE(4, public, CLAM::Text, Configuration);
	DYN_CONTAINER_ATTRIBUTE(5, public, std::vector<Extractor>, Sources, Source);
	DYN_CONTAINER_ATTRIBUTE(6, public, std::vector<AttributeMap>, Maps, Map);
	DYN_CONTAINER_ATTRIBUTE(7, public, std::vector<Song>, Songs, Song);
	DYN_CONTAINER_ATTRIBUTE(8, public, std::vector<InstantView>, Views, View);

	void DefaultInit()
	{
		AddAll();
		UpdateData();
	}
public:
	typedef std::list<CLAM_Annotator::SchemaAttribute> ScopeSchema;
public:
	void LoadFrom(CLAM::Storage & storage);
	void MapAllSchemaAttributes(Extractor & extractor);
	bool LoadSchema();
	void DumpSchema();
	void DumpSchema(std::ostream & os);

	const std::string & PoolSuffix() const
	{
		static std::string defaultSuffix = ".pool";
		if (!HasPoolSuffix()) return defaultSuffix;
		if (GetPoolSuffix()=="") return defaultSuffix;
		return GetPoolSuffix();
	}

	void InitInstance(const std::string & scope, unsigned instance, CLAM::DescriptionDataPool & data)
	{
		mSchema.InitInstance(scope, instance, data );
	}
	void InitializeMissingAttributes(CLAM::DescriptionDataPool & pool)
	{
		mSchema.InitMissingAttributes(pool);
	}
	std::string GetDescriptorsFileName(const std::string & songName)
	{
		std::vector<CLAM_Annotator::Song> fileNames = GetSongs();
		std::vector<CLAM_Annotator::Song>::iterator it = fileNames.begin();
		for (int i=0 ; it != fileNames.end(); it++, i++)
		{
			CLAM_Annotator::Song & song = *it;
			if (song.GetSoundFile() != songName) continue;
			if (song.HasPoolFile())
				return song.GetPoolFile();
			else
				return song.GetSoundFile() + PoolSuffix();
		}
		return ""; // BadName
	}
	void AppendSong(const std::string & songFileName)
	{
		std::vector<Song> & songs = GetSongs();
		songs.push_back(Song());
		songs.back().SetSoundFile(songFileName);
	}
	void RemoveSong(const std::string & songFileName)
	{
		std::cout << "Removing '"<<  songFileName << "'" << std::endl;
		std::vector<Song> & songs = GetSongs();
		for (std::vector<Song>::iterator it= songs.begin();
				it!= songs.end(); it++)
		{
			if (it->GetSoundFile() != songFileName) continue;
			std::cout << "Found to remove '"<<  it->GetSoundFile() << "'" << std::endl;
			songs.erase(it);
			return;
		}
	}
	CLAM::DescriptionScheme & GetDescriptionScheme()
	{
		return mDescriptionScheme;
	}
	CLAM_Annotator::Schema & GetAnnotatorSchema()
	{
		return mSchema;
	}
	ScopeSchema GetScopeSchema(const std::string & scope) const;
	std::list<std::string> GetNamesByScopeAndType(const std::string & scope, const std::string & type) const;
	bool ValidateDataPool(const CLAM::DescriptionDataPool & dataPool, std::ostream & err)
	{
		return GetAnnotatorSchema().Validate(dataPool,err);
	}
	const CLAM_Annotator::SchemaAttribute & GetParentAttribute(const std::string & scope) const;
	const CLAM_Annotator::SchemaAttribute & GetAttributeScheme(const std::string & scope, const std::string & name) const
	{
		return mSchema.GetAttribute(scope, name);
	}
	TaskRunner * CreateExtractionTaskRunner(const std::string & filename);
	void SetProjectPath(const std::string & path);
	const std::string & File() const { return mFile; }
	const std::string & BaseDir() const { return mBasePath; }
	std::string RelativeToAbsolute(const std::string & file) const;
	std::string AbsoluteToRelative(const std::string & file) const;
	void CreatePoolScheme();
	void dumpExtractorInfo(Extractor & extractor);
	std::string aggregationScript() const;
private:
	CLAM::DescriptionScheme mDescriptionScheme;
	CLAM_Annotator::Schema mSchema;
	std::string mFile;
	std::string mBasePath;
};

}

#endif//_Project_hxx_

