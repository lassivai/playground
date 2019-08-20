#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <fstream>


template<class T>
static bool saveCerealBinary(T &t, const std::string &filename) {
  std::ofstream os(filename, std::ios::binary);
  if(os.is_open()) {
    cereal::BinaryOutputArchive archive(os);
    archive(t);
    return true;
  }
  return false;
}

template<class T>
static bool loadCerealBinary(T &t, const std::string &filename) {
  std::ifstream is(filename, std::ios::binary);
  if(is.is_open()) {
    cereal::BinaryInputArchive archive(is);
    archive(t);
    return true;
  }
  return false;
}

template<class T>
static bool saveCerealXML(T &t, const std::string &filename) {
  std::ofstream os(filename);
  if(os.is_open()) {
    cereal::XMLOutputArchive archive(os);
    archive(t);
    return true;
  }
  return false;
}

template<class T>
static bool loadCerealXML(T &t, const std::string &filename) {
  std::ifstream is(filename);
  if(is.is_open()) {
    cereal::XMLInputArchive archive(is);
    archive(t);
    return true;
  }
  return false;
}
