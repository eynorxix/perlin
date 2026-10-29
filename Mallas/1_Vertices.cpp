// 1. Incluir librerias
#include <iostream> // Para poder imprimir mensajes en la consola std::cout
#include <vector>   // Para poder usar std::vector
#include <cmath>    // Para funciones matematicas (sin, cos, etc.)
#include <fstream> // Para crear y escribir en archivos

// 2. Estructura de datos de vertice (¡ojo al ';' al final!)
struct Vertice {
    float x;
    float y;
    float z;
}; // <-- Faltaba este punto y coma

// 3. Declaracion de funciones
float calcularCapaRuido(float x, float z, float frecuencia, float amplitud);

// 4. Punto de entrada Principal
int main() {
    std::cout << "Iniciando la creacion de mallas individuales..." << std::endl;

    // 1. Dimension de la cuadricula
    int ancho = 10;
    int largo = 10;

    // 2. Parametros de la primera capa
    float frecuencia = 0.1f;
    float amplitud = 50.0f; // Punto decimal corregido (50.0f)

    // 3. Vector para guardar la primera malla
    std::vector<Vertice> mallaMontana;

    // 4. Bucles para generar los vertices de la primera malla
    for (int z = 0; z < largo; z++) {
        for (int x = 0; x < ancho; x++) { // <-- Faltaba este bucle for para la X

            // Calcular la altura y llamar a nuestra funcion
            float alturaY = calcularCapaRuido((float)x, (float)z, frecuencia, amplitud);

            // Creamos el vertice en la posicion actual
            Vertice v;
            v.x = (float)x;
            v.y = alturaY;
            v.z = (float)z; // <-- Faltaba la 'z' aqui

            // Guardamos los vertices en la malla de montañas
            mallaMontana.push_back(v);
        }
    }
    // 5. GUARDAR LOS VÉRTICES EN UN ARCHIVO .TXT
    std::ofstream archivo("malla_montana.txt");

    if (archivo.is_open()) {
        archivo << "# Lista de Vertices (X, Y, Z)\n";
        for (const auto& v : mallaMontana) {
            archivo << v.x << " " << v.y << " " << v.z << "\n";
        }
        archivo.close();
        std::cout << "¡Archivo 'malla_montana.txt' guardado exitosamente!" << std::endl;
    } else {
        std::cout << "Error al crear el archivo." << std::endl;
    }

    // El mensaje va FUERA de los bucles for
    std::cout << "Malla 1 generada con " << mallaMontana.size() << " vertices." << std::endl;

    return 0;
}

// 5. Funcion temporal para que compile y pruebes la ejecucion
float calcularCapaRuido(float x, float z, float frecuencia, float amplitud) {
    return (std::sin(x * frecuencia) + std::cos(z * frecuencia)) * amplitud;
}