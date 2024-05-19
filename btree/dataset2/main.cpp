#include <iostream>
#include <vector>
#include "BPlusTree.h"

using namespace std;

int main() {
    // Archivos para almacenar el índice, los datos y los metadatos
    string indexfile = "index.dat";
    string datafile = "data.dat";
    string metafile = "metadata.dat";
    string csvFile = "dataset.csv";

    // Crear un B+ Tree
    BPlusTree<int> btree(indexfile, datafile, metafile, csvFile);

    btree.printAllRecords();

    return 0;
}
