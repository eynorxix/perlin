/*
===============================================================================
 ARCHIVO: 1_octava.cpp
 OBJETIVO: Generar y visualizar la PRIMERA OCTAVA (Ruido Base Continentales).
 COMPATIBILIDAD: SFML 3.x (Arch Linux / yay)
===============================================================================
 CONFIGURACIÓN DE CAPAS Y BLOQUES (EQUIVALENCIA EN VÓXELES / MINECRAFT):
 -------------------------------------------------------------------------------
 - Frecuencia de la capa: 0.005f  (Transiciones ultra suaves, mapas grandes)
 - Amplitud / Peso en el terreno: 1.0f (100% del peso base de la elevación)
 
 - Distribución estimada de bloques al mapear a 3D (Eje Y):
     * Altura total del Chunk / Mundo: ~64 a 128 bloques
     * Capa Base / Roca Madre (Piso):  Y = 01 a 10  (Sólido constante)
     * Subsuelo profundo (Piedra):     Y = 11 a 30  (~20 bloques de grosor)
     * Superficie Base (Pradera/Suelo):Y = 31 a 45  (Controlado por esta Octava 1)
     * Montañas Altas / Cumbres:      Y = 46 a 80+ (Requiere sumar Octavas 2, 3)

 NOTA DE ARQUITECTURA:
 Este archivo procesa únicamente el ruido suave base (Capa 1).
===============================================================================
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <iostream>
#include <cstdint> // Para std::uint8_t en C++20 / SFML 3

// ============================================================================
// CLASE: PERLIN NOISE 2D (IMPLEMENTACIÓN DE LA PRIMERA OCTAVA)
// ============================================================================
class Octava1Perlin {
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
    Octava1Perlin(unsigned int semilla = 12345) {
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

// ============================================================================
// VISUALIZADOR SFML 3
// ============================================================================
int main() {
    const unsigned int ANCHO_VENTANA = 800;
    const unsigned int ALTO_VENTANA  = 600;

    // SFML 3: VideoMode requiere Vector2u ({ancho, alto})
    sf::RenderWindow window(sf::VideoMode({ANCHO_VENTANA, ALTO_VENTANA}), 
                            "Octava 1 - Ruido Base Continental (SFML 3)");

    const float FRECUENCIA_OCTAVA_1 = 0.005f;
    const unsigned int SEMILLA      = 42;

    Octava1Perlin octava1(SEMILLA);

    // SFML 3: Inicialización directa del tamaño de la imagen ({ancho, alto})
    sf::Image mapa_grises({ANCHO_VENTANA, ALTO_VENTANA}, sf::Color::Black);

    std::cout << "Generando grafica de la Octava 1 (Ruido Base) en SFML 3...\n";

    for (unsigned int y = 0; y < ALTO_VENTANA; ++y) {
        for (unsigned int x = 0; x < ANCHO_VENTANA; ++x) {
            
            float valor_altura = octava1.evaluar(x * FRECUENCIA_OCTAVA_1, y * FRECUENCIA_OCTAVA_1);

            // Reemplazado sf::Uint8 por std::uint8_t
            std::uint8_t tono_gris = static_cast<std::uint8_t>(valor_altura * 255.0f);

            // SFML 3: setPixel requiere sf::Vector2u({x, y})
            mapa_grises.setPixel({x, y}, sf::Color(tono_gris, tono_gris, tono_gris));
        }
    }

    // SFML 3: loadFromImage devuelve bool, cargamos a textura
    sf::Texture textura;
    if (!textura.loadFromImage(mapa_grises)) {
        std::cerr << "Error al cargar la textura desde la imagen.\n";
        return -1;
    }

    // SFML 3: Sprite requiere pasar la textura como referencia en el constructor
    sf::Sprite sprite(textura);

    std::cout << "¡Generacion completa! Cierra la ventana para continuar.\n";

    // Bucle de eventos en SFML 3 (Uso de std::optional)
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