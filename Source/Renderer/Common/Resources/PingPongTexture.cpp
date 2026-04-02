#include "Framework.h"
#include "Renderer/Common/Resources/PingPongTexture.h"

namespace Silent::Renderer
{
    void PingPongTextureBase::Swap()
    {
        _writeIdx = 1 - _writeIdx;
    }
}
