// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Lidar4.generated.h"

USTRUCT()
struct FLidarPoint
{
	GENERATED_BODY()

public:

	float X;
	float Y;
	float Z;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VEHICLE__API ULidar4 : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULidar4();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static bool DataOut(const FString& pszFileName, const FString HandleName, const TArray<FVector> PointCloud, FString& error,  const uint32 MaxFileSize);


	// Lidar Parameters
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float Height = 250;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float range = 5000;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		int Channel = 32;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		int PointsperSecond = 100000;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float RotationFrequency = 1;
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float VerticalFOV = 26.8; // [deg]
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float VerticalLow = -24.8;//[deg]
	UPROPERTY(EditAnywhere, Category = "Lidar Parameters")
		float VerticalHigh = 2;//[deg]

	//Display
	UPROPERTY(EditAnywhere, Category = "Debug")
		float pointsize;

	//Data Storage
	TArray<FLidarPoint> Points;
	TArray<FVector> Points2;
	TArray<float> Intensities;
	//UPROPERTY(EditAnywhere, Category = "Data Export")
	//FString Filename;
	FString HandleName = "LidarPoint";
	FString error = "none";
	const uint32 MaxFileSize = 20000;
	int FrameID;
	float TimeStart;
	float TimeEnd;

private:

	// Raycast Parameters
	float PhiMaxRadians;
	float Alpha;
	float CurrentHorAngle;
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
	
};


