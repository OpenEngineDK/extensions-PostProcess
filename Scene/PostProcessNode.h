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
#include <Math/Matrix.h>
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
            // Shader for the effect
            Resources::IShaderResourcePtr effect;

            // FBO attributes
            vector<Resources::FrameBuffer*> fbos;
            unsigned int currentFbo;
            Vector<4, int> dimensions;
            bool useDepthTexture;
            unsigned int colorBuffers;

            bool merge;

            unsigned int time;

        public:
            // Public variables
            bool enabled;
            
        public:
            PostProcessNode();
            PostProcessNode(Math::Vector<4, int> dims, 
                            Resources::IShaderResourcePtr effect, 
                            bool useDepth = true, 
                            unsigned int colorBuffers = 1,
                            unsigned int framebuffers = 1);
            ~PostProcessNode();

            void Handle(Renderers::RenderingEventArg arg);

            /**
             * Initialize effect specific settings, such as more
             * framebuffers or other variables.
             */
            virtual void Initialize(Renderers::RenderingEventArg arg);

            /**
             * Handles any logic that needs to be done before the
             * effect, such as setting viewingvolume dependent
             * uniforms or updating time uniforms.
             */
            virtual void PreEffect(Renderers::IRenderer& renderer, Math::Matrix<4,4,float> modelview);

            inline void NextFrameBuffer() { currentFbo++; currentFbo %= fbos.size(); }

            inline unsigned int CurrentFrameBuffer() const { return currentFbo; }
            inline Resources::FrameBuffer* GetCurrentFrameBuffer() const { return fbos[currentFbo]; }
            inline vector<Resources::FrameBuffer*> GetFrameBuffers() const { return fbos; }
            inline Math::Vector<4, int> GetDimension() const { return dimensions; }
            inline Resources::ITexture2DPtr GetTexture(unsigned int buffer, unsigned int framebuffer = 0) { return fbos[framebuffer]->GetTexAttachement(buffer); }
            inline Resources::IShaderResourcePtr GetEffect() { return effect; }

        };

    }

}

#endif
