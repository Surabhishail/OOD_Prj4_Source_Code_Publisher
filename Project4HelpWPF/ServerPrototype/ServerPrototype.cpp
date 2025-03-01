/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

Dirs Server::getDirs(const Repository::SearchPath& path)
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
char * sysStrToCharArray(std::string stdString) {
	char *temp = new char[stdString.length() + 1];


	for (size_t i = 0; i < stdString.length(); ++i)
		temp[i] = char(stdString[i]);
	temp[stdString.length()] = '\0';
	return temp;
}

char ** sysStrArrayToCharArray(std::vector<std::string> argv) {
	int a = argv.size();
	char **temp = new char*[a];
	for (int i = 0; i < argv.size(); ++i)
		temp[i] = sysStrToCharArray(argv[i]);

	return temp;
}


std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

std::function<Msg(Msg)> getConvertedFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getConvertedFiles");
  std::vector<std::string> cmdarg_;

 
  /*for (auto arg : msg.value("convert"))
  {
	  
  }*/
  for (int i = 1; i <= 7; i++)
  {
	  std::string countStr = Utilities::Converter<size_t>::toString(i);
      std::string arg = msg.value("convert" + countStr);
	  cmdarg_.push_back(arg);
  }
  char **argv = sysStrArrayToCharArray(cmdarg_);
  int argc = 7;

  Executive exec;
  exec.processCommandLineArgs(argc, argv);
  
  exec.extractFiles();
  exec.publish();
  std::vector<std::string> convertfiles = exec.getConvertedFiles();
    size_t count = 0;
    for (auto item : convertfiles)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("convertResult" + countStr, item);
    }
 // reply.attribute("convertResult", "Did Convert");
  
  return reply;
};
/*
std::function<Msg(Msg)> getFile = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("file");
  reply.file(mag.attributes()["name"]);
  return reply;
};

std::function<Msg(Msg)> convert = [](Msg msg) {
call code publisher and have it convert it
reply say they are done
then client will send msg back saying I need files
Just send list of files
Double click on any file then it will convert it request of that file only

***Try not to block receiver thread,Optionalll
create thread and using future and promise and then prepare msg.
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("file");
  reply.file(mag.attributes()["name"]);
  return reply;
};


*/

std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
	std::cout << "Search Path" << searchPath;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
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
  //Files files = server.getFiles();
  //show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n";

  std::cout << "\n  testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("getConvertedFiles", getConvertedFiles);

  //AM   server.addMsgProc("file", file);
	//AM   server.addMsgProc("convert", file);

  server.addMsgProc("serverQuit", echo);

  server.processMessages();
  
  Msg msg(serverEndPoint, serverEndPoint);  // send to self
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  /*msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  /*msg.command("getConvertedFiles");
  msg.attribute("convertResult","Did Convert");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/


  std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

