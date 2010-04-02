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
#include <Resources/IShaderResource.h>

using namespace std;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;

namespace OpenEngine {
    namespace Scene {

        PostProcessNode::PostProcessNode(){
            effect = IShaderResourcePtr();
            useDepthBuffer = false;
        }

        PostProcessNode::PostProcessNode(IShaderResourcePtr effect, bool useDepthBuffer)
            : effect(effect), useDepthBuffer(useDepthBuffer) {
            //texs = list<ITexture2DPtr>(colorbuffers);
        }

        void PostProcessNode::Handle(Renderers::RenderingEventArg arg){
            viewDim = arg.renderer.GetViewport().GetDimension();

            width = viewDim[2] - viewDim[0];
            height = viewDim[3] - viewDim[1];
            
            // Setup framebuffer
            glGenFramebuffersEXT(1, &fboID);
            CHECK_FOR_GL_ERROR();
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
            CHECK_FOR_GL_ERROR();

            // attach the depth buffer to the frame buffer
            if (useDepthBuffer){
                depthTex = ITexture2DPtr(new Texture2D<unsigned char>(width, height, 1));
                depthTex->SetColorFormat(DEPTH);
                depthTex->SetMipmapping(false);
                depthTex->SetCompression(false);
                depthTex->SetWrapping(CLAMP_TO_EDGE);
                arg.renderer.LoadTexture(depthTex.get());
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                          GL_DEPTH_ATTACHMENT_EXT,
                                          GL_TEXTURE_2D, depthTex->GetID(), 0);
            }else{
                GLuint depth;
                glGenRenderbuffersEXT(1, &depth);
                glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth);
                glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
                glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
                
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                             GL_RENDERBUFFER_EXT, depth);
            }
            CHECK_FOR_GL_ERROR();

            tex = ITexture2DPtr(new Texture2D<unsigned char>(width, height, 3));
            tex->SetColorFormat(RGB);
            tex->SetMipmapping(false);
            tex->SetCompression(false);
            tex->SetWrapping(CLAMP_TO_EDGE);
            arg.renderer.LoadTexture(tex.get());
            

            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                      GL_COLOR_ATTACHMENT0_EXT,
                                      GL_TEXTURE_2D, tex->GetID(), 0);
            CHECK_FOR_GL_ERROR();

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

            effect->Load();
            effect->SetTexture("color1", tex);
            if (useDepthBuffer)
                effect->SetTexture("depth", tex);
        }

    }
}
