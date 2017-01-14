/*
* Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*
* This code was autogenerated from ModuleLegacy.template
*/

#include "ApexUsingNamespace.h"
#include "Apex.h"
#include "ApexLegacyModule.h"
#include "ApexRWLockable.h"
#include "ModuleFrameworkLegacyRegistration.h"

namespace nvidia 
{ 
namespace apex
{
namespace legacy
{

class ModuleFrameworkLegacy : public ApexLegacyModule, public ApexRWLockable
{
public:
	APEX_RW_LOCKABLE_BOILERPLATE

	ModuleFrameworkLegacy(ApexSDKIntl* inSdk)
	{
  	  mName = "Framework_Legacy";
	  mSdk = inSdk;
	  mApiProxy = this;
	  ModuleFrameworkLegacyRegistration::invokeRegistration(mSdk->getParameterizedTraits());
	}

protected:
	void releaseLegacyObjects()
        {
          ModuleFrameworkLegacyRegistration::invokeUnregistration(mSdk->getParameterizedTraits());
        }
};

void instantiateModuleFrameworkLegacy()
{
	ApexSDKIntl *sdk = GetInternalApexSDK();
	ModuleFrameworkLegacy *impl = PX_NEW(ModuleFrameworkLegacy)(sdk);
	sdk->registerExternalModule((Module *) impl, (ModuleIntl *) impl);
}

}
}
}
