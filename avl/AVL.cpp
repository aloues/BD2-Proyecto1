#include "AVL.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    // Ruta al archivo CSV
    string filename = "dataset.csv";
    
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
    AVLFile<int> avl("records.dat", less, greater, equal_key, less_key, greater_key, get_key);

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
        for (auto& record : inorderRecords) { // Remove the const qualifier from auto&
            record.print();
        }

        vector<Record> rangeRecords = avl.rangeSearch(5, 15); // Búsqueda por rango
        for (auto& record : rangeRecords) { // Remove the const qualifier from auto&
            record.print();
        }

    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }

    return 0;
}
