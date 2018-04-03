
uniform mat4  mvp;

layout(location = 0) in  vec4  in_position;
layout(location = 1) in  ivec2 in_vertexID;

flat out int vertexID;

void main() {
    vec4 vert = vec4(in_position.xy,0,1.0);
    gl_Position = mvp * vert;
    vertexID = in_vertexID.x;
}
