/*
===============================================================================================
 ARCHIVO: terreno_grande.cpp
 OBJETIVO: Generar un terreno 3x mas grande que terreno_completo.cpp (2400 x 1800)
           combinando las 3 octavas mediante FBM (sin SFML, solo consola).
 COMPATIBILIDAD: C++17 o superior (g++)
===============================================================================================
 PESOS Y FRECUENCIAS DE LAS OCTAVAS:
 -------------------------------------------------------------------------------
 - Octava 1: Frecuencia = 0.005f | Amplitud = 1.00f  (Continentes / Masa Base)
 - Octava 2: Frecuencia = 0.010f | Amplitud = 0.50f  (Colinas / Valles)
 - Octava 3: Frecuencia = 0.020f | Amplitud = 0.25f  (Detalle Fino / Rocas)

 RESOLUCION:
 -------------------------------------------------------------------------------
 - terreno_completo : 800 x 600   =   480.000 puntos
 - terreno_grande   : 2400 x 1800 = 4.320.000 puntos  (3x en cada eje)

 SALIDAS:
 -------------------------------------------------------------------------------
 - terreno_datos_grande.txt  : filas "x,y,altura" (4.320.000 lineas, ~70 MB)
===============================================================================================
*/

#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>

// ============================================================================
// CLASE: PERLIN NOISE 2D
// ============================================================================
class PerlinNoise {
private:
    std::vector<int> p;

    float fade(float t) {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    float grad(int hash, float x, float y) {
        int h = hash & 7;
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }

public:
    PerlinNoise(unsigned int semilla = 12345) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(semilla);
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end());
    }

    float evaluar(float x, float y) {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;

        x -= std::floor(x);
        y -= std::floor(y);

        float u = fade(x);
        float v = fade(y);

        int A = p[X] + Y;
        int B = p[X + 1] + Y;

        float resultado = lerp(v, lerp(u, grad(p[A], x, y),
                                          grad(p[B], x - 1, y)),
                                 lerp(u, grad(p[A + 1], x, y - 1),
                                          grad(p[B + 1], x - 1, y - 1)));

        return (resultado + 1.0f) / 2.0f; // Escalar de [-1, 1] a [0, 1]
    }
};

int main() {
    const unsigned int ANCHO = 2400; // 3x de 800
    const unsigned int ALTO  = 1800; // 3x de 600

    const unsigned int SEMILLA = 42;
    PerlinNoise perlin(SEMILLA);

    // Parametros FBM para las 3 octavas
    const float freq1 = 0.005f, amp1 = 1.00f; // Octava 1: Continentes
    const float freq2 = 0.010f, amp2 = 0.50f; // Octava 2: Colinas
    const float freq3 = 0.020f, amp3 = 0.25f; // Octava 3: Detalles
    const float suma_amplitudes = amp1 + amp2 + amp3;

    std::cout << "Generando terreno grande " << ANCHO << "x" << ALTO
              << " (" << (ANCHO * ALTO) << " puntos)..." << std::endl;

    std::ofstream archivo("terreno_datos_grande.txt");
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir terreno_datos_grande.txt\n";
        return -1;
    }

    for (unsigned int y = 0; y < ALTO; ++y) {
        if (y % 300 == 0) {
            std::cout << "  Fila " << y << " / " << ALTO << std::endl;
        }
        for (unsigned int x = 0; x < ANCHO; ++x) {

            // Evaluamos las 3 octavas independientemente
            float octava1 = perlin.evaluar(x * freq1, y * freq1);
            float octava2 = perlin.evaluar(x * freq2, y * freq2);
            float octava3 = perlin.evaluar(x * freq3, y * freq3);

            // Suma ponderada FBM
            float altura_acumulada = (octava1 * amp1) + (octava2 * amp2) + (octava3 * amp3);
            altura_acumulada /= suma_amplitudes; // Normalizar entre [0.0, 1.0]

            // Guardar coordenadas (x, y) y altura en el archivo
            archivo << x << "," << y << "," << altura_acumulada << "\n";
        }
    }
    archivo.close();

    std::cout << "¡Listo! Archivo 'terreno_datos_grande.txt' generado con "
              << (ANCHO * ALTO) << " puntos." << std::endl;

    return 0;
}
