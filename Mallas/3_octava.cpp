/*
===============================================================================
 ARCHIVO: 3_octava.cpp
 OBJETIVO: Generar y visualizar la TERCERA OCTAVA (Detalles y Rugosidad).
 COMPATIBILIDAD: SFML 3.x (Arch Linux / yay)
===============================================================================
 CONFIGURACIÓN DE CAPAS Y BLOQUES (EQUIVALENCIA EN VÓXELES / MINECRAFT):
 -------------------------------------------------------------------------------
 - Frecuencia de la capa: 0.02f   (4x la frecuencia de la Octava 1)
 - Amplitud / Peso en el terreno: 0.25f (25% del peso en la altura final)
 
 - Impacto en el terreno 3D (Eje Y):
     * Otorga la apariencia de "pico" a las montañas y bordes rocosos.
     * En bloques, representa pequeños desniveles de apenas ~2 a 4 bloques,
       evitando que las cimas se vean redondas o perfectas.
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

class Octava3Perlin {
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
    Octava3Perlin(unsigned int semilla = 12345) {
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

        return (resultado + 1.0f) / 2.0f;
    }
};

int main() {
    const unsigned int ANCHO_VENTANA = 800;
    const unsigned int ALTO_VENTANA  = 600;

    sf::RenderWindow window(sf::VideoMode({ANCHO_VENTANA, ALTO_VENTANA}), 
                            "Octava 3 - Detalle Fino y Rugosidad (SFML 3)");

    // FRECUENCIA 4X MAYOR QUE LA OCTAVA 1
    const float FRECUENCIA_OCTAVA_3 = 0.02f; 
    const unsigned int SEMILLA      = 42;

    Octava3Perlin octava3(SEMILLA);
    sf::Image mapa_grises({ANCHO_VENTANA, ALTO_VENTANA}, sf::Color::Black);

    std::cout << "Generando grafica de la Octava 3 (Detalle)..." << std::endl;

    for (unsigned int y = 0; y < ALTO_VENTANA; ++y) {
        for (unsigned int x = 0; x < ANCHO_VENTANA; ++x) {
            
            float valor_altura = octava3.evaluar(x * FRECUENCIA_OCTAVA_3, y * FRECUENCIA_OCTAVA_3);
            std::uint8_t tono_gris = static_cast<std::uint8_t>(valor_altura * 255.0f);
            mapa_grises.setPixel({x, y}, sf::Color(tono_gris, tono_gris, tono_gris));
        }
    }

    sf::Texture textura;
    if (!textura.loadFromImage(mapa_grises)) return -1;
    sf::Sprite sprite(textura);

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