// Post process node.
// -------------------------------------------------------------------
// Copyright (C) 2010 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _POST_PROCESS_NODE_H_
#define _POST_PROCESS_NODE_H_

#include <Scene/ISceneNode.h>
#include <Core/IListener.h>
#include <Renderers/IRenderer.h>
#include <Resources/Texture2D.h>
#include <Resources/FrameBuffer.h>
#include <Math/Vector.h>
#include <vector>

namespace OpenEngine {
    namespace Resources {
        class IShaderResource;
        typedef boost::shared_ptr<IShaderResource> IShaderResourcePtr;
    }
    
    namespace Scene {
        
        class PostProcessNode : public Scene::ISceneNode,
                                public Core::IListener<Renderers::RenderingEventArg> {
            OE_SCENE_NODE(PostProcessNode, ISceneNode)

        protected:
            // FBO attributes
            Resources::FrameBuffer* fbo;

            // Shader for the effect
            Resources::IShaderResourcePtr effect;

            bool merge, blend;
            
        public:
            PostProcessNode();
            PostProcessNode(Resources::IShaderResourcePtr effect);

            void Handle(Renderers::RenderingEventArg arg);

            inline unsigned int GetFboID() const { return fbo->GetID(); }
            inline Math::Vector<4, int> GetDimension() const { return fbo->GetDimension(); }
            inline Resources::ITexture2DPtr GetTexture() { return fbo->GetTexAttachement(0); }
            inline Resources::IShaderResourcePtr GetEffect() { return effect; }

        };

    }

}

#endif
