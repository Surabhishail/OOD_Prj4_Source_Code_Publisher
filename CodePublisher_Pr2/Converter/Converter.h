#pragma once
///////////////////////////////////////////////////////////////////////////
// Converter.h   : defines source code conversion to webpage functions   //
// ver 1.1                                                               //
//                                                                       // 
// Application   : Project Code Publisher                                //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
// Author        : Surabhi Shail, sshail@syr.edu                        //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodeConverter class which accepts DependencyTable
*  and uses its relationships to create linked webpages that point to each
*  other based on dependencies. It can also convert single files if needed.
*  The conversion process filters HTML special characters before printing
*  them into output files. The resulting output of this converter is a list
*  of files (vector<string>) of the created webpages.
*  
*  Public Interface:
* =======================
*  convertFile(file);
*  addPreCodeHTML(std::string title);
*  addButtons();
*  addDependencyLinks(std::string file)
*  addClosingTags();
*  skipSpecialChars(std::string line);
*  
*
*  Required Files:
* =======================
*  Converter.h Converter.cpp FileSystem.h FileSystem.cpp
*  ActionAndRules.h ActionAndRules.cpp AbstrSynTree.h
*  AbstrSynTree.cpp ConfigureParser.h  ConfigureParser.cpp
*  ScopeStack.h ScopeStack.cpp
*
*  Maintainence History:
* =======================
*  ver 1.1 - 3 Mar 2019
*  - Added functionality of getting function and class scopes and adding <div> to hide/show 
*    comments, class and functions
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include "../DependencyTable/DependencyTable.h"
#include <vector>
#include <string>
#include <fstream>

class CodeConverter
{
public:
	CodeConverter();
	CodeConverter(const DependencyTable& dt);

	const std::string& outputDir() const;
	void outputDir(const std::string& dir);
	bool createOutpurDir();

	void setDepTable(const DependencyTable& dt);

	std::vector<std::string> convert();
	std::string convert(const std::string& filepath);
	std::vector<std::string> convert(const std::vector<std::string>& files);

	const std::vector<std::string> convertedFiles() const;

	void clear();

private:
	bool convertFile(std::string file);
	void addPreCodeHTML(const std::string& title);
	void addPreTag();
	void addButtons();
	void addDependencyLinks(std::string file);
	void addClosingTags();
	void skipSpecialChars(std::string& line);

	// handles adding <div> by fetching scopes of functions and classes
	void handleComment(std::string& line);
	void handleClass(std::string& line, int, std::vector < std::vector<int>>);
	void handleFunction(std::string& line, int, std::vector<std::vector<int>>);

	std::vector<std::vector<int>> getClassLines(std::string file);
	std::vector<std::vector<int>> getFunctionLines(std::string file);
private:
	DependencyTable dt_;
	std::string outputDir_ = "..\\ConvertedWebpages\\";
	std::vector<std::string> convertedFiles_;
	std::vector<std::string> filesToConvert_;
	std::ofstream out_;
};

