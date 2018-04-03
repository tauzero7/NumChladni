
uniform sampler2D tex;
uniform int       haveTessShader;
uniform int       numNodesPerTriangle;
uniform float     cosWT;
uniform float     meshOpacity;
uniform int       useColor;
uniform int       useGridOnly;
uniform float     scaleFactor;
uniform int       useDotProd;
uniform int       whichShading;
uniform int       wireframe;

layout(location = 0) out vec4 frag_color;

in vec3  bc;
in vec3  norm;
in float hValue;
in float dotProd;

// quad interpolation only
flat in vec3 gValue13;
flat in vec3 gValue46;
flat in vec3 g1,g2,g3,g4,g5,g6;
flat in vec3 dp13;
flat in vec3 dp46;

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

    float v = 0.0;
    float h = 0.0;
    vec3 nv = vec3(0);

    if (haveTessShader==1) {
        //v = clamp(scaleFactor*hValue*cosWT + 0.5,0,1);
        v = clamp(hValue*cosWT*0.2 + 0.5,0,1);
        h = hValue;
    } else {
        if (numNodesPerTriangle==3) {
            h = hValue;
            nv = norm;
        } else {
            vec3 b1 = bc*(2*bc-1);
            vec3 b2 = 4*bc.xyx*bc.yzz;
            h = dot(gValue13,b1) + dot(gValue46,b2);
            nv = g1*b1.x + g2*b1.y + g3*b1.z + g4*b2.x + g5*b2.y + g6*b2.z;
        }
        v = clamp(h*cosWT*scaleFactor + 0.5,0,1);
    }

    if (wireframe==1) {
        v = 1.0;
    }

    switch (whichShading) {
        default:
        case 0: {
            color = vec3(v);
            break;
        }
        case 1: {
            color = vec3(v)*(mySign(h)*0.2 + 0.8);
            break;
        }
        case 2: {
            color = hsv2rgb(vec3(v,1,1));
            break;
        }
        case 3: {
            color = hsv2rgb(vec3(v,1,1))*(mySign(h)*0.2 + 0.8);
            break;
        }
        case 4: {
            color = bc;
            break;
        }
    }
    if (useDotProd==1) {
        color *= abs(dotProd) + 0.5;
    }
    frag_color = vec4(color,1);
}
