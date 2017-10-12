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

struct color {
	float red;
	float blue;
	float green;
};

const int width = 500;
const int height = 500;
Vertex view[width][height];
VertexArray fraktal(Points, height * width);


complex potegowanie(complex, complex);
bool modul(complex);
int calculate(complex);
complex clear(complex);


color colors[121];

void HSV2RGB(float hue, float sat, float val, float &red, float &grn, float &blu)
{
	int i;
	float f, p, q, t;
	if(val==0) {red=0; grn=0; blu=0;}
	else{
		hue/=60;
		i = (int)(hue);
		f = hue-i;
		p = val*(1-sat);
		q = val*(1-(sat*f));
		t = val*(1-(sat*(1-f)));
		if (i==0) {red=val; grn=t; blu=p;}
		else if (i==1) {red=q; grn=val; blu=p;}
		else if (i==2) {red=p; grn=val; blu=t;}
		else if (i==3) {red=p; grn=q; blu=val;}
		else if (i==4) {red=t; grn=p; blu=val;}
		else if (i==5) {red=val; grn=p; blu=q;}
	}
}
 
//initialize array with colors used to color different levels
void initializeColors()
{
	int level;
	float r, g, b;
	 
	for (level=0; level<120; level++)
	{
		HSV2RGB(2.1*level, 0.85, 0.6, r, g, b);
		//colors[level] = (TColor)(int)(r*255) + ( << 8) + ( << 16);
		colors[level].red = (int)(r*255);
		colors[level].blue = (int)(b*255);
		colors[level].green = (int)(g*255);
	}
	//colors[120] = 0;
}


int main(int argc, char** argv){
	cout << "Wprowadz ilosc iteracji: ";
	cin >> maxIter;
	initializeColors();
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
			// view[j][i].position = Vector2f(i, j);
			// view[j][i].color = Color::Red;
			fraktal[i*500+j].position = Vector2f(j, i);
			//fraktal[i*500+j].color = Color::Red;
			fraktal[i*500+j].color = Color(colors[color].red, colors[color].green, colors[color].blue);
			// cout << color << " ";
			cout << colors[color].red << endl;
		}
		// cout << endl;
	}
	
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(width, height), "SFML works!", Style::Default, settings);
    CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
 
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }
 
        window.clear();
        // window.draw(shape);
        window.draw(fraktal);
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