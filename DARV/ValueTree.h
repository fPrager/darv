#pragma once
#include "stdafx.h"
#include <QTreeWidget>

class ValueTree
{
public:

	struct TreeElement
	{
		public:
			QString		_id;
			QString		_prefix;
			QString		_sufix;
			QTreeWidgetItem* _item;

			TreeElement(QString id, QString prefix, QString sufix)
			{
				_id		= id;
				_prefix = prefix;
				_sufix	= sufix;
				_item   = new QTreeWidgetItem();
			}
	};

	ValueTree(QTreeWidget* tree)
	{
		_tree = tree;
		_elements = QList<TreeElement*>();
		_header = QStringList();
	}
	~ValueTree(void)
	{}

	void SetHeaders(QStringList headers)
	{
		_header = headers;
		_tree->setHeaderLabels(headers);
	}

	int GetColumnIndex(QString id)
	{
		int index = -1;
		for(int i = 0; i < _header.size(); i++)
		{
			if(_header[i] == id)
				index = i;
		}
		if(index == -1)
		{
			qDebug() << "WARNING: No Header with " << id << "!";
			return 0;
		}
		else
			return index;
	}

	void SetColumnCnt(int count)
	{
		if(count != _tree->columnCount())
		{
			_tree->setColumnCount(count);
			for(int i = 0; i < _elements.size(); i++)
				UpdateElement(_elements[i]->_id, "");
		}
	}

	bool HasElement(QString id)
	{
		if(id == "")
		{
			return false;
		}
		else
			return (GetElement(id) != 0);
	}

	void AddElement(QString id, QString prefix, QString sufix, QString parentId = "")
	{
		if(id == "")
		{
			qDebug() << "No element-id is given";
			return;
		}

		if(GetElement(id) != 0)
		{
			qDebug() << "The ValueTree already has an element with the id " << id;
		}

		TreeElement* e = new TreeElement(id, prefix, sufix);
		if(parentId != 0)
		{
			TreeElement* parent = GetElement(parentId);
			if(parent == 0)
			{
				qDebug() << "No parent-element with the " << parentId << " (for the new element " << id << ")";
			}
			else
			{
				parent->_item->addChild(e->_item);
			}
		}
		else
		{
			_tree->addTopLevelItem(e->_item);
		}
		_elements.append(e);
		UpdateElement(e->_id, "");
	}

	void UpdateElement(QString id, QString value, int column)
	{
		TreeElement* e  = GetElement(id);

		if(e==0)
		{
			qDebug() << "Can't update the element " << id << ": no such element in tree";
			return;
		}

		if(column > _tree->columnCount())
		{
			qDebug() << "Can't update the element " << id << ": tree has no column " << column;
			return;
		}

		e->_item->setText(column, e->_prefix +value + e->_sufix);
	}

	void UpdateElement(QString id, QString value)
	{
		TreeElement* e  = GetElement(id);

		if(e==0)
		{
			qDebug() << "Can't update the element " << id << ": no such element in tree";
			return;
		}

		for(int i = 0; i < _tree->columnCount(); i++)
		{
			e->_item->setText(i, e->_prefix + value + e->_sufix);
		}
	}

	TreeElement* GetElement(QString id)
	{
		for(int i = 0; i < _elements.size(); i++)
		{
			if(_elements[i]->_id == id)
				return _elements[i];
		}
		return 0;
	}

	void Clear()
	{
		_elements.clear();
		_tree->clear();
	}

	QTreeWidget* _tree;
	QList<TreeElement*> _elements;
	QStringList _header;
};

