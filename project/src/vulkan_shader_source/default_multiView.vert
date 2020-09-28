#version 440 core
#extension GL_EXT_shader_io_blocks : enable
#extension GL_ARB_enhanced_layouts : enable
#extension GL_OVR_multiview2 : enable

layout( set = 0, binding = 0 ) uniform Buffer
{
	mat4 viewMatrix[2];
	mat4 projectionMatrix[2];
	mat4 transformMatrix;
}ubo;

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec2 vertexTexColor;
layout ( location = 0 ) out vec2 outTexCoord;

out gl_PerVertex { vec4 gl_Position; };

void main( void )
{
	//ubo.projectionMatrix[gl_ViewID_OVR][1][1] = -ubo.projectionMatrix[gl_ViewID_OVR][1][1];
	//ubo.projectionMatrix[gl_ViewID_OVR][1][[3] = -ubo.projectionMatrix[gl_ViewID_OVR][1][3];
	
	gl_Position = ubo.projectionMatrix[gl_ViewID_OVR] * ( ubo.viewMatrix[gl_ViewID_OVR] * ( ubo.transformMatrix * vec4( vertexPosition,  1.0 ) ) );
    	//gl_Position.y = -gl_Position.y;
    	//gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;
	outTexCoord = vertexTexColor;
}