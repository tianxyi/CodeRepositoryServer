/////////////////////////////////////////////////////////////////////
//  Executive.h  - define an interface to Publisher Project#2      //
//  ver 1.0                                                        //
//  Language:      Visual C++, Visual Studio 2017                  //
//  Platform:      Dell G5, Windows 10							   //
//  Application:   Prototype for CSE687 - OOD Projects             //
//  Author:        Tianxiang Yi, Syracuse University			   //
//                 (315) 278-3890, tyi100@syr.edu		           //
/////////////////////////////////////////////////////////////////////
/*
*  Package Description:
* ======================
*  This package build a Client that porvides a Graphical User Interface(GUI)
*  for the Code Publishr build in project#2. Because C++ standard libraries	
*  do not provide any support for GUIs.
*  Therefore, the implementation of the class was moved to the .cpp file
*  leaving the .h file free of any CLR-incompatible types. However, we
*  still need to use the C++ class in C++/CLI environment and under /CLR
*  code. To do so, we created an interface (IPublisher shown below) that
*  declares the methods we want the C++/CLI Shim to use. 
*  Inheritance allows the use of an interface pointer (IPublisher*) to
*  point into any derived class objects (Publisher).
*  We declared an object factory that returned IPublisher*
*  and implemented the function in the .cpp file to return Publisher*.
*  This way, the C++/CLI translator could use Publisher through
*  its interface.
*
*  Publisher is a Source Code Publisher, it converts source code file 
* content to a Web page
* Executive package accepts path, patterns and regular expression from
* commandline. And it using DirExplorerN to find files to process.
* Then it select files to process using Regex.
* What's more, it invokes Convert Package to convert files like 
* translating markup, inserting tag and webpage template.
* Last, it output file in Display Package and display each processed 
* file using Process.
*
*
*  Public Interface:
* ===================
*  Publisher factory;
*  IPublisher* instance = factory.createPublisher();
*  instance->setCmd();
*  instance->convert();
*  std::string<std::vector> = instance->GetConvertedFiles();
*
*  Required Files:
* =================
* Executive.h
* Executive.cpp
* DirExplorer-Naive.h				 // find files on path
* FileSystem.h, FileSystem.cpp       // Directory and Path classes
* StringUtilities.h                  // Title function
* CodeUtilities.h                    // ProcessCmdLine class
* Process.h							 // Display each files in window
* Convert.h						  	 // Convert function
* Display.h							 // Display function
* Parser.h							 // Parsing file
* Toker.h
* Semi.h
* ScopeStack.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Apri 2019
* - first release
*
*/
#pragma once



#include <vector>
#include <string>

class IPublisher {
public:
	//-----< Setup the commandlineargument through local variable >-------
	virtual void setCmd() = 0;
	//-----< receive the commandlineargument from GUI >-------
	virtual void setcmdd(const std::vector<std::string>& argv) = 0;
	//-----< Demenstrate the requirement I did in Project >-------
	virtual void show() = 0;
	//-----< Convert file >-------
	virtual void convert() = 0;
	//-----< Get converted files  >-------
	virtual std::vector<std::string> getConvertedFilesList() = 0;
	
	virtual ~IPublisher() {};
};

//extern "C" {
	//-----< Instantiates Publisher* as IPublisher  >-------
	struct PublisherFactory {

		 IPublisher* createPublisher();

	};
//}

