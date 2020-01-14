/////////////////////////////////////////////////////////////////////
//  Executive.cpp - CodePublisher for Project #2                    //
//  ver 1.0                                                        //
//  Language:      Visual C++, Visual Studio 2017                  //
//  Platform:      Dell G5, Windows 10							   //
//  Application:   Prototype for CSE687 - OOD Projects             //
//  Author:        Tianxiang Yi, Syracuse University			   //
//                 (315) 278-3890, tyi100@syr.edu		           //
/////////////////////////////////////////////////////////////////////
#define IN_DLL

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Toker.h"
#include "../SemiExpression/Semi.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "Executive.h"

#include "../CodeUtilities/CodeUtilities.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "../Convert/Convert.h"
#include"../Process/Process.h"

#include<Windows.h>
using namespace Lexer;
using namespace Utilities;
using Demo = Logging::StaticLogger<1>;

#include "../FileSystem/FileSystem.h"
#include <map>
#include <string>
#define Util StringHelper

using namespace CodeAnalysis;
using namespace FileSystem;

//----< Publisher class   >-----------------
class Publisher : public IPublisher {
public:
	virtual void setCmd() override;
		
	virtual void setcmdd(const std::vector<std::string>& argv) override;

	virtual void show() override;

	virtual void convert() override;

	virtual std::vector<std::string> getConvertedFilesList() override;
	
	Publisher() {};

	~Publisher() {};
	
	std::vector<std::string> matchregex(DirExplorerN &d, std::regex r);
		
	std::vector<std::string> getdlink(ASTNode* p);
private:
	std::vector<std::string> files_;
	std::string path_;
	std::vector<std::string> Patterns;
	 std::vector<int> Options;
	 std::vector<std::string> Regexes;
};

IPublisher * PublisherFactory::createPublisher()
{
	return new Publisher;
}

//----< show vector content  >-----------------
void show(const std::vector<std::string> &V) {
	for (size_t i = 0; i < V.size(); i++) {

		std::cout << " " << V[i] << "\n";
	}
}
//----< get dependency link   >-----------------
std::vector<std::string> getDpendencyLink(ASTNode* p) {
	std::vector<std::string> VD;
	auto iter2 = p->statements_.begin();
	while (iter2 != p->statements_.end()) {
		if ((*iter2)->size() > 2) {
			if (((*(*iter2))[2]).find('"') != std::string::npos) {
				std::string text = ((*(*iter2))[2]);
				text.erase(
					std::remove(text.begin(), text.end(), '\"'), text.end());
				VD.push_back(Path::getFullFileSpec(text));
			}
		}
		++iter2;
	}
	return VD;
}
//----< demenstrate requirement 3 & 5   >-----------------
std::vector<std::string> demoReq35( DirExplorerN &d,std::regex r) {
	d.search();
	std::cout << "\n";
	d.showfind();
	std::vector<std::string> Vmatch;
	d.copyname(Vmatch);
	std::cout << std::endl;
	std::vector<std::string> V0;

	for (size_t i = 0; i < Vmatch.size(); i++) {
		if (std::regex_match(Path::getName(Vmatch[i]), r)) {
			V0.push_back(Vmatch[i]);
		}
	}

	Util::title(" Matching files to regexes - Available regexes: [A-B](.*)");
	show(V0);
	d.showStats();
	return V0;
	std::cout << "\n";
}




//-----< Set commandline  >-------
void Publisher::setCmd()
{
	int argc_ = 7;
	char argv_[7][40] = { "D:\\test\\AmmarProject2Sol\\Debug" ,"..\\Storage","/s","/demo","*.h","*.cpp","[A-B](.*)" };//pppppppppppppppppp
	path_ = argv_[1];
	Options.clear();
	Options.push_back(argv_[2][1]);
	Patterns.clear();
	Patterns.push_back(argv_[4]);
	Patterns.push_back(argv_[5]);
	Regexes.clear();
	Regexes.push_back(argv_[6]);
	
}
//-----< Set commandline from GUI  >-------
void Publisher::setcmdd(const std::vector<std::string>& argv)
{
  char argv_[7][40] = { "D:\\test\\AmmarProject2Sol\\Debug" ,"..\\Storage","/s","/demo","*.h","*.cpp","[A-B](.*)" };
	size_t argc = argv.size();
	if (argc < 1) {
		std::cout << "\n command line parse error";
		return;
	}
	path_ = "..\\"+argv[0];
  Regexes.clear();
  Regexes.push_back(argv[1]);
	Options.clear();
  Options.push_back(argv_[2][1]);

	Patterns.clear();
  Patterns.push_back(argv_[4]);
  Patterns.push_back(argv_[5]);

}
//-----< Write to Console requirement I have done  >-------
void Publisher::show()
{
	Util::Title("Demonstrating Requirement  #3&#4 - Interface and Objectfactory");

	std::cout << "\n I have an interface named IPublisher for the Publisher developed in ";
	std::cout << "\n Project#2. It has three function setCmd(vector<string>), convert() and getConvertedFiles()";
	std::cout << "\n Publisher class is inherit from this Interface ";
	std::cout << "\n Also the object factory provided in this class named PublisherFactory ";
	std::cout << "\n which returns a pointer to the interface.";
	std::cout << "\n ";

	Util::Title("Demonstrating Requirement  #5 - Translator and dynamiclibrary");
	std::cout << "\n Translator Package is available and it got ITranslator Interface and Translator class.";
	std::cout << "\n It is responsibled to delegate calls from the GUI into the publisher's method";
	std::cout << "\n The most important function of this package is mToN(List<String>) and nToM(vector<string>)";
	std::cout << "\n It is responsibled for transformation of the structure between native C++ code and managed code";
	std::cout << "\n ";

	Util::Title("Demonstrating Requirement  #6 - Client WPFGUI");
	std::cout << "\n WPF GUI package is avialable";
	std::cout << "\n It got two views--'Navigation' to navigate and 'Converted Files' to display file conversion results";
	std::cout << "\n ";

	Util::Title("Demonstrating Requirement  #7 - Automated test Unit");
	std::cout << "\n The default path is Project directory and it will automatically test";
	std::cout << "\n set default pattern:'*.h','*.cpp' and defualt regex[A-B](.*)";
	std::cout << "\n You could browse the directory and set path where you want to convert ";
	std::cout << "\n Also you could change the Pattern and press the Key Enter";
	std::cout << "\n Then it will automatically pop out the window to display view in GUI";
}

//-----< Convert Files  >-----------------------------------------------
void Publisher::convert()
{

	DirExplorerN de(path_);
	for (auto patt : Patterns)
	{
		de.addPattern(patt);
	}
	if (true)
	{
		de.recurse();
	}
	std::vector<std::string> reg = Regexes;
	std::regex pattern(reg[0]);

	std::vector<std::string> Dirfiles = matchregex(de, pattern);
	std::string fileSpec;
	std::unordered_map<std::string, std::vector<std::string>> table;

	//guide container to add <div> of class 

	std::map<std::string, std::map<size_t, size_t>> classlist;

	//guide container to add <div> of function

	std::map<std::string, std::map<size_t, size_t>> funclist;

	std::cout << "Parsing file to displayed or hidden Function & Class bodies: \n";
	for (size_t i = 0; i < Dirfiles.size(); i++) {
		std::map<size_t, size_t> mapc;
		std::map<size_t, size_t> mapf;
		fileSpec = Dirfiles[i];
		ConfigParseForCodeAnal configure;
		Parser* pParser = configure.Build();
		std::string name;
		try
		{
			if (pParser)
			{
				name = FileSystem::Path::getName(fileSpec);
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << name << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return;

			}
			Repository* pRepo = Repository::getInstance();
			pRepo->package() = name;
			// parse the package
			while (pParser->next())
			{
				pParser->parse();
			}
			std::cout << "\n";
			// final AST operations
			ASTNode* pGlobalScope = pRepo->getGlobalScope();
			table.emplace(fileSpec, getdlink(pGlobalScope));

			complexityEval(pGlobalScope);

			TreeWalk(pGlobalScope, mapc, mapf);

			std::cout << "-- Parsed:" << name;
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
			std::cout << "\n  exception caught at line " << __LINE__ << " ";
			std::cout << "\n  in package \"" << name << "\"";
		}
		std::cout << "\n";
		classlist.emplace(name, mapc);
		funclist.emplace(name, mapf);

	}
	//Add default Denpency link .h .cpp
	auto it3 = table.begin();
	while (it3 != table.end()) {
		if (it3->first.back() == 'h') {

			std::string dptcy = it3->first;
			dptcy.pop_back();
			it3->second.push_back(dptcy + "cpp");
		}
		it3++;
	}
	//Determine whether the Denpendency link exist
	int has = 0;
	std::vector<std::string> matched = de.copyfind();
	auto its = table.begin();
	while (its != table.end()) {
		std::vector<std::string> Vv;
		for (size_t i = 0; i < its->second.size(); i++) {
			std::string file = its->second[i];

			for (size_t j = 0; j < matched.size(); j++) {
				if (Path::getName(file) == Path::getName(matched[j])) {
					has = 1; break;
				}
			}
			if (has == 1) {
				Vv.push_back(file);
				has = 0;
			}
		}
		table[its->first] = Vv;
		its++;
	}
	std::cout << "Converting file to webpage \n";
	files_.clear();
	//convert file through package
	for (size_t i = 0; i < Dirfiles.size(); i++) {

		std::string namea = Path::getName(Dirfiles[i]);
		Convert c(Dirfiles[i]);
		c.setDepTable(table[Dirfiles[i]]);
		c.setClasses(classlist[namea]);
		c.setFunction(funclist[namea]);
		c.convertFile();
		std::string file_ = "..\\..\\convertedPages\\" + namea + ".html";//???
		files_.push_back(Path::getFullFileSpec(file_));

	}
}
//-----< get Converted Files  >-------
std::vector<std::string> Publisher::getConvertedFilesList()
{
	return files_;
}
//-----< find file that mached regex  >-------
std::vector<std::string> Publisher::matchregex(DirExplorerN & d, std::regex r)
{
	d.search();

	std::vector<std::string> Vmatch;
	d.copyname(Vmatch);
	
	std::vector<std::string> V0;

	for (size_t i = 0; i < Vmatch.size(); i++) {
		if (std::regex_match(Path::getName(Vmatch[i]), r)) {
			V0.push_back(Vmatch[i]);
		}
	}
	return V0;
}

//-----< get dependency link  >-------
std::vector<std::string> Publisher::getdlink(ASTNode* p)
{

	std::vector<std::string> VD;
	auto iter2 = p->statements_.begin();
	while (iter2 != p->statements_.end()) {
		if ((*iter2)->size() > 2) {
			if (((*(*iter2))[2]).find('"') != std::string::npos) {
				std::string text = ((*(*iter2))[2]);
				text.erase(
					std::remove(text.begin(), text.end(), '\"'), text.end());
				VD.push_back(Path::getFullFileSpec(text));
			}
		}
		++iter2;
	}
	return VD;
}


#ifdef TEST_EXECUTIVE
int main()
{

	PublisherFactory factory;
	IPublisher* pub = factory.createPublisher();
	pub->setCmd();
	pub->convert();
	std::vector<std::string> files;
	files = pub->getConvertedFilesList();
	for (auto item : files) {
		std::cout << item << "\n";
	}
	std::cout << "Hello World!\n";
}
#endif

