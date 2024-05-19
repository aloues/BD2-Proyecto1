#ifndef AVL_HPP
#define AVL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
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

    long left = -1;
    long right = -1;
    int height = 0;

    void print() const{
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

    static Record read_record(const string& datafile, long pos) {
        ifstream file(datafile, ios::binary);

        if (!file.is_open()) {
            throw runtime_error("Could not open file");
        }

        file.seekg(pos);

        Record record;
        file.read(reinterpret_cast<char*>(&record), sizeof(Record));

        file.close();
        return record;
    }

    long write_record(const string& datafile) {
        ofstream file(datafile, ios::binary | ios::app);

        if (!file.is_open()) {
            throw runtime_error("Could not open file");
        }
        
        long pos = file.tellp();

        file.write(reinterpret_cast<const char*>(this), sizeof(Record));

        file.close();

        return pos;
    }
};

vector<Record> readCSV(const string& filename);

template <class T, typename TK>
class AVLFile {
private:
    string filename;
    long pos_root;
    function<bool(const T&, const T&)> less;
    function<bool(const T&, const T&)> greater;
    function<bool(const T&, const TK&)> equal_key;
    function<bool(const T&, const TK&)> less_key;
    function<bool(const T&, const TK&)> greater_key;
    function<TK(const T&)> get_key;

public:
    AVLFile(string filename,
            function<bool(const T&, const T&)> less,
            function<bool(const T&, const T&)> greater,
            function<bool(const T&, const TK&)> equal_key,
            function<bool(const T&, const TK&)> less_key,
            function<bool(const T&, const TK&)> greater_key,
            function<TK(const T&)> get_key);

    T find(TK key) const;
    void insert(T record);
    bool remove(TK key);
    vector<T> inorder() const;
    vector<T> rangeSearch(TK begin_key, TK end_key) const;

private:
    void inorder(long pos_node, vector<T>& result, fstream& file) const;
    T find(long pos_node, TK key, fstream& file) const;
    long findMinNode(long pos_node, fstream& file) const;
    int height(long pos_node, fstream& file) const;
    int balancingFactor(long pos_node, fstream& file) const;
    void rangeSearch(long pos_node, TK begin_key, TK end_key, fstream& file, vector<T>& result) const;

    void insert(long pos_node, T record, fstream& file);
    bool remove(long pos_node, TK key, fstream& file);
    void balance(long pos_node, fstream& file);
    void leftRotate(long pos_node, fstream& file);
    void rightRotate(long pos_node, fstream& file);
    void updateHeight(long pos_node, fstream& file);
};



template <class T, typename TK>
AVLFile<T, TK>::AVLFile(string filename,
    function<bool(const T&, const T&)> less,
    function<bool(const T&, const T&)> greater,
    function<bool(const T&, const TK&)> equal_key,
    function<bool(const T&, const TK&)> less_key,
    function<bool(const T&, const TK&)> greater_key,
    function<TK(const T&)> get_key)
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

template <class T, typename TK>
T AVLFile<T, TK>::find(TK key) const{
    fstream file(this->filename, ios::binary | ios::in);
    T foundRecord = find(this->pos_root, key, file);
    file.close();
    return foundRecord;
}

template <class T, typename TK>
T AVLFile<T, TK>::find(long pos_node, TK key, fstream& file) const{
    if (pos_node == -1) {
        throw runtime_error("Record not found");
    }

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

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

template <class T, typename TK>
void AVLFile<T, TK>::insert(T record) {
    fstream file(this->filename, ios::binary | ios::in | ios::out);
    insert(this->pos_root, record, file);
    file.close();
}

template <class T, typename TK>
void AVLFile<T, TK>::insert(long pos_node, T record, fstream& file) {
    if (pos_node == -1) {
        file.seekp(0, ios::end);
        long pos = file.tellp();
        file.write(reinterpret_cast<const char*>(&record), sizeof(T));
        if (this->pos_root == -1) {
            this->pos_root = pos;
            file.seekp(0, ios::beg);
            file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
        }
        return;
    }

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    if (less(record, node)) {
        if (node.left == -1) {
            file.seekp(0, ios::end);
            long pos = file.tellp();
            node.left = pos;
            file.write(reinterpret_cast<const char*>(&record), sizeof(T));
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(T));
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
            file.write(reinterpret_cast<const char*>(&record), sizeof(T));
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(T));
        }
        else {
            insert(node.right, record, file);
        }
    }

    balance(pos_node, file);
}

template <class T, typename TK>
bool AVLFile<T, TK>::remove(TK key) {
    fstream file(this->filename, ios::binary | ios::in | ios::out);
    bool result = remove(this->pos_root, key, file);
    file.close();
    return result;
}

template <class T, typename TK>
bool AVLFile<T, TK>::remove(long pos_node, TK key, fstream& file) {
    if (pos_node == -1) {
        return false;
    }

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

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
            T min_node_record;
            file.read(reinterpret_cast<char*>(&min_node_record), sizeof(T));
            node = min_node_record;
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(T));
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

template <class T, typename TK>
long AVLFile<T, TK>::findMinNode(long pos_node, fstream& file)const  {
    while (true) {
        file.seekg(pos_node, ios::beg);
        T node;
        file.read(reinterpret_cast<char*>(&node), sizeof(T));
        if (node.left == -1) {
            return pos_node;
        }
        pos_node = node.left;
    }
}

template <class T, typename TK>
void AVLFile<T, TK>::balance(long pos_node, fstream& file) {
    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    updateHeight(pos_node, file);
    int balance_factor = balancingFactor(pos_node, file);

    if (balance_factor > 1) {
        long left_pos = node.left;
        file.seekg(left_pos, ios::beg);
        T left_node;
        file.read(reinterpret_cast<char*>(&left_node), sizeof(T));

        if (balancingFactor(left_pos, file) < 0) {
            leftRotate(left_pos, file);
        }
        rightRotate(pos_node, file);
    }
    else if (balance_factor < -1) {
        long right_pos = node.right;
        file.seekg(right_pos, ios::beg);
        T right_node;
        file.read(reinterpret_cast<char*>(&right_node), sizeof(T));

        if (balancingFactor(right_pos, file) > 0) {
            rightRotate(right_pos, file);
        }
        leftRotate(pos_node, file);
    }
}

template <class T, typename TK>
int AVLFile<T, TK>::height(long pos_node, fstream& file)const  {
    if (pos_node == -1) return 0;

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));
    return node.height;
}

template <class T, typename TK>
int AVLFile<T, TK>::balancingFactor(long pos_node, fstream& file) const {
    if (pos_node == -1) return 0;

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));
    return height(node.left, file) - height(node.right, file);
}

template <class T, typename TK>
void AVLFile<T, TK>::leftRotate(long pos_node, fstream& file) {
    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    long new_pos = node.right;
    file.seekg(new_pos, ios::beg);
    T new_node;
    file.read(reinterpret_cast<char*>(&new_node), sizeof(T));

    node.right = new_node.left;
    new_node.left = pos_node;

    file.seekp(pos_node, ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(T));
    file.seekp(new_pos, ios::beg);
    file.write(reinterpret_cast<const char*>(&new_node), sizeof(T));

    updateHeight(pos_node, file);
    updateHeight(new_pos, file);

    if (pos_node == this->pos_root) {
        this->pos_root = new_pos;
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
    }
}

template <class T, typename TK>
void AVLFile<T, TK>::rightRotate(long pos_node, fstream& file) {
    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    long new_pos = node.left;
    file.seekg(new_pos, ios::beg);
    T new_node;
    file.read(reinterpret_cast<char*>(&new_node), sizeof(T));

    node.left = new_node.right;
    new_node.right = pos_node;

    file.seekp(pos_node, ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(T));
    file.seekp(new_pos, ios::beg);
    file.write(reinterpret_cast<const char*>(&new_node), sizeof(T));

    updateHeight(pos_node, file);
    updateHeight(new_pos, file);

    if (pos_node == this->pos_root) {
        this->pos_root = new_pos;
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char*>(&this->pos_root), sizeof(long));
    }
}

template <class T, typename TK>
void AVLFile<T, TK>::updateHeight(long pos_node, fstream& file) {
    if (pos_node == -1) return;

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    node.height = 1 + max(height(node.left, file), height(node.right, file));
    file.seekp(pos_node, ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(T));
}

template <class T, typename TK>
vector<T> AVLFile<T, TK>::inorder() const {
    fstream file(this->filename, ios::binary | ios::in);
    vector<T> result;
    inorder(this->pos_root, result, file);
    file.close();
    return result;
}

template <class T, typename TK>
void AVLFile<T, TK>::inorder(long pos_node, vector<T>& result, fstream& file) const {
    if (pos_node == -1) return;

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    inorder(node.left, result, file);
    result.push_back(node);
    inorder(node.right, result, file);
}

template <class T, typename TK>
vector<T> AVLFile<T, TK>::rangeSearch(TK begin_key, TK end_key) const {
    fstream file(this->filename, ios::binary | ios::in);
    vector<T> result;
    rangeSearch(this->pos_root, begin_key, end_key, file, result);
    file.close();
    return result;
}

template <class T, typename TK>
void AVLFile<T, TK>::rangeSearch(long pos_node, TK begin_key, TK end_key, fstream& file, vector<T>& result) const {
    if (pos_node == -1) return;

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

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
