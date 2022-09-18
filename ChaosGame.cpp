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
const float PI = 4.0 * atan(1.0);

const sf::Color TEXT_BOX_BACKGROUND_COLOR(0, 0, 0, 127);
int numManuallyGenPoints = 0;
int currentResX = 1920;
int currentResY = 1080;
double scaleMultiplier = .75;

bool updateRenderedPoints = true;

sf::RectangleShape currentPoint;


int main()
{
	srand(rand());

	currentResX = sf::VideoMode::getDesktopMode().width * scaleMultiplier;
	currentResY = sf::VideoMode::getDesktopMode().height * scaleMultiplier;
	float aspectRatio = (float)currentResX / (float)currentResY;
	std::string currentMessage = "DEBUG";
	std::string pointsDisplayMessage = "-1";

	//VideoMode Object
	sf::VideoMode vm(currentResX, currentResY);
	sf::RenderWindow window(vm, APP_NAME, sf::Style::Close);
	window.setFramerateLimit(60);

	//create a point
	sf::Vector2f defaultSize(RECT_SIDE_LENGTH, RECT_SIDE_LENGTH);
	sf::Vector2f largeSize(RECT_SIDE_LENGTH * 2, RECT_SIDE_LENGTH * 2);
	const sf::Vector2f RENDERING_SIZE(1, 1);

	sf::Vector2f defaultOrigin(RECT_SIDE_LENGTH / 2, RECT_SIDE_LENGTH / 2);
	sf::Vector2f largeOrigin(RECT_SIDE_LENGTH, RECT_SIDE_LENGTH);
	const sf::Vector2f RENDERING_ORIGIN(0.5, 0.5);

	sf::Font font;
	font.loadFromFile("fonts/SebastianSerifNbp-7weB.ttf");

	sf::Text messageBox;
	messageBox.setFont(font);
	messageBox.setCharacterSize(75 / aspectRatio * scaleMultiplier);

	//Text Offset from the edge of the screen, based on font size, and aspect ratio
	sf::Vector2f minCharSpacing(font.getLineSpacing(messageBox.getCharacterSize()), font.getLineSpacing(messageBox.getCharacterSize()));
	messageBox.setPosition(minCharSpacing);

	sf::Text pointsDisplay;
	pointsDisplay.setFont(font);
	pointsDisplay.setCharacterSize(75 / aspectRatio * scaleMultiplier);


	//Message Box background
	const int TEXT_BOX_BORDER = 5;
	sf::Vector2f textBoundingBoxVector;
	sf::RectangleShape textBackground;
	textBackground.setFillColor(TEXT_BOX_BACKGROUND_COLOR);

	//Point Display Background
	sf::RectangleShape pointsDisplayBkgd;
	pointsDisplayBkgd.setFillColor(TEXT_BOX_BACKGROUND_COLOR);

	//Helper line that connects the user's recently generated point, to their mouse
	sf::RectangleShape shapeVisualizer;
	const float LINE_WIDTH = 2;
	shapeVisualizer.setFillColor(sf::Color::White);




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
			currentMessage = "Welcome to the Chaos Game!\nUse Left Mouse Button to create at least 3 points.";
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
			currentMessage = "Finished!\nPress Return to start again or Escape to exit.";
			break;
		}

		//Draw the stuff!
		window.clear(sf::Color(20, 20, 20, 255));


		if (appState == ApplicationState::DRAWING || updateRenderedPoints)
		{
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
						currentPoint.setSize(defaultSize);
						currentPoint.setOrigin(defaultOrigin);
					}
					else
					{
						currentPoint.setFillColor(sf::Color::White);
						currentPoint.setSize(RENDERING_SIZE);
						currentPoint.setOrigin(RENDERING_ORIGIN);
					}
				}

				window.draw(currentPoint);
			}


			pointsDisplay.setString("User-created Points: " + std::to_string(numManuallyGenPoints));
			pointsDisplay.setPosition(currentResX - pointsDisplay.getLocalBounds().width - minCharSpacing.x, currentResY - pointsDisplay.getLocalBounds().height - minCharSpacing.y);
			pointsDisplayBkgd.setPosition((pointsDisplay.getPosition().x - 2) - TEXT_BOX_BORDER, (pointsDisplay.getPosition().y + 7) - TEXT_BOX_BORDER);


			textBoundingBoxVector.x = pointsDisplay.getLocalBounds().width + (TEXT_BOX_BORDER * 2);
			textBoundingBoxVector.y = pointsDisplay.getLocalBounds().height + (TEXT_BOX_BORDER * 2);
			pointsDisplayBkgd.setSize(textBoundingBoxVector);

			window.draw(pointsDisplayBkgd);
			window.draw(pointsDisplay);


			//THE DRAW ORDER HERE MATTERS!!!
			messageBox.setString(currentMessage);
			textBackground.setPosition((messageBox.getPosition().x - 2) - TEXT_BOX_BORDER, (messageBox.getPosition().y + 7) - TEXT_BOX_BORDER);

			textBoundingBoxVector.x = messageBox.getLocalBounds().width + (TEXT_BOX_BORDER * 2);
			textBoundingBoxVector.y = messageBox.getLocalBounds().height + (TEXT_BOX_BORDER * 2);
			textBackground.setSize(textBoundingBoxVector);

			window.draw(textBackground);
			window.draw(messageBox);


			float mag = 0;
			if (appState == ApplicationState::DRAWING && pointPositions.size() > 0)
			{
				shapeVisualizer.setPosition(pointPositions[pointPositions.size() - 1]);

				sf::Vector2f pointDistances = (sf::Vector2f)sf::Mouse::getPosition(window) - pointPositions[pointPositions.size() - 1];

				float mx = pointPositions[pointPositions.size() - 1].x - sf::Mouse::getPosition(window).x;
				float my = pointPositions[pointPositions.size() - 1].y - sf::Mouse::getPosition(window).y;

				mag = (mx * mx) + (my * my);

				float angle = atan2(-my, -mx);

				shapeVisualizer.setRotation(angle * 180 / PI);
			}

			shapeVisualizer.setSize(sf::Vector2f(sqrt(mag), LINE_WIDTH));
			window.draw(shapeVisualizer);


			window.display();

			if (appState == ApplicationState::RENDERING)
			{
				appState = ApplicationState::FINISHED;
				updateRenderedPoints = true;
			}
			else
				updateRenderedPoints = false;
		}
	}

	return 0;
}

void GeneratePointOnClick(sf::RenderWindow& _window)
{
	if (appState == ApplicationState::WELCOME || appState == ApplicationState::DRAWING)
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
	sf::Vector2f startingPoint(rand() % currentResX, rand() % currentResY);
	sf::Vector2f randomVertex = pointPositions[rand() % numManuallyGenPoints];
	sf::Vector2f previousVertex;
	sf::Vector2f midPoint;

	float scaler = (float)numManuallyGenPoints / (float(numManuallyGenPoints) + 3.0f);
	std::cout << "Scaler: " << scaler << std::endl;
	pointPositions.reserve(numManuallyGenPoints + _resolution);
	int seed = rand() % numManuallyGenPoints;
	randomVertex = pointPositions[seed];

	for (int i = 0; i < _resolution; i++)
	{
		seed = rand() % numManuallyGenPoints;
		randomVertex = pointPositions[seed];

		midPoint.x = startingPoint.x + (randomVertex.x - (startingPoint.x)) * scaler;
		midPoint.y = startingPoint.y + (randomVertex.y - (startingPoint.y)) * scaler;

		startingPoint = midPoint;
		pointPositions.emplace_back(midPoint);

		previousVertex = randomVertex;
	}
	appState = ApplicationState::RENDERING;
	updateRenderedPoints = true;
}
void ResetApplication()
{
	pointPositions.clear();
	numManuallyGenPoints = 0;
	updateRenderedPoints = true;
	appState = ApplicationState::WELCOME;
}
void GeneratePoint(sf::Vector2f inputPoint)
{
	pointPositions.push_back(inputPoint);
	numManuallyGenPoints++;
	if (pointPositions.size() > 0) appState = ApplicationState::DRAWING;

	updateRenderedPoints = true;
}
void GenerateRandomPoint()
{
	sf::Vector2f randPoint(rand() % currentResX, rand() % currentResY);
	GeneratePoint(randPoint);
}