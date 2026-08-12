#include "Framework.h"
#include "Input/AnalogAxis.h"

namespace Silent::Input
{
    const std::vector<std::vector<AnalogAxisId>> ANALOG_AXIS_ID_GROUPS =
    {
        /** Gameplay */
        {
            AnalogAxisId::Move,
            AnalogAxisId::Camera
        },
        /** Raw */
        {
            AnalogAxisId::Mouse,
            AnalogAxisId::StickLeft,
            AnalogAxisId::StickRight
        },
        /** Recordable */
        {
            AnalogAxisId::Move,
            AnalogAxisId::Camera
        }
    };

    const std::vector<AnalogAxisGroupId> USER_ANALOG_AXIS_GROUP_IDS =
    {
        AnalogAxisGroupId::Gameplay
    };

    const std::vector<AnalogAxisGroupId> RAW_ANALOG_AXIS_GROUP_IDS =
    {
        AnalogAxisGroupId::Raw
    };

    AnalogAxis::AnalogAxis(AnalogAxisId axisId)
    {
        _id = axisId;
    }

    AnalogAxisId AnalogAxis::GetId() const
    {
        return _id;
    }

    bool AnalogAxis::IsLocked() const
    {
        return _isLocked;
    }

    void AnalogAxis::Lock()
    {
        _isLocked = true;
    }

    void AnalogAxis::Unlock()
    {
        _isLocked = false;
    }
}
