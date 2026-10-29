/*
===============================================================================
 ARCHIVO: terreno_completo.cpp
 OBJETIVO: Unir las 3 octavas mediante FBM (Fractal Brownian Motion) y
           convertir el mapa de altura en biomas con colores.
 COMPATIBILIDAD: SFML 3.x (Arch Linux / yay)
===============================================================================
 PESOS Y FRECUENCIAS DE LAS OCTAVAS:
 -------------------------------------------------------------------------------
 - Octava 1: Frecuencia = 0.005f | Amplitud = 1.00f  (Continentes / Masa Base)
 - Octava 2: Frecuencia = 0.010f | Amplitud = 0.50f  (Colinas / Valles)
 - Octava 3: Frecuencia = 0.020f | Amplitud = 0.25f  (Detalle Fino / Rocas)
 
 Normalización: Se divide la suma total entre (1.0 + 0.5 + 0.25 = 1.75) 
 para garantizar que el resultado final quede siempre en el rango [0.0, 1.0].
===============================================================================
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <iostream>
#include <cstdint>
#include <fstream>

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

// Asignar colores según la altura acumulada
sf::Color obtenerColorBioma(float altura) {
    if (altura < 0.35f) {
        return sf::Color(30, 80, 180);   // Agua Profunda
    } else if (altura < 0.42f) {
        return sf::Color(60, 120, 220);  // Agua Poco Profunda
    } else if (altura < 0.47f) {
        return sf::Color(220, 208, 130); // Playa / Arena
    } else if (altura < 0.65f) {
        return sf::Color(70, 160, 50);   // Pradera / Pasto
    } else if (altura < 0.78f) {
        return sf::Color(110, 110, 110); // Montaña / Roca
    } else {
        return sf::Color(245, 245, 255); // Nieve / Cima
    }
}

int main() {
    const unsigned int ANCHO_VENTANA = 800;
    const unsigned int ALTO_VENTANA  = 600;

    sf::RenderWindow window(sf::VideoMode({ANCHO_VENTANA, ALTO_VENTANA}), 
                            "Mapa de Biomas - Suma de 3 Octavas (SFML 3)");

    const unsigned int SEMILLA = 42;
    PerlinNoise perlin(SEMILLA);

    // Parámetros FBM para las 3 octavas
    const float freq1 = 0.005f, amp1 = 1.00f; // Octava 1: Continentes
    const float freq2 = 0.010f, amp2 = 0.50f; // Octava 2: Colinas
    const float freq3 = 0.020f, amp3 = 0.25f; // Octava 3: Detalles
    const float suma_amplitudes = amp1 + amp2 + amp3;

    sf::Image mapa_terreno({ANCHO_VENTANA, ALTO_VENTANA}, sf::Color::Black);

    std::cout << "Generando mapa unificado combinando las 3 octavas...\n";

    std::ofstream archivo("terreno_datos.txt");
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir terreno_datos.txt\n";
        return -1;
    }

    for (unsigned int y = 0; y < ALTO_VENTANA; ++y) {
        for (unsigned int x = 0; x < ANCHO_VENTANA; ++x) {
            
            // Evaluamos las 3 octavas independientemente
            float octava1 = perlin.evaluar(x * freq1, y * freq1);
            float octava2 = perlin.evaluar(x * freq2, y * freq2);
            float octava3 = perlin.evaluar(x * freq3, y * freq3);

            // Suma ponderada FBM
            float altura_acumulada = (octava1 * amp1) + (octava2 * amp2) + (octava3 * amp3);
            altura_acumulada /= suma_amplitudes; // Normalizar entre [0.0, 1.0]

            // Guardar coordenadas (x, y) y altura en el archivo
            archivo << x << "," << y << "," << altura_acumulada << "\n";

            // Aplicar color de bioma según el valor final
            sf::Color color_pixel = obtenerColorBioma(altura_acumulada);
            mapa_terreno.setPixel({x, y}, color_pixel);
        }
    }
    archivo.close();

    sf::Texture textura;
    if (!textura.loadFromImage(mapa_terreno)) return -1;
    sf::Sprite sprite(textura);

    std::cout << "¡Generacion completa! Cierra la ventana para terminar.\n";

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
    
    return 0;
}