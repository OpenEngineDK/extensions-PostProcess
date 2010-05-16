varying vec2 texCoord;

void main(void)
{
    texCoord = (gl_Vertex.xy + 1.0) * 0.5;

    gl_Position = gl_Vertex;
}
