// reading an entire binary file
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unistd.h>
#include <assert.h>
#include "layout.h"

using namespace std;


typedef struct {
    int *content;
    long size;
} FileContent;


void save(int i, std::vector<Body> &bodies) {

    std::string nameFile = to_string(i).append(".bin");

    std::ofstream outfile (nameFile, std::ofstream::binary);

    char block[3 * 4];
    int *triplet = (int *)&block;
    //for (vector<Body>::iterator body = bodies.begin() ; body != bodies->end(); ++body) {
    for(Body body : bodies){
        triplet[0] = floor(body.pos.x + 0.5);
        triplet[1] = floor(body.pos.y + 0.5);
        triplet[2] = floor(body.pos.z + 0.5);
        //    cout << triplet[0] << "," << triplet[1] << "," << triplet[2] << " <- node" << endl;
        outfile.write(block, 3 * 4);
    }
    outfile.close();
}

bool readFile(const std::string &fileName, FileContent &result) {
    streampos size;
    ifstream file (fileName, ios::in|ios::binary|ios::ate);
    
    if (file.is_open())
    {
        size = file.tellg();
        char* memblock = new char[size];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        file.close();
        result.size = size/4;
        result.content = (int *) memblock;
        return true;
    } else {
        return false;
    }
}

int getIterationNumberFromPositionFileName(const std::string &positionFileName) {
    cmatch match;
    regex pattern(".*?(\\d+)\\.bin$");
    regex_match(positionFileName.c_str(), match, pattern);
    if (match.size() == 2) {
        try {
            return stoi(match[1]) + 1;
        } catch (std::exception ex) {
            std::cout << "Exception Generated: " << ex.what();
        }
    }
    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << "\n"
        << "  layout++ links.bin [positions.bin]" << "\n"
        << "Where" << "\n"
        << " `links.bin` is a path to the serialized graph. See " << "\n"
        << "    https://github.com/anvaka/ngraph.tobinary for format description" << "\n"
        << "  `positions.bin` is optional file with previously saved positions. " << "\n"
        << "    This file should match `links.bin` graph, otherwise bad things " << "\n"
        << "    will happen" << "\n";
        return EXIT_FAILURE;
    }

    const std::string graphFileName = argv[1];
    srand(42);

    char cwd[1024];
    if (getcwd(cwd, 1024) != NULL) {
        cout << cwd << "\n";
    } else {
        cout << errno;
    }

    cout << "Loading links from " << graphFileName << "... " << "\n";
    FileContent graphFile;
    bool result = readFile(graphFileName, graphFile);
    assert(result == true);
    Layout graphLayout;
    int startFrom = 0;
    if (argc < 3) {
        graphLayout.init(graphFile.content, graphFile.size);
        cout << "Done. " << "\n";
        cout << "Loaded " << graphLayout.getBodiesCount() << " bodies;" << "\n";
    } else {
        const std::string posFileName = argv[2];
        startFrom = getIterationNumberFromPositionFileName(posFileName);
        cout << "Loading positions from " << posFileName << "... ";
        FileContent positions;
        bool result = readFile(posFileName, positions);
        assert(result == true);
        cout << "Done." << endl;
        graphLayout.init(graphFile.content, graphFile.size, positions.content, positions.size);
        cout << "Loaded " << graphLayout.getBodiesCount() << " bodies;" << "\n";
    }
    // TODO: This should be done via arguments, but doing it inline now:
    // If current folder containsfil 'weights.bin' we will try to assign
    // nodes weights from this file
    /*FileContent weights = readFile("weights.bin");
    if (weights != nullptr) {
        cout << "Detected weights.bin file in the current folder." << endl;
        cout << "Assuming each node has assigned body weight. Reading weights..." << endl;
        cout << "Size: " << weights.size;
        graphLayout.setBodiesWeight(weights.content);
    }*/
    int i = 0;
    while(!graphLayout.step()){
        i++;
        std::cout << "Step " << i << "\n";
        bool done = graphLayout.step();
        if(i % 1000 == 0) {
            save(i, graphLayout.getBodies());
        }
    }
    cout << "Done!" << "\n";
    delete[] graphFile.content;
    return EXIT_SUCCESS;
}
