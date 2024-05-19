#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>  // Para medir el tiempo

using namespace std;

class Bucket {
public:
    int depth;
    vector<string> records;
    string filename;

    Bucket(int depth, int index) : depth(depth) {
        filename = "bucket_" + to_string(index) + ".txt";
        load();  // Carga registros existentes al inicializar
    }

    ~Bucket() {
        save();  // Guarda registros al destruir
    }

    void insert(const string& record) {
        records.push_back(record);
        save();
    }

    // Metodo para eliminar un registro basado en la clave.
    bool remove(const string& key) {
        bool removed = false;
        records.erase(remove_if(records.begin(), records.end(), [&key, &removed](const string& record) {
            if (record.substr(0, record.find(';')) == key) {
                removed = true;
                return true;
            }
            return false;
        }), records.end());
        if (removed) save();
        return removed;
    }

    // Metodo para buscar un registro basado en la clave.
    string search(const string& key) {
        auto it = find_if(records.begin(), records.end(), [&key](const string& record) {
            return record.substr(0, record.find(';')) == key;
        });
        return (it != records.end()) ? *it : "";
    }

    void load() {
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) records.push_back(line);
        }
    }

    void save() {
        ofstream file(filename);
        for (const auto& record : records) {
            file << record << endl;
        }
    }

    bool is_full() {
        return records.size() >= 10;
    }

    void split(Bucket*& new_bucket, int new_bucket_index, int local_depth) {
        // Crear un nuevo bucket con profundidad local actualizada y un nombre de archivo basado en el nuevo indice.
        new_bucket = new Bucket(local_depth, new_bucket_index);
        vector<string> temp_records = std::move(records);
        records.clear();

        // Redistribuir registros entre el bucket actual y el nuevo bucket basado en su nueva profundidad local.
        for (const auto& record : temp_records) {
            string key = record.substr(0, record.find(';'));
            int new_index = stoi(key) % (1 << local_depth);
            if (new_index == new_bucket_index) {
                new_bucket->insert(record);
            } else {
                insert(record);
            }
        }
    }

    void print() {
        for (const auto& record : records) {
            cout << formatRecord(record) << endl;
        }
    }

    private:
    string formatRecord(const string& record) {
        stringstream ss(record);
        string item;
        vector<string> tokens;
        while (getline(ss, item, ';')) {
            tokens.push_back(item);
        }
        string formatted = "\n";
        vector<string> headers = {"ID", "Job Title", "Salary Estimate", "Rating", "Company Name",
                                            "Location", "Headquarters", "Size", "Founded", "Type of ownership",
                                            "Industry", "Sector", "Revenue"};
        for (size_t i = 0; i < tokens.size() && i < headers.size(); i++) {
            formatted += headers[i] + ": " + tokens[i] + "\n";
        }
        return formatted;
    }
};

class Directory {
public:
    int global_depth;
    vector<Bucket*> buckets;

    Directory(int depth) : global_depth(depth) {
        buckets.resize(1 << depth);
        for (int i = 0; i < (1 << depth); ++i) {
            buckets[i] = new Bucket(depth, i);  // Pasamos el indice para el nombre del archivo
        }
    }

    ~Directory() {
        for (auto& bucket : buckets) {
            delete bucket;
        }
    }

    int hash(const string& key) {
        return stoi(key) % (1 << global_depth);
    }

    void insert(const string& key, const string& record) {
        int index = hash(key);
        if (buckets[index]->is_full()) {
            if (buckets[index]->depth == global_depth) {
                double_directory();
            }
            split_bucket(index);
            insert(key, record);  // Reinsertar el registro despues de dividirlo
        } else {
            buckets[index]->insert(record);
        }
    }

    // Metodo para eliminar un registro basado en la clave.
    bool remove(const string& key) {
        int index = hash(key);
        return buckets[index]->remove(key);
    }

    // Metodo para buscar un registro basado en la clave.
    string search(const string& key) {
        int index = hash(key);
        return buckets[index]->search(key);
    }

    void split_bucket(int index) {
        int local_depth_increment = 1 << buckets[index]->depth;
        int buddy_index = index ^ local_depth_increment;

        if (buckets[buddy_index] == nullptr) {
            buckets[buddy_index] = new Bucket(buckets[index]->depth + 1, buddy_index);
        }

        buckets[index]->depth++;
        vector<string> temp_records = std::move(buckets[index]->records);
        buckets[index]->records.clear();

        for (const auto& record : temp_records) {
            string key = record.substr(0, record.find(';'));
            int new_index = stoi(key) % (1 << buckets[index]->depth);
            if (new_index == buddy_index) {
                buckets[buddy_index]->insert(record);
            } else {
                buckets[index]->insert(record);
            }
        }

        buckets[index]->save();
        buckets[buddy_index]->save();
    }

    void double_directory() {
        int old_size = 1 << global_depth;
        int new_size = old_size * 2;
        buckets.resize(new_size);

        for (int i = old_size - 1; i >= 0; i--) {
            buckets[i * 2] = buckets[i];
            buckets[i * 2 + 1] = new Bucket(buckets[i]->depth, i * 2 + 1);
            buckets[i * 2 + 1]->depth = buckets[i]->depth; // Copiar la profundidad inicial
        }

        global_depth++;
        for (int i = 0; i < new_size; i++) {
            if (buckets[i] != nullptr) {
                buckets[i]->save(); // Guardar el estado actual de todos los buckets
            }
        }
    }

    void print() {
        cout << "Directory (Global Depth: " << global_depth << ")\n";
        for (size_t i = 0; i < buckets.size(); ++i) {
            cout << "Bucket " << i << " (Local Depth: " << buckets[i]->depth << ")\n";
            buckets[i]->print();
        }
    }
};

int main() {
    Directory dir(3);  // Inicialmente, el directorio tiene una profundidad global de 3.

    // Carga datos desde el CSV.
    try {
        ifstream file("dataset_1_100.csv");
        string line;

        while (getline(file, line)) {
            if (line.empty() || line.front() == 'C') continue;
            string key = line.substr(0, line.find(';'));
            dir.insert(key, line);
        }
        file.close();
    } catch (const exception& e) {
        cerr << "Error al abrir o leer el archivo: " << e.what() << endl;
        return 1;
    }

    // Insertando nuevos empleadores manualmente para probar la división del bucket.
    string newEmployer1 = "16;Data Scientist;$100K-$150K (Glassdoor est.);4.5;NewTech Co.;New York, NY;New York, NY;1001 to 5000 employees;1999;Company - Private;Tech;Information Technology;$500 million to $1 billion (USD)";
    string newEmployer2 = "24;Data Scientist;$120K-$180K (Glassdoor est.);4.8;InnovateTech;San Francisco, CA;San Francisco, CA;501 to 1000 employees;2005;Company - Private;Tech;Information Technology;$1 to $2 billion (USD)";
    string newEmployer3 = "32;Data Scientist;$110K-$170K (Glassdoor est.);4.7;DataWorld;Seattle, WA;Seattle, WA;1001 to 5000 employees;2010;Company - Private;Tech;Information Technology;$500 million to $1 billion (USD)";




    auto start_insert = chrono::high_resolution_clock::now();
    dir.insert("16", newEmployer1);
    dir.insert("24", newEmployer2);
    dir.insert("32", newEmployer3);
    auto end_insert = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_insert = end_insert - start_insert;
    cout << "Insert operation took " << duration_insert.count() << " ms" << endl;




    // Prueba de Búsqueda <- Sí Funciona
    cout<< "------------------------------------------------------------------------------------------"<<endl;
    cout<< "Antes de eliminar y Buscando:"<<endl;
    cout<< "------------------------------------------------------------------------------------------"<<endl;
    string keyToSearch = "16";




    auto start_search = chrono::high_resolution_clock::now();
    string foundRecord = dir.search(keyToSearch);
    if (!foundRecord.empty()) {
        cout << "Registro encontrado: " << endl << foundRecord << "\n";
    } else {
        cout << "No se encontro el registro con clave " << keyToSearch << ".\n";
    }
    auto end_search = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_search = end_search - start_search;
    cout << "Search operation took " << duration_search.count() << " ms" << endl;




    // Imprimir el directorio y los buckets para ver la distribución de datos después de las inserciones
    dir.print();

    //Prueba de eliminacion <- Sí Funciona
    cout<< "------------------------------------------------------------------------------------------"<<endl;
    cout<< "Despues de eliminar:"<<endl;
    cout<< "------------------------------------------------------------------------------------------"<<endl;
    string keyToRemove = "24";
    
    
    
    auto start_remove = chrono::high_resolution_clock::now();
    if (dir.remove(keyToRemove)) {
        cout << "Registro con clave " << keyToRemove << " eliminado correctamente.\n";
    } else {
        cout << "No se encontro el registro con clave " << keyToRemove << ".\n";
    }
    auto end_remove = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_remove = end_remove - start_remove;
    cout << "Remove operation took " << duration_remove.count() << " ms" << endl;




    dir.print(); //Mostrar despues de eliminar 24 <- Sí Funciona

    return 0;
}