
uniform mat4  proj_matrix;
uniform mat4  view_matrix;
uniform vec3  camPos;

uniform sampler2D tex;
uniform int   currEV;
uniform float cosWT;
uniform float scaleFactor;

in int vIdx[];

layout(triangles) in;
layout(triangle_strip,max_vertices=3) out;

out float hValue;
out vec3  bc;
out vec3  norm;

out float dotProd;

float getElemVal( int n, int idx ) {
    if (idx<-1) {
        return 0;
    }
    return texelFetch(tex,ivec2(idx,n),0).r;
}

void main() 
{
    vec4 vert;
    vec3 v1,v2,v3;
    float h1,h2,h3;
    vec3  n1,n2,n3;

    h1 = getElemVal(currEV,vIdx[0]);
    h2 = getElemVal(currEV,vIdx[1]);
    h3 = getElemVal(currEV,vIdx[2]);

    v1 = vec3( gl_in[0].gl_Position.xy, h1*cosWT*scaleFactor );
    v2 = vec3( gl_in[1].gl_Position.xy, h2*cosWT*scaleFactor );
    v3 = vec3( gl_in[2].gl_Position.xy, h3*cosWT*scaleFactor );
    n1 = normalize(cross(v2-v1,v3-v1));
    n2 = normalize(cross(v3-v2,v1-v2));
    n3 = normalize(cross(v1-v3,v2-v3));

    vert = vec4(v1,1);
    gl_Position = proj_matrix * view_matrix * vert;
    hValue = h1;
    bc   = vec3(1,0,0);
    norm = n1;
    dotProd = dot(normalize(camPos.xyz-vert.xyz),n1);
    EmitVertex();
        

    vert = vec4(v2,1);
    gl_Position = proj_matrix * view_matrix * vert;
    hValue = h2;
    bc   = vec3(0,1,0);
    norm = n2;
    dotProd = dot(normalize(camPos.xyz-vert.xyz),n2);
    EmitVertex();
        

    vert = vec4(v3,1);
    gl_Position = proj_matrix * view_matrix * vert;
    hValue = h3;
    bc   = vec3(0,0,1);
    norm = n3;
    dotProd = dot(normalize(camPos.xyz-vert.xyz),n3);
    EmitVertex();
    

    EndPrimitive();
}
