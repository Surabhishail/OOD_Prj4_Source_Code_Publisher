///////////////////////////////////////////////////////////////////////////
// Executive.cpp : Integrates all the code publisher functionalities     //                                           
// ver 1.0                                                               //
//                                                                       // 
// Application   : Project Code Publisher                                //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Source:        : Ammar Salman, Syracuse University                    //
//                 313/788-4694, assalman@syr.edu                        //
// Author:         Surabhi Shail, sshail@syr.edu                         //
///////////////////////////////////////////////////////////////////////////

/*
*  Package Operations:
* =======================
*  This package is used to provide the functionality of
*  CodePublisher. 
*/
#include "Executive.h"

using namespace Utilitiess;
using namespace FileSystem;

ProcessCmdLine::Usage customUsage();

Executive::Executive()
{
}

// -----< process cmd to get info >------------------------------------
bool Executive::processCommandLineArgs(int argc, char ** argv)
{
	pcl_ = new ProcessCmdLine(argc, argv);
	pcl_->usage(customUsage());

	preface("Command Line: ");
	pcl_->showCmdLine();

	if (pcl_->parseError())
	{
		pcl_->usage();
		std::cout << "\n\n";
		return false;
	}
	dirIn_ = pcl_->path();
	return true;
}

// -----< return input directory - from PCL >----------------------------
const std::string & Executive::workingDirectory() const
{
	return dirIn_;
}

// -----< set output directory >-----------------------------------------
void Executive::outputDirectory(const std::string & dir)
{
	dirOut_ = dir;
	cconv_.outputDir(dir);
}

// -----< return output directory >--------------------------------------
const std::string & Executive::outputDirectory() const
{
	return dirOut_;
}

// -----< extract files - after processing cmd >-------------------------
bool Executive::extractFiles()
{
	Loader ld(pcl_->path());

	for (auto patt : pcl_->patterns())
	{
		ld.addPattern(patt);
	}

	if (pcl_->hasOption("s"))
	{
		ld.recurse();
	}

	bool res = ld.search() && ld.match_regexes(pcl_->regexes());
	ld.showStats();

	files_ = ld.filesList();

	return res;
}


// -----< publish - files extracted from directory explorer >---------------
void Executive::publish()
{
	convertedFiles_ = cconv_.convert(files_);
}

// -----< publish - single file >-------------------------------------------
void Executive::publish(const std::string & file)
{
	display_.display(cconv_.convert(file));
}

// -----< publish - must provide list >-------------------------------------
void Executive::publish(const std::vector<std::string>& files)
{
	display_.display(cconv_.convert(files));
}

// -----< gets display mode as set by PCL >---------------------------------
Utilitiess::DisplayMode Executive::displayMode() const
{
	return pcl_->displayMode();
}

std::vector<std::string> Executive::getConvertedFiles() {
	return convertedFiles_;
}

// -----< command line usage >----------------------------------------------
ProcessCmdLine::Usage customUsage()
{
	std::string usage;
	usage += "\n  Command Line: path [/option]* [/pattern]* [/regex]*";
	usage += "\n    path is relative or absolute path where processing begins";
	usage += "\n    [/option]* are one or more options of the form:";
	usage += "\n      /s     - walk directory recursively";
	usage += "\n      /demo  - run in demonstration mode (cannot coexist with /debug)";
	usage += "\n      /debug - run in debug mode (cannot coexist with /demo)";
	//usage += "\n      /h - hide empty directories";
	//usage += "\n      /a - on stopping, show all files in current directory";
	usage += "\n    [pattern]* are one or more pattern strings of the form:";
	usage += "\n      *.h *.cpp *.cs *.txt or *.*";
	usage += "\n    [regex] regular expression(s), i.e. [A-B](.*)";
	usage += "\n";
	return usage;
}
// ------< Demonstrating Requirements 1 and 2 >------------------------------
void Executive::req12()
{
	std::cout << "\n\nPROJECT CODE PUBLISHER" << std::endl;
	std::cout << "============================================================";

	std::cout << "\n\nDemonstrating Requirement 1" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "\nProject Code Publisher uses Visual Studio 2017 and C++ Windows Console Projects." << std::endl;

	std::cout << "\n\nDemonstrating Requirement 2" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "\nProject Code Publisher uses Windows Presentation Foundation and C# to implement the Client GUI." << std::endl;
}

// ------< Demonstrating Requirement 3 >---------------------------------------
void Executive::req3()
{
	std::cout << "\n\nDemonstrating Requirement 3" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "\nCreated an Interface for Code Publisher and Object Factory as per the requirement." << std::endl;
}

// ------< Demonstrating Requirement 4 >---------------------------------------
void Executive::req4()
{
	std::cout << "\n\nDemonstrating Requirement 4" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "\nThe Executive package of the Code Publisher is a static library." << std::endl;
}

// ------< Demonstrating Requirement 5 >---------------------------------------
void Executive::req5()
{
	std::cout << "\n\nDemonstrating Requirement 5" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "Provided Shim that acts as a C++/CLI translator.";
}

// ------< Demonstrating Requirement 6 >---------------------------------------
void Executive::req6()
{
	std::cout << "\n\nDemonstrating Requirement 6" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "\nProvided a WPF GUI with two views.";
}

// ------< Demonstrating Requirement 7 >---------------------------------------
void Executive::req7()
{
	std::cout << "\n\nDemonstrating Requirement 7" << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "Well, this is the automated test unit" << std::endl;
	std::cout << "\n\n  All Requirements met. " << std::endl;
}

// ------< Deletes the files from ConvertedWebpages >--------------------------
void Executive::deleteExistingFiles()
{
	std::string dirPath = "../ConvertedWebpages";
	std::vector<std::string> prevFiles = FileSystem::Directory::getFiles(dirPath);
	for (auto files : prevFiles)
		FileSystem::File::remove(dirPath + "/" + files);
}



#ifdef USE_EXEC

#include <iostream>

// -----< NORMAL USE MAIN > ------------------------------------------
int main(int argc, char ** argv) {
	Executive ex;
	if (!ex.processCommandLineArgs(argc, argv)) {
		std::cout << "\n  Failed to process command line arguments. Terminating\n\n";
		return 1;
	}

	if (!ex.extractFiles()) {
		std::cout << "\n  Found no files to process. Terminating.\n\n";
		return 0;
	}

	ex.publish();
	return 0;
}

#endif
