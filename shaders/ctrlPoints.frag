
uniform  vec3 pointColor;
uniform  vec3 pointColorHighlighted;
uniform  int  activePoint;

in vec2 tc;
flat in ivec2 vProp;

layout(location = 0) out vec4 out_frag_color;


void main() {
    vec4 color = vec4(pointColor,1);
    float r = length(tc);
    if (r>1.0) {
        discard;
    }

    if (vProp.y==1) {
        color = vec4(0,0,1,1);
    }
    if (vProp.x==activePoint) {
        color = vec4(pointColorHighlighted,1);
    }


    float val = sqrt(1.0-r*r);

    out_frag_color = color*val;
}
