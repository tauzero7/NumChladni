
uniform int haveTessShader;

layout(location = 0) in vec3 in_position;

out vec2 vPosition;
out int  vIdx;

void main() {
    vec4 vert = vec4(in_position.xy,0,1);
    gl_Position = vert;

    vIdx = int(in_position.z);
    vPosition = in_position.xy;
}
