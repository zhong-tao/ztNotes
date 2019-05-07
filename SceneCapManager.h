// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureComponent.h"
#include "Engine.h"
#include "SceneCapManager.generated.h"

UCLASS()
class VEHICLE__API ASceneCapManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneCapManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere)
		TArray<UCaptureComponent*> Camera;

	UPROPERTY(EditAnywhere)
		AActor* Vehicle;

	/*Receives image from the attached render targets*/
	UPROPERTY(EditAnywhere)
		TArray<UTextureRenderTarget2D *> Views;

	/* Size of RenderTarget*/
	UPROPERTY(EditAnywhere)
		int width = 512;
	UPROPERTY(EditAnywhere)
		int height = 512;

	// Lidar Parameters
	//UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
	//	float Height = 250;
	//UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
	//	float range = 5000;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		int Channel = 32;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		int PointsperSecond = 100000;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float RotationFrequency = 5;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float VerticalFOV = 26.8; // [deg]
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float VerticalLow = -24.8;//[deg]
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float VerticalHigh = 2;//[deg]

	// Raycast Parameters
	float PhiMaxRadians;
	float Alpha;
	float LastHorAngle = 0;
	float CurrentHorAngle = 0;
	FMatrix RotationMatrix;
	FMatrix VerRotInit;
	FMatrix VerRotMatrix;
	TArray<FRotator> Ver;
	FMatrix FrameMatrix;
	float dPoint;
	FVector Forward;
	FVector Direction;
	FVector DirectionIntermediate;
	FVector CurrentPoint;

	/*Data output*/
	//bool DataOut(const FString& pszFileName, const FString HandleName, const FFloat16Color* FloatRGBA, const int width, const int height, FString& error, const uint32 MaxFileSize);
	/*Array to save data*/
	TArray<FFloat16Color> Front; 
	/*TArray<FFloat16Color> Left;
	TArray<FFloat16Color> Right;
	TArray<FFloat16Color> Back;*/

	UPROPERTY(EditAnywhere)
		FString path;
	UPROPERTY(EditAnywhere)
		bool capture_alpha = false;

	TArray<FFloat16Color> FloatImage;

	int start = 0;
	int end = 512;

	int FrameID=0;
	FString HandleName = "DepthPoint";
	FString error = "none";
	const uint32 MaxFileSize = 50000;

private:
	bool Output(const FString& pszFileName, const FFloat16Color* FloatRGBA,const float Start,const float end, FString& error, const uint32 MaxFileSize);

	
	
};
