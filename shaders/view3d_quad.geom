
uniform mat4  proj_matrix;
uniform mat4  view_matrix;
uniform vec3  camPos;

uniform sampler2D tex;
uniform int   currEV;
uniform float cosWT;
uniform float scaleFactor;

in int vIdx[];

layout(triangles_adjacency) in;
layout(triangle_strip,max_vertices=3) out;


out vec3  bc;
out float hValue;
out float dotProd;
flat out vec3 gValue13;
flat out vec3 gValue46;
flat out vec3 g1,g2,g3,g4,g5,g6;


float getElemVal( int n, int idx ) {
    if (idx<-1) {
        return 0;
    }
    return 2*texelFetch(tex,ivec2(idx,n),0).r;
}

void main() 
{
    vec4 vert;
    vec3 v1,v2,v3,v4,v5,v6;
    float h1,h2,h3,h4,h5,h6;
    vec3 n1,n2,n3,n4,n5,n6;

    h1 = getElemVal(currEV,vIdx[0]);
    h2 = getElemVal(currEV,vIdx[1]);
    h3 = getElemVal(currEV,vIdx[2]);
    h4 = getElemVal(currEV,vIdx[3]);
    h5 = getElemVal(currEV,vIdx[4]);
    h6 = getElemVal(currEV,vIdx[5]);

    v1 = vec3( gl_in[0].gl_Position.xy, h1*cosWT*scaleFactor );
    v2 = vec3( gl_in[1].gl_Position.xy, h2*cosWT*scaleFactor );
    v3 = vec3( gl_in[2].gl_Position.xy, h3*cosWT*scaleFactor );
    v4 = vec3( gl_in[3].gl_Position.xy, h4*cosWT*scaleFactor );
    v5 = vec3( gl_in[4].gl_Position.xy, h5*cosWT*scaleFactor );
    v6 = vec3( gl_in[5].gl_Position.xy, h6*cosWT*scaleFactor );

    n1 = normalize(cross(v4-v1,v6-v1));
    n2 = normalize(cross(v5-v2,v4-v2));
    n3 = normalize(cross(v6-v3,v5-v3));

#if 1
    n4 = 0.5*(n1+n2);
    n5 = 0.5*(n2+n3);
    n6 = 0.5*(n3+n1);
#else
    n4 = normalize(cross(v5-v4,v6-v4));
    n5 = normalize(cross(v6-v5,v4-v5));
    n6 = normalize(cross(v4-v6,v5-v4));
#endif

    vert = vec4(v1,1);
    gl_Position = proj_matrix * view_matrix * vert;
    gValue13  = vec3(h1,h2,h3);
    gValue46  = vec3(h4,h5,h6);
    bc   = vec3(1,0,0);
    g1 = n1; g2 = n2; g3 = n3;
    g4 = n4; g5 = n5; g6 = n6;
    dotProd = dot(normalize(camPos.xyz-vert.xyz),n1);
    EmitVertex();
    
    
    vert = vec4(v2,1);
    gl_Position = proj_matrix * view_matrix * vert;
    gValue13  = vec3(h1,h2,h3);
    gValue46  = vec3(h4,h5,h6);
    bc   = vec3(0,1,0);
    g1 = n1; g2 = n2; g3 = n3;
    g4 = n4; g5 = n5; g6 = n6;
    dotProd = dot(normalize(camPos.xyz-vert.xyz),n2);
    EmitVertex();
    
    
    vert = vec4(v3,1);
    gl_Position = proj_matrix * view_matrix * vert;
    gValue13  = vec3(h1,h2,h3);
    gValue46  = vec3(h4,h5,h6);
    bc   = vec3(0,0,1);
    g1 = n1; g2 = n2; g3 = n3;
    g4 = n4; g5 = n5; g6 = n6;
    dotProd = dot(normalize(camPos.xyz-vert.xyz),n3);
    EmitVertex();
    
    EndPrimitive();
}
