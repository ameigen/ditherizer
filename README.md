# ditherizer
Simple ordered dithering implimentation with SFML and Eigen.

Images must be placed in the 'input' folder. Examples have been included below. 

As of right now the project does not accept user input for what degree Bayer matrix will be calculated and applied. You can manually change this inside of the driver function.
Simply change 'x' to the degree you wish to calculate.
```
int main()
{
  ...
  bayerDithering(filename, Image, bayerMatrix(x));
  ...
}
```
The target colors for reduction can also be changed inside of the bayerDithering() function.
Examples include: Black, White, Blue, Red, Cyan
You may also achieve this with standard 255 color delcarations as such:

```
void bayerDithering(std::string filename, sf::Image Image, Eigen::MatrixXf threshold)
{
  ... 
	sf::Color main = sf::Color::White;
	sf::Color sub = sf::Color::Black;
  
        sf::Color main(255,255,255)
        sf::Color main(0,0,0,);
  ...
}
```

<img src="https://i.imgur.com/QoD51bH.jpg" width="50%" height="50%"><img src="https://i.imgur.com/zpT28Xq.jpg" width="50%" height="50%">
<img src="https://i.imgur.com/rsqli5h.jpg" width="50%" height="50%"><img src="https://i.imgur.com/xfo75Y8.jpg" width="50%" height="50%">
