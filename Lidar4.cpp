// Fill out your copyright notice in the Description page of Project Settings.

#include "Lidar4.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DataExport.h"
#include "Engine.h"
#include "AllowWindowsPlatformTypes.h"
#include "Windows.h"
#include "HideWindowsPlatformTypes.h"
#include <iostream>

using namespace std;

// Sets default values for this component's properties
ULidar4::ULidar4()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULidar4::BeginPlay()
{
	Super::BeginPlay();

	// init laser rotation parameters
	Forward = FVector(1, 0, 0);
	Direction = FVector(1, 0, 0);

	CurrentHorAngle = 0;

	/*Vertical Calculation*/
	/*float VerResolution = VerticalFOV / (Channel - 1);*/
	float VerResolution = 1.33;
	float angle;
	for (int i = 0; i < Channel; i++)
	{
		angle = VerticalLow + i*VerResolution;
		Ver.Add(FRotator(angle, 0, 0));
		
	}
	
		// Get the PointCloudRenderingComponent
		// PCRenderer = GetPointCloudRenderingComponent(); Don't really need now because we are not updating it
	int FrameID = 0;
		// ...
	
}


// Called every frame
void ULidar4::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector RayCastStart, RayCastEnd;
	RayCastStart = GetOwner()->GetActorLocation() + FVector(0, 0, Height);

	float ActorYaw = GetOwner()->GetActorRotation().Yaw;
	Direction = GetOwner()->GetActorForwardVector();

	/*Hotizontal Calculation*/
	int PointsToScanWithOneLaser = FMath::RoundHalfFromZero(PointsperSecond * DeltaTime / Channel);
	if (PointsToScanWithOneLaser <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: no points requested this frame, try increasing the number of points per second."));
		return;
	}
	if (PointsToScanWithOneLaser > PointsperSecond * 0.05 / Channel)
		PointsToScanWithOneLaser = int(PointsperSecond * 0.05 / Channel);
	const float AnglePerTick = RotationFrequency * 360.0f * DeltaTime;
	const float HorAngleResolution = AnglePerTick / PointsToScanWithOneLaser;

	/*Detect Spin*/
	if (CurrentHorAngle >=360.f && FrameID<=20)
	{
		CurrentHorAngle = CurrentHorAngle - 360.0;

		FrameID++;

		///*Store Hit Position*/
		FString FileName=FString::Printf(TEXT("D:\\Repos\\VS\\%d.bin"), FrameID);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("D:\\Repos\\VS\\%d.bin"), FrameID));
		DataOut(FileName, HandleName, Points2, error, MaxFileSize);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"), Points2.Num()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), *Ver[0].ToString()));
		Points2.Empty();
	}

	float CurrentHorAngle_init = CurrentHorAngle;

	for (int i = 0; i < Channel; i++)
	{
		/*back to horizontal startpoint of this frame*/
		CurrentHorAngle = CurrentHorAngle_init;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), Ver[i].Pitch));

		for (int j = 0; j < PointsToScanWithOneLaser; j++)
		{
			CurrentHorAngle = CurrentHorAngle + HorAngleResolution;
			FRotator Horizontal(float(0), CurrentHorAngle + ActorYaw, float(0));
			FRotator ResultRot = UKismetMathLibrary::ComposeRotators(Ver[i], Horizontal);

			/*define raycast parameters*/
			RayCastEnd = RayCastStart + UKismetMathLibrary::GetForwardVector(ResultRot) * range;//ray cast end
			ECollisionChannel Channel = ECollisionChannel::ECC_Visibility; 
			FHitResult Hits;

			/*execute raycast*/
			GetWorld()->LineTraceSingleByChannel(Hits, RayCastStart, RayCastEnd, Channel);

			/*Draw DebugPoint or Line*/
			float durationtime = 1 / RotationFrequency/1000;
			//DrawDebugPoint(
			//	GetWorld(),
			//	Hits.ImpactPoint,
			//	pointsize,  //size
			//	FColor(255, 0, 0),
			//	false,  //persistent (never goes away)
			//	durationtime  //point leaves a trail on moving object
			//);
			/*count++;*/
			/*DrawDebugLine(GetWorld(),
				RayCastStart,
				Hits.ImpactPoint,
				FColor(255, 0, 0),
				false,
				durationtime,
				0.f,
				1.0f);	*/	

			/*store*/
			/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), *Hits.ImpactPoint.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), Hits.ImpactPoint.Y));*/
			Points2.Emplace(Hits.ImpactPoint);
		}
		
	}
}

bool ULidar4::DataOut(const FString& pszFileName, const FString HandleName, const TArray<FVector> PointCloud, FString& error, const uint32 MaxFileSize)
{

	HANDLE hFile;
	HANDLE hMapFile;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("enter")));
	hFile = CreateFile(
		(LPCWSTR)*pszFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	//hFile = CreateFile(
	//	"D:\\Repos\\VS\\Memmap.dat",
	//	GENERIC_READ | GENERIC_WRITE,
	//	FILE_SHARE_READ | FILE_SHARE_WRITE,
	//	NULL,
	//	CREATE_ALWAYS,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL
	//);

	if (hFile != INVALID_HANDLE_VALUE) {
		error = "Could open file object";
	}
	else if (hFile == INVALID_HANDLE_VALUE) {
		error = "Could not open file object";
	
		return false;
	}

	hMapFile = CreateFileMapping(
		hFile,                   // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		2097152,              // maximum object size (low-order DWORD)
		NULL);   // name of mapping object

	if (hMapFile == NULL) {
		DWORD dwLastError = GetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "hMapFile==NULL");
		return false;
	}

	PVOID pBuf;
	pBuf = MapViewOfFile(
		hMapFile,		// handle to map object
		FILE_MAP_ALL_ACCESS,					// read/write permission
		0,
		0,
		0);

	if (pBuf == NULL)
	{
		error = "Could not map view of file.";
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("ERROR"), MB_OK);
		LocalFree(lpMsgBuf);// Free the buffer.
		CloseHandle(hFile);
		CloseHandle(hMapFile);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, error);
		return false;
	}

	//int i = 0;
	//for (auto& ItPt : PointCloud)
	//{
	//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"),ItPt[0]));
	//	*((float*)pBuf + i)= ItPt[0];
	//	*((float*)pBuf + i + 1) = ItPt[1];
	//	*((float*)pBuf + i + 2) = ItPt[2];
	//	i++;
	//}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d"), PointCloud.Num()));
	float a = 100;
	for (int i = 0; i < PointCloud.Num(); i++)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), PointCloud[i].Y));
		
	/*	*((float*)pBuf + i) = a;
		*((float*)pBuf + i + 1) = a;
		*((float*)pBuf + i + 2) = a;*/
		*((float*)pBuf + 3*i) = PointCloud[i].X;
		*((float*)pBuf + 3*i + 1) = PointCloud[i].Y;
		*((float*)pBuf + 3*i + 2) = PointCloud[i].Z;
	}

	UnmapViewOfFile(pBuf);
	error = "could write";

	CloseHandle(hFile);
	CloseHandle(hMapFile);

	return true;
}
