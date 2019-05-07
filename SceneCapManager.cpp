// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneCapManager.h"
#include "AllowWindowsPlatformTypes.h"
#include "Windows.h"
#include "HideWindowsPlatformTypes.h"
#include <string>
#include <vector>

// Sets default values
ASceneCapManager::ASceneCapManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//bAllowTickBeforeBeginPlay = false;
	
}

// Called when the game starts or when spawned
void ASceneCapManager::BeginPlay()
{
	Super::BeginPlay();
	/*TArray<UActorComponent*> a;
	a=Vehicle->GetComponentsByClass(UCaptureComponent::StaticClass());*/
	//Camera.Append((Vehicle->GetComponentsByClass(UCaptureComponent::StaticClass())));
	/*Camera.Empty();
	Camera.Append(a);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"),a.Num()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), Camera.Num()));*/
	/*if (a.Num() > 0)
	{
		for (int i = 0; i < a.Num(); i++)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d%s"), i,a[i].));
		}
	}*/
	
}

// Called every frame
void ASceneCapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FrameID>100)
	{
		return;
	}

	int PointsToScanWithOneLaser = FMath::RoundHalfFromZero(PointsperSecond * DeltaTime / Channel);
	if (PointsToScanWithOneLaser <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: no points requested this frame, try increasing the number of points per second."));
		return;
	}
	if (PointsToScanWithOneLaser > PointsperSecond * 0.05 / Channel)
		PointsToScanWithOneLaser = int(PointsperSecond * 0.05 / Channel);
	const float AnglePerTick = RotationFrequency * 360.0f * DeltaTime;

	//float AnglePerTick = 30;
	LastHorAngle = CurrentHorAngle;
	CurrentHorAngle = CurrentHorAngle + AnglePerTick;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), AnglePerTick));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%f"), LastHorAngle));
	
	/*views needed to record in this frame*/
	int NumOfActiveView = floor(CurrentHorAngle/90) - floor(LastHorAngle / 90) + 1;	

	/*One spin completed*/
	if (CurrentHorAngle >= 360.f )
	{
		CurrentHorAngle = CurrentHorAngle - 360.0;
		FrameID++;
	}

	//TArray<UTextureRenderTarget2D*> ActiveViews;
	int StartViewID = floor(LastHorAngle / 90);
	int EndViewID =floor(CurrentHorAngle / 90);
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), StartViewID));

	FString a[] = { TEXT("F"),TEXT("R"),TEXT("B"),TEXT("L") };

	if (EndViewID >= StartViewID)
	{
		for (int j = StartViewID; j <= EndViewID; j++)
		{
			FloatImage.Empty();

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), j));
			char name[2048];
			if (*path != NULL)
			{
				sprintf_s(name, "%s%08d%s.bin", TCHAR_TO_ANSI(*path), FrameID, TCHAR_TO_ANSI(*a[j]));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("directory= NULL")));
				return;
			}

			/*Calculate Start and end pixel for this view*/
			if (NumOfActiveView == 1)
			{
				//only one view
				float startangle = fmod(LastHorAngle, 90)*PI / 180;
				float endangle = 90 * PI / 180;
				if (fmod(CurrentHorAngle, 90) != 0)
				{
					endangle = fmod(CurrentHorAngle, 90)*PI / 180;
				}
				start = floor(0.5*width - tan(45 * PI / 180 - startangle)*width*0.5);				
				end = floor(0.5*width - tan(45 * PI / 180 - endangle)*width*0.5);
				
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%f"), startangle));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), start));
			}
			else if (j == EndViewID && NumOfActiveView > 1)
			{
				//last view
				float startangle = 0;
				float endangle = 90 * PI / 180;
				if (fmod(CurrentHorAngle, 90) != 0)
				{
					endangle = fmod(CurrentHorAngle, 90)*PI / 180;
				}
				start = 0;
				end = floor(0.5*width - tan(45 * PI / 180 - endangle)*width*0.5);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), LastHorAngle));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%f"), fmod(LastHorAngle, 90)));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), start));
			}
			else if (j == StartViewID && NumOfActiveView > 1)
			{
				//first view
				float startangle = fmod(LastHorAngle, 90)*PI / 180;
				float endangle = 90;
				start = floor(0.5*width - tan(45 * PI / 180 - startangle)*width*0.5);
				end = width;
			}
			else
			{
				//middle view
				float startangle = 0;
				float endangle = 90;
				start = 0;
				end = width;
			}
			
			//Read Pixels
			FTextureRenderTargetResource* RenderTargetResource = Views[j]->GameThread_GetRenderTargetResource();
			RenderTargetResource->ReadFloat16Pixels(FloatImage);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatImage %d"), FloatImage.Num()));
			FFloat16Color* FloatRGBA = &FloatImage[0];

			/*output data of this frame*/
			if (FloatRGBA != NULL)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), j));
				Output(name, FloatRGBA, start, end, error, MaxFileSize);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"),FloatImage.Num()));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatRGBA != NULL")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatRGBA is NULL")));
				return;
			}
		}
	}
	if (EndViewID < StartViewID)
	{
		for (int j = StartViewID; j <= 3; j++)
		{
			FloatImage.Empty();

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), j));
			char name[2048];
			if (*path != NULL)
			{
				sprintf_s(name, "%s%08d%s.bin", TCHAR_TO_ANSI(*path), FrameID, TCHAR_TO_ANSI(*a[j]));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("directory= NULL")));
				return;
			}

			/*Calculate Start and end for this view*/
			
			if (j == EndViewID && NumOfActiveView > 1)
			{
				//last view
				float startangle = 0;
				float endangle = 90 * PI / 180;
				if (fmod(CurrentHorAngle, 90) != 0)
				{
					endangle = fmod(CurrentHorAngle, 90)*PI / 180;
				}
				start = 0;
				end = floor(0.5*width - tan(45 * PI / 180 - endangle)*width*0.5);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), start));
			}
			else if (j == StartViewID && NumOfActiveView > 1)
			{
				//first view
				float startangle = fmod(LastHorAngle, 90)*PI / 180;
				float endangle = 90;
				start = floor(0.5*width - tan(45 * PI / 180 - startangle)*width*0.5);
				end = width;
			}
			else
			{
				//middle view
				float startangle = 0;
				float endangle = 90;
				int start = 0;
				int end = width;
			}

			//Read Pixels
			FTextureRenderTargetResource* RenderTargetResource = Views[j]->GameThread_GetRenderTargetResource();
			RenderTargetResource->ReadFloat16Pixels(FloatImage);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatImage %d"), FloatImage.Num()));
			FFloat16Color* FloatRGBA = &FloatImage[0];

			/*output data of this frame*/
			if (FloatRGBA != NULL)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), j));
				Output(name, FloatRGBA, start, end, error, MaxFileSize);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"),FloatImage.Num()));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatRGBA != NULL")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatRGBA is NULL")));
				return;
			}
			/*delete FloatRGBA;
			FloatRGBA = NULL;*/
			/*delete RenderTargetResource;
			RenderTargetResource = NULL;*/
		}
		for (int j = 0; j <= EndViewID; j++)
		{
			FloatImage.Empty();

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), j));
			char name[2048];
			if (*path != NULL)
			{
				sprintf_s(name, "%s%08d%s.bin", TCHAR_TO_ANSI(*path), FrameID, TCHAR_TO_ANSI(*a[j]));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("directory= NULL")));
				return;
			}

			/*Calculate Start and end for this view*/
			if (j == EndViewID && NumOfActiveView > 1)
			{
				//last view
				float startangle = 0;
				float endangle = 90 * PI / 180;
				if (fmod(CurrentHorAngle, 90) != 0)
				{
					endangle = fmod(CurrentHorAngle, 90)*PI / 180;
				}
				start = 0;
				end = floor(0.5*width - tan(45 * PI / 180 - endangle)*width*0.5);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), start));
			}
			else if (j == StartViewID && NumOfActiveView > 1)
			{
				//first view
				float startangle = fmod(LastHorAngle, 90)*PI / 180;
				float endangle = 90;
				start = floor(0.5*width - tan(45 * PI / 180 - startangle)*width*0.5);
				end = width;
			}
			else
			{
				//middle view
				float startangle = 0;
				float endangle = 90;
				int start = 0;
				int end = width;
			}

			//Read Pixels
			FTextureRenderTargetResource* RenderTargetResource = Views[j]->GameThread_GetRenderTargetResource();
			RenderTargetResource->ReadFloat16Pixels(FloatImage);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatImage %d"), FloatImage.Num()));
			FFloat16Color* FloatRGBA = &FloatImage[0];

			/*output data of this frame*/
			if (FloatRGBA != NULL)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), j));
				Output(name, FloatRGBA, start, end, error, MaxFileSize);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"),FloatImage.Num()));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatRGBA != NULL")));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FloatRGBA is NULL")));
				return;
			}
			/*delete FloatRGBA;
			FloatRGBA = NULL;*/
			/*delete RenderTargetResource;
			RenderTargetResource = NULL;*/
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("%d"), NumOfActiveView);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), ActiveViews.Num()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d"), ActiveViews.Num()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d %d"), StartViewID,EndViewID));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d %f"), FrameID, CurrentHorAngle));
}

bool ASceneCapManager::Output(const FString& pszFileName, const FFloat16Color* FloatRGBA, const float Start, const float end, FString& error, const uint32 MaxFileSize)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"),Start));

	HANDLE hFile;
	HANDLE hMapFile;

	hFile = CreateFile(
		(LPCWSTR)*pszFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile != INVALID_HANDLE_VALUE) {
		error = "Could open file object";
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, error);
	}
	else if (hFile == INVALID_HANDLE_VALUE) {
		error = "Could not open file object";
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, error);
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

	////get pixels
	for (int x = Start; x < end; x++) {
		for (int y = 0; y < height; y++) {
			//FFloat16Color c = FloatRGBA[y*width + x + f * width*height];
			FFloat16Color c = FloatRGBA[y*width + x];
			int adr = y * width + x;
			if (capture_alpha) {
				*((float*)pBuf + adr) = c.A.GetFloat();

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Alpha %f"), c.A.GetFloat()));

			}
			//else {
			//	/*for_rgbe[adr + 0] = c.R.GetFloat();
			//	for_rgbe[adr + 1] = c.G.GetFloat();
			//	for_rgbe[adr + 2] = c.B.GetFloat();*/
			//}
		}
	}

		UnmapViewOfFile(pBuf);
		error = "could write";
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, error);
		CloseHandle(hFile);
		CloseHandle(hMapFile);

		return true;
}
