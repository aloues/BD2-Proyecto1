# Informe del Proyecto de Organización de Archivos

## Autores
- **Nombre:** Luis Izaguirre
- **Nombre:** Ernesto Ormeño
- **Nombre:** Ximena Lindo
- **Nombre:** Aleksander Paico


## Introducción
### Objetivo del proyecto:
El objetivo de este proyecto es aplicar y demostrar el entendimiento de diversas técnicas de organización de archivos en memoria secundaria, utilizando un conjunto de datos real sobre trabajos y salarios en ciencia de datos para 2024. Las técnicas seleccionadas permitirán optimizar operaciones fundamentales como la inserción, eliminación y búsqueda dentro de los archivos.

### Descripción del dominio de datos a usar:
Este proyecto utilizará el conjunto de datos "Data Science Jobs & Salaries 2024" disponible en [Kaggle](https://www.kaggle.com/datasets/fahadrehman07/data-science-jobs-and-salary-glassdoor?select=Glassdoor_Salary_Cleaned_Version.csv), que contiene información detallada sobre los puestos de trabajo en ciencia de datos, incluyendo salarios, ubicaciones, y niveles de experiencia.

### Resultados esperados:
Se espera demostrar la eficacia de las técnicas de indexación seleccionadas en términos de velocidad y eficiencia de acceso a los datos, así como su capacidad para manejar grandes volúmenes de información con minimalismo en el acceso a memoria secundaria.

## Técnicas Utilizadas
### AVL File
#### Descripción de la técnica:
El archivo AVL utiliza un árbol binario de búsqueda autoequilibrado para mantener los registros ordenados, lo cual facilita operaciones de búsqueda, inserción y eliminación eficientes, ejecutadas en un tiempo logarítmico respecto al número de elementos.

#### Algoritmos de inserción, eliminación y búsqueda:
- **Inserción:** Se añade un nuevo registro manteniendo el equilibrio del árbol para asegurar eficiencia.
- **Eliminación:** El registro se elimina y se efectúa un rebalanceo del árbol para mantener la eficiencia de futuras operaciones.
- **Búsqueda:** Se realiza de forma binaria, aprovechando la estructura equilibrada del árbol.

### ISAM Sparse Index
#### Descripción de la técnica:
ISAM, o Index Sequential Access Method, es un método de indexación que mantiene los datos ordenados y utiliza un índice disperso junto con páginas de desbordamiento para manejar inserciones, lo que facilita búsquedas rápidas pero puede complicar las inserciones continuas.

#### Algoritmos de inserción, eliminación y búsqueda:
- **Inserción:** Se insertan registros en páginas de desbordamiento cuando no hay espacio suficiente en la página principal.
- **Eliminación:** Los registros son eliminados y las páginas de desbordamiento ajustadas acordemente.
- **Búsqueda:** Empieza con una búsqueda rápida en el índice para localizar la página y sigue con una búsqueda secuencial en la página identificada.

### Extendible Hashing
#### Descripción de la técnica:
Extendible Hashing es una técnica de hash que se ajusta dinámicamente para acomodar crecimientos o reducciones en la cantidad de datos, lo que permite un acceso muy rápido a los datos mediante el uso de una función de hash que dirige cada búsqueda al bloque correspondiente de manera directa.

#### Algoritmos de inserción, eliminación y búsqueda:
- **Inserción:** Acomoda nuevos registros ajustando dinámicamente la estructura de hash.
- **Eliminación:** Elimina registros y ajusta la estructura de hash según sea necesario.
- **Búsqueda:** Utiliza la función de hash para localizar el registro de manera eficiente y rápida.

### Análisis Comparativo Teórico
Las tres técnicas de indexación presentadas tienen sus fortalezas y limitaciones. El archivo AVL ofrece un excelente balance entre inserción, eliminación y búsqueda gracias a su estructura autoequilibrada, lo que lo hace ideal para aplicaciones donde estas operaciones son igualmente frecuentes. El ISAM, por su parte, ofrece búsquedas muy rápidas pero puede enfrentar desafíos con el manejo de inserciones masivas debido a las páginas de desbordamiento. Finalmente, Extendible Hashing ofrece un rendimiento superior en búsquedas gracias a su acceso directo mediante hash, pero las operaciones de inserción y eliminación pueden requerir ajustes dinámicos complejos en la estructura de hash, lo que puede ser computacionalmente costoso.

Cada escenario ideal de uso dependiendo de las características del conjunto de datos y los requisitos de operación. Por ejemplo:

- **AVL File** es preferido cuando las operaciones de inserción, eliminación y búsqueda deben ser todas eficientes y el tamaño del conjunto de datos no es extremadamente grande, dado que el coste de mantener el árbol balanceado puede aumentar con conjuntos de datos muy grandes.
  
- **ISAM Sparse Index** es ideal para situaciones donde las búsquedas son mucho más frecuentes que las inserciones y los datos no cambian frecuentemente, ya que proporciona accesos muy rápidos a los datos a través de su índice estructurado y páginas de desbordamiento.

- **Extendible Hashing** se destaca en aplicaciones que necesitan accesos de búsqueda extremadamente rápidos y pueden manejar inserciones dinámicas con cambios estructurales, como en bases de datos en tiempo real con crecimiento o decrecimiento de datos.

### Implementación del Parser SQL
REVISAR ESTO PARA EL PARSER SQL
El parser SQL desarrollado para este proyecto permite interpretar y ejecutar sentencias SQL sobre las estructuras de datos implementadas. Soporta las siguientes operaciones:
- **CREATE TABLE:** Crea una nueva tabla a partir de un archivo CSV, utilizando una de las técnicas de indexación especificadas.
- **SELECT:** Permite la búsqueda de registros específicos basada en criterios definidos.
- **INSERT:** Añade nuevos registros a la tabla respetando la técnica de indexación utilizada.
- **DELETE:** Elimina registros de la tabla según el criterio especificado.

La implementación busca maximizar la eficiencia y minimizar los accesos a disco, traduciendo las operaciones SQL en manipulaciones directas sobre las estructuras de datos.

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
- **Código fuente:** [GitHub Classroom](#)
- **Video explicativo:** [YouTube](#)
```
