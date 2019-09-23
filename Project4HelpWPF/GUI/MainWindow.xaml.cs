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

namespace WpfApp1
{
  public partial class MainWindow : Window
  {
    public MainWindow()
    {
      InitializeComponent();
    }

    private string WDirectory { get; set; }
    private string Patterns { get; set; }
    private string Regex { get; set; }

    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private CsEndPoint serverEndPoint;

        private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_ 
      = new Dictionary<string, Action<CsMessage>>();

        //CsEndPoint serverEndPoint = new CsEndPoint();
        

    //----< process incoming messages on child thread >----------------

    private void processMessages()
    {
      ThreadStart thrdProc = () => {
        while (true)
        {
          CsMessage msg = translater.getMessage();
          string msgId = msg.value("command");
          if (dispatcher_.ContainsKey(msgId))
            dispatcher_[msgId].Invoke(msg);
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
        private void clearFiles()
        {
            lstConverted.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir)
    {
      DirList.Items.Add(dir);
    }
        private void addConvertedList(string conv_file)
        {
            lstConverted.Items.Add(conv_file);
        }
        //----< function dispatched by child thread to main thread >-------

        private void insertParent()
    {
      DirList.Items.Insert(0, "..");
    }
    //----< add client processing for message with key >---------------

    private void addClientProc(string key, Action<CsMessage> clientProc)
    {
      dispatcher_[key] = clientProc;
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
    }

     //----< adds getConvertedFiles processing into dispatcher dictionary >-------

        private void DispatchergetConvertedFiles()
        {
            Action<CsMessage> getConvertedFiles = (CsMessage rcvMsg) =>
            {
                Action clrConvertFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrConvertFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("convertResult"))
                    {
                        Action<string> doPublish = (string conv_file) =>
                        {
                            addConvertedList(conv_file);
                        };
                        Dispatcher.Invoke(doPublish, new Object[] { enumer.Current.Value });
                    }

                }
                
            };
            addClientProc("getConvertedFiles", getConvertedFiles);
        }

        //----< load all dispatcher processing >---------------------------

        private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
    }
    //----< start Comm, fill window display with dirs and files >------

    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      // start Comm
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = "localhost";
      endPoint_.port = 8082;
      translater = new Translater();
      translater.listen(endPoint_);

      // start processing messages
      processMessages();

      // load dispatcher
      loadDispatcher();

      serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;

            //txtPath.Text = "Storage";
            //pathStack_.Push("../Storage");

       WDirectory = "../CodeUtilities";
       WDirectory = System.IO.Path.GetFullPath(WDirectory);
       Patterns = "*.h$$*.cpp";
       Regex = "[G-H](.*)";

            txtPath.Text = WDirectory;
            txtPatterns.Text = Patterns;
            txtRegex.Text = Regex;

      pathStack_.Push("../CodeUtilities");
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      msg.remove("command");
     // msg.add("command", "getFiles");
      //translater.postMessage(msg);
            /*
             CsMessage msgF = new CsMessage();
         msgF.add("to",CsEnd
         msgF.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getFile");
      msg.add("file","filename.h");
      translater.postMessage(msgF);
      msgF.attributes["path"] = "Filename.h";
            translater.postMessage(msgF);

      */
            //test1();
        }
        /*
         private void startDemo()
         {
         
             
             
            } */
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
      if(selectedDir == "..")
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
      txtPath.Text = removeFirstDir(pathStack_.Peek());
      
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
      msg.remove("command");
     }
    //----< first test not completed >---------------------------------

    void test1()
    {
      MouseButtonEventArgs e = new MouseButtonEventArgs(null, 0, 0);
      DirList.SelectedIndex = 1;
      DirList_MouseDoubleClick(this, e);
    }

        private void lstConverted_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            try
            {
                if (!displayNotepad.IsChecked.Value)
                    System.Diagnostics.Process.Start(lstConverted.SelectedItem.ToString());
                else
                    System.Diagnostics.Process.Start("Notepad.exe", lstConverted.SelectedItem.ToString());
            }
            catch (Exception)
            {
            }

        }

        private void btnPublish_Click(object sender, RoutedEventArgs e)
        {
            lstConverted.Items.Clear();
            String Subdir = "";
            Dispatcher.Invoke(() =>
            {
                if (cbRecurse.IsChecked.ToString() == "True")
                {
                    Subdir = "/s";
                }
            });
            String cmd = "";
            Dispatcher.Invoke(() =>
            {
                cmd = "FROMGui" + "$$" + txtPath.Text + "$$" + Subdir + "$$" + txtPatterns.Text + "$$" + txtRegex.Text + "$$";
            });
            String[] argv = cmd.Split(new String[] { "$$" }, StringSplitOptions.None);
            int argc = argv.Length;
            
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getConvertedFiles");
            int count = 0;
            foreach (string arg in argv)
            {
                string countStr = (++count).ToString();
          
                msg.add("convert"+countStr,arg);

            }
            
            translater.postMessage(msg);
            msg.remove("command");
            DispatchergetConvertedFiles();
     
        }

        private void txtPatterns_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Patterns = txtPatterns.Text;
                txtPath_TextChanged(this, null);
            }

        }

        private void btnBrowse_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog dialog;
            dialog = new System.Windows.Forms.FolderBrowserDialog();

            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                WDirectory = dialog.SelectedPath;
                txtPath.Text = WDirectory;
            }

        }

    private void txtPath_TextChanged(object sender, TextChangedEventArgs e)
     {
            // build message to get dirs and post it
            /*CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command"); */




        }
    }
}
