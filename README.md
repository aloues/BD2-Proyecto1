# Informe del Proyecto de Organización de Archivos

## Autores
- **Nombre:** Luis Izaguirre
- **Nombre:** Ernesto Ormeño
- **Nombre:** Ximena Lindo
- **Nombre:** Aleksander Paico


## Introducción
### Objetivo del proyecto:
El objetivo de este proyecto es aplicar y demostrar el entendimiento de diversas técnicas de organización de archivos en memoria secundaria, utilizando un conjunto de datos real sobre trabajos y salarios en ciencia de datos para 2024. Las técnicas seleccionadas permitirán optimizar operaciones fundamentales como la inserción, eliminación y búsqueda dentro de los archivos. Otros objetivos que tenemos son los siguientes:

- Implementar métodos de organización de archivos (AVL, B+Tree y Extended Hashing) para luego ejecutar sus métodos de búsqueda, inserción, eliminación y búsqueda por rango y realizar un análisis comparativo teórico en base a los accesos de memoria secundaria.
- Comparar el desempeño de las técnicas de indexación de archivos sobre el dominio de datos. Tanto para la inserción como para la búsqueda en el dataset.

### Descripción del dominio de datos a usar:
Este proyecto utilizará el conjunto de datos "Data Science Jobs & Salaries 2024" disponible en [Kaggle](https://www.kaggle.com/datasets/fahadrehman07/data-science-jobs-and-salary-glassdoor?select=Glassdoor_Salary_Cleaned_Version.csv), que contiene información detallada sobre los puestos de trabajo en ciencia de datos, incluyendo salarios, ubicaciones, y niveles de experiencia. Este conjunto de datos consiste de dos tablas, ambas de 743 registros y, para poder analizar el análisis comparativo, hemos partido cada tabla en tres grupos:

- Tabla 1 o 2 con 100 registros
- Tabla 1 o 2 con 370 registros
- Tabla 1 o 2 con 743 registros

Esto con el fin de evaluar el tiempo de ejecución de los métodos implementados con diferentes tamaños de datos.

### Resultados esperados:
Se espera demostrar la eficacia de las técnicas de indexación seleccionadas en términos de velocidad y eficiencia de acceso a los datos, así como su capacidad para manejar grandes volúmenes de información con minimalismo en el acceso a memoria secundaria.También podríamos decir que esperamos lo siguiente:

- Establecer qué técnica de indexación es la más eficiente en tiempo de ejecución y acceso a memoria secundaria. Tanto para la operación de inserción como para la búsqueda en el dataset.
- Esperamos disminuir la cantidad de accesos a memoria secundaria, ordenar, recuperar y eliminar los registros de una manera eficiente.

## Técnicas Utilizadas
### AVL File
#### Descripción de la técnica:
El archivo AVL utiliza un árbol binario de búsqueda autoequilibrado para mantener los registros ordenados, lo cual facilita operaciones de búsqueda, inserción y eliminación eficientes, ejecutadas en un tiempo logarítmico respecto al número de elementos.

#### Algoritmos de inserción, eliminación y búsqueda:
- **Inserción:** Se añade un nuevo registro manteniendo el equilibrio del árbol para asegurar eficiencia.

```cpp
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
        } else {
            insert(node.left, record, file);
        }
    } else if (greater(record, node)) {
        if (node.right == -1) {
            file.seekp(0, ios::end);
            long pos = file.tellp();
            node.right = pos;
            file.write(reinterpret_cast<const char*>(&record), sizeof(T));
            file.seekp(pos_node, ios::beg);
            file.write(reinterpret_cast<const char*>(&node), sizeof(T));
        } else {
            insert(node.right, record, file);
        }
    }

    balance(pos_node, file);
}
```

- **Eliminación:** El registro se elimina y se efectúa un rebalanceo del árbol para mantener la eficiencia de futuras operaciones.

```cpp
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
            } else {
                file.seekp(pos_node, ios::beg);
                file.write(reinterpret_cast<const char*>(&temp), sizeof(long));
            }
        } else {
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
    } else {
        bool result = remove(node.left, key, file);
        balance(pos_node, file);
        return result;
    }
}

```

- **Búsqueda:** Se realiza de forma binaria, aprovechando la estructura equilibrada del árbol.

```cpp
template <class T, typename TK>
T AVLFile<T, TK>::find(long pos_node, TK key, fstream& file) const {
    if (pos_node == -1) {
        throw runtime_error("Record not found");
    }

    file.seekg(pos_node, ios::beg);
    T node;
    file.read(reinterpret_cast<char*>(&node), sizeof(T));

    if (equal_key(node, key)) {
        return node;
    } else if (less_key(node, key)) {
        return find(node.right, key, file);
    } else {
        return find(node.left, key, file);
    }
}

```

### B+Tree Index
#### Descripción de la técnica:
El árbol B+ es una estructura de datos de árbol balanceado que permite búsquedas, inserciones y eliminaciones eficientes y mantiene los datos ordenados. Para permitir recorridos secuenciales rápidos, los nodos hoja del árbol B+ están enlazados. Los sistemas de bases de datos y de archivos se benefician especialmente de esta estructura.

#### Algoritmos de inserción, eliminación y búsqueda:
- **Inserción:** Se agrega un nuevo registro al nodo de la hoja adecuado. La división se propaga hacia arriba en el árbol si el nodo de la hoja se llena.
- **Eliminación:** Se elimina un registro del nodo hoja correspondiente. Si el nodo hoja queda con muy pocos elementos, se combina con un nodo hermano o se redistribuyen los elementos.
- **Búsqueda:** Se realiza una búsqueda binaria en los nodos internos y una búsqueda secuencial en los nodos hoja.

```cpp
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
    } else {
        return searchRec(key, node.children[i]);
    }
}
```

### Extendible Hashing
#### Descripción de la técnica:
Extendible Hashing es una técnica de hash que se ajusta dinámicamente para acomodar crecimientos o reducciones en la cantidad de datos, lo que permite un acceso muy rápido a los datos mediante el uso de una función de hash que dirige cada búsqueda al bloque correspondiente de manera directa.

#### Algoritmos de inserción, eliminación y búsqueda:
- **Inserción:** Acomoda nuevos registros ajustando dinámicamente la estructura de hash.
- **Eliminación:** Elimina registros y ajusta la estructura de hash según sea necesario.
- **Búsqueda:** Utiliza la función de hash para localizar el registro de manera eficiente y rápida.

### Análisis Comparativo Teórico
Las tres técnicas de indexación presentadas tienen sus fortalezas y limitaciones. El archivo AVL ofrece un excelente balance entre inserción, eliminación y búsqueda gracias a su estructura autoequilibrada, lo que lo hace ideal para aplicaciones donde estas operaciones son igualmente frecuentes. El B+ Tree, por su parte, ofrece búsquedas muy rápidas y es altamente eficiente en operaciones de acceso a datos gracias a su estructura de árbol balanceado y su soporte para operaciones de rango. Finalmente, Extendible Hashing ofrece un rendimiento superior en búsquedas gracias a su acceso directo mediante hash, pero las operaciones de inserción y eliminación pueden requerir ajustes dinámicos complejos en la estructura de hash, lo que puede ser computacionalmente costoso.

Cada escenario ideal de uso dependiendo de las características del conjunto de datos y los requisitos de operación. Por ejemplo:

- **AVL File** es preferido cuando las operaciones de inserción, eliminación y búsqueda deben ser todas eficientes y el tamaño del conjunto de datos no es extremadamente grande, dado que el coste de mantener el árbol balanceado puede aumentar con conjuntos de datos muy grandes. Las complejidades de cada uno de sus métodos son los siguientes:

- Inserción: O(log n)
- Búsqueda: O(log n)
- Delete: O(log n)
  
- **B+ Tree** es ideal para situaciones donde las búsquedas son mucho más frecuentes que las inserciones y los datos no cambian frecuentemente, ya que proporciona accesos muy rápidos a los datos a través de su índice estructurado. Las complejidades de cada uno de sus métodos son las siguientes:

- Inserción: O(log n)
- Búsqueda: O(log n)
- Delete: O(log n)

- **Extendible Hashing** se destaca en aplicaciones que necesitan accesos de búsqueda extremadamente rápidos y pueden manejar inserciones dinámicas con cambios estructurales, como en bases de datos en tiempo real con crecimiento o decrecimiento de datos. Las complejidades de cada uno de sus métodos son los siguientes (m es el tamaño de los buckets del Extendible Hashing):

- Inserción: O(log n)
- Búsqueda: O(log n) + O(m)
- Delete: O(log n) + O(m)

### Implementación del Parser SQL

Para nuestro Parser de sentencias SQL creamos las siguientes clases:

- Token: Esta clase representa los tokens o componentes de nuestra query (ej. SELECT, TABLE, FROM, etc.) y en ella se puede encontrar que definimos los tipos de token que existen en el atributo enum type. También definimos cómo se vería cada token como string o lexema.

- Scanner: Esta clase representa el scanner que utilizará el parser para poder leer, identificar y obtener los tokens de la query (este sería un string) teniendo en cuenta las definiciones de cada token en la clase Token. También hemos definido con un unordered map algunas palabras reservadas o símbolos con significado especial como el * que representa el token ALL.

- Parser: Esta clase representa el parser SQL que tiene un atributo de tipo Scanner que representa el scanner que utilizará para luego verificar la sintaxis de la sentencia SQL en base a las querys presentadas en el enunciado del proyecto tales como:
    - create table Customer from file “C:\data.csv” using index hash(“DNI”)
    - select * from Customer where DNI = x
    - select * from Customer where DNI between x and y
    - insert into Customer values (...)
    - delete from Customer where DNI = x


## Resultados Experimentales
### Cuadro y/o gráfico comparativo:
-medir tiempo de ejecución y los accesos al disco para las operaciones.
- presentacion visual graficos o tablas comparando
- viendo diferencias de cada una para cargas de trabajo y tamaños de datos.

### Discusión y análisis de los resultados:
puntos posible a ver:
- tecnica mas ventajosa que otra 
- las situaciones en las que el costo de mantener la estructura de datos puede no justificar su uso.

## Pruebas de uso y presentación

### Funcionalidad del aplicativo en el video:

## Conclusión

## Enlaces de interés
- **Link proyecto word:** [Google Drive](https://docs.google.com/document/d/1vadt2WTfk3kzQxVkr5JO7ioLIMwdiF08qMULMHMZG9o/edit?usp=sharing)
- **Video explicativo:** [YouTube](#)
```
