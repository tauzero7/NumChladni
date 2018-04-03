
uniform mat4 mvp;
uniform vec2 scale;
uniform float pointSize;
uniform int   activePoint;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

flat in  ivec2 vertexProp[];

     out vec2  tc;
flat out ivec2 vProp;

// ---------------------------------------------------------------------
//   
// ---------------------------------------------------------------------
void main()
{
    vec2 s = pointSize*scale;

    if (vertexProp[0].x==activePoint) {
       s *= 2.0;
    }

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(s*vec2(-1,-1),0.0,0.0));
    tc  = vec2(-1.0,-1.0);
    vProp = vertexProp[0];
    EmitVertex();

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(s*vec2(1,-1),0.0,0.0));
    tc  = vec2( 1.0,-1.0);
    vProp = vertexProp[0];
    EmitVertex();

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(s*vec2(-1,1),0.0,0.0));
    tc  = vec2(-1.0, 1.0);
    vProp = vertexProp[0];
    EmitVertex();

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(s*vec2(1,1),0.0,0.0));
    tc  = vec2( 1.0, 1.0);
    vProp = vertexProp[0];
    EmitVertex();

   EndPrimitive();
}

