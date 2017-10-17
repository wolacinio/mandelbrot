#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <functional>

using namespace std;
using namespace sf;

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
bool modul(complex);
int calculate(complex);
complex clear(complex);
void initialize(color colors[]);
void compute(color colors[]);
void renderingThread(RenderWindow* window, View * view);

const int width = 1366;
const int height = 768;
int *maxIter;
float minX = -2.0;
float minY = -1.25;
float maxX = 0.5;
float maxY = 1.25;

Vertex view[width][height];
Mutex mutex;
VertexArray fraktal(Points, height * width);
complex potegowanie(complex, complex);

int main(int argc, char** argv){
	maxIter = new int;
	int temp = atoi(argv[1]);
	maxIter = &temp;
	color colors[*maxIter];
	Thread thread1(&initialize, colors);
	thread1.launch();
	thread1.wait();
	Thread thread2(&compute, colors);
	thread2.launch();

	ContextSettings settings;
	settings.antialiasingLevel = 0;
	RenderWindow window(VideoMode(width, height), "Madelbrot", Style::Default, settings);
    View view(FloatRect(0, 0, width, height));
    window.setView(view);

	Thread thread(bind(&renderingThread, &window, &view));
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

void initialize(color colors[]){
	mutex.lock();
	int levelNum = *maxIter;
	for (int level=0; level < *maxIter; level++)
	{
		colors[level].red = 255.0*level/(*maxIter);
		colors[level].blue = 255.0*log(level)/log((*maxIter));
		colors[level].green = 255.0*level/(*maxIter);
	}
	mutex.unlock();
}

void compute(color colors[]){
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

void renderingThread(RenderWindow* window, View * view){
    while (window->isOpen())
    {
        Event event;
        while (window->pollEvent(event))
        {
            if (event.key.code == Keyboard::Escape)
                window->close();
   //          if (Keyboard::isKeyPressed(Keyboard::Left)){
			// 	view->zoom(0.5f);
			// }
        }
 		View currentView = window->getView();
        window->clear();
        window->draw(fraktal);
        window->display();
        sleep(milliseconds(10));
    }
}