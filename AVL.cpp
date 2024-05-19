#include "AVL.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// Función para leer el archivo CSV y convertirlo en un vector de Records
vector<Record> readCSV(const string& filename) {
    vector<Record> records;
    ifstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Could not open CSV file");
    }

    string line;
    // Leer el encabezado
    if (getline(file, line)) {
        // No hacemos nada con el encabezado por ahora
    }

    while (getline(file, line)) {
        stringstream ss(line);
        Record record;
        string temp;

        try {
            getline(ss, temp, ';');
            cout << "Converting key: " << temp << endl;  // Imprimir el valor antes de la conversión
            record.key = stoi(temp);

            getline(ss, record.job_title, ';');
            getline(ss, record.salary_estimate, ';');

            getline(ss, temp, ';');
            cout << "Converting rating: " << temp << endl;  // Imprimir el valor antes de la conversión
            record.rating = stod(temp);

            getline(ss, record.company_name, ';');
            getline(ss, record.location, ';');
            getline(ss, record.headquarters, ';');
            getline(ss, record.size, ';');

            getline(ss, temp, ';');
            cout << "Converting founded: " << temp << endl;  // Imprimir el valor antes de la conversión
            record.founded = stoi(temp);

            getline(ss, record.type_of_ownership, ';');
            getline(ss, record.industry, ';');
            getline(ss, record.sector, ';');
            getline(ss, record.revenue, ';');
            getline(ss, record.competitors, ';');

            records.push_back(record);
        } catch (const invalid_argument& e) {
            cerr << "Invalid argument error parsing line: " << line << endl;
        } catch (const out_of_range& e) {
            cerr << "Value out of range error parsing line: " << line << endl;
        }
    }

    file.close();
    return records;
}

int main() {
    // Ruta al archivo CSV
    string filename = "dataset_1_100.csv";
    
    // Leer el archivo CSV
    vector<Record> records;
    try {
        records = readCSV(filename);
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Definir las funciones de comparación y claves para el AVL tree
    auto less = [](const Record& a, const Record& b) { return a.key < b.key; };
    auto greater = [](const Record& a, const Record& b) { return a.key > b.key; };
    auto equal_key = [](const Record& a, const int& key) { return a.key == key; };
    auto less_key = [](const Record& a, const int& key) { return a.key < key; };
    auto greater_key = [](const Record& a, const int& key) { return a.key > key; };
    auto get_key = [](const Record& a) { return a.key; };

    // Crear el AVL tree
    AVLFile<Record, int> avl("records.dat", less, greater, equal_key, less_key, greater_key, get_key);

    // Insertar los registros en el AVL tree
    for (const auto& record : records) {
        avl.insert(record);
    }

    // Realizar operaciones de prueba
    try {
        Record foundRecord = avl.find(10); // Buscar un registro con clave 10
        foundRecord.print();

        avl.remove(10); // Eliminar el registro con clave 10

        vector<Record> inorderRecords = avl.inorder(); // Obtener recorrido inorder
        for (const auto& record : inorderRecords) {
            record.print();
        }

        vector<Record> rangeRecords = avl.rangeSearch(5, 15); // Búsqueda por rango
        for (const auto& record : rangeRecords) {
            record.print();
        }

    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }

    return 0;
}
