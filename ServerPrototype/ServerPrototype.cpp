/////////////////////////////////////////////////////////////////////////
//  ServerPrototype.cpp - Console App that processes incoming messages //
//  ver 1.0                                                            //
//  Language:      Visual C++, Visual Studio 2017                      //
//  Platform:      Dell G5, Windows 10							                   //
//  Application:   Prototype for CSE687 - OOD Projects                 //
//  Author:        Tianxiang Yi, Syracuse University			             //
//                 (315) 278-3890, tyi100@syr.edu		                   //
/////////////////////////////////////////////////////////////////////////
#include "ServerPrototype.h"
#include "../FileSystem/FileSystem.h"
#include <chrono>
#include "../Project2Starter/Executive.h" 
#include <cstdlib>


namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository1;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

Files Server::getFiles(const Repository1::SearchPath& path)
{
  return Directory::getFiles(path);
}

//----< demenstrate requirement #5   >-----------------

void Repository1::Server::show3()
{
 
  Title("Demonstrating Requirement  #5-Message");

  std::cout << "\n I build messages that contains command getDirs, getFiles, convert, get";
  std::cout << "\n getDirs message support navigating remote directory and returns all sub dirs";
  std::cout << "\n getFiles message support navigating remote directory and returns all files";
  std::cout << "\n convert message support converting source code in the server";
  std::cout << "\n get message support downloading converted file from server.";
  std::cout << "\n when server receive message, it will reply message have same command and they";
  std::cout << "\n got information that client needed. The detailed impemented is followed:";

  
}



Dirs Server::getDirs(const Repository1::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

//----< callable object get to request converted file   >----------------
std::function<Msg(Msg)> get = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
  reply.command("get");
  reply.value("sendingFile");
  reply.file(msg.attributes()["name"]);
  std::string test = reply.value("sendingFile");
  std::cout << "\n" << test << " ";
  reply.attribute("name", msg.attributes()["name"]);
	return reply;
};
//----< callable object convert to convert on server   >----------------

std::function<Msg(Msg)> convert = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("convert");
  PublisherFactory factory;
  IPublisher * pPublisher = factory.createPublisher();
  std::vector<std::string> arg;
  std::string path = msg.value("path");
  std::string regex = msg.value("Regex");
  arg.push_back(path);
  arg.push_back(regex);
  pPublisher->setcmdd(arg);
  pPublisher->convert();
  std::vector<std::string> fileList;
  fileList = pPublisher->getConvertedFilesList();
  size_t count = 0;
  for (auto item : fileList)
  {
    std::string countStr = Utilities1::Converter<size_t>::toString(++count);

    reply.attribute("filed" + countStr, Path::getName(item));
  }
  return reply;
};
//----< callable object getFiles to request return filelist of remote dir   >----------------

std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
	if (path != ".")
	{
		searchPath = searchPath + "\\" + path;
	}

    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities1::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};
//----< callable object getDirs to request DirList of remote dir   >---------
std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities1::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);												
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};
//------------------< server that process message   >-----------------
int main(int argc, char *argv[])
{

  std::cout << "\n";


  if (argc < 2) { return 0; }
  MsgPassingCommunication::EndPoint serverEndPoint("localhost", atoi(argv[1]));
  Server server(serverEndPoint, "ServerPrototype");
  server.show3();
  server.start();

  


  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("convert", convert);
  server.addMsgProc("get", get);
  server.addMsgProc("serverQuit", echo);
 
  
  
  server.processMessages();
  Msg msg(serverEndPoint, serverEndPoint); 
  msg.name("msgToSelf");

  std::cin.get();
  

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

