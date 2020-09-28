#version 440 core
#extension GL_EXT_shader_io_blocks : enable
#extension GL_ARB_enhanced_layouts : enable

layout ( set = 1, binding = 0 ) uniform sampler2D texSampler;

layout ( location = 0 ) in vec2 fragmentColor;
layout ( location = 0 ) out vec4 outColor;
void main()
{
	outColor = texture(texSampler, fragmentColor);
	//outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}