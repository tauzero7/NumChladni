
uniform sampler2D tex;
uniform int       numNodesPerTriangle;
uniform float     cosWT;
uniform float     meshOpacity;
uniform int       useColor;
uniform int       useGridOnly;
uniform int       showIsolines;
uniform float     scaleFactor;

layout(location = 0) out vec4 frag_color;

noperspective in vec3 dist;
in vec2 tc;
in vec3 N13;
in vec3 N46;

vec3 wireCol = vec3(1,1,1);
vec3 fillCol = vec3(0,0,0);


float mySign( float val ) {
    if (val<=0.0) {
        return -1.0;
    }
    return 1.0;
}

vec3 hsv2rgb( vec3 hsv )
{
   /*
    * Purpose:
    * Convert HSV values to RGB values
    * All values are in the range [0.0 .. 1.0]
    */
   float S, H, V, F, M, N, K;
   int   I;

   H = hsv.r; //->h;  /* Hue */
   S = hsv.g; //->s;  /* Saturation */
   V = hsv.b; //->v;  /* value or brightness */

   vec3 rgb = vec3(0.0);
   if ( S == 0.0 ) {
      /*
       * Achromatic case, set level of grey
       */
      rgb.r = V;
      rgb.g = V;
      rgb.b = V;
   } else {
      /*
       * Determine levels of primary colours.
       */
      if (H >= 1.0) {
         H = 0.0;
      } else {
         H = H * 6;
      } /* end if */
      I = int(H);   /* should be in the range 0..5 */
      F = H - I;     /* fractional part */

      M = V * (1 - S);
      N = V * (1 - S * F);
      K = V * (1 - S * (1 - F));

      if (I == 0) { rgb.r = V; rgb.g = K; rgb.b = M; }
      if (I == 1) { rgb.r = N; rgb.g = V; rgb.b = M; }
      if (I == 2) { rgb.r = M; rgb.g = V; rgb.b = K; }
      if (I == 3) { rgb.r = M; rgb.g = N; rgb.b = V; }
      if (I == 4) { rgb.r = K; rgb.g = M; rgb.b = V; }
      if (I == 5) { rgb.r = V; rgb.g = M; rgb.b = N; }
   } /* end if */

   return rgb;
}


void main() {
    vec3 color = vec3(0.2);

    float d = min(dist[0],min(dist[1],dist[2]));
    float I = exp(-0.3*d*d);
    color = I*wireCol + (1.0-I)*fillCol;

    if (useGridOnly==0) {

        float hValue = 0.0;
        if (numNodesPerTriangle==3) {
            vec3 zeta = vec3(1-tc.x-tc.y,tc);
            hValue = dot(N13,zeta);
        } else {
            vec3 zeta = vec3(1-tc.x-tc.y,tc);
            vec3 n1 = zeta*(2*zeta-1);
            vec3 n2 = 4*zeta.xyx*zeta.yzz;
            hValue = dot(N13,n1) + dot(N46,n2);
        }

        float v = clamp(scaleFactor*hValue*cosWT + 0.5,0,1);
        if (showIsolines==1) {
            v *= mySign(sin(2*PI*hValue*4))*0.2 + 0.8;
        } else {
            v *= (mySign(hValue)*0.2 + 0.8);
        }

        if (useColor==1) {
            color = vec3(color.r,0,0)*meshOpacity + hsv2rgb(vec3(v,1,1));
        } else {
            color = vec3(color.r,0,0)*meshOpacity + vec3(v);
        }
    }
    frag_color = vec4(color,1);
}
