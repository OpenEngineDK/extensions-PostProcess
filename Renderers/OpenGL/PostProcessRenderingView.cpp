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
                    mergeShader = glCreateProgram();
                    
                    // Create and compile the vertex program
                    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
                    const GLchar** vertexSource = new const GLchar*[1];
                    vertexSource[0] = "varying vec2 texCoord;void main(void){texCoord = (gl_Vertex.xy + 1.0) * 0.5;gl_Position = gl_Vertex;}";
                    glShaderSource(vertexID, 1, vertexSource, NULL);
                    glCompileShader(vertexID);
                    glAttachShader(copyShader, vertexID);
                    glAttachShader(mergeShader, vertexID);
                    
                    // Create and compile the copy fragment program
                    GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);
                    const GLchar** fragSource = new const GLchar*[1];
                    fragSource[0] = "uniform sampler2D src;varying vec2 texCoord;void main(void){gl_FragColor = texture2D(src, texCoord);}";
                    //fragSource[0] = "uniform sampler2D src;varying vec2 texCoord;void main(void){gl_FragColor = vec4(1.0,0.0,1.0,1.0);}";
                    glShaderSource(fragID, 1, fragSource, NULL);
                    glCompileShader(fragID);
                    glAttachShader(copyShader, fragID);

                    glLinkProgram(copyShader);

                    glUseProgram(copyShader);
                    GLuint loc = glGetUniformLocation(copyShader, "src");
                    glUniform1i(loc, 1);
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
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->GetCurrentFrameBuffer()->GetID());
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_FOR_GL_ERROR();
                
                node->VisitSubNodes(*this);

                node->PreEffect(arg->renderer, modelViewMatrix[mvIndex]);

                // @TODO should the not initialized framebuffers be
                // initialized with the data from the current framebuffer?
                /*
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->effectFb->GetID());
                CHECK_FOR_GL_ERROR();
                
                node->GetEffect()->ApplyShader();
                glBindTexture(GL_TEXTURE_2D, node->GetTexture(0)->GetID());
                glRecti(-1,-1,1,1);
                node->GetEffect()->ReleaseShader();
                */

                // Reset to the previous framebuffer and render
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFbo);
                glViewport(prevDims[0], prevDims[1], prevDims[2], prevDims[3]);
                CHECK_FOR_GL_ERROR();
                
                node->GetEffect()->ApplyShader();
                glBindTexture(GL_TEXTURE_2D, node->GetTexture(0)->GetID());
                glRecti(-1,-1,1,1);
                node->GetEffect()->ReleaseShader();

                /*
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, node->effectFb->GetTexAttachement(0)->GetID());

                glDisable(GL_DEPTH_TEST);
                
                glBegin(GL_QUADS);

                glTexCoord2f(0,0);
                glVertex3f(0,0,0);
                
                glTexCoord2f(1,0);
                glVertex3f(40,0,0);
                
                glTexCoord2f(1,1);
                glVertex3f(40,30,0);
                
                glTexCoord2f(0,1);
                glVertex3f(0,30,0);

                glEnd();

                glEnable(GL_DEPTH_TEST);
                */

                // @TODO
                // Copy the final image to the final textures
                
                // Draw the picture onto the original framebuffer.
                /*
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFbo);
                glViewport(prevDims[0], prevDims[1], prevDims[2], prevDims[3]);
                CHECK_FOR_GL_ERROR();

                glUseProgram(copyShader);
                glBindTexture(GL_TEXTURE_2D, node->effectFb->GetTexAttachement(0)->GetID());
                glRecti(-1,-1,1,1);
                glUseProgram(0);
                CHECK_FOR_GL_ERROR();
                */

                glBindTexture(GL_TEXTURE_2D, 0);

                // @TODO instead of switching framebuffers, then
                // switch the texture ids in the framebuffers.
                node->NextFrameBuffer();

            }
            
        }
    }
}
