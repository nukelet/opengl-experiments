#version 330 core

in vec4 vertex_position;
out vec4 frag_color;

void main()
{
        frag_color = vertex_position;
}