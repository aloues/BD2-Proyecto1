#ifndef AVL_HPP
#define AVL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <functional>

using namespace std;

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

    long left;
    long right;
    int height;

    Record() : left(-1), right(-1), height(-1) {}

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

template <typename TK>
class AVLFile {
private:
    string filename;
    long pos_root;
    function<bool(const Record&, const Record&)> less;
    function<bool(const Record&, const Record&)> greater;
    function<bool(const Record&, const TK&)> equal_key;
    function<bool(const Record&, const TK&)> less_key;
    function<bool(const Record&, const TK&)> greater_key;
    function<TK(const Record&)> get_key;

public:
    AVLFile(string filename,
            function<bool(const Record&, const Record&)> less,
            function<bool(const Record&, const Record&)> greater,
            function<bool(const Record&, const TK&)> equal_key,
            function<bool(const Record&, const TK&)> less_key,
            function<bool(const Record&, const TK&)> greater_key,
            function<TK(const Record&)> get_key);

    Record find(TK key) const;
    void insert(Record record);
    bool remove(TK key);
    vector<Record> inorder() const;
    vector<Record> rangeSearch(TK begin_key, TK end_key) const;

private:
    void inorder(long pos_node, vector<Record>& result, fstream& file) const;
    Record find(long pos_node, TK key, fstream& file) const;
    long findMinNode(long pos_node, fstream& file) const;
    int height(long pos_node, fstream& file) const;
    int balancingFactor(long pos_node, fstream& file) const;
    void rangeSearch(long pos_node, TK begin_key, TK end_key, fstream& file, vector<Record>& result) const;

    void insert(long pos_node, Record record, fstream& file);
    bool remove(long pos_node, TK key, fstream& file);
    void balance(long pos_node, fstream& file);
    void leftRotate(long pos_node, fstream& file);
    void rightRotate(long pos_node, fstream& file);
    void updateHeight(long pos_node, fstream& file);
};



template <typename TK>
AVLFile<TK>::AVLFile(string filename,
    function<bool(const Record&, const Record&)> less,
    function<bool(const Record&, const Record&)> greater,
    function<bool(const Record&, const TK&)> equal_key,
    function<bool(const Record&, const TK&)> less_key,
    function<bool(const Record&, const TK&)> greater_key,
    function<TK(const Record&)> get_key)
    : filename(filename), less(less), greater(greater), equal_key(equal_key), less_key(less_key), greater_key(greater_key), get_key(get_key) {
    fstream file(this->filename, ios::app | ios::binary | ios::in | ios::out);

    file.seekg(0, ios::end);
    int bytes = file.tellg();

    if (bytes == 0) {
        cout << "Creating file..." << endl;
        this->pos_root = -1;
        file.write((char*)&this->pos_root, sizeof(long));
    }
    else {
        file.seekg(0, ios::beg);
        cout << "Reading file..." << endl;
        file.read((char*)&this->pos_root, sizeof(long));
    }
    cout << "Init complete!" << endl;
    file.close();
}

template <typename TK>
Record AVLFile<TK>::find(TK key) const{
    fstream file(this->filename, ios::binary | ios::in);
    Record foundRecord = find(this->pos_root, key, file);
    file.close();
    return foundRecord;
}

template <typename TK>
Record AVLFile<TK>::find(long pos_node, TK key, fstream& file) const{
    if (pos_node == -1) {
        throw runtime_error("Record not found");
    }

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    if (equal_key(node, key)) {
        return node;
    }
    else if (less_key(node, key)) {
        return find(node.right, key, file);
    }
    else {
        return find(node.left, key, file);
    }
}

template <typename TK>
void AVLFile<TK>::insert(Record record) {
    fstream file(this->filename, ios::binary | ios::in | ios::out);
    insert(this->pos_root, record, file);
    file.close();
}

template <typename TK>
void AVLFile<TK>::insert(long pos_node, Record record, fstream& file) {
    if (pos_node == -1) {
        file.seekp(0, ios::end);
        long pos = file.tellp();
        
        file.write(reinterpret_cast<const char*>(&record), sizeof(Record));

        if (this->pos_root == -1) {
            this->pos_root = pos;
            file.seekp(0, ios::beg);
            file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
        }
        return;
    }

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    if (less(record, node)) {
        if (node.left == -1) {
            file.seekp(0, ios::end);
            long pos = file.tellp();
            node.left = pos;
            file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(Record));
        }
        else {
            insert(node.left, record, file);
        }
    }
    else if (greater(record, node)) {
        if (node.right == -1) {
            file.seekp(0, ios::end);
            long pos = file.tellp();
            node.right = pos;
            file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(Record));
        }
        else {
            insert(node.right, record, file);
        }
    }

    balance(pos_node, file);
}

template <typename TK>
bool AVLFile<TK>::remove(TK key) {
    fstream file(this->filename, ios::binary | ios::in | ios::out);
    bool result = remove(this->pos_root, key, file);
    file.close();
    return result;
}

template <typename TK>
bool AVLFile<TK>::remove(long pos_node, TK key, fstream& file) {
    if (pos_node == -1) {
        return false;
    }

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    if (equal_key(node, key)) {
        if (node.left == -1 || node.right == -1) {
            long temp = (node.left != -1) ? node.left : node.right;
            if (pos_node == this->pos_root) {
                this->pos_root = temp;
                file.seekp(0, ios::beg);
                file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
            }
            else {
                file.seekp(pos_node, ios::beg);
                file.write(reinterpret_cast<const char*>(&temp), sizeof(long));
            }
        }
        else {
            long min_node = findMinNode(node.right, file);
            file.seekg(min_node, ios::beg);
            Record min_node_record;
            file.read(reinterpret_cast<char*>(&min_node_record), sizeof(Record));
            node = min_node_record;
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(Record));
            remove(node.right, get_key(node), file);
        }
        balance(pos_node, file);
        return true;
    }
    
    if (less_key(node, key)) {
        bool result = remove(node.right, key, file);
        balance(pos_node, file);
        return result;
    }
    else {
        bool result = remove(node.left, key, file);
        balance(pos_node, file);
        return result;
    }
}

template <typename TK>
long AVLFile<TK>::findMinNode(long pos_node, fstream& file)const  {
    while (true) {
        file.seekg(pos_node, ios::beg);
        Record node;
        file.read(reinterpret_cast<char*>(&node), sizeof(Record));
        if (node.left == -1) {
            return pos_node;
        }
        pos_node = node.left;
    }
}

template <typename TK>
void AVLFile<TK>::balance(long pos_node, fstream& file) {
    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    updateHeight(pos_node, file);
    int balance_factor = balancingFactor(pos_node, file);

    if (balance_factor > 1) {
        long left_pos = node.left;
        file.seekg(left_pos, ios::beg);
        Record left_node;
        file.read(reinterpret_cast<char*>(&left_node), sizeof(Record));

        if (balancingFactor(left_pos, file) < 0) {
            leftRotate(left_pos, file);
        }
        rightRotate(pos_node, file);
    }
    else if (balance_factor < -1) {
        long right_pos = node.right;
        file.seekg(right_pos, ios::beg);
        Record right_node;
        file.read(reinterpret_cast<char*>(&right_node), sizeof(Record));

        if (balancingFactor(right_pos, file) > 0) {
            rightRotate(right_pos, file);
        }
        leftRotate(pos_node, file);
    }
}

template <typename TK>
int AVLFile<TK>::height(long pos_node, fstream& file)const  {
    if (pos_node == -1) return 0;

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));
    return node.height;
}

template <typename TK>
int AVLFile<TK>::balancingFactor(long pos_node, fstream& file) const {
    if (pos_node == -1) return 0;

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));
    return height(node.left, file) - height(node.right, file);
}

template <typename TK>
void AVLFile<TK>::leftRotate(long pos_node, fstream& file) {
    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    long new_pos = node.right;
    file.seekg(new_pos, ios::beg);
    Record new_node;
    file.read(reinterpret_cast<char*>(&new_node), sizeof(Record));

    node.right = new_node.left;
    new_node.left = pos_node;

    file.seekp(pos_node, ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(Record));
    file.seekp(new_pos, ios::beg);
    file.write(reinterpret_cast<const char*>(&new_node), sizeof(Record));

    updateHeight(pos_node, file);
    updateHeight(new_pos, file);

    if (pos_node == this->pos_root) {
        this->pos_root = new_pos;
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
    }
}

template <typename TK>
void AVLFile<TK>::rightRotate(long pos_node, fstream& file) {
    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    long new_pos = node.left;
    file.seekg(new_pos, ios::beg);
    Record new_node;
    file.read(reinterpret_cast<char*>(&new_node), sizeof(Record));

    node.left = new_node.right;
    new_node.right = pos_node;

    file.seekp(pos_node, ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(Record));
    file.seekp(new_pos, ios::beg);
    file.write(reinterpret_cast<const char*>(&new_node), sizeof(Record));

    updateHeight(pos_node, file);
    updateHeight(new_pos, file);

    if (pos_node == this->pos_root) {
        this->pos_root = new_pos;
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
    }
}

template <typename TK>
void AVLFile<TK>::updateHeight(long pos_node, fstream& file) {
    if (pos_node == -1) return;

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    node.height = 1 + max(height(node.left, file), height(node.right, file));
    file.seekp(pos_node, ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(Record));
}

template <typename TK>
vector<Record> AVLFile<TK>::inorder() const {
    fstream file(this->filename, ios::binary | ios::in);
    vector<Record> result;
    inorder(this->pos_root, result, file);
    file.close();
    return result;
}

template <typename TK>
void AVLFile<TK>::inorder(long pos_node, vector<Record>& result, fstream& file) const {
    if (pos_node == -1) return;

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    inorder(node.left, result, file);
    result.push_back(node);
    inorder(node.right, result, file);
}

template <typename TK>
vector<Record> AVLFile<TK>::rangeSearch(TK begin_key, TK end_key) const {
    fstream file(this->filename, ios::binary | ios::in);
    vector<Record> result;
    rangeSearch(this->pos_root, begin_key, end_key, file, result);
    file.close();
    return result;
}

template <typename TK>
void AVLFile<TK>::rangeSearch(long pos_node, TK begin_key, TK end_key, fstream& file, vector<Record>& result) const {
    if (pos_node == -1) return;

    file.seekg(pos_node, ios::beg);
    Record node;
    file.read(reinterpret_cast<char*>(&node), sizeof(Record));

    if (less_key(node, end_key)) {
        rangeSearch(node.left, begin_key, end_key, file, result);
    }

    if (!less_key(node, begin_key) && !greater_key(node, end_key)) {
        result.push_back(node);
    }

    if (greater_key(node, begin_key)) {
        rangeSearch(node.right, begin_key, end_key, file, result);
    }
}

#endif // AVL_HPP
