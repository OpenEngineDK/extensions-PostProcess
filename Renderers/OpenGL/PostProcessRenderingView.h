// Post process rendering view.
// -------------------------------------------------------------------
// Copyright (C) 2010 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _POST_PROCESS_RENDERING_VIEW_H_
#define _POST_PROCESS_RENDERING_VIEW_H_

#include <Renderers/OpenGL/RenderingView.h>

namespace OpenEngine {
namespace Renderers {
namespace OpenGL {

using namespace OpenEngine::Renderers;
    
 class PostProcessRenderingView : public RenderingView {

 public:
     PostProcessRenderingView(Viewport& viewport);
     
     void VisitPostProcessNode(PostProcessNode* node);
 };

}
}
}

#endif
