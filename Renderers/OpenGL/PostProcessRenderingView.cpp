// Post process renderingview
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "PostProcessRenderingView.h"

#include <Scene/PostProcessNode.h>
#include <Display/OrthogonalViewingVolume.h>
#include <Resources/IShaderResource.h>
#include <Logging/Logger.h>

using namespace OpenEngine::Display;
using namespace OpenEngine::Resources;

namespace OpenEngine {
    namespace Renderers {
        namespace OpenGL {
            
            using namespace OpenEngine::Scene;
            
            PostProcessRenderingView::PostProcessRenderingView()
                : RenderingView() {}

            void PostProcessRenderingView::Handle(RenderingEventArg arg){
                if (arg.renderer.GetCurrentStage() == IRenderer::RENDERER_INITIALIZE){
                    logger.info << "init renderingview" << logger.end;
                    
                    copyShader = glCreateProgram();
                    
                    // Create and compile the vertex program
                    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
                    const GLchar** vertexSource = new const GLchar*[1];
                    vertexSource[0] = "varying vec2 texCoord;void main(void){texCoord = (gl_Vertex.xy + 1.0) * 0.5;gl_Position = gl_Vertex;}";
                    glShaderSource(vertexID, 1, vertexSource, NULL);
                    glCompileShader(vertexID);
                    glAttachShader(copyShader, vertexID);
                    
                    // Create and compile the copy fragment program
                    GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);
                    const GLchar** fragSource = new const GLchar*[1];
                    fragSource[0] = "uniform sampler2D src;varying vec2 texCoord;void main(void){gl_FragColor = texture2D(src, texCoord);}";
                    glShaderSource(fragID, 1, fragSource, NULL);
                    glCompileShader(fragID);
                    glAttachShader(copyShader, fragID);

                    glLinkProgram(copyShader);

                    glUseProgram(copyShader);
                    GLuint loc = glGetUniformLocation(copyShader, "src");
                    glUniform1i(loc, 0);
                    glUseProgram(0);
                }
                RenderingView::Handle(arg);
            }

            void PostProcessRenderingView::VisitPostProcessNode(PostProcessNode* node) {
                // if the node isn't enabled or there is no fbo
                // support then just proceed as usual.
                if (arg->renderer.FrameBufferSupport() == false ||
                    node->enabled == false) {
                    node->VisitSubNodes(*this);
                    return;
                }
                
                // Save the previous state
                GLint prevFbo;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &prevFbo);
                Vector<4, GLint> prevDims;
                glGetIntegerv(GL_VIEWPORT, prevDims.ToArray());
                CHECK_FOR_GL_ERROR();

                // Setup the new framebuffer
                Vector<2, int> dims = node->GetDimension();
                glViewport(0, 0, dims[0], dims[1]);
                CHECK_FOR_GL_ERROR();

                // Use the new framebuffer
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->GetSceneFrameBuffer()->GetID());
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_FOR_GL_ERROR();
                
                node->VisitSubNodes(*this);

                node->PreEffect(arg->renderer, modelViewMatrix[mvIndex]);

                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->GetEffectFrameBuffer()->GetID());
                glDisable(GL_DEPTH_TEST);
                CHECK_FOR_GL_ERROR();
                
                node->GetEffect()->ApplyShader();
                glRecti(-1,-1,1,1);
                node->GetEffect()->ReleaseShader();

                glEnable(GL_DEPTH_TEST);

                // Copy the final image to the final textures
                if (node->GetFinalTexs().size() != 0){
                    if (node->GetFinalTexs()[0]->GetID() == 0){
                        // Initialize the final texs and setup the
                        // effect shader to use them.
                        for (unsigned int i = 0; i < node->GetFinalTexs().size(); ++i){
                            ITexture2DPtr tex = node->GetFinalTexs()[i];
                            arg->renderer.LoadTexture(tex);
                            string colorid = "finalColor" + Utils::Convert::ToString<unsigned int>(i);
                            if (node->GetEffect()->GetUniformID(colorid) >= 0)
                                node->GetEffect()->SetTexture(colorid, tex);
                            CHECK_FOR_GL_ERROR();
                        }
                    }
                    for (unsigned int i = 0; i < node->GetFinalTexs().size(); ++i){
                        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
                        glBindTexture(GL_TEXTURE_2D, node->GetFinalTexs()[i]->GetID());
                        GLsizei width = node->GetFinalTexs()[i]->GetWidth();
                        GLsizei height = node->GetFinalTexs()[i]->GetHeight();
                        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
                    }
                    glReadBuffer(GL_COLOR_ATTACHMENT0);
                    CHECK_FOR_GL_ERROR();
                }

                // copy the picture onto the original framebuffer.
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFbo);
                glViewport(prevDims[0], prevDims[1], prevDims[2], prevDims[3]);
                CHECK_FOR_GL_ERROR();

                if (!(node->offscreenRendering)){
                    glUseProgram(copyShader);
                    glBindTexture(GL_TEXTURE_2D, node->GetEffectFrameBuffer()->GetTexAttachment(0)->GetID());
                    glRecti(-1,-1,1,1);
                    CHECK_FOR_GL_ERROR();
                }
                glUseProgram(0);

                glBindTexture(GL_TEXTURE_2D, 0);
            }
            
        }
    }
}
