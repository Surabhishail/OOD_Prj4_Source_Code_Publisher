#pragma once
///////////////////////////////////////////////////////////////////////////
// Dependencies.h : defines dependency table structure                //
// ver 1.0                                                               //
//                                                                       // 
// Application   : OOD-S19 Instructor Solution                           //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Author        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines Dependencies class which is used to hold files
*  relationships after dependency analysis is performed. It is simply a
*  wrapper around std::unordered_map which uses string key and vector
*  value. Therefore, every file in the analysis will point to a vector
*  of all the files it depends upon.
*
*
*  Required Files:
* =======================
*  DependencyTable.h DependencyTable.cpp Logger.h Logger.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include "../CppParser/Parser/ActionsAndRules.h"
#include "../Logger/Logger.h"
#include <sstream>

using namespace CodeAnalysis;
using namespace Logging;

class Dependencies
{
public:
	using file = std::string;
	using files = std::vector<file>;
	using dependencies = std::vector<file>;
	using iterator =  std::unordered_map<file, dependencies>::iterator;


	Dependencies();
	Dependencies(const files& list);
	~Dependencies();

	bool has(const std::string& file);

	void addDependency(file filename, file dependency);
	void removeDependency(file filename, file dependency);

	void removeEntry(file filename);

	const dependencies& getDependencies(file filename) const;
	const dependencies& operator[](const file& filename) const;

	void getDependentFiles(file filename);

	// these two functions allow 'for' loops in the form of
	// for(auto entry : dt) {...}
	iterator begin();
	iterator end();
	void clear() { table_.clear(); }

	files getFiles() const;

	void display();

private:
	std::unordered_map<file, dependencies> table_;
};


// -----< default ctor >---------------------------------------------------
Dependencies::Dependencies() {
	LoggerDebug::write("\n  Created instance of DependencyTable using default ctor\n");
}

// -----< files initializer ctor >-----------------------------------------
/* sets the keys for the undordered map */
Dependencies::Dependencies(const files & list)
{
	std::cout << "\n  Created instance of Dependencies given files list\n";
	table_.reserve(list.size()); // improves performance 
	for (auto file : list)
		table_[file];
}

// -----< dtor - used only for logging >-----------------------------------
Dependencies::~Dependencies() {
	std::cout << "\n  Destroying Dependencies instance";
}

// -----< has function - searches map for given key >----------------------
bool Dependencies::has(const std::string & file)
{
	return table_.find(file) != table_.end();
}

// -----< add dependency function - adds filename to key's vector >--------
void Dependencies::addDependency(file filename, file dependency)
{
	std::cout << "\n  Adding dependency from " + filename + " to " + dependency;
	// this will create entry if key didnt exists in the map
	Dependencies::dependencies & deps = table_[filename];
	auto dep = std::find(deps.begin(), deps.end(), dependency);
	if (dep == deps.end()) // don't add depndency if it already exists
		deps.push_back(dependency);
}

// -----< remove dependency function >-------------------------------------
void Dependencies::removeDependency(file filename, file dependency)
{
	std::cout << "\n  Removing dependency from " + filename + " to " + dependency;
	Dependencies::dependencies & deps = table_[filename];
	auto dep = std::find(deps.begin(), deps.end(), dependency);
	if (dep != deps.end())
		deps.erase(dep);
}

// -----< remove entry - removes key/value pair from map >-----------------
void Dependencies::removeEntry(file filename)
{
	std::cout << "\n  Removing entry [" + filename + "] from dependency table";
	auto entry = table_.find(filename);
	if (entry != table_.end())
		table_.erase(entry);
}

// -----< get key's dependencies >-----------------------------------------
const Dependencies::dependencies & Dependencies::getDependencies(file filename) const
{
	return table_.at(filename);
}

// -----< operator [] to provide easier access >---------------------------
const Dependencies::dependencies & Dependencies::operator[](const file & filename) const
{
	return table_.at(filename);
}

void Dependencies::getDependentFiles(file filename) 
{
	std::string name;
	std::string depFile;
	std::vector<std::string> dependentFiles;
	Repository* pRepo = Repository::getInstance();
	pRepo->package() = name;

	ASTNode* pGlobalScope = pRepo->getGlobalScope();
	auto itr = pGlobalScope->statements_.begin();
	while (itr != pGlobalScope->statements_.end())
	{
		std::string depFile = (*itr)->ToString();
		int pos = depFile.find("\"");
		std::string dFile;
		dFile = depFile.substr(pos + 1, depFile.length() - 1);
		dFile.erase(std::remove(dFile.begin(), dFile.end(), '"'), dFile.end());
		//dependentFiles.push_back(dFile);
		addDependency(filename, dFile);
		std::cout << dFile << std::endl;

		std::cout << dFile.length();
		itr++;
	}
	//return dependentFiles;
}


// -----< begin() iterator - allows for loop >-----------------------------
Dependencies::iterator Dependencies::begin()
{
	return table_.begin();
}

// -----< end() iterator - allows for loop >-------------------------------
Dependencies::iterator Dependencies::end()
{
	return table_.end();
}

// -----< get keys of map >------------------------------------------------
Dependencies::files Dependencies::getFiles() const
{
	files list;
	for (auto entry : table_)
		list.push_back(entry.first);
	return list;
}

// -----< display entire table in formatted way >--------------------------
void Dependencies::display()
{
	std::cout << "\n  Dependency table:";
	for (auto entry : table_) {
		std::cout << "\n  -- " << entry.first;
		for (auto dep : entry.second)
			std::cout << "\n   -> " << dep;
	}
}

