
uniform mat4 pvm;

uniform vec4 border;
uniform int  tickType;
uniform vec2 gridOrigin;
uniform vec2 gridStep;

layout(location = 0) in vec2 in_position;

void main(void)
{
    vec4 verts = vec4(in_position,-0.5,1);
    int id = gl_InstanceID;

    if (tickType==1) {      // x-axis
        verts.x = border[0] + in_position.x*(border[1]-border[0]);
        verts.y = gridOrigin[1];
    }
    else if (tickType==2) { // y-axis
        verts.x = gridOrigin[0];
        verts.y = border[2] + in_position.x*(border[3]-border[2]);
    }
    else if (tickType==3) { // parallel to x-axis
        verts.x = border[0] + in_position.x*(border[1]-border[0]);
        verts.y = gridOrigin[1] + (id+1)*gridStep.y;
    }
    else if (tickType==4) {
        verts.x = border[0] + in_position.x*(border[1]-border[0]);
        verts.y = gridOrigin[1] - (id+1)*gridStep.y;
    }
    else if (tickType==5) { // parallel to y-axis
        verts.x = gridOrigin[0] + (id+1)*gridStep.x;
        verts.y = border[2] + in_position.x*(border[3]-border[2]);
    }
    else if (tickType==6) {
        verts.x = gridOrigin[0] - (id+1)*gridStep.x;
        verts.y = border[2] + in_position.x*(border[3]-border[2]);
    }
    gl_Position = pvm * verts;
}

