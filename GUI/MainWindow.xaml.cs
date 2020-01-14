/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for OOD Project #4                     //
// ver 1.0                                                         //
// Tianxiang Yi, CSE687 - Object Oriented Design, Spring 2019      //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines the GUI using MainWindows.xaml file 
 * This package depends on Translator project to call function from Comm 
 * This provide some event handle function of GUI, such like button click
 * to send convertFile message, key up to change the Regex, download file etc
 * The GUI has two views, one far navigating the remote directory and anthoer
 * for displaying convreted files in server and downloaded files in client.
 * It is a client that could send request through comm to server and
 * process reply message to get information it needed, to displayed to user.n 
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translaterlibrary
 * 
 * Maintenance History:
 * --------------------
 *  ver 1.0 - April 23rd, 2019
 *    - first release
 */


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
 

    public MainWindow()
    {
      InitializeComponent();
    }

    public const string store = "..\\SaveFiles";

    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private CsEndPoint serverEndPoint_;
    private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_
      = new Dictionary<string, Action<CsMessage>>();

    private string Regex { get; set; }
    List<String> argv = new List<string>();
    //----< process incoming messages on child thread >----------------

    private void processMessages()
    {
      ThreadStart thrdProc = () =>
      {
        while (true)
        {
          CsMessage msg = translater.getMessage();
         // if (msg.attributes.Count == 0) continue;

          if (msg.attributes.ContainsKey("command"))
          {
            string msgId = msg.value("command");
            if (dispatcher_.ContainsKey(msgId))
              dispatcher_[msgId].Invoke(msg);
          }
        }
      };

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

    //----< load get received file processing into dispatcher dictionary >---------------

    private void DispatcherLoadget()
    {
      Action<CsMessage> get = (CsMessage rcvMsg) => {
        this.Dispatcher.Invoke(() =>
        {
          var enumer = rcvMsg.attributes.GetEnumerator();

          while (enumer.MoveNext())
          {
            string key = enumer.Current.Key;
            if (key.Contains("name"))
            {
              Action<string> show = (string f) =>
              {
                string t = System.IO.Path.Combine(store, f);
                t = System.IO.Path.GetFullPath(t);
                if (!File.Exists(t)) { return; }

                if (!(lstDownloaded.Items.Contains(f)))
                {
                  lstDownloaded.Items.Add(f);
                }

                System.Diagnostics.Process.Start(t); //
              };
              Dispatcher.Invoke(show, new Object[] { enumer.Current.Value });
            }
          }
        });
        };
      addClientProc("get", get);
      
    }

    //----< load convert processing into dispatcher dictionary >---------------

    private void DispatcherLoadconvert()
    {
      
        Action<CsMessage> convert = (CsMessage rcvMsg) =>
      {
        this.Dispatcher.Invoke(() =>
        {
          Action clrList = () =>
        {
          clearList();
        };
        Dispatcher.Invoke(clrList, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();

        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("filed"))
          {

            Action<string> dolist = (string file) =>
            {
              addList(file);
            };

            Dispatcher.Invoke(dolist, new Object[] { enumer.Current.Value });

          }
        }

        lstConverted.SelectedIndex = 1;
        LstConverted_MouseDoubleClick(lstConverted, null);
        });

      };
        addClientProc("convert", convert);
      
      
    }

    //----< load getDirs processing into dispatcher dictionary >-------

    private void DispatcherLoadGetDirs()
    {
      Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
      {
        Action clrDirs = () =>
        {
          clearDirs();
        };
        Dispatcher.Invoke(clrDirs, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("dir"))
          {
            Action<string> doDir = (string dir) =>
            {
              addDir(dir);
            };
            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });

          }
        }
     
        Action insertUp = () =>
        {
          insertParent();
        };
        Dispatcher.Invoke(insertUp, new Object[] { });
      };
      
      addClientProc("getDirs", getDirs);
      argv.Clear();
      
    }
    //----< load getFiles processing into dispatcher dictionary >------

    private void DispatcherLoadGetFiles()
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

 
    //----< load all dispatcher processing >---------------------------

    private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
      DispatcherLoadGetFiles();
      DispatcherLoadconvert();
      DispatcherLoadget();
      
    }
    //----< Demonstrate requirement3 to write in console >------
    public void req3()
    {
      Console.Write("\n  Demonstrating Requirement  #3-Assemble Projects to Client Server\n");
      Console.Write(" ===================================================================\n");
      Console.Write("  This is the client side of the Project4 which assemble parts of Projects\n" );
      Console.Write("  You could see that the Client need not wait for reply in the following\n");
      Console.Write("  example of demonstrate navigating remote dirs. This is the configuration\n");
      Console.Write("  of client side: \n");
    }
    //----< Demonstrate requirement4&5 to write in console >------
    public void req45() {
      Console.Write("\n\n  Demonstrating Requirement  #4&5-GUI and Message \n");
      Console.Write(" ===================================================================\n");
      Console.Write("  You can see the GUI on the client side got two tag, one is Navigation\n");
      Console.Write("  and another is ConvertedFiles. The Navigation tag got one address bar and\n");
      Console.Write("  two textlist which shows the dirs and files in Storage of server side \n");
      Console.Write("  When you set the path of address bar, the client will automatically send \n");
      Console.Write("  GetDir msg and GetFile msg to server. When you request dirs first,   \n");
      Console.Write("   you could say the client don't need to wait for msg reply to send the \n");
      Console.Write("  second request get files: \n");
      Console.Write("  I designed four send messages from client and four reply messages from server to\n");
      Console.Write("  satisfy requirement.The detailed demonstrate is in the beginning of server side \n");
    }
    //----< Demonstrate requirement6 to write in console >------
    public void req61()
    {
      Console.Write("\n\n  Demonstrating Requirement  #6-Convert \n");
      Console.Write(" =====================================================================\n");
      Console.Write("  When click the 'Up' button, the client would send message that contain\n");
      Console.Write("  command 'convert' to server and server will convert files and their path \n");
      Console.Write("  and regex(you could change the Regex with Key up in GUI) contained in message. \n");
      Console.Write("  And when converted, server will send the name of converted file list  \n" );
      Console.Write("  back to GUI which will show in ConvertedFileList\n");
      Console.Write("  At Server side,file converted stored in path \\convertedPage in thr root folder \n");
    }
    //----< Demonstrate requirement6 to write in console >------
    public void req62()
    {
      Console.Write("\n\n  Demonstrating Requirement  #6-Download \n");
      Console.Write(" =====================================================================\n");
      Console.Write("  When you click the converted fileList item, the client would send separate message\n");
      Console.Write("  to server to request download the file and server will send message that\n");
      Console.Write("  contains the file back to client. When receive the message that contain \n");
      Console.Write("  key word 'file', the client will go to file storage path to display it in \n");
      Console.Write("  browser, you could right click it to view source page code of the html\n");
      Console.Write("  At Client side,downloaded file in path ..\\SaveFiles related to GUI.exe\n");
      Console.Write("  Unfortunately, the server will post file message twice to client beacuse\n");
      Console.Write("  of some bugs in Fawcett's Comm code, so I have to display each received file twice\n");
    }
    // --------------< Load the default setting >---------------------
    private void Window_Loaded(object sender, RoutedEventArgs e) { 
      // start Comm
      String[] arguments = Environment.GetCommandLineArgs();
      req3();
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = "localhost";
      endPoint_.port = int.Parse(arguments[2]);
      translater = new Translater();;
      translater.listen(endPoint_);
      // start processing messages
      processMessages();

      // load dispatcher
      loadDispatcher();

       serverEndPoint_ = new CsEndPoint();
      serverEndPoint_.machineAddress = "localhost";
      serverEndPoint_.port = int.Parse(arguments[1]);//
      

      req45();

      PathTextBlock.Text = "Storage";
      Regex= "[A-B](.*)";
      txtregex.Text = Regex;
      pathStack_.Push("../Storage");
      CsMessage msg = new CsMessage();
      
      msg.add("to", CsEndPoint.toString(serverEndPoint_));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);

      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
       Up_Click(sender, e);
    

    }

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
        // don't pop off "Storage"
        if (pathStack_.Count > 1)  
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

      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint_));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);

      // build message to get files and post it
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);

     
    }

    //-----< Enter the Regex to be changed >-------

    private void TxtRegexes_KeyUp(object sender, System.Windows.Input.KeyEventArgs e)
    {
      if (e.Key == Key.Enter)
      {
        Regex = txtregex.Text;
        
      }
    }

    //-----< Click Up button to Convert files in server >-------
    private void Up_Click(object sender, RoutedEventArgs e)
    {
      argv.Clear();
      req61();
      argv.Add(removeFirstDir(pathStack_.Peek()));
 
      if (Regex != null)
      {
        argv.Add(Regex);
      }
      else
      {
        argv.Add("*.*");
      }


      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint_));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "convert");
      if (Regex != null)
      {
        msg.add("Regex", Regex);
      }
      else
      {
        msg.add("Regex", "[A-B](.*)");
      }
      msg.add("path", removeFirstDir(pathStack_.Peek()));
     
      translater.postMessage(msg);
      tabcontrol.SelectedIndex = 1;
    }

    //-----< Click DownloadedFiles to Display the File in Browser >----------

    private void LstDownloaded_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
     
      string select = lstDownloaded.SelectedItem.ToString();
      string path = System.IO.Path.Combine(store, select);
      path = System.IO.Path.GetFullPath(path);
      if (!File.Exists(path)) { return; }
      System.Diagnostics.Process.Start(path);
    }

    //-----< Click ConvertedFiles to Send Request to Download the File >-------

    private void LstConverted_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      req62();
      if (lstConverted.SelectedIndex == -1) { return; }

      String select = lstConverted.SelectedItem.ToString();
      CsMessage msg1 = new CsMessage();
      msg1.add("to", CsEndPoint.toString(serverEndPoint_));
      msg1.add("from", CsEndPoint.toString(endPoint_));
      msg1.add("command", "get");
      msg1.add("name", select);//
      translater.postMessage(msg1);

    }
  }
}
