

layout(location = 0) in  vec4  in_position;
layout(location = 1) in  ivec2 in_vertexID;

flat out ivec2  vertexProp;

// ---------------------------------------------------------------------
//   
// ---------------------------------------------------------------------
void main()
{
    vec4 vert;
    vert = vec4(in_position.xy,0,1.0);
    gl_Position = vert;
    vertexProp = in_vertexID;
}
