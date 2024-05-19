#ifndef BPlusTree_H
#define BPlusTree_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "node.h"

using namespace std;

template <typename TK>
class BPlusTree {
private:
    string indexfile;
    string datafile;
    string metadatafile;
    string csvFile;
    int T; // TAM_PAG = (2 * T) - 1
    Metadata metadata;

    PageIndex readPageIndex(long pos);
    void writePageIndex(PageIndex& page, long pos);
    long allocateNewPage(PageIndex& page);
    Record readRecord(long pos);
    long writeRecord(Record& record);
    void updateMetadata(Metadata& metadata);
    void rangeSearchRec(TK begin, TK end, vector<Record>& result, PageIndex& node);
    void printIndexRec(PageIndex& node, long pos);
    Metadata initFiles();
    void shiftLevel(TK key, long value, long currentPos, long childPos);
    long findParent(long currentPos, long childPos);
    Record searchRec(TK key, long nodePos);
    void load();
    void printAllRecordsRec(PageIndex& node);
public:
    BPlusTree(const string& idxfile, const string& datafile, const string& metadatafile, const string& csvFile);
    Record search(TK key);
    bool insert(TK key, Record& record);
    void rangeSearch(TK begin, TK end, vector<Record>& result);
    void printIndex();
    void printAllRecords();
};

template <typename TK>
BPlusTree<TK>::BPlusTree(const string& idxfile, const string& datafile, const string& metadatafile, const string& csvFile)
    : T(floor((R + 1) / 2)), indexfile(idxfile), datafile(datafile), metadatafile(metadatafile), csvFile(csvFile) {
    metadata = initFiles();
    load();
}

template <typename TK>
void BPlusTree<TK>::load() {
    vector<Record> records = readCSV(csvFile);
    for (auto& record : records) {
        insert(record.key, record);
    }
}

template <typename TK>
Metadata BPlusTree<TK>::initFiles() {
    return init_files(datafile, indexfile, metadatafile);
}

template <typename TK>
void BPlusTree<TK>::updateMetadata(Metadata& metadata) {
    update_metadata(metadatafile, metadata);
}

template <typename TK>
PageIndex BPlusTree<TK>::readPageIndex(long pos) {
    return read_page_index(indexfile, pos);
}

template <typename TK>
void BPlusTree<TK>::writePageIndex(PageIndex& page, long pos) {
    ofstream file(indexfile, ios::binary | ios::in | ios::out);
    file.seekp(pos, ios::beg);
    file.write((char*)&page, sizeof(PageIndex));
    file.close();
}

template <typename TK>
long BPlusTree<TK>::allocateNewPage(PageIndex& page) {
    return allocate_new_page(indexfile, page);
}

template <typename TK>
Record BPlusTree<TK>::readRecord(long pos) {
    return Record().read_record(datafile, pos);
}

template <typename TK>
long BPlusTree<TK>::writeRecord(Record& record) {
    return record.write_record(datafile);
}

template <typename TK>
Record BPlusTree<TK>::search(TK key) {
  if (metadata.rootPos == -1) return Record();
  return searchRec(key, metadata.rootPos);
}

template <typename TK>
Record BPlusTree<TK>::searchRec(TK key, long nodePos) {
    PageIndex node = readPageIndex(nodePos);

    int i = 0;

    while (i < node.count && key > node.entries[i]) {
        i++;
    }

    if (node.entries[i] == key) {
        long address = node.values[i];
        return readRecord(address);
    }

    if (node.is_leaf) {
        return Record();
    }else {
        return searchRec(key, node.children[i]);
    }
}

template <typename TK>
bool BPlusTree<TK>::insert(TK key, Record& record) {
    if(metadata.rootPos == -1) {
        PageIndex root;
        root.is_leaf = true;
        root.count = 1;
        root.entries[0] = key;
        root.values[0] = writeRecord(record);
        metadata.rootPos = allocateNewPage(root);

        return true;
    }else {
        Record result = search(key);
        if (result.key == key) return false;

        long currentPos = metadata.rootPos;
        PageIndex current = readPageIndex(currentPos);
        long parentPos = -1;

        while(current.is_leaf == false) {
            parentPos = currentPos;
            
            for(int i = 0; i < current.count; i++){
                if(key < current.entries[i]){
                    currentPos = current.children[i];
                    current = readPageIndex(currentPos);
                    break;
                }

                if(i == current.count - 1){
                    currentPos = current.children[i + 1];
                    current = readPageIndex(currentPos);
                    break;
                }
            }
        }

        if(current.count < 2 * T - 1){
            int i = 0;

            while(key > current.entries[i] && i < current.count){
                i++;
            }

            for(int j = current.count; j > i; j--){
                current.entries[j] = current.entries[j - 1];
                current.values[j] = current.values[j - 1];
            }

            current.entries[i] = key;
            current.values[i] = writeRecord(record);
            current.count++;
            current.children[current.count] = current.children[current.count - 1];
            current.children[current.count - 1] = -1;

            writePageIndex(current, currentPos);
        } else {
            PageIndex newLeaf;

            int tempNode[T * 2];
            memset(tempNode, -1, sizeof(tempNode));

            int tempValues[T * 2];
            memset(tempValues, -1, sizeof(tempValues));

            for(int i = 0; i < T * 2 - 1; i++){
                tempNode[i] = current.entries[i];
                tempValues[i] = current.values[i];
            }

            int i = 0, j;

            while (key > tempNode[i] && i < T * 2 - 1){
                i++;
            }

            for(int j = T * 2; j > i; j--){
                tempNode[j] = tempNode[j - 1];
                tempValues[j] = tempValues[j - 1];
            }

            tempNode[i] = key;
            tempValues[i] = writeRecord(record);

            newLeaf.is_leaf = true;
            current.count = T;
            newLeaf.count = (2 * T) - T;

            long newLeafPos = allocateNewPage(newLeaf);
            current.children[current.count] = newLeafPos;
            newLeaf.children[newLeaf.count] = current.children[2 * T - 1];

            current.children[newLeaf.count] = current.children[2 * T - 1];
            // current.children[2 * T - 1] = -1;

            for(int i = 0; i < current.count; i++){
                current.entries[i] = tempNode[i];
                current.values[i] = tempValues[i];
            }

            for(int i = 0, j = current.count; i < newLeaf.count; i++, j++){
                newLeaf.entries[i] = tempNode[j];
                newLeaf.values[i] = tempValues[j];
            }

            current.next = newLeafPos;

            writePageIndex(current, currentPos);
            writePageIndex(newLeaf, newLeafPos);

            if(currentPos == metadata.rootPos){
                PageIndex newRoot;
                newRoot.entries[0] = newLeaf.entries[0];
                newRoot.values[0] = newLeaf.values[0];
                newRoot.children[0] = currentPos;
                newRoot.children[1] = newLeafPos;
                newRoot.is_leaf = false;
                newRoot.count = 1;
                metadata.rootPos = allocateNewPage(newRoot);
            } else {
                shiftLevel(newLeaf.entries[0], newLeaf.values[0], parentPos, newLeafPos);
            }
        }
    }

    metadata.n++;
    updateMetadata(metadata);
    
    return true;
}

template <typename TK>
void BPlusTree<TK>::shiftLevel(TK key, long value, long currentPos, long childPos) {
    PageIndex current = readPageIndex(currentPos);

    if(current.count < 2 * T - 1){
        int i = 0;

        while (key > current.entries[i] && i < current.count) i++;
        for(int j = current.count; j > i; j--){
            current.entries[j] = current.entries[j - 1];
            current.values[j] = current.values[j - 1];
        }

        for(int j = current.count + 1; j > i + 1; j--){
            current.children[j] = current.children[j - 1];
        }

        current.entries[i] = key;
        current.values[i] = value;
        current.children[i + 1] = childPos;
        current.count++;

        writePageIndex(current, currentPos);
    } else {
        PageIndex newChild;

        // El error está aquí, cuando se intercambian e insertan los valores, hay valores en -1
        int tempKey[2 * T];
        memset(tempKey, -1, sizeof(tempKey));
        int tempValue[2 * T];
        memset(tempValue, -1, sizeof(tempValue));
        long tempChild[2 * T + 1];
        memset(tempChild, -1, sizeof(tempChild));

        for(int i = 0; i < 2 * T - 1; i++){
            tempKey[i] = current.entries[i];
            tempValue[i] = current.values[i];
        }

        for(int i = 0; i < 2 * T; i++){
            tempChild[i] = current.children[i];
        }

        int i = 0, j;

        while (key > tempKey[i] && i < 2 * T - 1) i++;

        for(int j = 2 * T; j > i; j--){
            tempKey[j] = tempKey[j - 1];
            tempValue[j] = tempValue[j - 1];
        }

        tempKey[i] = key;
        tempValue[i] = value;

        for(int j = 2 * T + 1; j > i + 1; j--){
            tempChild[j] = tempChild[j - 1];
        }

        tempChild[i + 1] = childPos;
        newChild.is_leaf = false;
        current.count = T;

        newChild.count = (2 * T - 1) - T;

        for(int i = 0, j = current.count + 1; i < newChild.count; i++, j++){
            newChild.entries[i] = tempKey[j];
            newChild.values[i] = tempValue[j];
        }

        for(int i = 0, j = current.count + 1; i < newChild.count + 1; i++, j++){
            newChild.children[i] = tempChild[j];
        }

        writePageIndex(current, currentPos);
        long newChildPos = allocateNewPage(newChild);
        if(currentPos == metadata.rootPos) {
            PageIndex newRoot;
            newRoot.entries[0] = current.entries[current.count];
            newRoot.values[0] = current.values[current.count];
            newRoot.children[0] = currentPos;
            newRoot.children[1] = newChildPos;
            newRoot.is_leaf = false;
            newRoot.count = 1;
            metadata.rootPos = allocateNewPage(newRoot);
            updateMetadata(metadata);
        } else {
            shiftLevel(current.entries[current.count], current.values[current.count], findParent(metadata.rootPos, currentPos) , newChildPos);
        }
    }
}

template <typename TK>
long BPlusTree<TK>::findParent(long currentPos, long childPos) {
    PageIndex current = readPageIndex(currentPos);
    PageIndex currentChild0 = readPageIndex(current.children[0]);    
    
    if(current.is_leaf || currentChild0.is_leaf) return -1;

    for(int i = 0; i < current.count + 1; i++){
        if(current.children[i] == childPos) return currentPos;
        long result = findParent(current.children[i], childPos);
        if(result != -1) return result;
    }

    return -1;
}

template <typename TK>
void BPlusTree<TK>::rangeSearch(TK begin, TK end, vector<Record>& result) {
    if (metadata.rootPos == -1) return;
    PageIndex root = readPageIndex(metadata.rootPos);
    rangeSearchRec(begin, end, result, root);
}

template <typename TK>
void BPlusTree<TK>::rangeSearchRec(TK begin, TK end, vector<Record>& result, PageIndex& node) {
    if (!node.is_leaf) {
        // Si el nodo no es hoja, buscar el hijo apropiado
        int pos = 0;
        while (pos < node.count && begin >= node.entries[pos]) {
            pos++;
        }
        long address = node.children[pos];
        PageIndex childNode = readPageIndex(address);
        rangeSearchRec(begin, end, result, childNode);
    } else {
        // Si es una hoja, buscar los registros en el rango especificado
        int pos = 0;
        while (pos < node.count && begin > node.entries[pos]) {
            pos++;
        }

        // A partir de este nodo, agregar todos los registros <= end
        bool done = false;
        while (!done) {
            for (int i = pos; i < node.count; i++) {
                if (node.entries[i] > end) {
                    done = true;
                    break;
                }
                long address = node.values[i];
                Record record = readRecord(address);
                result.push_back(record);
            }
            if (!done && node.next != -1) {
                long next = node.next;
                node = readPageIndex(next);
                pos = 0; // Reiniciar la posición para el nuevo nodo
            } else {
                done = true;
            }
        }
    }
}

template <typename TK>
void BPlusTree<TK>::printIndex() {
    if (metadata.rootPos == -1) return;
    PageIndex root = readPageIndex(metadata.rootPos);
    cout << "Root: " << metadata.rootPos << endl;
    cout << "Metadata: " << metadata.n << endl;
    printIndexRec(root, metadata.rootPos);
}

template <typename TK>
void BPlusTree<TK>::printIndexRec(PageIndex& node, long pos) {
    cout << "Node: " << pos << endl;
    cout << "Is leaf: " << node.is_leaf << endl;
    cout << "Count: " << node.count << endl;
    cout << "Next: " << node.next << endl;
    cout << "Entries: ";
    for (int i = 0; i < node.count; i++) {
        cout << node.entries[i] << " ";
    }
    cout << endl;
    cout << "Values: ";
    for (int i = 0; i < node.count; i++) {
        cout << node.values[i] << " ";
    }
    cout << endl;
    cout << "Children: ";
    for (int i = 0; i <= node.count; i++) {
        cout << node.children[i] << " ";
    }

    cout << endl;
    
    if(node.is_leaf) {
        cout << "Records: " << endl;

        for (int i = 0; i < node.count; i++) {
            Record record = readRecord(node.values[i]);
            record.print();
        }
    }

    cout << endl;
    cout << endl;
    if (!node.is_leaf) {
        for (int i = 0; i <= node.count; i++) {
            PageIndex child = readPageIndex(node.children[i]);
            printIndexRec(child, node.children[i]);
        }
    }
}

template <typename TK>
void BPlusTree<TK>::printAllRecords() {
    if (metadata.rootPos == -1) return;
    Record().printHeader();
    cout << "------------------------------------------------------------------------------------\n";
    PageIndex root = readPageIndex(metadata.rootPos);
    printAllRecordsRec(root);
}

template <typename TK>
void BPlusTree<TK>::printAllRecordsRec(PageIndex& node) {
    if (!node.is_leaf) {
        PageIndex child = readPageIndex(node.children[0]);
        printAllRecordsRec(child);
        return;  // Asegurarse de no continuar en el nodo actual después de la llamada recursiva
    }

    // Iterar sobre todas las hojas
    while (node.next != -1) {
        for (int i = 0; i < node.count; i++) {
            Record record = readRecord(node.values[i]);
            record.printTableFormat();
        }
        long next = node.next;
        node = readPageIndex(next);
    }

    // Imprimir la última hoja
    for (int i = 0; i < node.count; i++) {
        Record record = readRecord(node.values[i]);
        record.printTableFormat();
    }
}

#endif
