#version 330 core
out vec4 FragColor;

in vec4 colour;
void main()
{
	FragColor = colour;
}