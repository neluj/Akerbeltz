#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <istream>
#include <sstream>
#include <vector>
#include <iostream>
#include "search.h"
#include "position.h"

using namespace Xake;

struct PerftSample{

  std::string fen_notation;
  //Saves nodes for each depth on [depth-1]
  std::vector<long long int> depthNodes; 

};

void loadPerftSampleLine(std::vector<PerftSample>& samples){

    namespace fs = std::filesystem;

    fs::path relativePath = "perftsuite.epd";
    fs::path fullPath = fs::current_path() / relativePath;

    std::cout << "Intentando abrir: " << fullPath << std::endl;

    std::fstream fileIn;
    fileIn.open(fullPath, std::ios::in);

    if (!fileIn.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo: " << fullPath << "\n";
        return;
    }

  std::string line; 

  while (std::getline(fileIn, line)) { 

    std::string word;
    std::vector<std::string> words;
    std::string delimiter = " ;";

    size_t pos = 0;
    std::string token;
    
    while ((pos = line.find(delimiter)) < std::string::npos) {
        token = line.substr(0, pos);        
        words.push_back(token);
        line.erase(0, pos + delimiter.length());
    }

    words.push_back(line);


    PerftSample sample;

    sample.fen_notation = words.at(0);

    for(std::size_t ind = 1; ind < words.size(); ++ind){

      std::stringstream depthStream(words.at(ind));
      std::string name, depthString;
      std::vector<std::string> words;
      depthStream>>name>>depthString;

      sample.depthNodes.push_back(std::stoll(depthString));
    }

    samples.push_back(sample); 
    
  } 

}

TEST(PerftTest, DISABLED_FileTest) {
//TEST(PerftTest, FileTest) {

  std::vector<PerftSample> samples;

  loadPerftSampleLine(samples);
  int lineNumber = 0;
  Xake::Time totalTimeMS = 0;

  for(auto &sample: samples){

    Position position; 

    const std::string FEN_POSITION = sample.fen_notation;
    ++lineNumber;
    
    position.set_FEN(FEN_POSITION);

    std::cout << "Test for line: " << lineNumber << ": " << sample.fen_notation << "\n";
    Search::SearchInfo searchInfo;

    for(size_t ind = 0; ind < sample.depthNodes.size(); ++ind){

      std::cout << "Depth " << ind+1 << " :" << "\n";
      searchInfo.depth = ind+1;
      //Start counting time
      searchInfo.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();

      NodesSize searchedNodes = Search::perftTest(position, searchInfo);

      totalTimeMS += searchInfo.realTime;
      
      EXPECT_EQ(searchedNodes, sample.depthNodes.at(ind));
      
    }

    std::cout << "\n" << "Total ms: " << totalTimeMS << "\n\n";

  }

}