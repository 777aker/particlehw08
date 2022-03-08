#version 150 compatibility
#extension GL_EXT_geometry_shader4 : enable
layout(points) in;
layout(triangle_strip,max_vertices=4) out;

out vec2 Tex2D;
out vec4 Color;

float size = .3;

void billboard(float x, float y, int s, int t) {
	// copy color 
	Color = gl_FrontColorIn[0];
	// set texture coordinates
	Tex2D = vec2(s,t);
	// determine position
	vec2 delta = vec2(x,y);
	vec4 p = gl_PositionIn[0];
	p.x += dot(delta,gl_ModelViewMatrix[0].xy);
	p.y += dot(delta,gl_ModelViewMatrix[1].xy);
	p.z += dot(delta,gl_ModelViewMatrix[2].xy);
	gl_Position = gl_ModelViewProjectionMatrix*p;
	// emit vertex
	EmitVertex();
}

void main() {
	billboard(-size,-size,0,0);
	billboard(+size,-size,1,0);
	billboard(-size,+size,0,1);
	billboard(+size,+size,1,1);
	EndPrimitive();
}