#include "Framework.h"
#include "Engine/Renderer/Base.h"

#include "Engine/Application.h"

namespace Silent::Renderer
{
    void RendererBase::SignalResize()
    {
        _isResized = true;
    }

    void RendererBase::SubmitDebugGui(std::function<void()> drawFunc)
    {
        if constexpr (IS_DEBUG)
        {
            _debugGuiDrawCalls.push_back(drawFunc);
        }
    }

    void RendererBase::SubmitDebugLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }
    
            auto line = Line
            {
                .From  = from,
                .To    = to,
                .Color = Vector4(color.ToGlmVec4())
            };
            _debugLines.push_back(line);
        }
    }
}
