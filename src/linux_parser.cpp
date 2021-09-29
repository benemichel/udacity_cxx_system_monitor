#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {

  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }

  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string mem_total_key = "MemTotal:";
  string mem_free_key = "MemFree:";

  float total, free;

  std::ifstream filestream(kProcDirectory +  kMeminfoFilename);
  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      linestream >> key;

      if (key == mem_free_key) {
        linestream >> free;
        
      }
      else if (key == mem_total_key) {
        linestream >> total;
      }
    }

    return (total - free) / total;
  }

  return 0;
 }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime;
  string line;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if (filestream.is_open()) {
      filestream >> uptime;
      return uptime;
    }

  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    string line;
    std::getline(stream, line);

    string dump;
    long utime;
    long ntime;
    long systime;
    long idle;
    long iowait;
    long irq;
    long softirq;
    long steal;
    long gtime;
    long gntime;
    std::istringstream linestream(line);
    linestream >> dump >> utime >> ntime >> systime >> idle >> iowait >> irq >> softirq >> steal >> gtime >> gntime;
    long total = utime + ntime + systime + irq + softirq + steal + idle +iowait;

    return total;
  }

  return 0;
 }

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string dump;
  long utime, stime, cutime, cstime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for( int i = 0; i < 13; i++) {
        linestream >> dump;
      }
      linestream >> utime >> stime >> cutime >> cstime;
      long total = utime + stime + cutime + cstime;
      return total;
    }
  }

  return 0;


 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    string line;
    std::getline(stream, line);

    string dump;
    long utime;
    long ntime;
    long systime;
    long idle;
    long iowait;
    long irq;
    long softirq;
    long steal;
    long gtime;
    long gntime;
    std::istringstream linestream(line);
    linestream >> dump >> utime >> ntime >> systime >> idle >> iowait >> irq >> softirq >> steal >> gtime >> gntime;
    long total = utime + ntime + systime + irq + softirq + steal;

    return total;
  }

  return 0;
 }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  return Jiffies() - ActiveJiffies(); }

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() { 
  return (float)ActiveJiffies() / (float)Jiffies();
  }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  long nprocesses = 0;
  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      linestream >> key;

      if (key == "processes") {
        linestream >> nprocesses;
        return nprocesses;
      }
    }
  }

  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  long nprocesses = 0;
  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      linestream >> key;

      if (key == "procs_running") {
        linestream >> nprocesses;
        return nprocesses;
      }
    }
  }

  return 0;
 }

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmd;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, cmd);  //complete line
    return cmd;
  }
  return cmd;
}

// TODO: Read and return the memory used by a process

string LinuxParser::Ram(int pid) { 
  string ram;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      linestream >> key;

      if (key == "VmSize") {
        linestream >> ram;
        return ram;
      }
    }
  }

  return ram;
   }

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    string line;
    string uid;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      linestream >> key;

      if (key == "Uid") {
        linestream >> uid;
        return uid;
      }
    }
  }

  return "";
 }

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string line;
  string user, x, id;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> id;
      if (id == uid) {
        return user;
      }
    }
  }
  return user;
 }

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long uptime = 0;

  std:: ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    for (int i = 0; i < 22; i++) {
      filestream >> uptime;
    }
  }
  long uptime_process = LinuxParser::UpTime() - uptime / sysconf(_SC_CLK_TCK);

  return uptime_process;

 }
