///////////////////////////////////////////////////////////////////////////
// Converter.cpp : implements and provides test stub for Converter.h     //
// ver 1.1                                                               //
//                                                                       // 
// Application   : Project Code Publisher                                //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
// Author        : Surabhi Shail, sshail@syr.edu                        //
///////////////////////////////////////////////////////////////////////////

#include "Converter.h"
#include "../FileSystem/FileSystem.h"
#include "../CppParser/ScopeStack/ScopeStack.h"
#include "../CppParser/AbstractSyntaxTree/AbstrSynTree.h"
#include "../CppParser/Parser/ConfigureParser.h"
#include "../CppParser/Parser/ActionsAndRules.h"
#include "../CppParser/Utilities/Utilities.h"
#include <iostream>
#include <algorithm>

using namespace FileSystem;
using namespace CodeAnalysis;

// -----< default ctor >--------------------------------------------------
CodeConverter::CodeConverter()
{
}

// -----< ctor to set dep table >-----------------------------------------
CodeConverter::CodeConverter(const DependencyTable & dt) : dt_(dt)
{
	std::cout << "\n  CodeConverter initialized with DependencyTable\n";
}

// -----< set dependency table function >---------------------------------
void CodeConverter::setDepTable(const DependencyTable & dt)
{
	dt_ = dt;
}

// -----< convert pre-set dependency table >------------------------------
std::vector<std::string> CodeConverter::convert()
{
	if (!createOutpurDir()) {
		std::cout << "\n\n  Failed to start conversion due to inability to create output directory";
		std::cout << "\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_);
		return std::vector<std::string>();
	}

	std::cout << "\n\n  Converting files to webpages";

	std::cout << "\n  Output directory: " + Path::getFullFileSpec(outputDir_);
	for (auto entry : dt_) {
		if (convertFile(entry.first)) {
			std::cout << "\n  -- Converted: ";
		}
		else {
			std::cout << "\n  -- Failed:    ";
		}

		std::string filename = Path::getName(entry.first);
		std::cout << filename;
	}
	std::cout << "\n";
	return convertedFiles_;
}

// -----< convert single file given path >----------------------------------
std::string CodeConverter::convert(const std::string & filepath)
{
	if (!createOutpurDir()) {
		std::cout << "\n\n  Failed to start conversion due to inability to create output directory";
		std::cout << "\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_);
		return "";
	}

	std::cout << "\n\n  Converting files to webpages";
	filesToConvert_.push_back(Path::getName(filepath));
	std::cout << "\n  Output directory: " + Path::getFullFileSpec(outputDir_);

	if (convertFile(filepath)) {
		std::cout << "\n  -- Converted: ";
	}
	else {
		std::cout << "\n  -- Failed:    ";
	}

	std::string filename = Path::getName(filepath);
	std::cout << filename;

	return filename + ".html";
}

std::vector<std::string> CodeConverter::convert(const std::vector<std::string>& files)
{
	clear();
	dt_ = DependencyTable(files);
	filesToConvert_ = files;
	return convert();
}

// -----< get set output directory >----------------------------------------
const std::string & CodeConverter::outputDir() const
{
	return outputDir_;
}

// -----< set output directory >--------------------------------------------
void CodeConverter::outputDir(const std::string & dir)
{
	outputDir_ = dir;
}

// -----< create output directory >-----------------------------------------
/* returns success of creation. it could fail if dir is sys protected */
bool CodeConverter::createOutpurDir()
{
	if (!Directory::exists(outputDir_)) {
		std::cout << "\n  Creating output directory. Path: " + Path::getFullFileSpec(outputDir_);
		return Directory::create(outputDir_);
	}
	std::cout << "\n  Output directory already exists. Proceeding...";
	return true;
}

// -----< get list of converted files >-------------------------------------
const std::vector<std::string> CodeConverter::convertedFiles() const
{
	return convertedFiles_;
}

void CodeConverter::clear()
{
	convertedFiles_.clear();
	dt_.clear();
}

// -----< private - read file and create webpage >--------------------------
bool CodeConverter::convertFile(std::string file)
{
	std::ifstream in(file);
	if (!in.is_open() || !in.good()) {
		std::cout << "\n\nError -- unable to read file, path may be invalid.";
		in.close();
		return false;
	}

	std::string filename = Path::getName(file);
	std::string outputPath = outputDir_ + filename + ".html";
	out_.open(outputPath, std::ofstream::out);
	if (!out_.is_open() || !out_.good()) {
		std::cout << "Error -- unable to open output file for writing.";
		in.close();
		return false;
	}
	// write everything to file 
	addPreCodeHTML(filename);
	addDependencyLinks(file);

	addButtons();
	addPreTag();
	// vectors to get scope of functions and classes
	std::vector<std::vector<int>> functionLines = getFunctionLines(file);
	std::vector<std::vector<int>> classLines = getClassLines(file);
	int lineCount = 1;
	while (in.good()) {
		std::string line;
		while (std::getline(in, line)) {
			skipSpecialChars(line);
			handleComment(line);
			handleClass(line, lineCount, classLines);
			handleFunction(line, lineCount, functionLines);
			out_ << line << std::endl;
			lineCount++;
		}
	}

	addClosingTags();
	out_.close();

	convertedFiles_.push_back(outputPath);
	return true;
}

// -----< private - add generic HTML preliminary markup >-------------------
void CodeConverter::addPreCodeHTML(const std::string& title)
{
	out_ << "<DOCTYPE !HTML>" << std::endl;
	out_ << "<html>" << std::endl;
	out_ << "  <head>" << std::endl;
	out_ << "    <Title>" << title << "</Title>" << std::endl;
	out_ << "    <style>" << std::endl;
	out_ << "      body {" << std::endl;
	out_ << "        padding:15px 40px;" << std::endl;
	out_ << "        font-family: Consolas;" << std::endl;
	out_ << "        font-size: 1.25em;" << std::endl;
	out_ << "        font-weight: normal;" << std::endl;
	out_ << "      }" << std::endl;
	out_ << "      </style>" << std::endl;
	out_ << "		 <script src =\"../script.js\"></script>" << std::endl;
	out_ << "  </head>" << std::endl << std::endl;
	out_ << "  <body>" << std::endl;
}

// -----< private - add pre tag >------------------------------------------
/* seperated into seperate function to allow for dependencies addition
  before the actual code of the file */
void CodeConverter::addPreTag()
{
	out_ << "    <pre>" << std::endl;
}

void CodeConverter::addButtons()
{
	out_ << "		   <div style = {position:fixes; top = 0; right=0}\">" << std::endl;
	out_ << "             <button onclick =\"toggleVisibility(\'comments');\"> Comments </buttons> &nbsp" << std::endl;
	out_ << "             <button onclick =\"toggleVisibility('class');\"> Class </buttons> &nbsp" << std::endl;
	out_ << "             <button onclick =\"toggleVisibility('function');\"> Functions </buttons> &nbsp" << std::endl;
	out_ << "         </div>" << std::endl;
}
// -----< private - add depedency links markup code >----------------------
void CodeConverter::addDependencyLinks(std::string file)
{
	std::string filename = Path::getName(file);
	if (!dt_.has(file)) { // in case of single file conversion
		std::cout << "\n    No entry found in DependencyTable for [" + filename + "]. Skipping dependency links..";
		return;
	}

	if (dt_[file].size() == 0) { // in case the file has no dependencies
		std::cout << "\n    No dependencies found for [" + filename + "]. Skipping dependency links..";
		return;
	}
	std::unordered_set<std::string> setFile;

	for (auto f : filesToConvert_) {
		setFile.insert(Path::getName(f));
	}
	std::unordered_set<std::string> ::iterator itr;
	out_ << "    <h3>Dependencies: " << std::endl;
	for (auto dep : dt_[file]) {
		std::string str = Path::getFullFileSpec(dep);
		//str.pop_back();
		auto iter = setFile.find(Path::getName(str));
		if (iter != setFile.end()) {
			std::string temp = "../ConvertedWebpages/" + Path::getName(str);
			out_ << "      <a href=\"" << temp << ".html\">" << Path::getName(str) << "</a>" << std::endl;
			out_ << "&nbsp;";
			//}
		}
	}
	out_ << "    </h3>";
}

// -----< private - add generic HTML markup closing tags >-----------------
void CodeConverter::addClosingTags()
{
	out_ << "    </pre>" << std::endl;
	out_ << "  </body>" << std::endl;
	out_ << "</html>" << std::endl;
}

// -----< private - replace HTML special chars >---------------------------
/* note: the function uses while loop to make sure ALL special characters
  are replaced instead of just the first encounter. */
void CodeConverter::skipSpecialChars(std::string & line)
{
	size_t pos = line.npos;
	while ((pos = line.find('<')) != line.npos)
		line.replace(pos, 1, "&lt;");

	while ((pos = line.find('>')) != line.npos)
		line.replace(pos, 1, "&gt;");
}

//-----< Applies the <div> to hide/show comments >---------------------------
void CodeConverter::handleComment(std::string & line)
{
	size_t pos = line.find('/');
	if (line.npos == pos)
	{
		if (line.npos - 1 == '*')
		{
			line += "</div>";
		}
	}
	else if (line[pos + 1] == '/') {
		line.replace(pos, 1, "<div class =\"comments\">/");
		line += "</div>";
	}

	else if (line[pos + 1] == '*') {
		line.replace(pos, 1, "<div class =\"comments\">/");
		if (line[line.length() - 2] == '*' && line[line.length() - 1] == '/')
		{
			line += "</div>";
		}
	}
	size_t pos2 = line.find('*');
	if (line.npos != pos)
	{
		if (line[pos2 + 1] == '/')
		{
			line += "</div>";
		}
	}
}

//-----< Applies the <div> to hide/show functions >---------------------------
void CodeConverter::handleFunction(std::string& line, int lineCount, std::vector<std::vector<int>> functionLines)
{
	for (auto funline : functionLines) {
		if (lineCount == funline[0]) {
			size_t pos = line.find('{');
			if (line.npos != pos) {
				line.insert(pos + 1, "<div class =\"function\">");
			}
			else
				line += "<div class =\"function\">";
		}
		else if (lineCount == funline[1]) {
			size_t pos = line.find('}');
			if (line.npos != pos)
				line.insert(pos, "</div>");
		}
	}
}

//-----< Applies the <div> to hide/show class >---------------------------
void CodeConverter::handleClass(std::string& line, int lineCount, std::vector<std::vector<int>> classLines)
{
	for (auto clline : classLines) {
		if (lineCount == clline[0]) {
			size_t pos = line.find('{');
			if (line.npos != pos) {
				line.insert(pos + 1, "<div class =\"class\">");
			}
			else
				line += "<div class =\"class\">";
		}
		else if (lineCount == clline[1]) {
			size_t pos = line.find('}');
			line.insert(pos, "</div>");
		}
	}
}

//-----< Fetches scope of class by using Parser >---------------------------
std::vector<std::vector<int>> CodeConverter::getClassLines(std::string file)
{
	try {
		ConfigParseForCodeAnal configure;
		Parser* pParser = configure.Build();
		std::string fileSpec = FileSystem::Path::getFullFileSpec(file);
		std::string name;
		std::vector<std::vector<int>> classScope;
		if (pParser)
		{
			name = FileSystem::Path::getName(file);
			if (!configure.Attach(fileSpec))
			{
				std::cout << "\n  could not open file " << name << std::endl;
			}
		}
		else
		{
			std::cout << "\n\n  Parser not built\n\n";
		}
		Repository* pRepo = Repository::getInstance();
		pRepo->package() = name;
		while (pParser->next())
		{
			pParser->parse();
		}
		std::cout << "\n";
		ASTNode* pGlobalScope = pRepo->getGlobalScope();

		classScope = CodeAnalysis::TreeWalkToGetLineCountOfClass(pGlobalScope, classScope);
		std::cout << "\n";
		return classScope;

	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n    " << ex.what() << "\n\n";
		std::cout << "\n  exception caught at line " << __LINE__ << " ";
		return { {0, 0} };
	}
}

//-----< Fetches scope of funtion by using Parser >---------------------------
std::vector<std::vector<int>> CodeConverter::getFunctionLines(std::string file)
{
	try {
		ConfigParseForCodeAnal configure;
		Parser* pParser = configure.Build();
		std::string fileSpec = FileSystem::Path::getFullFileSpec(file);
		std::string name;
		std::vector<std::vector<int>> functionScope;
		if (pParser)
		{
			name = FileSystem::Path::getName(file);
			if (!configure.Attach(fileSpec))
			{
				std::cout << "\n  could not open file " << name << std::endl;
			}
		}
		else
		{
			std::cout << "\n\n  Parser not built\n\n";
		}
		Repository* pRepo = Repository::getInstance();
		pRepo->package() = name;
		while (pParser->next())
		{
			pParser->parse();
		}
		std::cout << "\n";
		ASTNode* pGlobalScope = pRepo->getGlobalScope();

		functionScope = CodeAnalysis::TreeWalkToGetLineCountOfFunction(pGlobalScope, functionScope);
		return functionScope;
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n    " << ex.what() << "\n\n";
		std::cout << "\n  exception caught at line " << __LINE__ << " ";
		return { {0, 0} };
	}
}

#ifdef TEST_CONVERTER

// -----< test stub for converter class >-----------------------------------
int main() {

	DependencyTable dt;
	dt.getFileDependencies("Converter.h");
	dt.getFileDependencies("Converter.cpp");
	dt.getFileDependencies("../DependencyTable/DependencyTable.cpp");
	dt.getFileDependencies("../DependencyTable/DependencyTable.h");
	dt.display();

	CodeConverter cc(dt);

	std::cout << "\n";

	cc.convert();
	return 0;
}

#endif