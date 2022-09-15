#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

enum class ApplicationState
{
	WELCOME = 0,
	DRAWING = 1,
	COMPUTING = 2,
	RENDERING = 3,
	FINISHED = 4
};

void GeneratePoint(sf::Vector2f inputPoint);
void GeneratePointOnClick(sf::RenderWindow&);
void GenerateRandomPoint();
void GenerateSierpinksiTrianglePoints(int);
void ResetApplication();

ApplicationState appState = ApplicationState::WELCOME;
std::vector<sf::Vector2f> pointPositions;

const std::string APP_NAME = "Chaos Game";
const int PIXEL_TOLERANCE_VAL = 10;
const int RECT_SIDE_LENGTH = 6;
const int additionalPointsToGenerate = 100000;

const sf::Color TEXT_BOX_BACKGROUND_COLOR(0,0,0,127);
int numManuallyGenPoints = 0;
int currentResX = 1920;
int currentResY = 1080;
double scaleMultiplier = .75;

bool windowNeedsUpdating = true;

sf::RectangleShape currentPoint;


int main()
{
	srand(rand());


	currentResX = sf::VideoMode::getDesktopMode().width * scaleMultiplier;
	currentResY = sf::VideoMode::getDesktopMode().height * scaleMultiplier;
	float aspectRatio = (float)currentResX / (float)currentResY;
	std::string currentMessage = "DEBUG";

	//VideoMode Object
	sf::VideoMode vm(currentResX, currentResY);
	sf::RenderWindow window(vm, APP_NAME, sf::Style::Close);
	window.setFramerateLimit(60);

	//create a point
	sf::Vector2f defaultSize(2, 2);
	sf::Vector2f largeSize(RECT_SIDE_LENGTH, RECT_SIDE_LENGTH);

	sf::Vector2f defaultOrigin(1, 1);
	sf::Vector2f largeOrigin(RECT_SIDE_LENGTH / 2, RECT_SIDE_LENGTH / 2);

	sf::Font font;
	font.loadFromFile("fonts/SebastianSerifNbp-7weB.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(75 / aspectRatio * scaleMultiplier);

	text.setPosition(font.getLineSpacing(text.getCharacterSize()), font.getLineSpacing(text.getCharacterSize()));


	const int TEXT_BOX_BORDER = 5;
	sf::Vector2f textBoundingBoxVector;
	sf::RectangleShape textBackground;
	textBackground.setPosition((text.getPosition().x - 2) - TEXT_BOX_BORDER, (text.getPosition().y+7) - TEXT_BOX_BORDER);

	

	//Main game loop
	while (window.isOpen())
	{
		//Event processing
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) ResetApplication();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) GenerateRandomPoint();
				break;

			case sf::Event::MouseButtonPressed:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
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


		if (windowNeedsUpdating)
		{
			std::cout << "X: " << textBoundingBoxVector.x << ", Y: " << textBoundingBoxVector.y << std::endl;


			//Draw the stuff!
			window.clear(sf::Color(20, 20, 20, 255));



			//Draw all points!
			for (int i = pointPositions.size() - 1; i >= 0; i--)
			{
				currentPoint.setPosition(pointPositions[i]);
				currentPoint.setOrigin(defaultOrigin);
				currentPoint.setSize(defaultSize);

				if (pointPositions.size() <= numManuallyGenPoints)
				{
					if (i == 0)
					{
						currentPoint.setFillColor(sf::Color::Green);
						currentPoint.setOrigin(largeOrigin);
						currentPoint.setSize(largeSize);
					}
					else if (pointPositions.size() > 1 && i == pointPositions.size() - 1)
					{
						currentPoint.setFillColor(sf::Color::Yellow);
					}
					else
					{
						currentPoint.setFillColor(sf::Color::White);
					}
				}
				else
				{
					if (i < numManuallyGenPoints)
					{
						currentPoint.setFillColor(sf::Color::Blue);
					}
					else
					{
						currentPoint.setFillColor(sf::Color::White);
						currentPoint.setSize(defaultSize);
						currentPoint.setOrigin(defaultOrigin);
					}
				}

				window.draw(currentPoint);
			}





			//THE ORDER HERE MATTERS!!!
			text.setString(currentMessage);

			textBoundingBoxVector.x = text.getLocalBounds().width + (TEXT_BOX_BORDER * 2);
			textBoundingBoxVector.y = text.getLocalBounds().height + (TEXT_BOX_BORDER * 2);

			textBackground.setSize(textBoundingBoxVector);
			textBackground.setFillColor(TEXT_BOX_BACKGROUND_COLOR);

			window.draw(textBackground);
			window.draw(text);
			//Up to here!!


			window.display();

			if (appState == ApplicationState::RENDERING)
			{
				appState = ApplicationState::FINISHED;
				windowNeedsUpdating = true;
			}
			else
			windowNeedsUpdating = false;
		}
	}

	return 0;
}

void GeneratePointOnClick(sf::RenderWindow& _window)
{
	if ( appState == ApplicationState::WELCOME || appState == ApplicationState::DRAWING)
	{

		//Create a point where the mouse position is
		sf::Vector2f newPoint(sf::Mouse::getPosition(_window));

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
			GeneratePoint(newPoint);
		}

	}
}

void GenerateSierpinksiTrianglePoints(int _resolution)
{
	sf::Vector2f startingPoint(currentResX / 2, currentResY / 2);
	sf::Vector2f randomVertex = pointPositions[rand() % numManuallyGenPoints];
	sf::Vector2f midPoint;

	pointPositions.reserve(numManuallyGenPoints + _resolution);
	for (int i = 0; i < _resolution; i++)
	{
		 randomVertex = pointPositions[rand() % numManuallyGenPoints];

		 midPoint.x = (startingPoint.x + randomVertex.x) / 2;
		 midPoint.y = (startingPoint.y + randomVertex.y) / 2;
		 startingPoint = midPoint;

		 //pointPositions[i] = midPoint;
		 pointPositions.emplace_back(midPoint);

	}
	appState = ApplicationState::RENDERING;
	windowNeedsUpdating = true;
}

void ResetApplication()
{
	pointPositions.clear();
	numManuallyGenPoints = 0;
	windowNeedsUpdating = true;
	appState = ApplicationState::WELCOME;
}

void GeneratePoint(sf::Vector2f inputPoint)
{
	pointPositions.push_back(inputPoint);
	numManuallyGenPoints++;
	if (pointPositions.size() > 0) appState = ApplicationState::DRAWING;

	windowNeedsUpdating = true;
}
void GenerateRandomPoint()
{
	sf::Vector2f randPoint(rand() % currentResX, rand() % currentResY);
	GeneratePoint(randPoint);
}