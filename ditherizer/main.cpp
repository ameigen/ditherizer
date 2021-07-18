#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <time.h>
#include <Eigen/Dense>
#include "tgmath.h"
#include <vector>

sf::RenderWindow window;
sf::Texture texture;
sf::Sprite sprite;

std::string nameappend(std::string extension, std::string append)
{
	std::string file = extension;
	size_t pos = 0;
	for (auto i : file)
	{
		if (i == '.')
		{
			file.insert(pos, append);
		}
		pos += 1;
	}
	return file;
}

void bayerDithering(std::string filename, sf::Image Image, Eigen::MatrixXf threshold)
{
	sf::Image Output = Image;
	sf::Vector2u size = Image.getSize();
	int height = size.y;
	int width = size.x;
	int subsize = threshold.rows();
	float threshval = 0.0;
	std::vector<sf::Vector2u> positions;
	sf::Uint8 average;
	sf::Color main = sf::Color::White;
	sf::Color sub = sf::Color::Black;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
		
			threshval = threshold(i%subsize, j%subsize);
			sf::Color curcolor = Output.getPixel(i, j);
			average = (curcolor.r + curcolor.g + curcolor.b) / 3;
			float brightness = (float(average)) / 255.0;
			if (brightness - 0.5 > threshval) Output.setPixel(i, j, main);
			else Output.setPixel(i, j, sub);
		}
		window.clear();
		texture.loadFromImage(Output);
		sprite.setTexture(texture);
		window.draw(sprite);
		window.display();
	}
	std::cout << "Dithering complete...\n";
	std::string outputname = nameappend(filename, "_bayer");
	Output.saveToFile(std::string("./output/") += outputname);
}

Eigen::MatrixXf calculateBayer(Eigen::MatrixXf previous, int m, char flag = 'g')
{
	Eigen::MatrixXf nextlevel;
	Eigen::MatrixXf normalmatrix;
	Eigen::Matrix2f level0;
	Eigen::MatrixXf submatrix1;
	Eigen::MatrixXf submatrix2;
	Eigen::MatrixXf submatrix3;
	Eigen::MatrixXf submatrix4;
	level0(0, 0) = -0.25;
	level0(0, 1) = 0.25;
	level0(1, 0) = 0.5;
	level0(1, 1) = 0;

	int subsize = m * 2;
	int mainsize = m * 4;

	submatrix1.resize(subsize, subsize);
	submatrix2 = submatrix1;
	submatrix3 = submatrix1;
	submatrix4 = submatrix1;
	nextlevel.resize(mainsize, mainsize);
	normalmatrix = nextlevel;

	for (int row = 0; row < mainsize; ++row)
	{
		for (int col = 0; col < mainsize; ++col)
		{
			normalmatrix(row, col) = -0.5;
		}
	}
	for (int row = 0; row < m * 2; ++row)
	{
		for (int col = 0; col < m * 2; ++col)
		{
			submatrix1(row, col) = 0;
		}
	}
	for (int row = 0; row < m * 2; ++row)
	{
		for (int col = 0; col < m * 2; ++col)
		{
			submatrix2(row, col) = 3.0;
		}
	}
	for (int row = 0; row < m * 2; ++row)
	{
		for (int col = 0; col < m * 2; ++col)
		{
			submatrix3(row, col) = 2.0;
		}
	}
	for (int row = 0; row < m * 2; ++row)
	{
		for (int col = 0; col < m * 2; ++col)
		{
			submatrix4(row, col) = 1.0;
		}
	}
	float offset =  m * 4;
	float size = m * 2;
	float i = 1.0 / ((size * 2) * (size * 2));
	submatrix1 = (4 * previous);
	submatrix2 = (4 * previous) + submatrix2;
	submatrix3 = (4 * previous) + submatrix3;
	submatrix4 = (4 * previous) + submatrix4;

	nextlevel.block(0, 0, size, size) = submatrix1;
	nextlevel.block(offset - size , 0, size, size) = submatrix2;
	nextlevel.block(0, offset - size, size, size) = submatrix3;
	nextlevel.block(offset - size, offset - size, size, size) = submatrix4;
	
	if (flag == 'e')
	{
		nextlevel = nextlevel * i;
		nextlevel = nextlevel + normalmatrix;
	}
	return nextlevel;
}

Eigen::MatrixXf bayerMatrix(int level = 1)
{
	Eigen::MatrixXf currentmatrix;
	currentmatrix.resize(2, 2);
	currentmatrix(0, 0) = 0;
	currentmatrix(0, 1) = 2;
	currentmatrix(1, 0) = 3;
	currentmatrix(1, 1) = 1;
	int count = 1;
	if (level > 4)level = 3;
	while (count <= level)
	{
		if (count == level)
		{
			currentmatrix = calculateBayer(currentmatrix, count, 'e');
		}
		else
		{
			currentmatrix = calculateBayer(currentmatrix, count);
		}
		count++;
	}
	std::cout << currentmatrix << std::endl;
	return currentmatrix;
}

sf::Image grayscale(std::string filename, char flag = 'n')
{
	sf::Image Image;
	if (!Image.loadFromFile(std::string("./input/") += filename))
	{
		std::cout << "Failed to load " << filename << '\n';
		return Image;
	}
	else
	{
		sf::Vector2u size = Image.getSize();
		int height = size.x;
		int width = size.y;
		const sf::Uint8* ptr = Image.getPixelsPtr();
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				sf::Uint8 r = ptr[4 * (j * height + i) + 0];
				sf::Uint8 g = ptr[4 * (j * height + i) + 1];
				sf::Uint8 b = ptr[4 * (j * height + i) + 2];
				sf::Uint8 a = ptr[4 * (j * height + i) + 3];
				sf::Uint8 average = (r + g + b) / 3;
				sf::Color gray;
				gray.r = average;
				gray.g = average;
				gray.b = average;
				
				Image.setPixel(i, j, gray);
			}
		}
		if (flag != 'n')
		{
			std::string grayname = nameappend(filename, "_gray");
			Image.saveToFile(std::string("./grey/") += grayname);
		}
		return Image;
	}
}

void randomNoise(std::string filename, sf::Image Image)
{
	sf::Vector2u size = Image.getSize();
	int height = size.x;
	int width = size.y;
	const sf::Uint8* ptr = Image.getPixelsPtr();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			sf::Uint8 r = ptr[4 * (j * height + i) + 0];
			sf::Uint8 g = ptr[4 * (j * height + i) + 1];
			sf::Uint8 b = ptr[4 * (j * height + i) + 2];
			sf::Uint8 a = ptr[4 * (j * height + i) + 3];
			sf::Uint8 average = (r + g + b) / 3;
			sf::Color gray;
			gray.r = average;
			gray.g = average;
			gray.b = average;
			int random = rand() % 127 + 1;
			if ((int(average) + random - 127) > 127)
			{
				gray.r = 255;
				gray.g = 255;
				gray.b = 255;
			}
			else
			{
				gray.r = 0;
				gray.g = 0;
				gray.b = 0;
			}
			Image.setPixel(i, j, gray);
		}
	}
	std::string outputname = nameappend(filename, "_output");
	Image.saveToFile(std::string("./output/") += outputname);
}

int main()
{
	srand(time(NULL));

	bool processing = true;
	bool inputbuffer = true;
	char input = ' ';
	std::string filename;
	sf::Image Image;
	sf::Vector2u empty(0, 0);
	while (processing)
	{
		filename = "";
		std::cout << "What file do you want to ditherize?\n";
		std::cin >> filename;
		Image = grayscale(filename, 'y');
		if (Image.getSize() != empty)
		{
			sf::Vector2u imagedims = Image.getSize();
			window.create(sf::VideoMode(imagedims.x, imagedims.y), "Ditherizer");

			bayerDithering(filename, Image, bayerMatrix(2));
			std::cout << "Would you like to ditherize again? (Y/N) \n";
			inputbuffer = true;
			while (inputbuffer)
			{
				std::cin >> input;
				switch (input)
				{
				case 'Y':
				case 'y':
					inputbuffer = false;
					break;
				case 'N':
				case 'n':
					std::cout << "Thank you. Have a nice day!\n";
					return 0;
					break;
				default:
					std::cout << "!Invalid Input! Would you like to ditherize again? (Y/N) \n";
				}
			}
		}
	}
}