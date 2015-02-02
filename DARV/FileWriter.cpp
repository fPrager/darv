#include "stdafx.h"
#include "FileWriter.h"

FileWriter* FileWriter::_instance = NULL;

FileWriter::FileWriter(void)
{
	_asJSON = false;
	_folderSufix =  "";
}


FileWriter::~FileWriter(void)
{
}

FileWriter* FileWriter::GetInstance()
{
	if(_instance == 0)
	{
		_instance = new FileWriter();
	}
	return _instance;
}

bool FileWriter::MakeANewFolder(QString sufix, QStringList underFolderList)
{
	_folderSufix = sufix;
	mkdir(("Resources/Records/"+_folderSufix).toUtf8().constData());
	foreach(QString name, underFolderList)
	{
		mkdir(("Resources/Records/"+_folderSufix+"/"+name).toUtf8().constData());
	}
	return true;
}

bool FileWriter::SaveImage(QString folder, QString name, IplImage* data)
{
	return cvSaveImage(("Resources/Records/"+_folderSufix+"/"+folder+"/"+ name+ ".bmp").toUtf8().constData(), data);
}

bool FileWriter::UpdateEntry(QString fileName, QString entryName, QString value)
{
	bool fileIsIn = false;
	for(int i = 0; i < _files.size(); i++)
	{
		FileInfo* file = _files[i];
		if(file->_name == fileName)
		{
			AddEntry(file, FileEntry(entryName, value));
			fileIsIn = true;
		}
	}	
	if(!fileIsIn)
	{
		qDebug() << "FileWriter: Make a new file \"" << fileName << ".txt\"!";
		FileInfo* newFile = new FileInfo(fileName);
		if(_asJSON)
			MakeJSON(newFile);
		else
			MakeXML(newFile);
		AddEntry(newFile, FileEntry(entryName, value));
		_files.append(newFile);
	}
	return true;
}

bool FileWriter::WriteToFile(QString timeStamp, QString name)
{
	bool fileIsIn = false;
	for(int i = 0; i < _files.size(); i++)
	{
		FileInfo* file = _files[i];
		if(file->_name == name || name == "")
		{
			fileIsIn = true;
			if(_asJSON)
				return SaveEntriesAsJSON(timeStamp, file);
			else
				return SaveEntriesAsXML(timeStamp, file);
		}
	}	
	if(!fileIsIn)
	{
		qDebug() << "FileWriter: Make a new file \"" << name << ".txt\"!";
		FileInfo* newFile = new FileInfo(name);
		if(_asJSON)
			MakeJSON(newFile);
		else
			MakeXML(newFile);
		_files.append(newFile);
	}
	return true;
}

void FileWriter::AddEntry(FileInfo* info, FileEntry entry)
{
	bool hasThisEntry = false;
	for(int j = 0; j < info->_latestValues.size(); j++)
	{
		if(info->_latestValues[j]._name == entry._name)
		{
			info->_latestValues[j].AddValue(entry._lastValue);
			hasThisEntry = true;
		}
	}
	if(!hasThisEntry)
		info->_latestValues.append(entry);
}

void FileWriter::MakeJSON(FileInfo* info)
{
	std::ofstream outfile ("Resources/Records/"+_folderSufix.toStdString()+"/"+info->_name.toStdString() + ".txt");
	outfile.close();
}

void FileWriter::MakeXML(FileInfo* info)
{
	std::ofstream outfile ("Resources/Records/"+_folderSufix.toStdString()+"/"+info->_name.toStdString() + ".xml");
	 outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	 outfile << "<"+info->_name.toStdString()+">\n";
	outfile.close();
}

bool FileWriter::SaveEntriesAsJSON(QString timeStamp, FileInfo* info)
{
  std::ofstream myfile ("Resources/Records/"+_folderSufix.toStdString()+"/"+info->_name.toStdString() + ".xml", std::ios_base::app | std::ios_base::out);
  if (!myfile.is_open())
  {
		return false;
  }
  QString data = "[";
  data += timeStamp + " : [";
  for(int lineNr = 0; lineNr < info->FilledEntriesLength(); lineNr++)
  {
	 data += "[entry : ";
	for(int i = 0; i < info->_latestValues.size(); i++)
	{
		QString entry = "[" + info->_latestValues[i]._name + " : " + info->_latestValues[i]._values[lineNr] + "]";
		if(i!=0)
			data += "," + entry;
		else
		  data +=entry;
	}
	data += "]";
  }
  data += "]]";

  
  myfile.close();
  info->ClearEntries();
 return true;

}

bool FileWriter::SaveEntriesAsXML(QString timeStamp, FileInfo* info)
{
  std::ofstream myfile ("Resources/Records/"+_folderSufix.toStdString()+"/"+info->_name.toStdString() + ".xml", std::ios_base::app | std::ios_base::out);
  if (!myfile.is_open())
 {
		return false;
 }
 QString  data = "<data>\n<time> "+ timeStamp + "</time>\n";
 for(int lineNr = 0; lineNr < info->FilledEntriesLength(); lineNr++)
 {
	  data += "<entry>\n";
	  for(int i = 0; i < info->_latestValues.size(); i++)
	  {
		  QString entry = "<" + info->_latestValues[i]._name + ">" + info->_latestValues[i]._values[lineNr] + "</" + info->_latestValues[i]._name + ">\n";

			 data += entry;
		  //else
			 // line +=entry;
	  }
	  data += "</entry>\n";
 }
 data += "</data>\n";
 myfile << data.toStdString() << "";
 myfile.close();
   info->ClearEntries();
 return true;
}

bool FileWriter::CloseFile(QString fileName)
{
	if(_asJSON) 
		return true;

	std::ofstream myfile ("Resources/Records/"+_folderSufix.toStdString()+"/"+fileName.toStdString() + ".xml", std::ios_base::app | std::ios_base::out);
	if (myfile.is_open())
	  {
		myfile << "</" + fileName.toStdString() << ">";
		myfile.close();
		return true;
	  }
	  else 
		  return false;
}

void FileWriter::ClearFiles()
{
	for(int i = 0; i < _files.size(); i++)
	{
		FileInfo* file = _files[i];
		const char* name;
		if(_asJSON)
			name = ("Resources/Records/"+_folderSufix+"/"+file->_name+".txt").toUtf8().constData();
		else
			name = ("Resources/Records/"+_folderSufix+"/"+file->_name+".xml").toUtf8().constData();
		std::remove(name);
	}
	_files.clear();
}
