///////////////////////////////////////////////////////////////////////////
// DependencyTable.cpp : defines DependencyTable.h functions             //
// ver 1.1                                                               //
//                                                                       // 
// Application   : Project Code Publisher                                //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
// Author        : Surabhi Shail, sshail@syr.edu                        //
///////////////////////////////////////////////////////////////////////////

#include "DependencyTable.h"
#include <sstream>
#include "../FileSystem/FileSystem.h"
#include "../CppParser/AbstractSyntaxTree/AbstrSynTree.h"
#include "../CppParser/Parser/ConfigureParser.h"
#include "../CppParser/Parser/ActionsAndRules.h"
#include "../CppParser/Utilities/Utilities.h"
#define Util StringHelper

using namespace CodeAnalysis;

// -----< default ctor >---------------------------------------------------
DependencyTable::DependencyTable(){
  std::cout << "\n  Created instance of DependencyTable using default ctor\n";
}

// -----< files initializer ctor >-----------------------------------------
/* sets the keys for the undordered map */
DependencyTable::DependencyTable(const files & list)
{
	std::cout << "\n  Created instance of DependencyTable given files list\n";
  table_.reserve(list.size()); // improves performance 
  for (auto file : list)
    getFileDependencies(file);
}

// -----< dtor - used only for logging >-----------------------------------
DependencyTable::~DependencyTable(){
	std::cout << "\n  Destroying DependencyTable instance";
}

// -----< has function - searches map for given key >----------------------
bool DependencyTable::has(const std::string & file)
{
  return table_.find(file) != table_.end();
}

// -----< add dependency function - adds filename to key's vector >--------
void DependencyTable::addDependency(file filename, file dependency)
{
	std::cout << "\n  Adding dependency from " + filename + " to " + dependency;
  // this will create entry if key didnt exists in the map
  DependencyTable::dependencies & deps = table_[filename]; 
  auto dep = std::find(deps.begin(), deps.end(), dependency);
  if (dep == deps.end()) // don't add depndency if it already exists
    deps.push_back(dependency);
}

// -----< remove dependency function >-------------------------------------
void DependencyTable::removeDependency(file filename, file dependency)
{
  std::cout << "\n  Removing dependency from " + filename + " to " + dependency;
  DependencyTable::dependencies & deps = table_[filename];
  auto dep = std::find(deps.begin(), deps.end(), dependency);
  if (dep != deps.end())
    deps.erase(dep);
}

// -----< remove entry - removes key/value pair from map >-----------------
void DependencyTable::removeEntry(file filename)
{
	std::cout << "\n  Removing entry [" + filename + "] from dependency table";
  auto entry = table_.find(filename);
  if (entry != table_.end())
    table_.erase(entry);
}

// -----< get key's dependencies >-----------------------------------------
const DependencyTable::dependencies & DependencyTable::getDependencies(file filename) const
{
  return table_.at(filename);
}

// -----< operator [] to provide easier access >---------------------------
const DependencyTable::dependencies & DependencyTable::operator[](const file & filename) const
{
  return table_.at(filename);
}

// -----< extracts dependency and adds dependency to dependency table >---
void DependencyTable::getFileDependencies(file filename)
{
	std::string fileSpec;
	std::string name;
	fileSpec = FileSystem::Path::getFullFileSpec(filename);
	std::string msg = "Processing file: " + fileSpec;
	Util::title(msg);
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();

	try{
		if (pParser){
			name = FileSystem::Path::getName(filename);
			if (!configure.Attach(fileSpec))
				std::cout << "\n  could not open file " << name << std::endl;
		}
		else
			std::cout << "\n\n  Parser not built\n\n";
		Repository* pRepo = Repository::getInstance(); // save current package name
		pRepo->package() = name;
		// parse the package
		while (pParser->next())
			pParser->parse();
		std::cout << "\n";
		// final AST operations
		int count = 0;
		ASTNode* pGlobalScope = pRepo->getGlobalScope();
		auto itr = pGlobalScope->statements_.begin();
		while (itr != pGlobalScope->statements_.end()){
			std::string depFile = (*itr)->ToString();
			int pos = depFile.find("\"");
			if (pos >= 0) {
				std::string dFile;
				dFile = depFile.substr(pos + 1, depFile.length() - 1);
				dFile.erase(std::remove(dFile.begin(), dFile.end(), '"'), dFile.end());
				addDependency(filename, dFile);
				count++;
			}
			itr++;
		}
		if (count == 0) {
			addDependency(filename, " ");
			std::cout << "None. \nFile has no dependencies.\n";
		}
	}
	catch (std::exception ex) {
		std::cout << "\n\n    " << ex.what() << "\n\n";
	}
}

// -----< begin() iterator - allows for loop >-----------------------------
DependencyTable::iterator DependencyTable::begin()
{
  return table_.begin();
}

// -----< end() iterator - allows for loop >-------------------------------
DependencyTable::iterator DependencyTable::end()
{
  return table_.end();
}

// -----< get keys of map >------------------------------------------------
DependencyTable::files DependencyTable::getFiles() const
{
  files list;
  for (auto entry : table_)
    list.push_back(entry.first);
  return list;
}

// -----< display entire table in formatted way >--------------------------
void DependencyTable::display()
{
	std::cout << "\n  Dependency table:";
  for (auto entry : table_) {
	  std::cout << "\n  -- " << entry.first;
    for (auto dep : entry.second)
	  std::cout << "\n   -> " << dep;
  }

}


#ifdef TEST_DEBTABLE

#include <iostream>

// ----< test stub for dependency table package >---------------------------
int main(int argc, char ** argv) {
  std::cout << "Testing DependencyTable Operations";

  DependencyTable dt;

  dt.getFileDependencies("DependencyTable.cpp");
  dt.display();
  return 0;

}

#endif