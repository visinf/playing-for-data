/******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Crytek
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#version 420 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 secondary;

uniform mat4 ModelViewProj;
uniform vec2 PointSpriteSize;
uniform uint HomogenousInput;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out v2f
{
	vec4 secondary;
	vec4 norm;
} OUT;

void main(void)
{
	vec2 psprite[4] =
	{
		vec2(-1.0f, -1.0f),
		vec2(-1.0f,  1.0f),
		vec2( 1.0f, -1.0f),
		vec2( 1.0f,  1.0f)
	};

	vec4 pos = position;
	if(HomogenousInput == 0)
		pos = vec4(position.xyz, 1);

	gl_Position = ModelViewProj * pos;
	gl_Position.xy += PointSpriteSize.xy*0.01f*psprite[gl_VertexID%4]*gl_Position.w;
	OUT.secondary = secondary;
	OUT.norm = vec4(0, 0, 1, 1);
}
