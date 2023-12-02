#include "components/Camera.h"
#include "common/utils.h"
#include "common/constants.h"
#include "common/utilsMat.h"
#include <glm/gtx/string_cast.hpp>
#include <cmath>

void CameraComponent::GetInfo(sComponentInfo& compInfoOut)
{
	using namespace myutils;

	compInfoOut.componentName = "camera";
	compInfoOut.componentParameters.clear();

    this->AddCompParInfo("upVector", "vec3", this->upVector, compInfoOut);
    this->AddCompParInfo("distance", "float", this->distance, compInfoOut);
}

void CameraComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "upVector") {
        this->upVector = parameterIn.parameterVec3Value;
    }
    else if (parameterIn.parameterName == "distance") {
        this->distance = parameterIn.parameterFloatValue;
    }

    return;
}