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
                // Setup the new framebuffer
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
                IViewingVolume* prevViewingVolume = renderer->GetViewport().GetViewingVolume();

                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->GetFboID());
                CHECK_FOR_GL_ERROR();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_FOR_GL_ERROR();
                
                node->VisitSubNodes(*this);

                // Reset to the previous framebuffer
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFbo);
                CHECK_FOR_GL_ERROR();
                glViewport(prevDims[0], prevDims[1], prevDims[2], prevDims[3]);
                CHECK_FOR_GL_ERROR();

                //node->VisitSubNodes(*this);


                OrthogonalViewingVolume* ot = new OrthogonalViewingVolume(-1, 1, prevDims[0], prevDims[2], prevDims[1], prevDims[3]);
                renderer->ApplyViewingVolume(*ot);
                delete ot;


                node->GetEffect()->ApplyShader();

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, node->GetTexture()->GetID());

                glBegin(GL_QUADS);
                
                glTexCoord2f(0,1);
                glVertex3f(prevDims[0],prevDims[1],0);
                
                glTexCoord2f(0,0);
                glVertex3f(prevDims[0],prevDims[3],0);
                
                glTexCoord2f(1,0);
                glVertex3f(prevDims[2],prevDims[3],0);
                
                glTexCoord2f(1,1);
                glVertex3f(prevDims[2],prevDims[1],0);
                
                glEnd();

                glBindTexture(GL_TEXTURE_2D, 0);
                
                node->GetEffect()->ReleaseShader();

                renderer->ApplyViewingVolume(*prevViewingVolume);
                
            }
            
        }
    }
}
