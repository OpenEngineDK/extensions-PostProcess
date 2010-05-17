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
            fbo = NULL;
            dimensions = Vector<2, int>(0);
            time = 0;
            enabled = false;
            effectFb = NULL;
            finalTexs.clear();
        }

        PostProcessNode::PostProcessNode(IShaderResourcePtr effect, 
                                         Vector<2, int> dims, 
                                         unsigned int colorBuffers,
                                         bool useDepth)
            : effect(effect), 
              dimensions(dims), 
              time(0),
              enabled(true) {
            fbo = new FrameBuffer(dims, colorBuffers, useDepth);
            
            effectFb = new FrameBuffer(dims, colorBuffers, useDepth);

            finalTexs.clear();
            for (unsigned int i = 0; i < effectFb->GetNumberOfAttachments(); ++i)
                finalTexs.push_back(ITexture2DPtr(effectFb->GetTexAttachment(i)->Clone()));
        }

        PostProcessNode::PostProcessNode(Resources::IShaderResourcePtr effect, 
                                         Resources::FrameBuffer* prototype)
            : effect(effect),
              dimensions(prototype->GetDimension()),
              time(0),
              enabled(true){
            fbo = prototype->Clone();

            effectFb = prototype->Clone();

            finalTexs.clear();
            for (unsigned int i = 0; i < effectFb->GetNumberOfAttachments(); ++i)
                finalTexs.push_back(ITexture2DPtr(effectFb->GetTexAttachment(i)->Clone()));
        }


        PostProcessNode::~PostProcessNode(){
            delete fbo;
            delete effectFb;
        }

        void PostProcessNode::Handle(Renderers::RenderingEventArg arg){
            switch(arg.renderer.GetCurrentStage()){
            case Renderers::IRenderer::RENDERER_INITIALIZE:
                {
                    arg.renderer.BindFrameBuffer(effectFb);
                    arg.renderer.BindFrameBuffer(fbo);
                    
                    effect->Load();
                    // Setup shader texture uniforms
                    //for (unsigned int i = 0; i < fbos.size(); ++i){
                    for (unsigned int i = 0; i < 1; ++i){
                        //FrameBuffer* fbo = fbos[i];

                        ITexture2DPtr depthTex = fbo->GetDepthTexture();
                        // check for shorthand form for the first fbo.
                        if (i == 0){
                            if (effect->GetUniformID("depth") >= 0 && depthTex != NULL)
                                effect->SetTexture("depth", depthTex);
                            
                            for (unsigned int j = 0; j < fbo->GetNumberOfAttachments(); ++j){
                                string colorid = "color" + Utils::Convert::ToString<unsigned int>(j);
                                if (effect->GetUniformID(colorid) >= 0)
                                    effect->SetTexture(colorid, fbo->GetTexAttachment(j));
                            }
                        }
                        
                        string si = Utils::Convert::ToString<unsigned int>(i);

                        if (effect->GetUniformID("fb" + si + "depth") >= 0 && depthTex != NULL)
                            effect->SetTexture("fb" + si + "depth", depthTex);
                        
                        for (unsigned int j = 0; j < fbo->GetNumberOfAttachments(); ++j){
                            string colorid = "fb" + si + "color" + Utils::Convert::ToString<unsigned int>(j);
                            if (effect->GetUniformID(colorid) >= 0)
                                effect->SetTexture(colorid, fbo->GetTexAttachment(j));
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
