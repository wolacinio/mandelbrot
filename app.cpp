#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int maxIter;

struct complex
{
	float real;
	float imag;
};

const int width = 500;
const int height = 500;
Vertex view[height][width];
VertexArray fraktal(Points, height + width);

complex potegowanie(complex, complex);
bool modul(complex);
int calculate(complex);
complex clear(complex);

int main(int argc, char** argv){
	cout << "Wprowadz ilosc iteracji: ";
	cin >> maxIter;
	
	complex temp;
	int color;

	float minX = -2.0;
	float minY = -1.25;
	float maxX = 0.5;
	float maxY = 1.25;

	float ratioX = (maxX - minX) / width;
	float ratioY = (maxY - minY) / height;
	
	for (int i = 0; i < height; ++i) {
		temp.imag = i*ratioY + minY;
		for (int j = 0; j < width; ++j) {
			temp.real = j*ratioX + minX;
			color = calculate(temp);
			view[j][i].position = Vector2f(i, j);
			view[j][i].color = Color::Red;
			fraktal[i+j].position = Vector2f(i, j);
			fraktal[i+j].color = Color::Red;
			//cout << color << " ";
		}
		//cout << endl;
	}
	
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(height, width), "SFML works!", Style::Default, settings);
    CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
 
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
 
        window.clear();
        window.draw(shape);
        // window.draw(fraktal);
        window.display();
    }


	return false;
}


complex potegowanie(complex  a, complex  b){
	complex t;
	t.real = (a.real * a.real) - (a.imag * a.imag) + b.real;
	t.imag = 2.0 * a.real * a.imag;
	return t;
}

bool modul(complex  a){
	//return sqrt((a.real * a.real) + (a.imag * a.imag));
	return (a.real * a.real) + (a.imag * a.imag) < 4.0;
}

int calculate(complex  p){
	int i = 0;
	complex z;
	z = clear(p);
	do{
		z = potegowanie(z, p);
		i++;
	}while(modul(z) && (i < maxIter));

	return i;
}

complex clear(complex a){
	a.real = 0.0;
	a.imag = 0.0;
	return a;
}

// void initializeColors()
// {
// 	int level;
// 	float r, g, b;
	 
// 	for (level=0; level<120; level++)
// 	{
// 		HSV2RGB(2.1*level, 0.85, 0.6, r, g, b);
// 		colors[level] = (TColor)(int)(r*255) + ((int)(g*255) << 8) + ((int)(b*255) << 16);
// 	}
// 	colors[120] = (TColor)0;
// }