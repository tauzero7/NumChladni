
layout(location = 0) in vec3 in_position;
layout(location = 2) in vec4 in_color;

uniform mat4  view_matrix;
uniform mat4  proj_matrix;
uniform vec3  translate;

void main()
{
    vec4 vert = vec4(in_position+translate,1.0);
    gl_Position = proj_matrix * view_matrix * vert;
}
