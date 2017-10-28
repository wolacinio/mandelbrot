#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <functional>
#include <TGUI/TGUI.hpp>

using namespace std;
using namespace sf;
using namespace tgui;

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
void renderingThread(RenderWindow* window, View * view, Gui * gui);
void refreshValueSlider();
void executeAction(const int &);
void renderWidgets();
void runComputeThread();

const int width = 1366;
const int height = 768;
const int maxIter = 200;

float minX = -2.0;
float minY = -1.25;
float maxX = 0.5;
float maxY = 1.25;

auto sliderMinX = Slider::create();
auto sliderMaxX = Slider::create();
auto sliderMinY = Slider::create();
auto sliderMaxY = Slider::create();
auto labelMinX = Label::create();
auto labelMaxX = Label::create();
auto labelMinY = Label::create();
auto labelMaxY = Label::create();
auto windows = ChildWindow::create();

color colors[maxIter];
Thread colorsThread(&initialize, colors);
Thread computeThread(&compute, colors);
Vertex view[width][height];
Mutex mutex;
VertexArray fraktal(Points, height * width);
complex potegowanie(complex, complex);
ContextSettings * settings = new ContextSettings(0, 0, 0, 2, 0);
RenderWindow window(VideoMode(width, height), "Madelbrot", Style::Fullscreen, *settings);
Gui gui(window);

int main(){
	colorsThread.launch();
	colorsThread.wait();
	computeThread.launch();
	computeThread.wait();

    View view(sf::FloatRect(0, 0, width, height));
    
    view.setCenter(width/2, height/2);
    window.setView(view);
    renderWidgets();   
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.key.code == Keyboard::Escape || event.type == sf::Event::Closed)
                window.close();
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
           		view.move(-10, 0);
           	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
           		view.move(10, 0);
           	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
           		view.move(0, -10);
           	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
           		view.move(0, 10);
            gui.handleEvent(event);
        }
        window.setView(view);
        window.clear();
		window.draw(fraktal);
        gui.draw();
        window.display();
        sleep(milliseconds(25));
    }
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
	}while(modul(z) && (i < maxIter));

	return i;
}

complex clear(complex a){
	a.real = 0.0;
	a.imag = 0.0;
	return a;
}

void initialize(color colors[]){
	mutex.lock();
	int levelNum = maxIter;
	for (int level=0; level < maxIter; level++){
		colors[level].red = 255.0*level/(maxIter);
		colors[level].blue = 255.0*log(level)/log((maxIter));
		colors[level].green = 255.0*level/(maxIter);
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
			fraktal[i*width+j].position = sf::Vector2f(j, i);
			fraktal[i*width+j].color = sf::Color(colors[color].red, colors[color].green, colors[color].blue);
		}
	}
	mutex.unlock();
}

void runComputeThread(){
	computeThread.launch();
	computeThread.wait();
}

void refreshValueSlider(){
	labelMinX->setText(to_string((float)sliderMinX->getValue()/100));
    labelMinX->setPosition(100, 10);
    labelMinX->setTextSize(14);  

 	labelMaxX->setText(to_string((float)sliderMaxX->getValue()/100));
    labelMaxX->setPosition(100, 50);
    labelMaxX->setTextSize(14);

    labelMinY->setText(to_string((float)sliderMinY->getValue()/100));
    labelMinY->setPosition(100, 90);
    labelMinY->setTextSize(14);

    labelMaxY->setText(to_string((float)sliderMaxY->getValue()/100));
    labelMaxY->setPosition(100, 130);
    labelMaxY->setTextSize(14);
}

void executeAction(const int & option){	
	switch(option){
		case 0 : runComputeThread();
			break;
		case 1 : minX = (float)sliderMinX->getValue()/100;
			break;
		case 2 : maxX = (float)sliderMaxX->getValue()/100;
			break;
		case 3 : minY = (float)sliderMinY->getValue()/100;
			break;
		case 4 : maxY = (float)sliderMaxY->getValue()/100;
			break;
	}
	refreshValueSlider();
}



void renderWidgets(){
	auto label = Label::create();
	int tmin = -200, tmax = 200;

 	sliderMinX->setSize(Layout2d(100, 15));
    sliderMinX->setPosition(10, 30);
    sliderMinX->setMinimum(tmin);
    sliderMinX->setMaximum(tmax);
    sliderMinX->setValue(-200);
    sliderMinX->connect("ValueChanged", executeAction, 1);
 	windows->add(sliderMinX);       
    label->setText("Minimum X: ");
    label->setPosition(5, 10);
    label->setTextSize(14);
    windows->add(label);

    label = Label::create();
 	sliderMaxX->setSize(Layout2d(100, 15));
    sliderMaxX->setPosition(10, 70);
    sliderMaxX->setMinimum(tmin);
    sliderMaxX->setMaximum(tmax);
    sliderMaxX->setValue(50);
    sliderMaxX->connect("ValueChanged", executeAction, 2);
 	windows->add(sliderMaxX);       
    label->setText("Maksimum X: ");
    label->setPosition(5, 50);
    label->setTextSize(14);
    windows->add(label);

 	label = Label::create();
 	sliderMinY->setSize(Layout2d(100, 15));
    sliderMinY->setPosition(10, 110);
    sliderMinY->setMinimum(tmin);
    sliderMinY->setMaximum(tmax);
    sliderMinY->setValue(-125);
    sliderMinY->connect("ValueChanged", executeAction, 3);
 	windows->add(sliderMinY);       
    label->setText("Minimum Y: ");
    label->setPosition(5, 90);
    label->setTextSize(14);
    windows->add(label);

 	label = Label::create();
 	sliderMaxY->setSize(Layout2d(100, 15));
    sliderMaxY->setPosition(10, 150);
    sliderMaxY->setMinimum(tmin);
    sliderMaxY->setMaximum(tmax);
    sliderMaxY->setValue(125);
    sliderMaxY->connect("ValueChanged", executeAction, 4);
 	windows->add(sliderMaxY);       
    label->setText("Maksimum Y: ");
    label->setPosition(5, 130);
    label->setTextSize(14);
    windows->add(label);
 	
 	windows->add(labelMinX);
 	windows->add(labelMaxX);
 	windows->add(labelMinY);
 	windows->add(labelMaxY);
 	
 	refreshValueSlider();
 	
 	auto button = Button::create();
    button->setPosition(5, 180);
    button->setText("Generuj");
    button->setSize(90, 25);
    button->connect("pressed", executeAction, 0);
    windows->add(button);

    button = Button::create();
    button->setPosition(90, 180);
    button->setText("Exit");
    button->setSize(90, 25);
    button->connect("pressed", [=](){ window.close(); });

    windows->add(button);
    windows->setSize(185, 220);
    windows->setPosition(5, 5);
    windows->setTitle("Parametry");

    gui.add(windows);    
}