// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2016 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef CM_BOXPRUNING_H
#define CM_BOXPRUNING_H

#include "foundation/PxBounds3.h"
#include "PsArray.h"
#include "CmPhysXCommon.h"
#include "PxPhysXCommonConfig.h"
#include "GuAxes.h"

namespace physx
{

namespace Cm
{
	PX_PHYSX_COMMON_API bool CompleteBoxPruning(const PxBounds3* bounds, PxU32 nb, Ps::Array<PxU32>& pairs, const Gu::Axes& axes);
	PX_PHYSX_COMMON_API bool BipartiteBoxPruning(const PxBounds3* bounds0, PxU32 nb0, const PxBounds3* bounds1, PxU32 nb1, Ps::Array<PxU32>& pairs, const Gu::Axes& axes);
}
}
	
#endif // CM_BOXPRUNING_H