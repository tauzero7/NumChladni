
uniform vec3  lineColor;
uniform vec3  lineColorHighlighted;

flat in int isActive;

layout(location = 0) out vec4 out_frag_color;

void main() {
    vec4 color = vec4(lineColor,1);

    if (isActive==1) {
        color = vec4(lineColorHighlighted,1);
    }
    out_frag_color = color;
}
