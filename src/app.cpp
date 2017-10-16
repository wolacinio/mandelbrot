#include <iostream>
#include <cmath>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <functional>

using namespace std;
using namespace sf;

int *maxIter;

struct complex
{
	float real;
	float imag;
};

struct color {
	float red;
	float blue;
	float green;
	bool set = 0;
};
Mutex mutex;
const int width = 1366;
const int height = 768;
Vertex view[width][height];

VertexArray fraktal(Points, height * width);


complex potegowanie(complex, complex);
bool modul(complex);
int calculate(complex);
complex clear(complex);


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

void initializeColors(color colors[])
{
	int level;
	float r, g, b;
	for (int level=0; level < *maxIter; level++)
	{
		HSV2RGB(2.1*level, 0.85, 0.6, r, g, b);
		colors[level].red = (int)(r*255);
		colors[level].blue = (int)(b*255);
		colors[level].green = (int)(g*255);
	}
}

float minX = -2.0;
float minY = -1.25;
float maxX = 0.5;
float maxY = 1.25;

void initialize(color colors[])
{
	mutex.lock();
	int levelNum = *maxIter;
	for (int level=0; level < *maxIter; level++)
	{
		colors[level].red = 255.0*level/(*maxIter);
		colors[level].blue = 255.0*log(level)/log((*maxIter));

		colors[level].green = 255.0*level/(*maxIter);
		colors[level].set = true;
	}
	mutex.unlock();
}

void com(color colors[]){
	mutex.lock();
	complex temp;
	int color;
	float ratioX = (maxX - minX) / width;
	float ratioY = (maxY - minY) / height;
	for (int i = 0; i < height; i++) {
		temp.imag = i*ratioY + minY;
		for (int j = 0; j < width; j++) {
			temp.real = j*ratioX + minX;
			color = calculate(temp);
			fraktal[i*width+j].position = Vector2f(j, i);
			fraktal[i*width+j].color = Color(colors[color].red, colors[color].green, colors[color].blue);
		}
	}
	mutex.unlock();
}
void renderingThread(sf::RenderWindow* window, View * view)
{
    while (window->isOpen())
    {
        Event event;
        while (window->pollEvent(event))
        {
            if (event.key.code == Keyboard::Escape)
                window->close();
            if (Keyboard::isKeyPressed(Keyboard::Left)){
				view->zoom(0.5f);
			}

        }
 		View currentView = window->getView();
        window->clear();
        window->draw(fraktal);
        window->display();
        sleep(milliseconds(10));
    }
}

int main(int argc, char** argv){
	cout << argv[1];
	maxIter = new int;
	int temp = atoi(argv[1]);
	maxIter = &temp;
	color colors[*maxIter];
	//initialize(colors);
	sf::Thread thread1(&initialize, colors);
	thread1.launch();
	thread1.wait();
	//com(colors);		
	sf::Thread thread2(&com, colors);
	thread2.launch();

	ContextSettings settings;
	settings.antialiasingLevel = 0;
	RenderWindow window(VideoMode(width, height), "Madelbrot", Style::Default, settings);
    View view(sf::FloatRect(0, 0, width, height));
    window.setView(view);

	sf::Thread thread(std::bind(&renderingThread, &window, &view));
    thread.launch();

	return false;
}


complex potegowanie(complex  a, complex  b){
	complex t;
	t.real = (a.real * a.real) - (a.imag * a.imag) + b.real;
	t.imag = 2.0 * a.real * a.imag + b.imag;
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
	}while(modul(z) && (i < *maxIter));

	return i;
}

complex clear(complex a){
	a.real = 0.0;
	a.imag = 0.0;
	return a;
}