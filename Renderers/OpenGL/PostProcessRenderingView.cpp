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
            
            PostProcessRenderingView::PostProcessRenderingView(Viewport& viewport) : 
                IRenderingView(viewport), 
                RenderingView(viewport) {}

            void PostProcessRenderingView::VisitPostProcessNode(PostProcessNode* node) {
                // if the node isn't enabled then just proceed as
                // usual.
                if (node->enabled == false) {
                    node->VisitSubNodes(*this);
                    return;
                }
                
                // Setup the new framebuffer
                // @TODO update the projection matrix aswell in case
                // the dimension has changed from the previous.
                Vector<4, int> dims = node->GetDimension();
                glViewport(dims[0], dims[1], dims[2], dims[3]);
                CHECK_FOR_GL_ERROR();

                // Save the previous state
                GLint prevFbo;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &prevFbo);
                CHECK_FOR_GL_ERROR();
                Vector<4, GLint> prevDims;
                glGetIntegerv(GL_VIEWPORT, prevDims.ToArray());
                CHECK_FOR_GL_ERROR();

                // Use the new framebuffer
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->GetCurrentFrameBuffer()->GetID());
                CHECK_FOR_GL_ERROR();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_FOR_GL_ERROR();
                
                node->VisitSubNodes(*this);

                node->PreEffect(*renderer, modelViewMatrix[mvIndex]);

                // Reset to the previous framebuffer
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFbo);
                glViewport(prevDims[0], prevDims[1], prevDims[2], prevDims[3]);
                CHECK_FOR_GL_ERROR();

                if (currentTexture == 0)
                    glEnable(GL_TEXTURE_2D);

                node->GetEffect()->ApplyShader();
                
                glBindTexture(GL_TEXTURE_2D, node->GetTexture(0)->GetID());
                
                glRecti(-1,-1,1,1);
                
                node->GetEffect()->ReleaseShader();
                
                glBindTexture(GL_TEXTURE_2D, 0);

                if (currentTexture == 0)
                    glDisable(GL_TEXTURE_2D);

                // @todo should the not initialized framebuffers be
                // initialized with the data from the current framebuffer?

                node->NextFrameBuffer();

            }
            
        }
    }
}
