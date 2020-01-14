#pragma once
/////////////////////////////////////////////////////////////////////
// Comm.h - message-passing communication facility                 //
// ver 2.2                                                         //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017    //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines Sender and Receiver classes.
*  - Sender uses a SocketConnecter and supports connecting to multiple
*    sequential endpoints and posting messages.
*  - Receiver uses a SocketListener which returns a Socket on connection.!!!!!!!!!!!!!!
*  It also defines a Comm class
*  - Comm simply composes a Sender and a Receiver, exposing methods:
*    postMessage(Message) and getMessage()
*
*  Required Files:
*  ---------------
*  Comm.h, Comm.cpp,
*  Sockets.h, Sockets.cpp,
*  Message.h, Message.cpp,
*  Utilities.h, Utilities.cpp
*/

#include "../Message/Message.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "IComm.h"
#include <string>
#include <thread>

using namespace Sockets;

namespace MsgPassingCommunication
{
  ///////////////////////////////////////////////////////////////////
  // Receiver class

  class Receiver
  {
  public:
    Receiver(EndPoint ep, const std::string& name = "Receiver");
    template<typename CallableObject>
    void start(CallableObject& co);
    void stop();
    Message getMessage();
    BlockingQueue<Message>* queue();//why pointer --different listener thread putting msg to same class receiver !queue!
  private:
	  BlockingQueue<Message> rcvQ;
    SocketListener listener;
    std::string rcvrName;
  };

  ///////////////////////////////////////////////////////////////////
  // Sender class

  class Sender
  {
  public:
    Sender(const std::string& name = "Sender");
    ~Sender();
    void start();
    void stop();
    bool connect(EndPoint ep);
    void postMessage(Message msg);//jjkkkkkkkkkkkkkkkk
  private:
  	bool sendFile(Message msg);
	  BlockingQueue<Message> sndQ;//jjjjjjjjjjjjjjjjjjjjjj
    SocketConnecter connecter;
    std::thread sendThread;//what is this do?
    EndPoint lastEP;
    std::string sndrName;
  };

  class Comm : public IComm//qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
  {
  public:
    Comm(EndPoint ep, const std::string& name = "Comm");
    void start();
    void stop();
    void postMessage(Message msg);
    Message getMessage();
    std::string name();
  private:
    Sender sndr;
    Receiver rcvr;
    std::string commName;
    Sockets::SocketSystem socksys_;
  };

  //object factory	tidfhjklb
  inline IComm* IComm::create(const std::string& machineAddress, size_t port)
  {
    std::cout << "\n  creating an instance of Comm on the native heap";
    EndPoint ep(machineAddress, port);
    IComm* pComm = new Comm(ep, "created Comm");
    return pComm;
  }
}