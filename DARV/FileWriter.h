#pragma once
#include <stdafx.h>
// using ofstream constructors.
#include <iostream>
#include <fstream>  
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <direct.h>

struct FileEntry
{
	public:
		QString _name;
		QStringList	_values;
		QString _lastValue;
		FileEntry(QString name)
		{
			_name = name;
			_values = QStringList();
			_lastValue = "--";
		}

		FileEntry(QString name, QString value)
		{
			_name = name;
			_values = QStringList();
			AddValue(value);
		}

		void AddValue(QString value)
		{
			_values.append(value);
			_lastValue = value;
		}
};

struct FileInfo
{
	public:
		QString _name;
		QList<FileEntry> _latestValues;
		FileInfo(QString name)
		{
			_name = name;
			_latestValues = QList<FileEntry>();
		}

		int FilledEntriesLength()
		{
			int maxLength = 0;
			foreach(FileEntry entry, _latestValues)
				if(entry._values.length() > maxLength)
					maxLength = entry._values.length();
			foreach(FileEntry entry, _latestValues)
			{
				while(entry._values.length() < maxLength)
					entry.AddValue("--");
			}
			return maxLength;
		}

		void ClearEntries()
		{
			for(int i = 0; i < _latestValues.length(); i++)
				_latestValues[i]._values.clear();
		}
};

class FileWriter
{
public:
	static FileWriter* GetInstance();
	bool MakeANewFolder(QString sufix = "", QStringList underFolderList = QStringList());
	bool SaveImage(QString folder, QString name, IplImage* data);  
	bool UpdateEntry(QString fileName, QString entryName, QString value);
	bool WriteToFile(QString timeStamp, QString fileName = "");
	bool CloseFile(QString fileName = "");
	void ClearFiles();
private:
	static	FileWriter*	_instance;
	void	MakeJSON(FileInfo* info);
	void	MakeXML(FileInfo* info);
	void	AddEntry(FileInfo* info, FileEntry entry);
	bool	SaveEntriesAsJSON(QString timeStamp, FileInfo* info);
	bool	SaveEntriesAsXML(QString timeStamp, FileInfo* info);
	QList<FileInfo*>		_files;
	FileWriter(void);
	~FileWriter(void);

	bool   _asJSON;
	QString _folderSufix;
};

