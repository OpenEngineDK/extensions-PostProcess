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
#include <Logging/Logger.h>
#include <Utils/Convert.h>

using namespace std;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;

namespace OpenEngine {
    namespace Scene {

        PostProcessNode::PostProcessNode(){
            effect = IShaderResourcePtr();
            fbos.clear();
            currentFbo = 0;
            dimensions = Vector<4, int>(0);
            time = 0;
            enabled = false;
        }

        PostProcessNode::PostProcessNode(Vector<4, int> dims, 
                                         IShaderResourcePtr effect, 
                                         bool useDepth, 
                                         unsigned int colorBuffers,
                                         unsigned int framebuffers)
            : effect(effect), 
              currentFbo(0),
              dimensions(dims), 
              useDepthTexture(useDepth), 
              colorBuffers(colorBuffers),
              time(0),
              enabled(true) {
            for (unsigned int i = 0; i < framebuffers; ++i)
                fbos.push_back(new FrameBuffer(dims, colorBuffers, useDepthTexture));
        }

        PostProcessNode::~PostProcessNode(){
            for (unsigned int i = 0; i < fbos.size(); ++i)
                delete fbos[i];
        }

        void PostProcessNode::Handle(Renderers::RenderingEventArg arg){
            switch(arg.renderer.GetCurrentStage()){
            case Renderers::IRenderer::RENDERER_INITIALIZE:
                {
                    effect->Load();
                    
                    for (unsigned int i = 0; i < fbos.size(); ++i)
                        arg.renderer.BindFrameBuffer(fbos[i]);
                    
                    // Setup shader texture uniforms
                    for (unsigned int i = 0; i < fbos.size(); ++i){
                        FrameBuffer* fbo = fbos[i];

                        ITexture2DPtr depthTex = fbo->GetDepthTexture();
                        // check for shorthand form for the first fbo.
                        if (i == 0){
                            if (effect->GetUniformID("depth") >= 0 && depthTex != NULL)
                                effect->SetTexture("depth", depthTex);
                            
                            for (unsigned int j = 0; j < fbo->GetNumberOfAttachments(); ++j){
                                string colorid = "color" + Utils::Convert::ToString<unsigned int>(j);
                                if (effect->GetUniformID(colorid) >= 0)
                                    effect->SetTexture(colorid, fbo->GetTexAttachement(j));
                            }
                        }
                        
                        string si = Utils::Convert::ToString<unsigned int>(i);

                        if (effect->GetUniformID("fb" + si + "depth") >= 0 && depthTex != NULL)
                            effect->SetTexture("fb" + si + "depth", depthTex);
                        
                        for (unsigned int j = 0; j < fbo->GetNumberOfAttachments(); ++j){
                            string colorid = "fb" + si + "color" + Utils::Convert::ToString<unsigned int>(j);
                            if (effect->GetUniformID(colorid) >= 0)
                                effect->SetTexture(colorid, fbo->GetTexAttachement(j));
                        }
                    }
                
                    if (effect->GetUniformID("time") >= 0){
                        effect->SetUniform("time", (float)time);
                        arg.renderer.ProcessEvent().Attach(*this);
                    }
                    
                    // Initialize post process specific code
                    Initialize(arg);

                    break;
                }
            case Renderers::IRenderer::RENDERER_PROCESS:
                time += arg.approx / 1000.0f;
                effect->SetUniform("time", (float)time);
                break;
            default:
                ;
            }
        }

        void PostProcessNode::Initialize(RenderingEventArg arg){

        }

        void PostProcessNode::PreEffect(IRenderer& renderer, Matrix<4,4,float> modelview){

        }


    }
}
