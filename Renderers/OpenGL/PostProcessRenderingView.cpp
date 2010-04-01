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

                // Save the previous state
                GLint prevFbo;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &prevFbo);
                GLint prevDims[4];
                glGetIntegerv(GL_VIEWPORT, prevDims);

                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, node->GetFboID());
                
                node->VisitSubNodes(*this);

                // Reset to the previous framebuffer
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFbo);
                glViewport(prevDims[0], prevDims[1], prevDims[2], prevDims[3]);

                

                /*
                glBegin(GL_QUADS);
                
                glColor3f(1,1,0);
                glTexCoord2f(0,0);
                glVertex3f(0,0,0);
                
                glColor3f(0,1,0);
                glTexCoord2f(0,1);
                glVertex3f(0,height,0);
                
                glColor3f(0,0,1);
                glTexCoord2f(1,1);
                glVertex3f(width,height,0);
                
                glColor3f(1,0,0);
                glTexCoord2f(1,0);
                glVertex3f(width,0,0);
                
                glEnd();
                */
                
            }
            
        }
    }
}
