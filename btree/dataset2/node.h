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
    bool hourly;
    bool employer_provided;
    double min_salary;
    double max_salary;
    double avg_salary;
    string job_state;
    bool same_state;
    int age;
    bool python_yn;
    bool R_yn;
    bool spark;
    bool aws;
    bool excel;

    static void printHeader() {
        cout << "key" << setw(15)
             << "hourly" << setw(15)
             << "employer_provided" << setw(20)
             << "min_salary" << setw(15)
             << "max_salary" << setw(15)
             << "avg_salary" << setw(15)
             << "job_state" << setw(15)
             << "same_state" << setw(15)
             << "age" << setw(15)
             << "python_yn" << setw(15)
             << "R_yn" << setw(15)
             << "spark" << setw(15)
             << "aws" << setw(15)
             << "excel" << '\n';
    }

    void print() {
        cout << key << ';'
             << hourly << ';'
             << employer_provided << ';'
             << min_salary << ';'
             << max_salary << ';'
             << avg_salary << ';'
             << job_state << ';'
             << same_state << ';'
             << age << ';'
             << python_yn << ';'
             << R_yn << ';'
             << spark << ';'
             << aws << ';'
             << excel << '\n';
    }

    void printTableFormat() {
        cout << key << setw(15)
             << hourly << setw(15)
             << employer_provided << setw(20)
             << min_salary << setw(15)
             << max_salary << setw(15)
             << avg_salary << setw(15)
             << job_state << setw(15)
             << same_state << setw(15)
             << age << setw(15)
             << python_yn << setw(15)
             << R_yn << setw(15)
             << spark << setw(15)
             << aws << setw(15)
             << excel << '\n';
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

            string key;
            getline(ss, key, ',');
            record.key = stoi(key);

            string hourly;
            getline(ss, hourly, ',');
            record.hourly = hourly == "1";

            string employer_provided;
            getline(ss, employer_provided, ',');
            record.employer_provided = employer_provided == "1";

            string min_salary;
            getline(ss, min_salary, ',');
            record.min_salary = stod(min_salary);

            string max_salary;
            getline(ss, max_salary, ',');
            record.max_salary = stod(max_salary);

            string avg_salary;
            getline(ss, avg_salary, ',');
            record.avg_salary = stod(avg_salary);

            getline(ss, record.job_state, ',');

            string same_state;
            getline(ss, same_state, ',');
            record.same_state = same_state == "1";

            string age;
            getline(ss, age, ',');
            record.age = stoi(age);

            string python_yn;
            getline(ss, python_yn, ',');
            record.python_yn = python_yn == "1";

            string R_yn;
            getline(ss, R_yn, ',');
            record.R_yn = R_yn == "1";

            string spark;
            getline(ss, spark, ',');
            record.spark = spark == "1";

            string aws;
            getline(ss, aws, ',');
            record.aws = aws == "1";

            string excel;
            getline(ss, excel, ',');
            record.excel = excel == "1";

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

        file << this->key << ','
             << this->hourly << ','
             << this->employer_provided << ','
             << this->min_salary << ','
             << this->max_salary << ','
             << this->avg_salary << ','
             << this->job_state << ','
             << this->same_state << ','
             << this->age << ','
             << this->python_yn << ','
             << this->R_yn << ','
             << this->spark << ','
             << this->aws << ','
             << this->excel << '\n';

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

        string key;
        getline(ss, key, ',');
        record.key = stoi(key);

        string hourly;
        getline(ss, hourly, ',');
        record.hourly = hourly == "1";

        string employer_provided;
        getline(ss, employer_provided, ',');
        record.employer_provided = employer_provided == "1";

        string min_salary;
        getline(ss, min_salary, ',');
        record.min_salary = stod(min_salary);

        string max_salary;
        getline(ss, max_salary, ',');
        record.max_salary = stod(max_salary);

        string avg_salary;
        getline(ss, avg_salary, ',');
        record.avg_salary = stod(avg_salary);

        getline(ss, record.job_state, ',');

        string same_state;
        getline(ss, same_state, ',');
        record.same_state = same_state == "1";

        string age;
        getline(ss, age, ',');
        record.age = stoi(age);

        string python_yn;
        getline(ss, python_yn, ',');
        record.python_yn = python_yn == "1";

        string R_yn;
        getline(ss, R_yn, ',');
        record.R_yn = R_yn == "1";

        string spark;
        getline(ss, spark, ',');
        record.spark = spark == "1";

        string aws;
        getline(ss, aws, ',');
        record.aws = aws == "1";

        string excel;
        getline(ss, excel, ',');
        record.excel = excel == "1";

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
