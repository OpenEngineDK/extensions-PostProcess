// Post process node.
// -------------------------------------------------------------------
// Copyright (C) 2010 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Scene/PostProcessNode.h>
#include <Resources/IShaderResource.h>
#include <Display/Viewport.h>
#include <Meta/OpenGL.h>
#include <Logging/Logger.h>
#include <Resources/FrameBuffer.h>
#include <Utils/Convert.h>

using namespace std;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;

namespace OpenEngine {
    namespace Scene {

        PostProcessNode::PostProcessNode(){
            effect = IShaderResourcePtr();
            fbo = NULL;            
        }

        PostProcessNode::PostProcessNode(IShaderResourcePtr effect)
            : effect(effect) {
        }

        void PostProcessNode::Handle(Renderers::RenderingEventArg arg){
            effect->Load();

            bool useDepthBuffer = effect->GetUniformID("depth") >= 0;

            unsigned int colorBuffers = 0;
            string name = "color"  + Convert::ToString<unsigned int>(colorBuffers);
            while (effect->GetUniformID(name) >= 0){
                ++colorBuffers;
                name = "color"  + Convert::ToString<unsigned int>(colorBuffers);
            }

            fbo = new FrameBuffer(arg.renderer.GetViewport(), colorBuffers, useDepthBuffer);

            // Setup framebuffer
            GLuint fboID;
            glGenFramebuffersEXT(1, &fboID);
            CHECK_FOR_GL_ERROR();
            fbo->SetID(fboID);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
            CHECK_FOR_GL_ERROR();
            
            if (fbo->GetDepthTexture() != NULL){
                arg.renderer.LoadTexture(fbo->GetDepthTexture());
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                          GL_DEPTH_ATTACHMENT_EXT,
                                          GL_TEXTURE_2D, fbo->GetDepthTexture()->GetID(), 0);
            }else{
                Vector<4, int> viewDim = fbo->GetDimension();
                unsigned int width = viewDim[2] - viewDim[0];
                unsigned int height = viewDim[3] - viewDim[1];

                GLuint depth;
                glGenRenderbuffersEXT(1, &depth);
                glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth);
                glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
                glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
                
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                             GL_RENDERBUFFER_EXT, depth);
            }

            for (unsigned int i = 0; i < fbo->GetNumberOfAttachments(); ++i){
                ITexture2DPtr tex = fbo->GetTexAttachement(i);
                arg.renderer.LoadTexture(tex.get());
                
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                          GL_COLOR_ATTACHMENT0_EXT,
                                          GL_TEXTURE_2D, tex->GetID(), 0);
                CHECK_FOR_GL_ERROR();
            }

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

            if (useDepthBuffer)
                effect->SetTexture("depth", fbo->GetDepthTexture());
            for (unsigned int i = 0; i < fbo->GetNumberOfAttachments(); ++i){
                effect->SetTexture("color" + Convert::ToString<unsigned int>(i), fbo->GetTexAttachement(i));
            }
        }

    }
}
