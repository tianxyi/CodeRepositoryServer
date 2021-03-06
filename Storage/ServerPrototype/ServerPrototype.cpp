/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem/FileSystem.h"
#include <chrono>
#include "../Project2Starter/Executive.h" 



namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository1;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

Files Server::getFiles(const Repository1::SearchPath& path)
{
  return Directory::getFiles(path);
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

std::function<Msg(Msg)> get = [](Msg msg) {////////////////////////////////////
	Msg reply ;
	reply.to(msg.from());
	reply.from(msg.to());
  reply.value("sendingFile");//////
  reply.file(msg.attributes()["name"]);
  reply.command("get");
  reply.attribute("name", msg.attributes()["name"]);//
  std::cout << "sendnow";
  
  //fileMsg.file("AbstrSynTree.cpp.html");

	return reply;
};

std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");//
  if (path != "")
  {
    std::string searchPath = storageRoot;
	if (path != ".")//dang qian dir
	{
		searchPath = searchPath + "\\" + path;
	}

    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities1::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);//
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};
std::function<Msg(Msg)> convert = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("convert");

  PublisherFactory factory;//
  IPublisher * pPublisher = factory.createPublisher();//
  //Publisher pub;
  std::vector<std::string> arg;
  std::string path = msg.value("path");
  std::string regex = msg.value("Regex");
  arg.push_back(path);
  arg.push_back(regex);
  
  pPublisher->setcmdd(arg);//+...
  pPublisher->convert();
  std::vector<std::string> fileList;
  fileList= pPublisher->getConvertedFilesList();
  size_t count = 0;
  for (auto item : fileList)
  {
    std::string countStr = Utilities1::Converter<size_t>::toString(++count);

    reply.attribute("filed" + countStr, Path::getName(item));//e.g. file1:FileSystem.h
  }
  return reply;
};
std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;//1st create reply message and get msg information
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");//qqqqqqqqqqqq
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
        reply.attribute("dir" + countStr, item);//attribue yi dui attribute["dir"]= 1-path
																//attribute["dir"]= 2-path
																//attribute["dir"]= 3-path
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

std::function<Msg(Msg)> getFile = [](Msg msg) {//////////////////////////////////
  Msg reply;//1st create reply message and get msg information
reply.to(msg.from());
reply.from(msg.to());

reply.command("file"); 
reply.file(msg.attributes()["name"]);
//reply.attribute("file", msg.file());//?
//reply.file(msg.file());//?
return reply; 
};

int main()
{
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";

  //StaticLogger<1>::attach(&std::cout);
  //StaticLogger<1>::start();

  Server server(serverEndPoint, "ServerPrototype");
  server.start();

  std::cout << "\n  testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n";

  std::cout << "\n  testing message processing";//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);//echo functor
  server.addMsgProc("getFiles", getFiles);//getfile callable object
  server.addMsgProc("getDirs", getDirs);// callable object
  server.addMsgProc("convert", convert);
  server.addMsgProc("get", get);//
 // server.addMsgProc("file", getFile);//？？？？？？？？？
  server.addMsgProc("serverQuit", echo);
 
  
  
  server.processMessages();
  
  Msg msg(serverEndPoint, serverEndPoint);  // send to self (to,from)
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  //msg.command("convert");
  //server.postMessage(msg);
  ////msg.command("convert");
  //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 // msg.attribute("file", "qqq.cpp");
  //msg.file(".h");

 //MsgPassingCommunication::EndPoint epPeer1("localhost", 8081);
 // MsgPassingCommunication::Comm testq(epPeer1);
 // testq.start();
 // Msg ha(serverEndPoint, epPeer1);
 // ha.command("get");
 // ha.attribute("name", "AbstrSynTree.h.html");
 // testq.postMessage(ha);


  //Msg msg(serverEndPoint, serverEndPoint);
  //Msg fileMsg(epPeer1, serverEndPoint);
  //fileMsg.value("sendingFile");
  //fileMsg.file("AbstrSynTree.cpp.html");
  //server.postMessage(fileMsg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  


  

  std::cout << "\n  press enter to exit";
  std::cin.get();/////////////////////////////////////
  std::cout << "\n";

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

