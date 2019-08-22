//out vec4 gl_Position;



void main()
{
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
