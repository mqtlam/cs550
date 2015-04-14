#version 150

in  vec4 color;
out vec4 fColor;
uniform int numToonShades;

void main()
{
    fColor = color;
}
