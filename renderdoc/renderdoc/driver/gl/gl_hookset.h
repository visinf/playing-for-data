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


#pragma once

#include "gl_common.h"
#include "gl_legacy_procs.h"

struct GLHookSet
{
	// ++ dllexport
	PFNGLBINDTEXTUREPROC glBindTexture;
	PFNGLBLENDFUNCPROC glBlendFunc;
	PFNGLBLENDCOLORPROC glBlendColor;
	PFNGLCLEARPROC glClear;
	PFNGLCLEARCOLORPROC glClearColor;
	PFNGLCLEARDEPTHPROC glClearDepth;
	PFNGLCULLFACEPROC glCullFace;
	PFNGLDEPTHFUNCPROC glDepthFunc;
	PFNGLDISABLEPROC glDisable;
	PFNGLDRAWARRAYSPROC glDrawArrays;
	PFNGLENABLEPROC glEnable;
	PFNGLFRONTFACEPROC glFrontFace;
	PFNGLGENTEXTURESPROC glGenTextures;
	PFNGLDELETETEXTURESPROC glDeleteTextures;
	PFNGLGETERRORPROC glGetError;
	PFNGLGETFLOATVPROC glGetFloatv;
	PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
	PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
	PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv;
	PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
	PFNGLGETINTEGERVPROC glGetIntegerv;
	PFNGLGETSTRINGPROC glGetString;
	PFNGLHINTPROC glHint;
	PFNGLPIXELSTOREIPROC glPixelStorei;
	PFNGLPIXELSTOREFPROC glPixelStoref;
	PFNGLPOLYGONMODEPROC glPolygonMode;
	PFNGLREADPIXELSPROC glReadPixels;
	PFNGLREADBUFFERPROC glReadBuffer;
	PFNGLTEXIMAGE1DPROC glTexImage1D;
	PFNGLTEXIMAGE2DPROC glTexImage2D;
	PFNGLTEXPARAMETERIPROC glTexParameteri;
	PFNGLVIEWPORTPROC glViewport;
	// legacy
	PFNGLLIGHTFVPROC glLightfv;
	PFNGLMATERIALFVPROC glMaterialfv;
	PFNGLGENLISTSPROC glGenLists;
	PFNGLNEWLISTPROC glNewList;
	PFNGLENDLISTPROC glEndList;
	PFNGLCALLLISTPROC glCallList;
	PFNGLSHADEMODELPROC glShadeModel;
	PFNGLBEGINPROC glBegin;
	PFNGLENDPROC glEnd;
	PFNGLVERTEX3FPROC glVertex3f;
	PFNGLNORMAL3FPROC glNormal3f;
	PFNGLPUSHMATRIXPROC glPushMatrix;
	PFNGLPOPMATRIXPROC glPopMatrix;
	PFNGLMATRIXMODEPROC glMatrixMode;
	PFNGLLOADIDENTITYPROC glLoadIdentity;
	PFNGLFRUSTUMPROC glFrustum;
	PFNGLTRANSLATEFPROC glTranslatef;
	PFNGLROTATEFPROC glRotatef;
	// --

	// ++ gl
	PFNGLACTIVETEXTUREPROC glActiveTexture;
	PFNGLTEXSTORAGE2DPROC glTexStorage2D;
	PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
	PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
	PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;
	PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v;
	PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
	PFNGLGETSTRINGIPROC glGetStringi;
	PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
	PFNGLGETFLOATI_VPROC glGetFloati_v;
	PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
	PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
	PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
	PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
	PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLGETSHADERIVPROC glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLGETPROGRAMIVPROC glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
	PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv;
	PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
	PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback; // aliases glDebugMessageCallbackARB
	PFNGLGETOBJECTLABELPROC glGetObjectLabel;
	PFNGLOBJECTLABELPROC glObjectLabel;
	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
	PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
	PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLBINDBUFFERBASEPROC glBindBufferBase;
	PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
	PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
	PFNGLUNMAPBUFFERPROC glUnmapBuffer;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
	PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
	PFNGLGENSAMPLERSPROC glGenSamplers;
	PFNGLBINDSAMPLERPROC glBindSampler;
	PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
	PFNGLCLEARBUFFERFVPROC glClearBufferfv;
	PFNGLCLEARBUFFERIVPROC glClearBufferiv;
	PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
	PFNGLCLEARBUFFERFIPROC glClearBufferfi;
	PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf;
	PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
	PFNGLVIEWPORTARRAYVPROC glViewportArrayv;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
	PFNGLGETUNIFORMFVPROC glGetUniformfv;
	PFNGLGETUNIFORMIVPROC glGetUniformiv;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
	PFNGLUNIFORM3FVPROC glUniform3fv;
	PFNGLUNIFORM4FVPROC glUniform4fv;
	PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
	PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
	// --

	// ++ wgl
	// --

	// ++ glx
	// --
};

#include "gl_hookset_defs.h"
