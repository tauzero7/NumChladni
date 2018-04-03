
uniform ivec2 activeSegment;

layout(lines) in;
layout(line_strip, max_vertices=2) out;

flat in int vertexID[];
flat out int isActive;

// ---------------------------------------------------------------------
//   
// ---------------------------------------------------------------------
void main()
{
    int segmentIsActive = 0;
    if ((vertexID[0]==activeSegment.x && vertexID[1]==activeSegment.y)
        || (vertexID[0]==activeSegment.y && vertexID[1]==activeSegment.x)) {
        segmentIsActive = 1;
    }

    gl_Position = gl_in[0].gl_Position;
    isActive = segmentIsActive;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    isActive = segmentIsActive;
    EmitVertex();

    EndPrimitive();
}

