#ifndef SOCKETS_H
#define SOCKETS_H
/////////////////////////////////////////////////////////////////////////
// Sockets.h - C++ wrapper for Win32 socket api                        //
// ver 5.2                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// CST 4-187, Syracuse University, 315 443-3948, jfawcett@twcny.rr.com //
//---------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                      //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  Provides four classes that wrap the Winsock API(WSA):
*  Socket:
*  - provides all the functionality necessary to handle server /clients
*  - created by SocketListener after accepting a request
*  - usually passed to a client handling thread// thread for handling client request!!!!!!!!!!!!

*  SocketConnecter:
*  - adds the ability to connect to a server

*  SocketListener:
*  - adds the ability to listen for connections on a dedicated thread
*  - instances of this class are the only ones influenced by ipVer().
*    clients will use whatever protocol the server provides.

*  SocketSystem:
*  - Loads and unloads winsock2 library.  
*  - Declared once at beginning of execution
*

/*
* ToDo:
* - make SocketSystem a reference counted instance of Socket
* - write buffered recv which efficiently returns string or line
*   - reads and concatenates everything available into circular buffer
*   - parses out first string or line and moves start of buffer pointer
*     to begining of next
* -----------------------------------------------------------------------
*  Wait for The next items until Students have submitted their code
* -----------------------------------------------------------------------
* - build front end, e.g., Sender and Receiver classes!!!!
* - implement message facility: message class, sendMsg and recvMsg
* - Test and Display packages
*/

#ifndef WIN32_LEAN_AND_MEAN  // prevents duplicate includes of core parts of windows.h in winsock2.h
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>      // Windnows API
#include <winsock2.h>     // Windows sockets, ver 2
#include <WS2tcpip.h>     // support for IPv6 and other things
#include <IPHlpApi.h>     // ip helpers

#include <vector>
#include <string>
#include <atomic>

#include "../WindowsHelpers/WindowsHelpers.h"
#include "../Utilities/Utilities.h"
#include "../Logger/Logger.h"

#pragma warning(disable:4522)
#pragma comment(lib, "Ws2_32.lib")

namespace Sockets
{

  /////////////////////////////////////////////////////////////////////////////
  // SocketSystem class - manages loading and unloading Winsock library

  class SocketSystem
  {
  public:
    SocketSystem();//WSAStartup();
    ~SocketSystem();//WSACleanup();
  private:
    int iResult;
    WSADATA wsaData;//Windows Socket API
  };

  /////////////////////////////////////////////////////////////////////////////
  // Socket class
  // - used by server for client handling!!
  // - base for SocketConnecter and SocketListener classes

  class Socket
  {
  public:
    enum IpVer { IP4, IP6 };
    using byte = char;

    // disable copy construction and assignment
    Socket(const Socket& s) = delete;
    Socket& operator=(const Socket& s) = delete;

    Socket(IpVer ipver = IP4);//ipver
    Socket(::SOCKET);//?
    Socket(Socket&& s);//move constructor
    operator ::SOCKET() { return socket_; }//?
    Socket& operator=(Socket&& s);//move operator
    virtual ~Socket();

    IpVer& ipVer();
    bool send(size_t bytes, byte* buffer);
    bool recv(size_t bytes, byte* buffer);
    size_t sendStream(size_t bytes, byte* buffer);
    size_t recvStream(size_t bytes, byte* buffer);
    bool sendString(const std::string& str, byte terminator = '\0');
    std::string recvString(byte terminator = '\0');
    static std::string removeTerminator(const std::string& src);
    size_t bytesWaiting();
    bool waitForData(size_t timeToWait, size_t timeToCheck);
    bool shutDownSend();
    bool shutDownRecv();
    bool shutDown();
    void close();

    bool validState() { return socket_ != INVALID_SOCKET; }

  protected:
    WSADATA wsaData;
    ::SOCKET socket_;//?(SOCKET	=	UINT_PTR)
    struct addrinfo *result = NULL, *ptr = NULL, hints;//!!!!
    int iResult;
    IpVer ipver_ = IP4;
  };

  /////////////////////////////////////////////////////////////////////////////
  // SocketConnecter class
  // - supports connecting to a SocketListener

  class SocketConnecter : public Socket
  {
  public:
    SocketConnecter(const SocketConnecter& s) = delete;
    SocketConnecter& operator=(const SocketConnecter& s) = delete;

    SocketConnecter();//default constructor
    SocketConnecter(SocketConnecter&& s);//move constructor
    SocketConnecter& operator=(SocketConnecter&& s);//move assignment
    virtual ~SocketConnecter();

    bool connect(const std::string& ip, size_t port);
  };

  /////////////////////////////////////////////////////////////////////////////
  // SocketListener class!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // - listens for incoming connections
  // - each connection is handled on its own thread!!!!!!!!!!!!!!!!!!!!1

  class SocketListener : public Socket
  {
  public:
    SocketListener(const SocketListener& s) = delete;
    SocketListener& operator=(const SocketListener& s) = delete;

    SocketListener(size_t port, IpVer ipv = IP6);//bool connect(const std::string& ip, size_t port);
    SocketListener(SocketListener&& s);
    SocketListener& operator=(SocketListener&& s);

    virtual ~SocketListener();

    template<typename CallObj>
    bool start(CallObj& co);//!!!!!!!!!!!!!!

    void stop();
  private:
    bool bind();
    bool listen();
    Socket accept();
    std::atomic<bool> stop_ = false;
    size_t port_;//qqqqqqqqqqqqqqqq
    bool acceptFailed_ = false;
  };

  //----< SocketListener start function runs listener on its own thread >------
  /*
  *  - Accepts Callable Object that defines the operations
  *    to handle client requests.        // callable object is client handle
  *  - You will find an example Callable Object, ClientProc,!!!!!!!!!!!!!!!!!!
  *    used in the test stub below
  */
  template<typename CallObj>//what is start mean??
  bool SocketListener::start(CallObj& co)//template generic socket  -whyit is template 
	  //Callobject ----
  {
    if (!bind())
    {
      return false;
    }

    if (!listen())
    {
      return false;
    }

    // listen on a dedicated thread so server's main thread won't block

    std::thread ListenThread(//ListenThreadStart

      [&]()//capture waht?
    {
      StaticLogger<1>::write("\n  -- server waiting for connection");

      while (!acceptFailed_)
      {
        if (stop_.load())
          break;

        // Accept a client socket - blocking call!!!!!!!

        Socket clientSocket = accept();    // uses move ctor! block connection until 
        if (!clientSocket.validState()) {
          continue;
        }
        StaticLogger<1>::write("\n  -- server accepted connection");

        // start thread to handle client request

        //std::thread clientThread(std::ref(co), std::move(clientSocket));
        std::thread clientThread(co, std::move(clientSocket));//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!transfer ownership of socket
        clientThread.detach();  // detach - listener won't access thread again


      }
      StaticLogger<1>::write("\n  -- Listen thread stopping");
    }
		//ListenThreadEnd
							);
    ListenThread.detach();
    return true;
  }
}
#endif

