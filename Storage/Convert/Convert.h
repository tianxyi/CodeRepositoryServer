#pragma once
/////////////////////////////////////////////////////////////////////
// Convert.h - Converts file into a Webpage fragment               //
// ver 1.0                                                         //
// Tianxiang Yi, CSE687 - Object Oriented Design, Spring 2019      //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package contains one Convert class: Convert(string);
 * It purpose is replacing all html markup character to their exscape 
 * sequence equivalent and prpending "<pre>" and postpending "</pre>"
 * to the content.
 * It added function that support wrapping selected parts of the web page
 * in <div> blocks that can be displayed or hidden.
 * The parts to b selected are Comment blocks, Function bodies, Class bodies
 * Required Files:
 * ---------------
 * Convert.h
 *
 * Maintenance History:
 * --------------------
 * ver 1.0 : 11 Feb 2019
 * ver 1.1 : 25 Mar 2019
 * - second release
 *
*/
#include<fstream>
#include<vector>
#include<iostream>
#include<string>
#include<queue>
#include"..\FileSystem\FileSystem.h"
#include<map>
class Convert
{
public:
	Convert(const std::string& fullfile);

	// read file

	void readfile();

	// translate markup

	void translate();

	// insert the tag 

	void inserttag();

	// insert the html template

	void inserttemp(const std::string &string1, const std::string &string2);

	// show the content of the file

	void show();

	// return the content of the file

	std::string out();
	std::string showName();

	void setDepTable(const std::vector<std::string> & dt);

	void setClasses(const std::map<size_t, size_t> & map);
	
	void setFunction(const std::map<size_t, size_t> & func);

	void addPreCodeHtml();
	void addPreTag();
	void addClosingTags();
	void skipSpecialChars(std::string &line);
	
	void convertFile();

	void handleClasses(size_t count, std::string & line);

	void handleComments(std::string &line);
	void handleMultiComments(std::string &line, boolean &state);

	void addDependencyLinks();
private:
	std::string fullspec_;
	std::string filename_;
	std::string filepath_;
	std::vector<std::string> dt_;
	std::map<size_t, size_t> classes;
	std::map<size_t, size_t> function;
	std::string file_;//not need

	std::ofstream out_;
};

//----< construct Convert instance  >-----------------

inline Convert::Convert(const std::string& fullfile) {
	fullspec_ = FileSystem::Path::getFullFileSpec(fullfile);
	filename_ = FileSystem::Path::getName(fullfile);
	filepath_ = FileSystem::Path::getPath(fullfile);

}
//----< read string from file  >----------------------

inline void Convert::readfile() {
	std::ifstream in1(filename_);
	std::string s;
	while (!in1.eof()) {
		std::getline(in1, s);
		file_.append(s);
		file_.append("\n");
	}
	in1.close();
}
//----< translate the markup  >-----------------

inline void Convert::translate() {
	std::string::size_type l = file_.find("<", 0);
	while (l != std::string::npos) {
		file_.replace(l, 1, "&lt;");
		l = file_.find("<", l + 1);
	}

	std::string::size_type g = file_.find(">", 0);
	while (g != std::string::npos) {
		file_.replace(g, 1, "&gt;");
		g = file_.find(">", g + 1);
	}
}
//----< insert the <pre>/</pre> tag  >-----------------

inline void Convert::inserttag() {
	file_.insert(0, "<pre>");
	file_.append("<\\pre>");
	file_.append("\n");
}
//----< insert the html template  >-----------------

inline void Convert::inserttemp(const std::string &string1, const std::string &string2) {
	file_.insert(0, string1);
	file_.append(string2);
	file_.append("\n");


}
//----< show converted content   >-----------------

inline void Convert::show() {
	std::cout << file_;
}
//----< return converted content >-------------------

inline std::string Convert::out() {
	return file_;
}

inline std::string Convert::showName()
{
	return filename_;
}

inline void Convert::setDepTable(const std::vector<std::string>& dt)
{
	dt_ = dt;
}

inline void Convert::setClasses(const std::map<size_t, size_t>& map)
{
	classes = map;
}

inline void Convert::addPreCodeHtml()
{
	out_ << "<DOCTYPE !HTML>" << std::endl;
	out_ << "<html>" << std::endl;
	out_ << "  <head>" << std::endl;
	out_ << "    <Title>" << filename_ << "</Title>" << std::endl;
	//out_ << "    <style rel=\"stylesheet\" type=\"text/css\" href =\"style.css\">" << std::endl;
	out_ << "    <style >" << std::endl;
	out_ << "      body {" << std::endl;
	out_ << "        padding:15px 40px;" << std::endl;
	out_ << "        font-family: Consolas;" << std::endl;
	out_ << "        font-size: 1.25em;" << std::endl;
	out_ << "        font-weight: normal;" << std::endl;
	out_ << "      }" << std::endl;
	out_ << "      </style>" << std::endl;
	out_ << "    <link rel=\"stylesheet\" type=\"text/css\" href =\"style.css\">" << std::endl;
	out_ << "      <script src=\"script.js\"></script>" << std::endl;
	out_ << "  </head>" << std::endl << std::endl;
	out_ << "  <body>" << std::endl;
	out_ << " <div class=\"aaa\"><button class=\"absolute\" onclick = \"toggleVisibility('comments')\">Comment </button></div>" << std::endl;
	out_ << " <div class=\"bbb\"><button class=\"absolute\" onclick = \"toggleVisibility('classes')\">Class   </button></div>" << std::endl;
	out_ << " <div class=\"ccc\"><button class=\"absolute\" onclick = \"toggleVisibility('function')\">Function</button></div>" << std::endl;
}

inline void Convert::addPreTag()
{
	out_ << "    <pre>" << std::endl;
}

inline void Convert::addClosingTags()
{
	out_ << "    </pre>" << std::endl;
	out_ << "  </body>" << std::endl;
	out_ << "</html>" << std::endl;
}

inline void Convert::skipSpecialChars(std::string & line)
{
	size_t pos = line.npos;
	while ((pos = line.find('<')) != line.npos)
		line.replace(pos, 1, "&lt;");

	while ((pos = line.find('>')) != line.npos)
		line.replace(pos, 1, "&gt;");
}

inline void Convert::convertFile()
{
	std::ifstream in(fullspec_);//niubi

	if ( !in.good()) {//if (!in.is_open() || !in.good()) {
		std::cout << "Error -- unable to read file, path may be invalid.";
		//std::cout << fullspec_<<"\n";
		in.close();
	}
	std::string filename = filename_;
	std::string outputPath = "..//convertedPages//" + filename + ".html";//std::string outputPath = "..//..//convertedPages//" + filename + ".html";
	out_.open(outputPath, std::ofstream::out);//niubi
	if (!out_.is_open() || !out_.good()) {
		std::cout<<"Error -- unable to open output file for writing.";
		std::cout << outputPath << "\n";
		in.close();
	}
	 addPreCodeHtml();
	addPreTag();
	addDependencyLinks();
	
	while (in.good()) {
		std::string line;
		size_t count = 1;
		boolean state = false;
		auto it1 = classes.begin();
		auto it2 = function.begin();

		while (std::getline(in, line)) {
			skipSpecialChars(line);

			if (classes.size()!=0&&it1!=classes.end()) {
				if (count == it1->first) {
					size_t pos1 = line.find('{');
					if (pos1 != line.npos) {
						line.replace(pos1, 1, "<div class =\"classes\">{");
					}
				}

				if (count == it1->second) {
					line+= "</div>";
					it1++;
				}

			}

			if (function.size() != 0 && it2 != function.end()) {
				if (count == it2->first) {
					size_t pos2 = line.find('{');
					if (pos2 != line.npos) {
						line.replace(pos2, 1, "<div class =\"function\">{");
					}
				}

				if (count == it2->second) {
					line += "</div>";
					it2++;
				}
			}

			handleComments(line);
			handleMultiComments(line, state);
			out_ << line << std::endl;

			count++;
		}
	}

	addClosingTags();
	out_.close();
	std::cout << "-- Converted:" << filename<<"\n";


}

inline void Convert::handleClasses(size_t count, std::string & line)
{
	auto it = classes.begin();
	if (count == it->first) {
		line.replace(0, 1, "< div class = \"comments\">{");
	}
	if (count == it->second) {
		line += "</div>";
		it++;
	}
	//if(count==)
}
inline void Convert::handleMultiComments(std::string &line, boolean &state) {
	if (state == false) {
		size_t pos = line.find('/');
		if (pos == line.npos)return;
		if (pos == line.size() - 1)return;
		if (line[pos + 1] == '*') {
			line.replace(pos, 1, "<div class=\"comments\">/");
			state = true;

			//return;
			if (pos + 2 == line.npos || pos + 2 == line.size() - 1) { return; }

			size_t pos2 = line.find('*', pos + 2);

			if (pos2 == line.npos)  { return; }
			if (pos2 = line.size() - 1) { return; }
			
			if (line[pos2 + 1] == '/') {
				line+= "</div>";
				state = false;
				return;
			}


		}
	}
	else {
		size_t pos = line.find('*');

		if (pos == line.npos) { return; }
		if (pos == line.size() - 1) { return; }

		if (line[pos + 1] == '/') {
			line += "</div>";
			state = false;
			return;
		}
	}
}
inline void Convert::handleComments(std::string & line)
{
	size_t pos = line.find('/');
	if (pos == line.npos)return;
	if (pos == line.size() - 1) return;
	if (line[pos + 1] == '/') {
		
			line.replace(pos, 1, "<div class=\"comments\">/");
			line += "</div>";
		
			return;

	}



}

inline void Convert::addDependencyLinks()
{
	if (dt_.size() == 0) {
		//std::cout << "\n    No dependencies found for [" + filename_ + "]. Skipping dependency links..";
		return;
	}
	out_ << "    <h3>Dependencies: " << std::endl;
	for (auto dep : dt_) {
		out_ << "      <a href=\"" << FileSystem::Path::getName(dep) << ".html\">" << FileSystem::Path::getName(dep) << "</a>" << std::endl;
	}
	out_ << "    </h3>";
}

inline void Convert::setFunction(const std::map<size_t,size_t> &F) {
	function = F;

}

