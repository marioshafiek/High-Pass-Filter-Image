#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <mpi.h>
#include <ctime>// include this header 
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;




int ImageWidth = 4, ImageHeight = 4;
int L = 0;

int* inputImage(int* w, int* h, System::String^ imagePath) //put the size of image in w & h
{
	int* input;

	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height*BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i*BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("D://For University//HPC_ProjectTemplate//HPC_ProjectTemplate//Data//OutPut//" + index + ".png");
	cout << "result Image Saved " << index << endl;
}
//O(n2)
void ConvertFrom1D_to_2D(int* arr_1D, int** arr_2D)
{
	int indexFor1D = 0;

	//Loop to transfer values from 1D to 2D
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{	
			arr_2D[i][j] = arr_1D[indexFor1D++];
		}
	}

}
//O(1)
int checkifOutOfBounds(int i, int j, int* arr_2D)
{
	int value;
	if (i<0 || i>ImageHeight - 1 || j<0 || j>ImageWidth - 1)
	{
		value = 0;
		return value;
	}
	else
	{
		//1D --> 2D 
		//Index 2d --> 1D
		int Index = i * ImageWidth + j;
		return arr_2D[Index];
	}
}
//O(1)
int ConvertIndex(int i, int j, int* arr_2d)
{
	int ArrOfValues[9];
	double v = 0.1;
	double value ;
	ArrOfValues[0] = checkifOutOfBounds(i - 1, j - 1, arr_2d);
	//cout <<ArrOfValues[0] << endl;
	ArrOfValues[1] = checkifOutOfBounds(i - 1, j, arr_2d);
	//cout <<ArrOfValues[1] << endl;
	ArrOfValues[2] = checkifOutOfBounds(i - 1, j + 1, arr_2d);
	//cout <<ArrOfValues[2] << endl;
	ArrOfValues[3] = checkifOutOfBounds(i, j - 1, arr_2d);
	//cout <<ArrOfValues[3]<<endl;
	ArrOfValues[4] = checkifOutOfBounds(i, j, arr_2d);
	//cout <<ArrOfValues[4]<<endl;
	ArrOfValues[5] = checkifOutOfBounds(i, j + 1, arr_2d);
	//cout <<ArrOfValues[5]<<endl;
	ArrOfValues[6] = checkifOutOfBounds(i + 1, j - 1, arr_2d);
	//cout <<ArrOfValues[6]<<endl;
	ArrOfValues[7] = checkifOutOfBounds(i + 1, j, arr_2d);
	//cout <<ArrOfValues[7]<<endl;
	ArrOfValues[8] = checkifOutOfBounds(i + 1, j + 1, arr_2d);
	//cout <<ArrOfValues[8]<<endl;
	value = ((ArrOfValues[0] * v) + (ArrOfValues[1] * v) + (ArrOfValues[2] * v)) + ((ArrOfValues[3] * v) + (ArrOfValues[4]* v) + (ArrOfValues[5] * v)) + ((ArrOfValues[6] * v) + (ArrOfValues[7] * v) + (ArrOfValues[8] * v));
	//cout << value << endl;
	return (int)value;	

}
//O(n2)
void convertFrom2D_TO_1D(int** arr_2d, int* arr)
{
	int k = 0;
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			arr[k++] = arr_2d[i][j];
		}
	}
	
}
int main()
{

	//1D NewPixel
	MPI_Init(NULL, NULL);
	int world_size;	
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int* imageData;
	int* FinalArr;
	int start_s, stop_s, TotalTime = 0;
	System::String^ imagePath;
	std::string img;
	img = "D://For University//HPC_ProjectTemplate//HPC_ProjectTemplate//Data//Input//Image2.jpg";
	imagePath = marshal_as<System::String^>(img);
	imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);
	FinalArr = new int[ImageHeight * ImageWidth];
	cout << "Before rank 0" << endl;
	if (rank == 0)
	{
		
		cout << "Test" << endl;


	}
	/*for (int i = 0; i < ImageHeight * ImageWidth; i++)
	{
		int indexI = i % ImageWidth;
		int indexJ = abs(i - indexI) / ImageWidth;
		FinalArr[i] = ConvertIndex(indexJ, indexI, imageData);
	}*/

	


	
	//MPI_Bcast(FinalArr, ImageHeight*ImageWidth, MPI_INT, 0, MPI_COMM_WORLD);

	

		int sizeOfEachArray = (ImageHeight*ImageWidth)/world_size;
		int *arrFotScater = new int[sizeOfEachArray];
		int *arrForGather = new int[sizeOfEachArray];
		int* arrForFinal = new int[ImageHeight * ImageWidth];

		
	
		MPI_Scatter(imageData, sizeOfEachArray, MPI_INT, arrFotScater, sizeOfEachArray, MPI_INT, 0, MPI_COMM_WORLD);


		for(int i=0; i<sizeOfEachArray; i++)
		{
			//int index = arrFotScater[i];
			int indexInFirstMatrix = rank* sizeOfEachArray +i;
			int indexi = indexInFirstMatrix%ImageWidth;
			int indexj = abs(i-indexi) / ImageWidth;
			arrForGather[i] = ConvertIndex(indexj, indexi, imageData);
	
		}
		cout << "After forLoop" << endl;
		MPI_Gather(arrForGather,sizeOfEachArray,MPI_INT, arrForFinal ,sizeOfEachArray,MPI_INT,0,MPI_COMM_WORLD);
		if (rank == 0)
		{
			for (int i = 0; i < ImageHeight * ImageWidth; i++)
			{
				cout << arrForFinal[i] << endl;
			}
		}
		cout << "After Gathering" << endl;

		if (rank == 0)
		{
			createImage(arrForFinal, ImageWidth, ImageHeight, 5);
		}
		
		free(imageData);
		cout << "Before Finalize" << endl;
		MPI_Finalize();
		cout << "After Finalize" << endl;
	
	//system("pause");
	return 0;
}



