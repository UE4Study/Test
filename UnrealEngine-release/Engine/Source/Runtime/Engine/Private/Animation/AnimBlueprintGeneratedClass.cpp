// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	AnimBlueprintGeneratedClass.cpp: The object generated by compiling a AnimBlueprint
=============================================================================*/ 

#include "EnginePrivate.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimInstance.h"
#include "BonePose.h"

/////////////////////////////////////////////////////
// FStateMachineDebugData

UEdGraphNode* FStateMachineDebugData::FindNodeFromStateIndex(int32 StateIndex) const
{
	if (const TWeakObjectPtr<UEdGraphNode>* pResult = NodeToStateIndex.FindKey(StateIndex))
	{
		return pResult->Get();
	}
	return NULL;
}

UEdGraphNode* FStateMachineDebugData::FindNodeFromTransitionIndex(int32 TransitionIndex) const
{
	if (const TWeakObjectPtr<UEdGraphNode>* pResult = NodeToTransitionIndex.FindKey(TransitionIndex))
	{
		return pResult->Get();
	}
	return NULL;
}

/////////////////////////////////////////////////////
// FAnimBlueprintDebugData

#if WITH_EDITORONLY_DATA

void FAnimBlueprintDebugData::TakeSnapshot(UAnimInstance* Instance)
{
	checkSlow(&(CastChecked<UAnimBlueprintGeneratedClass>((UObject*)(Instance->GetClass()))->GetAnimBlueprintDebugData()) == this);

	if (SnapshotBuffer == NULL)
	{
		SnapshotBuffer = new TSimpleRingBuffer<FAnimationFrameSnapshot>(30*30);
	}

	SnapshotBuffer->WriteNewElementInitialized().InitializeFromInstance(Instance);
}

void FAnimBlueprintDebugData::ResetSnapshotBuffer()
{
	if (SnapshotBuffer != NULL)
	{
		delete SnapshotBuffer;
		SnapshotBuffer = NULL;
	}
}

float FAnimBlueprintDebugData::GetSnapshotLengthInSeconds()
{
	if (SnapshotBuffer != NULL)
	{
		//@TODO: Shouldn't use hardcoded snapshot length; ideally we actually use timestamps, etc...
		return SnapshotBuffer->Num() * 1.0f / 30.0f;
	}

	return 0.0f;
}

int32 FAnimBlueprintDebugData::GetSnapshotLengthInFrames()
{
	if (SnapshotBuffer != NULL)
	{
		//@TODO: Shouldn't use hardcoded snapshot length; ideally we actually use timestamps, etc...
		return SnapshotBuffer->Num();
	}

	return 0;
}

void FAnimBlueprintDebugData::SetSnapshotIndex(UAnimInstance* Instance, int32 NewIndex)
{
	if (SnapshotBuffer != NULL)
	{
		int32 SavedIndex = SnapshotIndex;
		int32 ClampedIndex = FMath::Clamp<int32>(NewIndex, 0, SnapshotBuffer->Num() - 1);
		SnapshotIndex = (NewIndex == INDEX_NONE) ? INDEX_NONE : ClampedIndex;

		// Apply the desired snapshot (or the most recent one if free-running was selected)
		if ((SnapshotIndex != SavedIndex) && (SnapshotBuffer->Num() > 0))
		{
			(*SnapshotBuffer)(ClampedIndex).CopyToInstance(Instance);
		}
	}
}

void FAnimBlueprintDebugData::SetSnapshotIndexByTime(UAnimInstance* Instance, double TargetTime)
{
	const int32 SavedIndex = SnapshotIndex;

	int32 NewIndex = INDEX_NONE;

	if ((SnapshotBuffer != NULL) && (SnapshotBuffer->Num() > 0))
	{
		if ((TargetTime < (*SnapshotBuffer)(0).TimeStamp) && (TargetTime >= (*SnapshotBuffer)(SnapshotBuffer->Num()-1).TimeStamp))
		{
			for (NewIndex = SnapshotBuffer->Num() - 1; NewIndex > 0; --NewIndex)
			{
				if (TargetTime < (*SnapshotBuffer)(NewIndex-1).TimeStamp)
				{
					break;
				}
			}
		}
	}

	// Determine which snapshot to use
	SetSnapshotIndex(Instance, NewIndex);

	// Stomp on the time that was played back from the snapshot since we were given an exact one
	Instance->CurrentLifeTimerScrubPosition = TargetTime;
}

void FAnimBlueprintDebugData::ResetNodeVisitSites()
{
	UpdatedNodesThisFrame.Empty(UpdatedNodesThisFrame.Num());
}

void FAnimBlueprintDebugData::RecordNodeVisit(int32 TargetNodeIndex, int32 SourceNodeIndex, float BlendWeight)
{
	new (UpdatedNodesThisFrame) FNodeVisit(SourceNodeIndex, TargetNodeIndex, BlendWeight);
}

void FAnimBlueprintDebugData::RecordNodeVisitArray(const TArray<FNodeVisit>& Nodes)
{
	UpdatedNodesThisFrame.Append(Nodes);
}

void FAnimBlueprintDebugData::AddPoseWatch(int32 NodeID, FColor Color)
{
	for (FAnimNodePoseWatch& PoseWatch : AnimNodePoseWatch)
	{
		if (PoseWatch.NodeID == NodeID)
		{
			PoseWatch.PoseDrawColour = Color;
			return;
		}
	}

	//Not found so make new one
	AnimNodePoseWatch.Add(FAnimNodePoseWatch());
	FAnimNodePoseWatch& NewAnimNodePoseWatch = AnimNodePoseWatch.Last();
	NewAnimNodePoseWatch.NodeID = NodeID;
	NewAnimNodePoseWatch.PoseDrawColour = Color;
	NewAnimNodePoseWatch.PoseInfo = MakeShareable(new FCompactHeapPose());
}

void FAnimBlueprintDebugData::RemovePoseWatch(int32 NodeID)
{
	for (int32 PoseWatchIdx = 0; PoseWatchIdx < AnimNodePoseWatch.Num(); ++PoseWatchIdx)
	{
		if (AnimNodePoseWatch[PoseWatchIdx].NodeID == NodeID)
		{
			AnimNodePoseWatch.RemoveAtSwap(PoseWatchIdx);
			return;
		}
	}
}

void FAnimBlueprintDebugData::UpdatePoseWatchColour(int32 NodeID, FColor Color)
{
	for (FAnimNodePoseWatch& PoseWatch : AnimNodePoseWatch)
	{
		if (PoseWatch.NodeID == NodeID)
		{
			PoseWatch.PoseDrawColour = Color;
			return;
		}
	}
}
/////////////////////////////////////////////////////
// FBinaryObjectWriter

class FBinaryObjectWriter : public FObjectWriter
{
public:
	FBinaryObjectWriter(UObject* Obj, TArray<uint8>& InBytes)
		: FObjectWriter(InBytes)
	{
		ArWantBinaryPropertySerialization = true;
		Obj->Serialize(*this);
	}
};

/////////////////////////////////////////////////////
// FBinaryObjectReader

class FBinaryObjectReader : public FObjectReader
{
public:
	FBinaryObjectReader(UObject* Obj, TArray<uint8>& InBytes)
		: FObjectReader(InBytes)
	{
		ArWantBinaryPropertySerialization = true;
		Obj->Serialize(*this);
	}
};

/////////////////////////////////////////////////////
// FAnimationFrameSnapshot

void FAnimationFrameSnapshot::InitializeFromInstance(UAnimInstance* Instance)
{
	FBinaryObjectWriter Writer(Instance, SerializedData);
	TimeStamp = Instance->LifeTimer;
}

void FAnimationFrameSnapshot::CopyToInstance(UAnimInstance* Instance)
{
	FBinaryObjectReader Reader(Instance, SerializedData);
	Instance->CurrentLifeTimerScrubPosition = TimeStamp;
}

#endif

/////////////////////////////////////////////////////
// UAnimBlueprintGeneratedClass

UAnimBlueprintGeneratedClass::UAnimBlueprintGeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootAnimNodeIndex = INDEX_NONE;
}

void UAnimBlueprintGeneratedClass::Link(FArchive& Ar, bool bRelinkExistingProperties)
{
	Super::Link(Ar, bRelinkExistingProperties);

	// @TODO: Shouldn't be necessary to clear these, but currently the class gets linked twice during compilation
	AnimNodeProperties.Empty();
	RootAnimNodeProperty = NULL;

	// Initialize derived members
	for (TFieldIterator<UProperty> It(this); It; ++It)
	{
		if (UStructProperty* StructProp = Cast<UStructProperty>(*It))
		{
			if (StructProp->Struct->IsChildOf(FAnimNode_Base::StaticStruct()))
			{
				AnimNodeProperties.Add(StructProp);
			}
		}
	}

	// Pull info down from root anim class
	UAnimBlueprintGeneratedClass* RootClass = this;
	while(UAnimBlueprintGeneratedClass* NextClass = Cast<UAnimBlueprintGeneratedClass>(RootClass->GetSuperClass()))
	{
		RootClass = NextClass;
	}

	if(RootClass != this)
	{
		// Copy root, state notifies and baked machines from the root class
		RootAnimNodeIndex = RootClass->RootAnimNodeIndex;
		AnimNotifies = RootClass->AnimNotifies;
		BakedStateMachines = RootClass->BakedStateMachines;
	}

	if (AnimNodeProperties.Num() > 0)
	{
		const bool bValidRootIndex = (RootAnimNodeIndex >= 0) && (RootAnimNodeIndex < AnimNodeProperties.Num());
		if (bValidRootIndex)
		{
			RootAnimNodeProperty = AnimNodeProperties[AnimNodeProperties.Num() - 1 - RootAnimNodeIndex];
		}
		else
		{
			UE_LOG(LogAnimation, Warning, TEXT("Invalid animation root node index %d on '%s' (only %d nodes)"), RootAnimNodeIndex, *GetPathName(), AnimNodeProperties.Num());
			AnimNodeProperties.Empty();

			// After the log instead of in the if() to make sure the log statement is emitted
			ensure(bValidRootIndex);
		}
	}

	if(RootClass != this)
	{
		if(OrderedSavedPoseIndices.Num() != RootClass->OrderedSavedPoseIndices.Num() || OrderedSavedPoseIndices != RootClass->OrderedSavedPoseIndices)
		{
			// Derived and our parent has a new ordered pose order, copy over.
			OrderedSavedPoseIndices = RootClass->OrderedSavedPoseIndices;
		}
	}
}

void UAnimBlueprintGeneratedClass::PurgeClass(bool bRecompilingOnLoad)
{
	Super::PurgeClass(bRecompilingOnLoad);

	AnimNotifies.Empty();
	TargetSkeleton = NULL;
#if WITH_EDITORONLY_DATA
	AnimBlueprintDebugData = FAnimBlueprintDebugData();
#endif

	BakedStateMachines.Empty();
}

uint8* UAnimBlueprintGeneratedClass::GetPersistentUberGraphFrame(UObject* Obj, UFunction* FuncToCheck) const
{
	if(!IsInGameThread())
	{
		// we cant use the persistent frame if we are executing in parallel (as we could potentially thunk to BP)
		return nullptr;
	}

	return Super::GetPersistentUberGraphFrame(Obj, FuncToCheck);
}

#if WITH_EDITORONLY_DATA

const int32* UAnimBlueprintGeneratedClass::GetNodePropertyIndexFromGuid(FGuid Guid, EPropertySearchMode::Type SearchMode /*= EPropertySearchMode::OnlyThis*/)
{
	if (SearchMode == EPropertySearchMode::OnlyThis)
	{
		return AnimBlueprintDebugData.NodeGuidToIndexMap.Find(Guid);
	}
	else
	{
		TArray<const UBlueprintGeneratedClass*> BlueprintHierarchy;
		GetGeneratedClassesHierarchy(this, BlueprintHierarchy);

		for (const UBlueprintGeneratedClass* Blueprint : BlueprintHierarchy)
		{
			if (const UAnimBlueprintGeneratedClass* AnimBlueprintClass = Cast<UAnimBlueprintGeneratedClass>(Blueprint))
			{
				const int32* NodeIndex = AnimBlueprintClass->AnimBlueprintDebugData.NodeGuidToIndexMap.Find(Guid);

				if (NodeIndex)
				{
					return NodeIndex;
				}
			}

		}
	}

	return NULL;
}

#endif