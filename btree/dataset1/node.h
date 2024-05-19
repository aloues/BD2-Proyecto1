#ifndef PAGEINDEX_H
#define PAGEINDEX_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

const int R = 5; // Definir el tamaño de la página (Número máximo de claves por página)

class Record {
public:
    int key;
    string job_title;
    string salary_estimate;
    double rating;
    string company_name;
    string location;
    string headquarters;
    string size;
    int founded;
    string type_of_ownership;
    string industry;
    string sector;
    string revenue;
    string competitors;

    static void printHeader() {
        cout << "key" << setw(25)
             << "job_title" << setw(25)
             << "salary_estimate" << setw(25)
             << "rating" << setw(25)
             << "company_name" << setw(25)
             << "location" << setw(25)
             << "headquarters" << setw(25)
             << "size" << setw(25)
             << "founded" << setw(25)
             << "type_of_ownership" << setw(25)
             << "industry" << setw(25)
             << "sector" << setw(25)
             << "revenue" << setw(25)
             << "competitors" << '\n';
    }

    void print() {
        cout << key << ';'
             << job_title << ';'
             << salary_estimate << ';'
             << rating << ';'
             << company_name << ';'
             << location << ';'
             << headquarters << ';'
             << size << ';'
             << founded << ';'
             << type_of_ownership << ';'
             << industry << ';'
             << sector << ';'
             << revenue << ';'
             << competitors << '\n';
    }

    void printTableFormat() {
        cout << key << setw(25)
                << job_title << setw(25)
                << salary_estimate << setw(25)
                << rating << setw(25)
                << company_name << setw(25)
                << location << setw(25)
                << headquarters << setw(25)
                << size << setw(25)
                << founded << setw(25)
                << type_of_ownership << setw(25)
                << industry << setw(25)
                << sector << setw(25)
                << revenue << setw(25)
                << competitors << '\n';
    }

    static Record read_record(const string& datafile, long pos) {
        ifstream file(datafile);

        if (!file.is_open()) {
            throw runtime_error("Could not open file");
        }

        file.seekg(pos);

        string line;
        if (getline(file, line)) {
            stringstream ss(line);
            Record record;

            string id;
            getline(ss, id, ';');
            record.key = stoi(id);

            getline(ss, record.job_title, ';');
            getline(ss, record.salary_estimate, ';');

            string rating;
            getline(ss, rating, ';');
            record.rating = stod(rating);

            getline(ss, record.company_name, ';');
            getline(ss, record.location, ';');
            getline(ss, record.headquarters, ';');
            getline(ss, record.size, ';');

            string founded;
            getline(ss, founded, ';');
            record.founded = stoi(founded);

            getline(ss, record.type_of_ownership, ';');
            getline(ss, record.industry, ';');
            getline(ss, record.sector, ';');
            getline(ss, record.revenue, ';');
            getline(ss, record.competitors, ';');

            file.close();
            return record;
        }

        file.close();
        throw out_of_range("Position out of range or no data found");
    }

    long write_record(const string& datafile) {
        ofstream file(datafile, ios::binary | ios::app);

        if (!file.is_open()) {
            throw runtime_error("Could not open file");
        }
        
        long pos = file.tellp();

        file << this->key << ';'
             << this->job_title << ';'
             << this->salary_estimate << ';'
             << this->rating << ';'
             << this->company_name << ';'
             << this->location << ';'
             << this->headquarters << ';'
             << this->size << ';'
             << this->founded << ';'
             << this->type_of_ownership << ';'
             << this->industry << ';'
             << this->sector << ';'
             << this->revenue << ';'
             << this->competitors << '\n';

        file.close();

        return pos;
    }
};

vector<Record> readCSV(const string& filename) {
    vector<Record> records;
    ifstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Could not open file");
    }

    string line;
    // Leer la primera línea (encabezados)
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        Record record;

        string id;
        getline(ss, id, ';');
        record.key = stoi(id);

        getline(ss, record.job_title, ';');
        getline(ss, record.salary_estimate, ';');

        string rating;
        getline(ss, rating, ';');
        record.rating = stod(rating);

        getline(ss, record.company_name, ';');
        getline(ss, record.location, ';');
        getline(ss, record.headquarters, ';');
        getline(ss, record.size, ';');

        string founded;
        getline(ss, founded, ';');
        record.founded = stoi(founded);

        getline(ss, record.type_of_ownership, ';');
        getline(ss, record.industry, ';');
        getline(ss, record.sector, ';');
        getline(ss, record.revenue, ';');
        getline(ss, record.competitors, ';');

        records.push_back(record);
    }

    file.close();
    return records;
}

// Estructura que representa una página de índice en el B+ Tree
struct PageIndex {
    int entries[R]; 
    long children[R + 1];
    int values[R];
    bool is_leaf;
    int count;
    long next;

    PageIndex() : is_leaf(true), count(0), next(-1) {
        for (int i = 0; i < R; i++) {
            entries[i] = -1;
            children[i] = -1;
            values[i] = -1;
        }
        children[R] = -1;
    }
};

// Funciones para leer y escribir páginas de índice en un archivo

PageIndex read_page_index(const string& filename, long pos) {
    ifstream file(filename, ios::binary);
    file.seekg(pos, ios::beg);
    PageIndex page;
    file.read(reinterpret_cast<char*>(&page), sizeof(PageIndex));
    file.close();
    return page;
}

void write_page_index(const string& filename, const PageIndex& page, long pos) {
    ofstream file(filename, ios::binary | ios::in | ios::out);
    file.seekp(pos, ios::beg);
    file.write(reinterpret_cast<const char*>(&page), sizeof(PageIndex));
    file.close();
}

long allocate_new_page(const string& filename, const PageIndex& page) {
    ofstream file(filename, ios::binary | ios::app);
    long pos = file.tellp();
    file.write(reinterpret_cast<const char*>(&page), sizeof(PageIndex));
    file.close();
    return pos;
}

struct Metadata {
    long rootPos; // Posición del nodo raíz en el archivo
    int n;        // Número total de elementos en el árbol
};

void init_file(const string& filename) {
    fstream file(filename, ios::binary | ios::in | ios::out | ios::app);

    if(!file) {
        file.open(filename, fstream::in | fstream::out | fstream::trunc);
    }

    file.close();
}

Metadata init_metadata(const string& metadatafile) {
    Metadata metadata;
    fstream file(metadatafile, ios::binary | ios::in | ios::out);

    if (!file) {
        file.open(metadatafile, ios::binary | ios::out);
        metadata.rootPos = -1;
        metadata.n = 0;
        file.write(reinterpret_cast<const char*>(&metadata), sizeof(Metadata));
    } else {
        file.read(reinterpret_cast<char*>(&metadata), sizeof(Metadata));
    }

    file.close();

    return metadata;
}

Metadata init_files(const string& datafile, const string& indexfile, const string& metadatafile) {
    init_file(datafile);
    init_file(indexfile);

    Metadata metadata = init_metadata(metadatafile);

    return metadata;
}

void update_metadata(const string& filename, const Metadata& metadata) {
    ofstream file(filename, ios::binary | ios::in | ios::out);
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<const char*>(&metadata), sizeof(Metadata));
    file.close();
}



#endif
