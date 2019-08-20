#pragma once

#include <cstdlib>
#include <string>
#include <fstream>
//#include <cstring>
//#include <chrono>
//#include <locale>
//#include <codecvt>
//#include <climits>


struct File
{
  static const int UNINITIALIZED = 0, CONTENT_READ = 1, OPEN_FAILED = -1, OPENED_FOR_WRITING = 2;
  int status = UNINITIALIZED;
  virtual ~File() {}
  virtual void open(std::string filename) {}
  virtual void save(std::string filename) {}
};


struct TextFile : public File
{
  std::string content;
  std::filebuf *buf = NULL;

  static bool Read(const std::string &filename, std::string &content) {
    if(filename.size() > 0) {
      TextFile file;
      file.readContent(filename);
      if(file.status == File::CONTENT_READ) {
        content = std::string(file.content);
        return true;
      }
    }
    return false;
  }
  static bool Write(const std::string &filename, const std::string &content) {
    if(filename.size() > 0) {
      TextFile file;
      file.openForWriting(filename);
      if(file.status == File::OPENED_FOR_WRITING) {
        file.writeToFile(content.c_str());
        file.finishWriting();
        return true;
      }
    }
    return false;
  }

  TextFile() {}

  TextFile(const std::string &filename) {
    readContent(filename);
  }

  ~TextFile() {
    if(buf != NULL) {
      delete buf;
    }
  }

  void writeToFile(const std::string &filename, const std::string &content) {
    openForWriting(filename);
    writeToFile(content);
    finishWriting();
  }

  void openForWriting(const std::string &filename) {
    buf = new std::filebuf();
    if(buf == NULL || buf->open(filename.c_str(), std::ios_base::out) == NULL) {
      printf("Error. Can't access file %s\n", filename.c_str());
      return;
    }
    status = OPENED_FOR_WRITING;
  }

  void writeToFile(const std::string &str) {
    if(status != OPENED_FOR_WRITING) {
      printf("Error. File not opened for writing.\n");
      return;
    }
    buf->sputn(str.c_str(), str.size());
  }

  void finishWriting() {
    buf->close();
    status = File::UNINITIALIZED;
  }

  void readContent(const std::string &filename) {
    buf = new std::filebuf();
    if(buf->open(filename.c_str(), std::ios_base::in) == NULL) {
      printf("Error. Can't open file %s\n", filename.c_str());
      status = OPEN_FAILED;
    }

    if(status != OPEN_FAILED) {
      std::streamsize size = buf->in_avail();
      char *tmp = new char[size+1];
      buf->sgetn(tmp, size);
      content = tmp;
      delete tmp;
      buf->close();
      status = File::CONTENT_READ;
    }
  }

  void print() {
    printf("%s", content.c_str());
  }
};

