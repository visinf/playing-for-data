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

struct GLHookSet
{
	// ++ dllexport
	PFNGLBINDTEXTUREPROC glBindTexture;
	PFNGLBLENDFUNCPROC glBlendFunc;
	PFNGLCLEARPROC glClear;
	PFNGLCLEARCOLORPROC glClearColor;
	PFNGLCLEARDEPTHPROC glClearDepth;
	PFNGLCLEARSTENCILPROC glClearStencil;
	PFNGLCOLORMASKPROC glColorMask;
	PFNGLCULLFACEPROC glCullFace;
	PFNGLDEPTHFUNCPROC glDepthFunc;
	PFNGLDEPTHMASKPROC glDepthMask;
	PFNGLSTENCILFUNCPROC glStencilFunc;
	PFNGLSTENCILMASKPROC glStencilMask;
	PFNGLSTENCILOPPROC glStencilOp;
	PFNGLDISABLEPROC glDisable;
	PFNGLDRAWBUFFERPROC glDrawBuffer;
	PFNGLDRAWELEMENTSPROC glDrawElements;
	PFNGLDRAWARRAYSPROC glDrawArrays;
	PFNGLENABLEPROC glEnable;
	PFNGLFLUSHPROC glFlush;
	PFNGLFINISHPROC glFinish;
	PFNGLFRONTFACEPROC glFrontFace;
	PFNGLGENTEXTURESPROC glGenTextures;
	PFNGLDELETETEXTURESPROC glDeleteTextures;
	PFNGLISENABLEDPROC glIsEnabled;
	PFNGLISTEXTUREPROC glIsTexture;
	PFNGLGETERRORPROC glGetError;
	PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
	PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
	PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv;
	PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
	PFNGLGETTEXIMAGEPROC glGetTexImage;
	PFNGLGETBOOLEANVPROC glGetBooleanv;
	PFNGLGETFLOATVPROC glGetFloatv;
	PFNGLGETDOUBLEVPROC glGetDoublev;
	PFNGLGETINTEGERVPROC glGetIntegerv;
	PFNGLGETPOINTERVPROC glGetPointerv;
	PFNGLGETSTRINGPROC glGetString;
	PFNGLHINTPROC glHint;
	PFNGLLOGICOPPROC glLogicOp;
	PFNGLPIXELSTOREIPROC glPixelStorei;
	PFNGLPIXELSTOREFPROC glPixelStoref;
	PFNGLPOLYGONMODEPROC glPolygonMode;
	PFNGLPOLYGONOFFSETPROC glPolygonOffset;
	PFNGLPOINTSIZEPROC glPointSize;
	PFNGLLINEWIDTHPROC glLineWidth;
	PFNGLREADPIXELSPROC glReadPixels;
	PFNGLREADBUFFERPROC glReadBuffer;
	PFNGLSCISSORPROC glScissor;
	PFNGLTEXIMAGE1DPROC glTexImage1D;
	PFNGLTEXIMAGE2DPROC glTexImage2D;
	PFNGLTEXIMAGE3DPROC glTexImage3D;
	PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D;
	PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
	PFNGLTEXPARAMETERFPROC glTexParameterf;
	PFNGLTEXPARAMETERFVPROC glTexParameterfv;
	PFNGLTEXPARAMETERIPROC glTexParameteri;
	PFNGLTEXPARAMETERIVPROC glTexParameteriv;
	PFNGLVIEWPORTPROC glViewport;
	// --

	// ++ glext
	PFNGLACTIVETEXTUREPROC glActiveTexture; // aliases glActiveTextureARB
	PFNGLTEXSTORAGE1DPROC glTexStorage1D;
	PFNGLTEXSTORAGE2DPROC glTexStorage2D;
	PFNGLTEXSTORAGE3DPROC glTexStorage3D;
	PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
	PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;
	PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
	PFNGLTEXBUFFERPROC glTexBuffer;
	PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
	PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
	PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
	PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
	PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
	PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
	PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
	PFNGLTEXBUFFERRANGEPROC glTexBufferRange;
	PFNGLTEXTUREVIEWPROC glTextureView;
	PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
	PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
	PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
	PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData;
	PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D;
	PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D;
	PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D;
	PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D;
	PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;
	PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;
	PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v;
	PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
	PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
	PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;
	PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex;
	PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
	PFNGLGETSTRINGIPROC glGetStringi;
	PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
	PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
	PFNGLGETFLOATI_VPROC glGetFloati_v;
	PFNGLGETDOUBLEI_VPROC glGetDoublei_v;
	PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
	PFNGLGETINTEGER64VPROC glGetInteger64v;
	PFNGLGETSHADERIVPROC glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
	PFNGLGETSHADERSOURCEPROC glGetShaderSource;
	PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
	PFNGLGETPROGRAMIVPROC glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
	PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
	PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv;
	PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
	PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv;
	PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;
	PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
	PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation;
	PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex;
	PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv;
	PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus; // aliases glGetGraphicsResetStatusARB
	PFNGLGETOBJECTLABELPROC glGetObjectLabel;
	PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel;
	PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog; // aliases glGetDebugMessageLogARB
	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
	PFNGLGETFRAMEBUFFERPARAMETERIVPROC glGetFramebufferParameteriv;
	PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
	PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
	PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv;
	PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
	PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
	PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
	PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
	PFNGLGETQUERYIVPROC glGetQueryiv;
	PFNGLGETSYNCIVPROC glGetSynciv;
	PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
	PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
	PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
	PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
	PFNGLGETNCOMPRESSEDTEXIMAGEPROC glGetnCompressedTexImage; // aliases glGetnCompressedTexImageARB
	PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glGetCompressedTextureImage;
	PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glGetCompressedTextureSubImage;
	PFNGLGETNTEXIMAGEPROC glGetnTexImage; // aliases glGetnTexImageARB
	PFNGLGETTEXTUREIMAGEPROC glGetTextureImage;
	PFNGLGETTEXTURELEVELPARAMETERFVPROC glGetTextureLevelParameterfv;
	PFNGLGETTEXTURELEVELPARAMETERIVPROC glGetTextureLevelParameteriv;
	PFNGLGETTEXTUREPARAMETERIIVPROC glGetTextureParameterIiv;
	PFNGLGETTEXTUREPARAMETERIUIVPROC glGetTextureParameterIuiv;
	PFNGLGETTEXTUREPARAMETERFVPROC glGetTextureParameterfv;
	PFNGLGETTEXTUREPARAMETERIVPROC glGetTextureParameteriv;
	PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
	PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
	PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
	PFNGLCLAMPCOLORPROC glClampColor; // aliases glClampColorARB
	PFNGLREADNPIXELSPROC glReadnPixels; // aliases glReadnPixelsARB
	PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
	PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
	PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
	PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
	PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
	PFNGLGETTRANSFORMFEEDBACKI64_VPROC glGetTransformFeedbacki64_v;
	PFNGLGETTRANSFORMFEEDBACKI_VPROC glGetTransformFeedbacki_v;
	PFNGLGETTRANSFORMFEEDBACKIVPROC glGetTransformFeedbackiv;
	PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex;
	PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation;
	PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv;
	PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName;
	PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
	PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
	PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv;
	PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
	PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
	PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
	PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
	PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
	PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
	PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
	PFNGLGETUNIFORMFVPROC glGetUniformfv;
	PFNGLGETUNIFORMIVPROC glGetUniformiv;
	PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
	PFNGLGETUNIFORMDVPROC glGetUniformdv;
	PFNGLGETNUNIFORMDVPROC glGetnUniformdv; // aliases glGetnUniformdvARB
	PFNGLGETNUNIFORMFVPROC glGetnUniformfv; // aliases glGetnUniformfvARB
	PFNGLGETNUNIFORMIVPROC glGetnUniformiv; // aliases glGetnUniformivARB
	PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv; // aliases glGetnUniformuivARB
	PFNGLGETVERTEXARRAYIVPROC glGetVertexArrayiv;
	PFNGLGETVERTEXARRAYINDEXED64IVPROC glGetVertexArrayIndexed64iv;
	PFNGLGETVERTEXARRAYINDEXEDIVPROC glGetVertexArrayIndexediv;
	PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
	PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
	PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv;
	PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
	PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
	PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v;
	PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv;
	PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv;
	PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData;
	PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv;
	PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv;
	PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glGetNamedRenderbufferParameteriv;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
	PFNGLBLENDCOLORPROC glBlendColor;
	PFNGLBLENDFUNCIPROC glBlendFunci; // aliases glBlendFunciARB
	PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate; // aliases glBlendFuncSeparateARB
	PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei; // aliases glBlendFuncSeparateiARB
	PFNGLBLENDEQUATIONPROC glBlendEquation;
	PFNGLBLENDEQUATIONIPROC glBlendEquationi; // aliases glBlendEquationiARB
	PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate; // aliases glBlendEquationSeparateARB
	PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei; // aliases glBlendEquationSeparateiARB
	PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
	PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
	PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
	PFNGLCOLORMASKIPROC glColorMaski;
	PFNGLSAMPLEMASKIPROC glSampleMaski;
	PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
	PFNGLMINSAMPLESHADINGPROC glMinSampleShading; // aliases glMinSampleShadingARB
	PFNGLDEPTHRANGEPROC glDepthRange;
	PFNGLDEPTHRANGEFPROC glDepthRangef;
	PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
	PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv;
	PFNGLDEPTHBOUNDSEXTPROC glDepthBoundsEXT;
	PFNGLCLIPCONTROLPROC glClipControl;
	PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
	PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLDETACHSHADERPROC glDetachShader;
	PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLSHADERBINARYPROC glShaderBinary;
	PFNGLPROGRAMBINARYPROC glProgramBinary;
	PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
	PFNGLVALIDATEPROGRAMPROC glValidateProgram;
	PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
	PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
	PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram;
	PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
	PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline;
	PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback; // aliases glDebugMessageCallbackARB
	PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl; // aliases glDebugMessageControlARB
	PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert; // aliases glDebugMessageInsertARB
	PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
	PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
	PFNGLOBJECTLABELPROC glObjectLabel;
	PFNGLOBJECTPTRLABELPROC glObjectPtrLabel;
	PFNGLENABLEIPROC glEnablei;
	PFNGLDISABLEIPROC glDisablei;
	PFNGLISENABLEDIPROC glIsEnabledi;
	PFNGLISBUFFERPROC glIsBuffer;
	PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
	PFNGLISPROGRAMPROC glIsProgram;
	PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline;
	PFNGLISQUERYPROC glIsQuery;
	PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
	PFNGLISSAMPLERPROC glIsSampler;
	PFNGLISSHADERPROC glIsShader;
	PFNGLISSYNCPROC glIsSync;
	PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback;
	PFNGLISVERTEXARRAYPROC glIsVertexArray;
	PFNGLGENBUFFERSPROC glGenBuffers; // aliases glGenBuffersARB
	PFNGLBINDBUFFERPROC glBindBuffer; // aliases glBindBufferARB
	PFNGLDRAWBUFFERSPROC glDrawBuffers;
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
	PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
	PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
	PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
	PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
	PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
	PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri;
	PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
	PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
	PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
	PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
	PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
	PFNGLFENCESYNCPROC glFenceSync;
	PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
	PFNGLWAITSYNCPROC glWaitSync;
	PFNGLDELETESYNCPROC glDeleteSync;
	PFNGLGENQUERIESPROC glGenQueries;
	PFNGLBEGINQUERYPROC glBeginQuery;
	PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed;
	PFNGLENDQUERYPROC glEndQuery;
	PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed;
	PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
	PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
	PFNGLQUERYCOUNTERPROC glQueryCounter;
	PFNGLDELETEQUERIESPROC glDeleteQueries;
	PFNGLBUFFERDATAPROC glBufferData; // aliases glBufferDataARB
	PFNGLBUFFERSTORAGEPROC glBufferStorage;
	PFNGLBUFFERSUBDATAPROC glBufferSubData;
	PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
	PFNGLBINDBUFFERBASEPROC glBindBufferBase;
	PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
	PFNGLBINDBUFFERSBASEPROC glBindBuffersBase;
	PFNGLBINDBUFFERSRANGEPROC glBindBuffersRange;
	PFNGLMAPBUFFERPROC glMapBuffer; // aliases glMapBufferARB
	PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
	PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
	PFNGLUNMAPBUFFERPROC glUnmapBuffer; // aliases glUnmapBufferARB
	PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
	PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks;
	PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
	PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback;
	PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
	PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback;
	PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback;
	PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
	PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback;
	PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced;
	PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
	PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers; // aliases glDeleteBuffersARB
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
	PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
	PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
	PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
	PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
	PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
	PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
	PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
	PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
	PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
	PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
	PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
	PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
	PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
	PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
	PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
	PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
	PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
	PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
	PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
	PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
	PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
	PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
	PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
	PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
	PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
	PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
	PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
	PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
	PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
	PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
	PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
	PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
	PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
	PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
	PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
	PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
	PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
	PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
	PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
	PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
	PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
	PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
	PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
	PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
	PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
	PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
	PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
	PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
	PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
	PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
	PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
	PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
	PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
	PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
	PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
	PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d;
	PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv;
	PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d;
	PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv;
	PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d;
	PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv;
	PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d;
	PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv;
	PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui;
	PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
	PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui;
	PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
	PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui;
	PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
	PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui;
	PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
	PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
	PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
	PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
	PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
	PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
	PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
	PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
	PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
	PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers;
	PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;
	PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
	PFNGLBINDIMAGETEXTURESPROC glBindImageTextures;
	PFNGLGENSAMPLERSPROC glGenSamplers;
	PFNGLBINDSAMPLERPROC glBindSampler;
	PFNGLBINDSAMPLERSPROC glBindSamplers;
	PFNGLBINDTEXTURESPROC glBindTextures;
	PFNGLDELETESAMPLERSPROC glDeleteSamplers;
	PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
	PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
	PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
	PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
	PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
	PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
	PFNGLPATCHPARAMETERIPROC glPatchParameteri;
	PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;
	PFNGLPOINTPARAMETERFPROC glPointParameterf; // aliases glPointParameterfARB
	PFNGLPOINTPARAMETERFVPROC glPointParameterfv; // aliases glPointParameterfvARB
	PFNGLPOINTPARAMETERIPROC glPointParameteri;
	PFNGLPOINTPARAMETERIVPROC glPointParameteriv;
	PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
	PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
	PFNGLMEMORYBARRIERPROC glMemoryBarrier;
	PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion;
	PFNGLTEXTUREBARRIERPROC glTextureBarrier;
	PFNGLCLEARDEPTHFPROC glClearDepthf;
	PFNGLCLEARBUFFERFVPROC glClearBufferfv;
	PFNGLCLEARBUFFERIVPROC glClearBufferiv;
	PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
	PFNGLCLEARBUFFERFIPROC glClearBufferfi;
	PFNGLCLEARBUFFERDATAPROC glClearBufferData;
	PFNGLCLEARBUFFERSUBDATAPROC glClearBufferSubData;
	PFNGLCLEARTEXIMAGEPROC glClearTexImage;
	PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage;
	PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData;
	PFNGLINVALIDATEBUFFERSUBDATAPROC glInvalidateBufferSubData;
	PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;
	PFNGLINVALIDATESUBFRAMEBUFFERPROC glInvalidateSubFramebuffer;
	PFNGLINVALIDATETEXIMAGEPROC glInvalidateTexImage;
	PFNGLINVALIDATETEXSUBIMAGEPROC glInvalidateTexSubImage;
	PFNGLSCISSORARRAYVPROC glScissorArrayv;
	PFNGLSCISSORINDEXEDPROC glScissorIndexed;
	PFNGLSCISSORINDEXEDVPROC glScissorIndexedv;
	PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf;
	PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
	PFNGLVIEWPORTARRAYVPROC glViewportArrayv;
	PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
	PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding;
	PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv;
	PFNGLUNIFORM1FPROC   glUniform1f;
	PFNGLUNIFORM1IPROC   glUniform1i;
	PFNGLUNIFORM1UIPROC  glUniform1ui;
	PFNGLUNIFORM1DPROC   glUniform1d;
	PFNGLUNIFORM2FPROC   glUniform2f;
	PFNGLUNIFORM2IPROC   glUniform2i;
	PFNGLUNIFORM2UIPROC  glUniform2ui;
	PFNGLUNIFORM2DPROC   glUniform2d;
	PFNGLUNIFORM3FPROC   glUniform3f;
	PFNGLUNIFORM3IPROC   glUniform3i;
	PFNGLUNIFORM3UIPROC  glUniform3ui;
	PFNGLUNIFORM3DPROC   glUniform3d;
	PFNGLUNIFORM4FPROC   glUniform4f;
	PFNGLUNIFORM4IPROC   glUniform4i;
	PFNGLUNIFORM4UIPROC  glUniform4ui;
	PFNGLUNIFORM4DPROC   glUniform4d;
	PFNGLUNIFORM1FVPROC  glUniform1fv;
	PFNGLUNIFORM1IVPROC  glUniform1iv;
	PFNGLUNIFORM1UIVPROC glUniform1uiv;
	PFNGLUNIFORM1DVPROC  glUniform1dv;
	PFNGLUNIFORM2FVPROC  glUniform2fv;
	PFNGLUNIFORM2IVPROC  glUniform2iv;
	PFNGLUNIFORM2UIVPROC glUniform2uiv;
	PFNGLUNIFORM2DVPROC  glUniform2dv;
	PFNGLUNIFORM3FVPROC  glUniform3fv;
	PFNGLUNIFORM3IVPROC  glUniform3iv;
	PFNGLUNIFORM3UIVPROC glUniform3uiv;
	PFNGLUNIFORM3DVPROC  glUniform3dv;
	PFNGLUNIFORM4FVPROC  glUniform4fv;
	PFNGLUNIFORM4IVPROC  glUniform4iv;
	PFNGLUNIFORM4UIVPROC glUniform4uiv;
	PFNGLUNIFORM4DVPROC  glUniform4dv;
	PFNGLUNIFORMMATRIX2FVPROC   glUniformMatrix2fv;
	PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
	PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
	PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
	PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
	PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
	PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;
	PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
	PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;
	PFNGLUNIFORMMATRIX2DVPROC   glUniformMatrix2dv;
	PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
	PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
	PFNGLUNIFORMMATRIX3DVPROC   glUniformMatrix3dv;
	PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
	PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
	PFNGLUNIFORMMATRIX4DVPROC   glUniformMatrix4dv;
	PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
	PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
	PFNGLPROGRAMUNIFORM1FPROC   glProgramUniform1f;
	PFNGLPROGRAMUNIFORM1IPROC   glProgramUniform1i;
	PFNGLPROGRAMUNIFORM1UIPROC  glProgramUniform1ui;
	PFNGLPROGRAMUNIFORM1DPROC   glProgramUniform1d;
	PFNGLPROGRAMUNIFORM2FPROC   glProgramUniform2f;
	PFNGLPROGRAMUNIFORM2IPROC   glProgramUniform2i;
	PFNGLPROGRAMUNIFORM2UIPROC  glProgramUniform2ui;
	PFNGLPROGRAMUNIFORM2DPROC   glProgramUniform2d;
	PFNGLPROGRAMUNIFORM3FPROC   glProgramUniform3f;
	PFNGLPROGRAMUNIFORM3IPROC   glProgramUniform3i;
	PFNGLPROGRAMUNIFORM3UIPROC  glProgramUniform3ui;
	PFNGLPROGRAMUNIFORM3DPROC   glProgramUniform3d;
	PFNGLPROGRAMUNIFORM4FPROC   glProgramUniform4f;
	PFNGLPROGRAMUNIFORM4IPROC   glProgramUniform4i;
	PFNGLPROGRAMUNIFORM4UIPROC  glProgramUniform4ui;
	PFNGLPROGRAMUNIFORM4DPROC   glProgramUniform4d;
	PFNGLPROGRAMUNIFORM1FVPROC  glProgramUniform1fv;
	PFNGLPROGRAMUNIFORM1IVPROC  glProgramUniform1iv;
	PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv;
	PFNGLPROGRAMUNIFORM1DVPROC  glProgramUniform1dv;
	PFNGLPROGRAMUNIFORM2FVPROC  glProgramUniform2fv;
	PFNGLPROGRAMUNIFORM2IVPROC  glProgramUniform2iv;
	PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv;
	PFNGLPROGRAMUNIFORM2DVPROC  glProgramUniform2dv;
	PFNGLPROGRAMUNIFORM3FVPROC  glProgramUniform3fv;
	PFNGLPROGRAMUNIFORM3IVPROC  glProgramUniform3iv;
	PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv;
	PFNGLPROGRAMUNIFORM3DVPROC  glProgramUniform3dv;
	PFNGLPROGRAMUNIFORM4FVPROC  glProgramUniform4fv;
	PFNGLPROGRAMUNIFORM4IVPROC  glProgramUniform4iv;
	PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv;
	PFNGLPROGRAMUNIFORM4DVPROC  glProgramUniform4dv;
	PFNGLPROGRAMUNIFORMMATRIX2FVPROC   glProgramUniformMatrix2fv;
	PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
	PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
	PFNGLPROGRAMUNIFORMMATRIX3FVPROC   glProgramUniformMatrix3fv;
	PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
	PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
	PFNGLPROGRAMUNIFORMMATRIX4FVPROC   glProgramUniformMatrix4fv;
	PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
	PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
	PFNGLPROGRAMUNIFORMMATRIX2DVPROC   glProgramUniformMatrix2dv;
	PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
	PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
	PFNGLPROGRAMUNIFORMMATRIX3DVPROC   glProgramUniformMatrix3dv;
	PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
	PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
	PFNGLPROGRAMUNIFORMMATRIX4DVPROC   glProgramUniformMatrix4dv;
	PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
	PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
	PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
	PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
	PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
	PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
	PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
	PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance;
	PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
	PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
	PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;
	PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
	PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
	PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
	PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
	PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;
	PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
	PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
	PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
	PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC glCheckNamedFramebufferStatusEXT;
	PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC glCompressedTextureImage1DEXT;
	PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC glCompressedTextureImage2DEXT;
	PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC glCompressedTextureImage3DEXT;
	PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC glCompressedTextureSubImage1DEXT;
	PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC glCompressedTextureSubImage2DEXT;
	PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC glCompressedTextureSubImage3DEXT;
	PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC glFramebufferDrawBuffersEXT;
	PFNGLGENERATETEXTUREMIPMAPEXTPROC glGenerateTextureMipmapEXT;
	PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC glGetCompressedTextureImageEXT;
	PFNGLGETNAMEDBUFFERSUBDATAEXTPROC glGetNamedBufferSubDataEXT;
	PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC glGetNamedBufferParameterivEXT;
	PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetNamedFramebufferAttachmentParameterivEXT;
	PFNGLGETTEXTUREIMAGEEXTPROC glGetTextureImageEXT;
	PFNGLGETTEXTUREPARAMETERIVEXTPROC glGetTextureParameterivEXT;
	PFNGLGETTEXTUREPARAMETERFVEXTPROC glGetTextureParameterfvEXT;
	PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC glGetTextureLevelParameterivEXT;
	PFNGLMAPNAMEDBUFFEREXTPROC glMapNamedBufferEXT;
	PFNGLMAPNAMEDBUFFERRANGEEXTPROC glMapNamedBufferRangeEXT;
	PFNGLNAMEDBUFFERDATAEXTPROC glNamedBufferDataEXT;
	PFNGLNAMEDBUFFERSTORAGEEXTPROC glNamedBufferStorageEXT;
	PFNGLNAMEDBUFFERSUBDATAEXTPROC glNamedBufferSubDataEXT;
	PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC glNamedCopyBufferSubDataEXT;
	PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC glNamedFramebufferTextureEXT;
	PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC glNamedFramebufferTexture1DEXT;
	PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC glNamedFramebufferTexture2DEXT;
	PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC glNamedFramebufferTexture3DEXT;
	PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC glNamedFramebufferRenderbufferEXT;
	PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC glNamedFramebufferTextureLayerEXT;
	PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC glNamedFramebufferParameteriEXT;
	PFNGLTEXTUREBUFFEREXTPROC glTextureBufferEXT;
	PFNGLTEXTUREBUFFERRANGEEXTPROC glTextureBufferRangeEXT;
	PFNGLTEXTUREIMAGE1DEXTPROC glTextureImage1DEXT;
	PFNGLTEXTUREIMAGE2DEXTPROC glTextureImage2DEXT;
	PFNGLTEXTUREIMAGE3DEXTPROC glTextureImage3DEXT;
	PFNGLTEXTUREPARAMETERFEXTPROC glTextureParameterfEXT;
	PFNGLTEXTUREPARAMETERFVEXTPROC glTextureParameterfvEXT;
	PFNGLTEXTUREPARAMETERIEXTPROC glTextureParameteriEXT;
	PFNGLTEXTUREPARAMETERIVEXTPROC glTextureParameterivEXT;
	PFNGLTEXTUREPARAMETERIIVEXTPROC glTextureParameterIivEXT;
	PFNGLTEXTUREPARAMETERIUIVEXTPROC glTextureParameterIuivEXT;
	PFNGLTEXTURESTORAGE1DEXTPROC glTextureStorage1DEXT;
	PFNGLTEXTURESTORAGE2DEXTPROC glTextureStorage2DEXT;
	PFNGLTEXTURESTORAGE3DEXTPROC glTextureStorage3DEXT;
	PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC glTextureStorage2DMultisampleEXT;
	PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC glTextureStorage3DMultisampleEXT;
	PFNGLTEXTURESUBIMAGE1DEXTPROC glTextureSubImage1DEXT;
	PFNGLTEXTURESUBIMAGE2DEXTPROC glTextureSubImage2DEXT;
	PFNGLTEXTURESUBIMAGE3DEXTPROC glTextureSubImage3DEXT;
	PFNGLUNMAPNAMEDBUFFEREXTPROC glUnmapNamedBufferEXT;
	// --
};

#include "gl_hookset_defs.h"
