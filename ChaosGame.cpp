#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

using namespace sf;

enum class ApplicationState
{
	WELCOME = 0,
	DRAWING = 1,
	COMPUTING = 2,
	RENDERING = 3,
	FINISHED = 4
};

void GeneratePointOnClick(RenderWindow&);
void GenerateSierpinksiTrianglePoints(int);
void ResetApplication();

ApplicationState appState = ApplicationState::WELCOME;
std::vector<Vector2f> pointPositions;

const std::string APP_NAME = "Chaos Game";
const int PIXEL_TOLERANCE_VAL = 10;

const int RECT_SIDE_LENGTH = 4;
int numberOfManuallyGeneratedPoints = 0;
const int additionalPointsToGenerate = 1000;

int currentResX = 1920;
int currentResY = 1080;

int main()
{
	srand(rand());


	double scaleMultiplier = 0.75f;

	currentResX = VideoMode::getDesktopMode().width * scaleMultiplier;
	currentResY = VideoMode::getDesktopMode().height * scaleMultiplier;
	float aspectRatio = (float)currentResX / (float)currentResY;
	std::string currentMessage = "DEBUG";

	//VideoMode Object
	VideoMode vm(currentResX, currentResY);
	RenderWindow window(vm, APP_NAME, Style::Close);
	window.setFramerateLimit(60);

	//create a point
	RectangleShape currentPoint;
	Vector2f defaultSize(RECT_SIDE_LENGTH, RECT_SIDE_LENGTH);
	Vector2f largeSize(RECT_SIDE_LENGTH * 2, RECT_SIDE_LENGTH * 2);

	Vector2f defaultOrigin(RECT_SIDE_LENGTH / 2, RECT_SIDE_LENGTH / 2);
	Vector2f largeOrigin(RECT_SIDE_LENGTH, RECT_SIDE_LENGTH);

	Font font;
	font.loadFromFile("fonts/Tecno Chaos.ttf");

	Text text;
	text.setFont(font);
	text.setCharacterSize(75 / aspectRatio * scaleMultiplier);
	text.setPosition(0, 0);

	while (window.isOpen())
	{
		//Event processing
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;

			case Event::KeyPressed:
				if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();
				if (Keyboard::isKeyPressed(Keyboard::Return)) ResetApplication();
				break;

			case Event::MouseButtonPressed:
				if (Mouse::isButtonPressed(Mouse::Left))
				{
					GeneratePointOnClick(window);
				}
				break;

			default:
				break;
			}
		}

		switch (appState)
		{
		case(ApplicationState::WELCOME):
			currentMessage = "Welcome to the Chaos Game!\nUse Left Mouse Button to create at least 3 points.\nUse Left Mouse Button on the Green point to start computing!";
			if (pointPositions.size() > 0) appState = ApplicationState::DRAWING;
			break;

		case(ApplicationState::DRAWING):
			if (pointPositions.size() < 3)
			{
				currentMessage = "Use Left Mouse Button to create another point within the window!\n" + std::to_string(3 - pointPositions.size()) + " more " + ((pointPositions.size() == 1) ? "points are" : "point is") + " required!";
			}
			else
				currentMessage = "Optional: Use Left Mouse Button to more points. \nUse Left Mouse Button on the Green point to start computing!";
			break;

		case(ApplicationState::COMPUTING):
			currentMessage = "Computing Shape using " + std::to_string(additionalPointsToGenerate) + " additional points.....";
			GenerateSierpinksiTrianglePoints(additionalPointsToGenerate);
			break;

		case(ApplicationState::RENDERING):
			currentMessage = "Rendering...";
			break;

		case(ApplicationState::FINISHED):
			currentMessage = "Finished! Total points on screen: " + std::to_string(pointPositions.size()) + "!!!\nPress Return to start again or Escape to exit.";
			break;
		}


		//Draw the stuff!
		window.clear();

		for (int i = pointPositions.size() - 1; i >= 0; i--)
		{
			currentPoint.setPosition(pointPositions[i]);
			currentPoint.setOrigin(largeOrigin);
			currentPoint.setSize(largeSize);

			if (pointPositions.size() <= numberOfManuallyGeneratedPoints)
			{
				if (i == 0)
				{
					currentPoint.setFillColor(Color::Green);
				}
				else if (pointPositions.size() > 1 && i == pointPositions.size() - 1)
				{
					currentPoint.setFillColor(Color::Yellow);
				}
				else
				{
					currentPoint.setFillColor(Color::White);
					currentPoint.setSize(defaultSize);
					currentPoint.setOrigin(defaultOrigin);
				}
			}
			else
			{
				if (i < numberOfManuallyGeneratedPoints)
				{
					currentPoint.setFillColor(Color::Blue);
				}
				else
				{
					currentPoint.setFillColor(Color::White);
					currentPoint.setSize(defaultSize);
					currentPoint.setOrigin(defaultOrigin);
				}
			}

			

			window.draw(currentPoint);
		}

		if (appState == ApplicationState::RENDERING) appState = ApplicationState::FINISHED;

		text.setString(currentMessage);
		window.draw(text);

		window.display();
	}

	return 0;
}

void GeneratePointOnClick(RenderWindow& _window)
{

	if ( appState == ApplicationState::WELCOME || appState == ApplicationState::DRAWING)
	{

		//Create a point where the mouse position is
		Vector2f newPoint(Mouse::getPosition(_window));

		/*
		*	Oh god where to begin...
		*
		*	We need to verify that the user has entered in at least 3 points.
		*	If the user clicks on their first point then move on to the computation phase.
		*	It will be virtually impossible to consistently click exactly on their first point they drew, so
		*	introduce a tolerance level that the user has to click within to connect their shape together.
		*/
		if (pointPositions.size() > 2 &&
			(newPoint.x <= (pointPositions[0].x + fmax(RECT_SIDE_LENGTH, PIXEL_TOLERANCE_VAL)) && newPoint.x >= (pointPositions[0].x - fmax(RECT_SIDE_LENGTH, PIXEL_TOLERANCE_VAL))) &&
			(newPoint.y <= (pointPositions[0].y + fmax(RECT_SIDE_LENGTH, PIXEL_TOLERANCE_VAL)) && newPoint.y >= (pointPositions[0].y - fmax(RECT_SIDE_LENGTH, PIXEL_TOLERANCE_VAL)))
			)
		{
			appState = ApplicationState::COMPUTING;
		}
		else
		{
			pointPositions.push_back(newPoint);
			numberOfManuallyGeneratedPoints++;
		}
	}
}

void GenerateSierpinksiTrianglePoints(int _resolution)
{
	Vector2f startingPoint(currentResX / 2, currentResY / 2);
	Vector2f randomVertex = pointPositions[rand() % numberOfManuallyGeneratedPoints];
	Vector2f midPoint;

	for (int i = 0; i < _resolution; i++)
	{
		 randomVertex = pointPositions[rand() % numberOfManuallyGeneratedPoints];

		 midPoint.x = (startingPoint.x + randomVertex.x) / 2;
		 midPoint.y = (startingPoint.y + randomVertex.y) / 2;
		 startingPoint = midPoint;

		 pointPositions.push_back(midPoint);

	}
	appState = ApplicationState::RENDERING;
}

void ResetApplication()
{
	pointPositions.clear();
	numberOfManuallyGeneratedPoints = 0;
	appState = ApplicationState::WELCOME;
}