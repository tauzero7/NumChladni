
uniform vec2 winSize;
uniform sampler2D tex;
uniform int currEV;

in int vIdx[];

layout(triangles_adjacency) in;
layout(triangle_strip,max_vertices=3) out;

noperspective out vec3 dist;

out vec3  N13;
out vec3  N46;
out vec2  tc;

float getElemVal( int n, int idx ) {
    if (idx<-1) {
        return 0;
    }
    return texelFetch(tex,ivec2(idx,n),0).r;
}

void main() 
{    
    vec2 p0 = winSize*gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = winSize*gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = winSize*gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;

    vec2 v0 = p2-p1;
    vec2 v1 = p2-p0;
    vec2 v2 = p1-p0;
    float area = abs(v1.x*v2.y - v1.y*v2.x);
    
    float h1,h2,h3,h4,h5,h6;
    h1 = getElemVal(currEV,vIdx[0]);
    h2 = getElemVal(currEV,vIdx[1]);
    h3 = getElemVal(currEV,vIdx[2]);
    h4 = getElemVal(currEV,vIdx[3]);
    h5 = getElemVal(currEV,vIdx[4]);
    h6 = getElemVal(currEV,vIdx[5]);


#if 1
    int i0 = 0;
    int i1 = 1;
    int i2 = 2;
#else 
    // show mid vertices
    int i0 = 3;
    int i1 = 4;
    int i2 = 5;
#endif    
    gl_Position = gl_in[i0].gl_Position;   
    dist = vec3(area/length(v0),0,0);
    N13 = vec3(h1,h2,h3);
    N46 = vec3(h4,h5,h6);
    tc  = vec2(0,0);
    EmitVertex();    
    
    
    gl_Position = gl_in[i1].gl_Position;
    dist = vec3(0,area/length(v1),0);
    N13 = vec3(h1,h2,h3);
    N46 = vec3(h4,h5,h6);
    tc  = vec2(1,0);
    EmitVertex();
        
    
    gl_Position = gl_in[i2].gl_Position;   
    dist = vec3(0,0,area/length(v2));
    N13 = vec3(h1,h2,h3);
    N46 = vec3(h4,h5,h6);
    tc  = vec2(0,1);
    EmitVertex();
    
    EndPrimitive();
}
