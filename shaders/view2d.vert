
layout(location = 0) in vec3 in_position;

uniform mat4 mvp;

out int vIdx;

void main() {
    vec4 vert = vec4(in_position.xy,0,1);
    gl_Position = mvp * vert;
    vIdx = int(in_position.z);
}
