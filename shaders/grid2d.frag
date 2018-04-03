
uniform int  tickType;

layout(location = 0) out vec4 out_frag_color;

void main(void)
{
    if (tickType==1 || tickType==2) {
        out_frag_color = vec4(vec3(0.5),1);
    } else {
        out_frag_color = vec4(vec3(0.2),1);
    }
}
