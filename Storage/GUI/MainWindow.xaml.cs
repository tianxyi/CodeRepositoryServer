///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 1.0                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for Project3HelpWPF demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1/1 : 07 Aug 2018
 * - fixed bug in DirList_MouseDoubleClick by returning when selectedDir is null
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using MsgPassingCommunication;
using System.IO;


namespace WpfApp1
{
  public partial class MainWindow : Window
  {
    //public static void Main()//???????
    //{
    //  String[] arguments = Environment.GetCommandLineArgs();
    //  Console.WriteLine("", String.Join(",", arguments));
    //}

    public MainWindow()
    {
      InitializeComponent();
    }

    public const string store = "..\\SaveFiles";//add myself for 

    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_ //MsgDispatcher = std::unordered_map<string,function<Msg(Msg)>>
      = new Dictionary<string, Action<CsMessage>>();

    private string Regex { get; set; }
    List<String> argv = new List<string>();
    //----< process incoming messages on child thread >----------------

    private void processMessages()
    {
      ThreadStart thrdProc = () =>
      {//*begin//take code fo this thread
        while (true)
        {
          CsMessage msg = translater.getMessage();
          string msgId = msg.value("command");//
          if (dispatcher_.ContainsKey(msgId))
            dispatcher_[msgId].Invoke(msg);//process
        }
      };//*endscope;

      rcvThrd = new Thread(thrdProc);
      rcvThrd.IsBackground = true;
      rcvThrd.Start();
    }
    //----< function dispatched by child thread to main thread >-------

    private void clearDirs()
    {
      DirList.Items.Clear();
    }
    //----< function dispatched by child thread to main thread >-------

    private void clearList()
    {
      lstConverted.Items.Clear();
    }
    //----< function dispatched by child thread to main thread >-------

    private void addDir(string dir)
    {
      DirList.Items.Add(dir);
    }
    private void addList(string file)
    {
      lstConverted.Items.Add(file);
    }
    //----< function dispatched by child thread to main thread >-------

    private void insertParent()
    {
      DirList.Items.Insert(0, "..");
    }
    //----< function dispatched by child thread to main thread >-------

    private void clearFiles()
    {
      FileList.Items.Clear();
    }
    //----< function dispatched by child thread to main thread >-------

    private void addFile(string file)
    {
      FileList.Items.Add(file);
    }
    //----< add client processing for message with key >---------------

    private void addClientProc(string key, Action<CsMessage> clientProc)
    {
      dispatcher_[key] = clientProc;
    }


    private void DispatcherLoadget()//?????????????how to open downloaded file
    {
      Action<CsMessage> get = (CsMessage rcvMsg) => {
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("name"))
          {/////////////////////////////////////////////////////////////
            Action<string> show = (string f) =>
            {
              string t = System.IO.Path.Combine(store,f);
              t = System.IO.Path.GetFullPath(t);
              if (!File.Exists(t)) { return; }

              System.Diagnostics.Process.Start(t);
            };
            Dispatcher.Invoke(show, new Object[] { enumer.Current.Value });
            ////////////////////////////////////////////////////////////////
          }
        }
      };
      addClientProc("get", get);

    }
    //----< load convert processing into dispatcher dictionary >---------------
    private void DispatcherLoadconvert()//how to add dispatcher!!!
    {
      Action<CsMessage> convert = (CsMessage rcvMsg) =>//?
      {/////////////////////////////////////////
        Action clrList = () =>
        {
          clearList();
        };
        Dispatcher.Invoke(clrList, new Object[] { });
        ////////////////////////////////
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("filed"))
          {/////////////////////////////////////////////////////////////
            Action<string> dolist = (string file) =>
            {
              
              addList(file);
              
            };

            Dispatcher.Invoke(dolist, new Object[] { enumer.Current.Value });
            ////////////////////////////////////////////////////////////////
          }
        }
        //////////////////////////////////////////////////////////////
        
      };
      /////////////////////////////////////////////////////
      addClientProc("convert", convert);
    }




    //----< load getDirs processing into dispatcher dictionary >-------

    private void DispatcherLoadGetDirs()//how to add dispatcher!!!
    {
      Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
      {/////////////////////////////////////////
        Action clrDirs = () =>
        {
          clearDirs();
        };
        Dispatcher.Invoke(clrDirs, new Object[] { });
        ////////////////////////////////
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("dir"))
          {/////////////////////////////////////////////////////////////
            Action<string> doDir = (string dir) =>
            {
              addDir(dir);
            };
            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
            ////////////////////////////////////////////////////////////////
          }
        }
        //////////////////////////////////////////////////////////////
        Action insertUp = () =>
        {
          insertParent();
        };
        Dispatcher.Invoke(insertUp, new Object[] { });
      };
      ///////////////////////////////////////////////////////
      addClientProc("getDirs", getDirs);
      argv.Clear();
    }
    //----< load getFiles processing into dispatcher dictionary >------

    private void DispatcherLoadGetFiles()//define how do you handle the message!!!!!!!!!!
    {
      Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
      {

        Action clrFiles = () =>
        {
          clearFiles();
        };
        Dispatcher.Invoke(clrFiles, new Object[] { });

        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;

          if (key.Contains("file"))
          {
            Action<string> doFile = (string file) =>
            {
              addFile(file);
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });

          }
        }
      };
      addClientProc("getFiles", getFiles);
    }

    //----< load getFiles processing into dispatcher dictionary >------
 
    //----< load all dispatcher processing >---------------------------

    private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
      DispatcherLoadGetFiles();
      DispatcherLoadconvert();
      DispatcherLoadget();
     // DispatcherLoadfile();////////////////////////
    }
    //----< start Comm, fill window display with dirs and files >------

    private void Window_Loaded(object sender, RoutedEventArgs e)//19/virtual void start() = 0; not require sendfile
    {
      // start Comm
      String[] arguments = Environment.GetCommandLineArgs();
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = "localhost";
      // endPoint_.port = arguments[0];//8082;//19/cant lauch commdline same port! not hard coded
      endPoint_.port = 8082;
       translater = new Translater();
      //19/string workingDir = "../clientfiles";
      translater.listen(endPoint_);

      // start processing messages
      processMessages();

      // load dispatcher
      loadDispatcher();

      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;

      PathTextBlock.Text = "Storage";
      Regex= "[A-B](.*)";
      txtregex.Text = Regex;
      pathStack_.Push("../Storage");//??????????
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);//

      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);//
      //test1();
      //////////////////////////////////////////////////////////////////////////
      //string[] args = Environment.GetCommandLineArgs();
      //foreach (string arg in args)
      //{
      //  if (arg == "/demo")
      //  {
      //    startDemonstration();//
      //  }
      //}

    }
    //private void startDemonstration()
    //{
    //  for (int i = 0; i < 3; i++)
    //  {
    //    DirList.SelectedIndex = 0;
    //    DirList_MouseDoubleClick(DirList, null);//?
    //  }
    //}
    //qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
    ///
    //----< strip off name of first part of path >---------------------

    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }
    //----< respond to mouse double-click on dir name >----------------

    private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      // build path for selected dir
      string selectedDir = (string)DirList.SelectedItem;
      if (selectedDir == null)
        return;
      string path;
      if (selectedDir == "..")
      {
        if (pathStack_.Count > 1)  // don't pop off "Storage"
          pathStack_.Pop();
        else
          return;
      }
      else
      {
        path = pathStack_.Peek() + "/" + selectedDir;
        pathStack_.Push(path);
      }
      // display path in Dir TextBlcok
      PathTextBlock.Text = removeFirstDir(pathStack_.Peek());

      // build message to get dirs and post it
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);

      // build message to get files and post it
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);

     
    }
    //----< first test not completed >---------------------------------

    //void test1()
    //{
    //  MouseButtonEventArgs e = new MouseButtonEventArgs(null, 0, 0);
    //  DirList.SelectedIndex = 1;
    //  DirList_MouseDoubleClick(this, e);
    //}
    private void TxtRegexes_KeyUp(object sender, System.Windows.Input.KeyEventArgs e)
    {
      if (e.Key == Key.Enter)
      {
        Regex = txtregex.Text;
        
      }
    }
    private  void Up_Click(object sender, RoutedEventArgs e)
    {
      argv.Clear();
      //argv.Add("dummy");//0
      argv.Add(removeFirstDir(pathStack_.Peek()));//1//////////////   Storage//...
      //if (true)
      //{
      //  argv.Add(@"\s");//2
      //}
      if (Regex != null)//3/////////////
      {
        argv.Add(Regex);
      }
      else
      {
        argv.Add("*.*");
      }

      //argv.Add("*.h");//4
      //argv.Add("*.cpp");//5

      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "convert");
      if (Regex != null)//3/////////////
      {
        msg.add("Regex", Regex);
      }
      else
      {
        msg.add("Regex", "[A-B](.*)");
      }
      msg.add("path", removeFirstDir(pathStack_.Peek()));
     // msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      tabcontrol.SelectedIndex = 1;//when has converted then go to ? async?
    }

    private void LstConverted_MouseDoubleClick(object sender, MouseButtonEventArgs e)////////////////
    {
      //if (lstConverted.SelectedIndex == -1) { return; }
      String select= lstConverted.SelectedItem.ToString();

      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;
      CsMessage msg1 = new CsMessage();
      msg1.add("to", CsEndPoint.toString(serverEndPoint));
      msg1.add("from", CsEndPoint.toString(endPoint_));
      msg1.add("command", "get");
      msg1.add("name", select);//


      // msg.add("path", pathStack_.Peek());
      translater.postMessage(msg1);
     //  System.Diagnostics.Process.Start(select);
    }
  }
}
