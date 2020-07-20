/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphConnectionDrawingPolicy.cpp"
#include "ENConnectionDrawingPolicy.h"


class FENAnimGraphConnectionDrawingPolicy : public FAnimGraphConnectionDrawingPolicy
{
public:
    FENAnimGraphConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
        : FAnimGraphConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj)
    {
        this->connectionDrawingPolicy = new FENConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
    }

    virtual void DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params) override;

    ~FENAnimGraphConnectionDrawingPolicy()
    {
        delete connectionDrawingPolicy;
    }

private:
    FENConnectionDrawingPolicy* connectionDrawingPolicy;
};
