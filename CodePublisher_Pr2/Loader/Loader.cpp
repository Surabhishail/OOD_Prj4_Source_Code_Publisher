/////////////////////////////////////////////////////////////////////
// Loader.cpp - Naive directory explorer and Loader                //
// ver 1.2                                                         //
// Source :     Jim Fawcett, CSE687 - Object Oriented Design       //
/////////////////////////////////////////////////////////////////////

#include <regex>
#include "Loader.h"

using namespace FileSystem;

//----< construct DirExplorerN instance with default pattern >-----

Loader::Loader(const std::string& path) : path_(path)
{
	patterns_.push_back("*.*");
}
//----< add specified patterns for selecting file names >----------

void Loader::addPattern(const std::string& patt)
{
	if (patterns_.size() == 1 && patterns_[0] == "*.*")
		patterns_.pop_back();
	patterns_.push_back(patt);
}
//----< set option to recusively walk dir tree >-------------------

void Loader::recurse(bool doRecurse)
{
	recurse_ = doRecurse;
}
//----< start Depth First Search at path held in path_ >-----------

bool Loader::search()
{
	std::cout << "\n  Files found matching patterns:\n";

	find(path_);
	// found files matching set patterns, return true
	if (files_.size() > 0) return true;
	// no files found, return false
	return false;
}
//----< search for directories and their files >-------------------
/*
  Recursively finds all the dirs and files on the specified path,
  executing doDir when entering a directory and doFile when finding a file
*/
void Loader::find(const std::string& path)
{
	// show current directory

	std::string fpath = Path::getFullFileSpec(path);
	doDir(fpath);

	for (auto patt : patterns_)
	{
		std::vector<std::string> files = Directory::getFiles(fpath, patt);
		for (auto f : files)
		{
			doFile(fpath, f);  // show each file in current directory
		}
	}

	std::vector<std::string> dirs = Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;
		if (recurse_)
		{
			find(dpath);   // recurse into subdirectories
		}
		else
		{
			doDir(dpath);  // show subdirectories
		}
	}
}

// ----< Finds files from DirNaiveN with the having matching regexes >---
bool Loader::match_regexes(const std::vector<std::string>& regexes)
{
	std::cout << "\n\n  Matching files to regexes - Available regexes: ";
	for (auto regex : regexes) {
		std::cout << regex + " ";
	}

	std::cout << "\n  Files found matching regexes: ";

	std::vector<std::string> filteredFiles; // to store files matching regex

	// match regex to files
	for (auto file : files_) {
		std::string filename = Path::getName(file);
		for (auto regex : regexes) {
			std::regex r(regex);
			if (std::regex_match(filename, r)) {
				filteredFiles.push_back(file);
				std::cout << "\n   -- " + file;
				break; // break to avoid adding same file twice to the  
					   // list in case it matches multiple regexes
			}
		}
	}

	std::cout << "\n";

	// found files matching regex?
	if (filteredFiles.size() > 0) {
		files_ = filteredFiles;
		return true;
	}

	std::cout << "   -- no files found";
	// no matches found
	return false;
}

//----< an application changes to enable specific file ops >-------
void Loader::doFile(const std::string& dir, const std::string& filename)
{
	++fileCount_;
	std::string filepath = dir + "\\" + filename;
	files_.push_back(filepath);
	//std::cout << "\n  --   " + filename;
}
//----< an application changes to enable specific dir ops >--------

void Loader::doDir(const std::string& dirname)
{
	++dirCount_;
	//std::cout << "\n  ++ " + dirname;
}
//----< return number of files processed >-------------------------

const Loader::files& Loader::filesList() const
{
	return files_;
}

size_t Loader::fileCount()
{
	return fileCount_;
}
//----< return number of directories processed >-------------------

size_t Loader::dirCount()
{
	return dirCount_;
}
//----< show final counts for files and dirs >---------------------

void Loader::showStats()
{
	std::cout << "\n\n  processed " << fileCount_ << " files in " << dirCount_ << " directories";
}



#ifdef TEST_LOADER  // only compile the following when defined

#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"

using namespace Utilitiess;

ProcessCmdLine::Usage customUsage()
{
	std::string usage;
	usage += "\n  Command Line: path [/option]* [/pattern]*";
	usage += "\n    path is relative or absolute path where processing begins";
	usage += "\n    [/option]* are one or more options of the form:";
	usage += "\n      /s - walk directory recursively";
	usage += "\n      [/demo | /debug] - display mode: either demo OR debug";
	//usage += "\n      /h - hide empty directories";
	//usage += "\n      /a - on stopping, show all files in current directory";
	usage += "\n    [pattern]* are one or more pattern strings of the form:";
	usage += "\n      *.h *.cpp *.cs *.txt or *.*";
	usage += "\n";
	return usage;
}

int main(int argc, char *argv[])
{
	Title("Demonstrate Loader, " + DirExplorerN::version());

	ProcessCmdLine pcl(argc, argv);
	pcl.usage(customUsage());

	preface("Command Line: ");
	pcl.showCmdLine();
	//putline();

	if (pcl.parseError())
	{
		pcl.usage();
		std::cout << "\n\n";
		return 1;
	}

	Loader ld(pcl.path());
	for (auto patt : pcl.patterns())
	{
		ld.addPattern(patt);
	}

	if (pcl.hasOption("s"))
	{
		ld.recurse();
	}

	bool res = ld.search() && ld.match_regexes(pcl.regexes());
	ld.showStats();

	if (!res) {
		std::cout << "\n\n  Failed to find any files matching patterns and regexes. Terminating..\n\n";
		return 1;
	}
	//std::cout << "\n\n";
	return 0;
}

#endif